/*
 * ps4-kexec - a kexec() implementation for Orbis OS / FreeBSD
 *
 * Copyright (C) 2015-2016 shuffle2 <godisgovernment@gmail.com>
 * Copyright (C) 2015-2016 Hector Martin "marcan" <marcan@marcan.st>
 *
 * This code is licensed to you under the 2-clause BSD license. See the LICENSE
 * file for more information.
 */

#include "linux_boot.h"
#include "types.h"
#include "string.h"
#include "x86.h"
#include "kernel.h"
#include "uart.h"
#include "acpi.h"

void uart_write_byte(u8 b);

static u64 vram_base = 0x100000000;
// Current code assumes it's a power of two.
static u64 vram_size = 1024 * 1024 * 1024;
static int vram_gb = 2;

#define DM_PML4_BASE ((kern.dmap_base >> PML4SHIFT) & 0x1ff)

#define MSR_GS_BASE     0xc0000101 /* 64bit GS base */

struct desc_struct {
    u16 limit0;
    u16 base0;
    u16 base1: 8, type: 4, s: 1, dpl: 2, p: 1;
    u16 limit: 4, avl: 1, l: 1, d: 1, g: 1, base2: 8;
} __attribute__((packed));

typedef void (*jmp_to_linux_t)(uintptr_t linux_startup, uintptr_t bootargs,
                               uintptr_t new_cr3, uintptr_t gdt_ptr);
extern uint8_t *jmp_to_linux;
extern size_t jmp_to_linux_size;

// FreeBSD DMAP addresses
struct linux_boot_info {
    void *linux_image;
    void *initramfs;
    size_t initramfs_size;
    struct boot_params *bp;
    char *cmd_line;
};
static struct linux_boot_info nix_info;

void set_nix_info(void *linux_image, struct boot_params *bp, void *initramfs,
                  size_t initramfs_size, char *cmd_line, int v)
{
    nix_info.linux_image = linux_image;
    nix_info.bp = bp;
    nix_info.initramfs = initramfs;
    nix_info.initramfs_size = initramfs_size;
    nix_info.cmd_line = cmd_line;
    vram_gb = v;
}

static volatile int halted_cpus = 0;

static void bp_add_smap_entry(struct boot_params *bp, u64 addr, u64 size,
                              u32 type)
{
    uint8_t idx = bp->e820_entries;
    bp->e820_map[idx].addr = addr;
    bp->e820_map[idx].size = size;
    bp->e820_map[idx].type = type;
    bp->e820_entries++;
}

void prepare_boot_params(struct boot_params *bp, u8 *linux_image)
{
    memset(bp, 0, sizeof(struct boot_params));
    struct boot_params *bp_src = (struct boot_params *)linux_image;
    memcpy(&bp->hdr, &bp_src->hdr, offsetof(struct setup_header, header) +
                                   ((u8 *)&bp_src->hdr.jump)[1]);

    // These values are from fw 1.01
    bp_add_smap_entry(bp, 0x0000000000, 0x0000008000, SMAP_TYPE_MEMORY);
    bp_add_smap_entry(bp, 0x0000008000, 0x0000078000, SMAP_TYPE_RESERVED);
    bp_add_smap_entry(bp, 0x0000080000, 0x000001a000, SMAP_TYPE_MEMORY);
    bp_add_smap_entry(bp, 0x000009a000, 0x0000006000, SMAP_TYPE_RESERVED);
    bp_add_smap_entry(bp, 0x00000a0000, 0x0000020000, SMAP_TYPE_RESERVED);
    bp_add_smap_entry(bp, 0x00000e0000, 0x0000010000, SMAP_TYPE_ACPI_RECLAIM);
    bp_add_smap_entry(bp, 0x0000100000, 0x0000300000, SMAP_TYPE_MEMORY);
    bp_add_smap_entry(bp, 0x0000400000, 0x0000080000, SMAP_TYPE_RESERVED);
    bp_add_smap_entry(bp, 0x0000480000, 0x0000200000, SMAP_TYPE_MEMORY);
    bp_add_smap_entry(bp, 0x0000680000, 0x0000080000, SMAP_TYPE_RESERVED);
    bp_add_smap_entry(bp, 0x0000700000, 0x007e8e8000, SMAP_TYPE_MEMORY);
    bp_add_smap_entry(bp, 0x007efe8000, 0x0000008000, SMAP_TYPE_ACPI_NVS);
    bp_add_smap_entry(bp, 0x007eff0000, 0x0000010000, SMAP_TYPE_ACPI_RECLAIM);
    // This used to be VRAM, but we reclaim it as RAM
    bp_add_smap_entry(bp, 0x007f000000, 0x0001000000, SMAP_TYPE_MEMORY);
    bp_add_smap_entry(bp, 0x0080000000, 0x0060000000, SMAP_TYPE_RESERVED);
    bp_add_smap_entry(bp, 0x00e0000000, 0x0018000000, SMAP_TYPE_RESERVED);
    bp_add_smap_entry(bp, 0x00f8000000, 0x0004000000, SMAP_TYPE_RESERVED);
    // Instead, carve out VRAM from the beginning of high memory
    bp_add_smap_entry(bp, vram_base, vram_gb * vram_size, SMAP_TYPE_RESERVED);
    bp_add_smap_entry(bp, vram_base + vram_gb * vram_size, 0x017f000000 - vram_gb * vram_size,
                      SMAP_TYPE_MEMORY);
}

#define WR32(a, v) *(volatile u32 *)PA_TO_DM(a) = (v)

#define MC_VM_FB_LOCATION 0x2024
#define MC_VM_FB_OFFSET 0x2068
#define HDP_NONSURFACE_BASE 0x2c04
#define CONFIG_MEMSIZE 0x5428

static void configure_vram(void)
{
    u64 mmio_base = 0xe4800000;
    u64 fb_base = 0x0f00000000;
    u64 fb_top = fb_base + vram_gb * vram_size - 1;

    WR32(mmio_base + MC_VM_FB_LOCATION, 0);
    WR32(mmio_base + HDP_NONSURFACE_BASE, 0);

    WR32(mmio_base + MC_VM_FB_LOCATION,
         ((fb_top >> 24) << 16) | (fb_base >> 24));
    WR32(mmio_base + MC_VM_FB_OFFSET, vram_base >> 22);
    WR32(mmio_base + HDP_NONSURFACE_BASE, fb_base >> 8);
    WR32(mmio_base + CONFIG_MEMSIZE, vram_gb * vram_size >> 20);
}

#define IA32_MTRR_DEF_TYPE 0x2ff
#define MTRR_BASE(i) (0x200 + 2*i)
#define MTRR_MASK(i) (0x201 + 2*i)

static void setup_mtrr(void)
{
    disable_interrupts();
    u64 cr0 = cr0_read();
    cr0_write((cr0 | CR0_CD) & (~(u64)CR0_NW));
    wbinvd();
    cr3_write(cr3_read()); // TLB flush

    wrmsr(IA32_MTRR_DEF_TYPE, 0); // MTRRs disabled
    
    // Low memory (0GB-2GB) = WB
    wrmsr(MTRR_BASE(0), 0x0000000006);
    wrmsr(MTRR_MASK(0), 0xff80000800);
    // High memory (4GB-8GB) = WB
    wrmsr(MTRR_BASE(1), 0x0100000006);
    wrmsr(MTRR_MASK(1), 0xff00000800);
    // High memory (8GB-10GB) = WB
    wrmsr(MTRR_BASE(2), 0x0200000006);
    wrmsr(MTRR_MASK(2), 0xff80000800);
    // VRAM (4GB-4GB+vram_size) = UC
    wrmsr(MTRR_BASE(3), 0x0100000000);
    wrmsr(MTRR_MASK(3), (0xffffffffff - vram_gb * vram_size + 1) | 0x800);
    
    wbinvd();
    cr3_write(cr3_read()); // TLB flush
    wrmsr(IA32_MTRR_DEF_TYPE, (3<<10)); // MTRRs enabled, default uncachable
    cr0_write(cr0);
    enable_interrupts(); 
}

static void cleanup_interrupts(void)
{
    int i;
    disable_interrupts();

    // Reset APIC stuff (per-CPU)
    *(volatile u32 *)PA_TO_DM(0xfee00410) = 1;
    for (i = 0x320; i < 0x380; i += 0x10)
        *(volatile u32 *)PA_TO_DM(0xfee00000 + i) = 0x10000;
    for (i = 0x480; i < 0x500; i += 0x10)
        *(volatile u32 *)PA_TO_DM(0xfee00000 + i) = 0xffffffff;
    for (i = 0x500; i < 0x540; i += 0x10)
        *(volatile u32 *)PA_TO_DM(0xfee00000 + i) = 0x10000;
    *(volatile u32 *)PA_TO_DM(0xfee00410) = 0;

    // Fix the LVT offset for thresholding
    wrmsr(0x413, (1L<<24) | (1L<<52));
    wrmsr(0xc0000408, (1L<<24) | (1L<<52));
}

#define DEFAULT_STACK	0

#define DPL0		0x0
#define DPL3		0x3

#define BPCIE_BAR2              0xc8800000
#define BPCIE_HPET_BASE         0x109000
#define BPCIE_HPET_SIZE         0x400

static void cpu_quiesce_gate(void *arg)
{


    // Ensure we can write anywhere
    cr0_write(cr0_read() & ~CR0_WP);

    // Interrupt stuff local to each CPU
    cleanup_interrupts();

    // We want to set up MTRRs on all CPUs
    setup_mtrr();

    if (curcpu() != 0) {
        // We're not on BSP. Try to halt.
        __sync_fetch_and_add(&halted_cpus, 1);
        cpu_stop();
    }
    
    uart_write_str("kexec: Waiting for secondary CPUs...\n");

    // wait for all cpus to halt
    while (!__sync_bool_compare_and_swap(&halted_cpus, 7, 7));

    uart_write_str("kexec: Secondary CPUs quiesced\n");

    //* Put ident mappings in current page tables
    // Should not be needed, but maybe helps for debugging?
    cr4_pge_disable();
    u64 *pml4_base = (u64 *)PA_TO_DM(cr3_read() & 0x000ffffffffff000ull);
    u64 *pdp_base = (u64 *)PA_TO_DM(*pml4_base & 0x000ffffffffff000ull);
    for (u64 i = 0; i < 4; i++) {
            pdp_base[i] = (i << 30) | PG_RW | PG_V | PG_U | PG_PS;
    }
    
    // Clear (really) low mem.
    // Linux reads from here to try and access EBDA...
    // get_bios_ebda reads u16 from 0x40e
    // reserve_ebda_region reads u16 from 0x413
    // Writing zeros causes linux to default to marking
    // LOWMEM_CAP(0x9f000)-1MB(0x100000) as reserved.
    // It doesn't match the ps4 e820 map, but that seems OK.
    memset((void *)0, 0, 0x1000);

    // Create a new page table hierarchy out of the way of linux
    // Accessed via freebsd direct map
    pml4_base = (u64 *)PA_TO_DM(0x1000); // "boot loader" as per linux boot.txt
    // We only use 1Gbyte mappings. So we need 2 * 0x200 * 8 = 0x2000 bytes :|
    memset(pml4_base, 0, 512 * sizeof(u64) * 2);
    pdp_base = pml4_base + 512;
    u64 pdpe = DM_TO_ID(pdp_base) | PG_RW | PG_V | PG_U;
    pml4_base[0] = pdpe;
    // Maintain the freebsd direct map
    pml4_base[DM_PML4_BASE] = pdpe;
    for (u64 i = 0; i < 4; i++) {
            pdp_base[i] = (i << 30) | PG_RW | PG_V | PG_U | PG_PS;
    }

    uart_write_str("kexec: Setting up GDT...\n");

    desc_ptr gdt_ptr;
    struct desc_struct *desc = (struct desc_struct *)(pdp_base + 512);
    gdt_ptr.limit = sizeof(struct desc_struct) * 0x100 - 1;
    gdt_ptr.address = DM_TO_ID(desc);

    // clear
    memset(desc, 0, gdt_ptr.limit + 1);
    // Most things are ignored in 64bit mode, and we will never be in
    // 32bit/compat modes, so just setup another pure-64bit environment...
    // Linux inits it's own GDT in secondary_startup_64
    // 0x10
    desc[2].limit0 = 0xffff;
    desc[2].base0 = 0x0000;
    desc[2].base1 = 0x0000;
    desc[2].type = SEG_TYPE_CODE | SEG_TYPE_EXEC_READ;
    desc[2].s = 1;
    desc[2].dpl = 0;
    desc[2].p = 1;
    desc[2].limit = 0xf;
    desc[2].avl = 0;
    desc[2].l = 1;
    desc[2].d = 0;
    desc[2].g = 0;
    desc[2].base2 = 0x00;
    // 0x18
    desc[3].limit0 = 0xffff;
    desc[3].base0 = 0x0000;
    desc[3].base1 = 0x0000;
    desc[3].type = SEG_TYPE_DATA | SEG_TYPE_READ_WRITE;
    desc[3].s = 1;
    desc[3].dpl = 0;
    desc[3].p = 1;
    desc[3].limit = 0xf;
    desc[3].avl = 0;
    desc[3].l = 0;
    desc[3].d = 0;
    desc[3].g = 0;
    desc[3].base2 = 0x00;
    // Task segment value
    // 0x20
    desc[4].limit0 = 0x0000;
    desc[4].base0 = 0x0000;
    desc[4].base1 = 0x0000;
    desc[4].type = SEG_TYPE_TSS;
    desc[4].s = 1;
    desc[4].dpl = 0;
    desc[4].p = 1;
    desc[4].limit = 0x0;
    desc[4].avl = 0;
    desc[4].l = 0;
    desc[4].d = 0;
    desc[4].g = 0;
    desc[4].base2 = 0x00;

    uart_write_str("kexec: Relocating stub...\n");

    // Relocate the stub and jump to it
    // TODO should thunk_copy be DMAP here?
    void *thunk_copy = (void *)(gdt_ptr.address + gdt_ptr.limit + 1);
    memcpy(thunk_copy, &jmp_to_linux, jmp_to_linux_size);
    // XXX The +0x200 is for the iret stack in linux_thunk.S
    uintptr_t lowmem_pos = DM_TO_ID(thunk_copy) + jmp_to_linux_size + 0x200;

    uart_write_str("kexec: Setting up boot params...\n");

    // XXX we write into this bootargs and pass it to the kernel, but in
    // jmp_to_linux we use the bootargs from the image as input. So they
    // MUST MATCH!
    struct boot_params *bp_lo = (struct boot_params *)lowmem_pos;
    *bp_lo = *nix_info.bp;
    lowmem_pos += sizeof(struct boot_params);

    struct setup_header *shdr = &bp_lo->hdr;
    shdr->cmd_line_ptr = lowmem_pos;
    shdr->ramdisk_image = DM_TO_ID(nix_info.initramfs) & 0xffffffff;
    shdr->ramdisk_size = nix_info.initramfs_size & 0xffffffff;
    bp_lo->ext_ramdisk_image = DM_TO_ID(nix_info.initramfs) >> 32;
    bp_lo->ext_ramdisk_size = nix_info.initramfs_size >> 32;
    shdr->hardware_subarch = X86_SUBARCH_PS4;
    // This needs to be nonzero for the initramfs to work
    shdr->type_of_loader = 0xd0; // kexec
    
    strlcpy((char *)DM_TO_ID(shdr->cmd_line_ptr), nix_info.cmd_line,
        nix_info.bp->hdr.cmdline_size);
    lowmem_pos += strlen(nix_info.cmd_line) + 1;

    uart_write_str("kexec: Cleaning up hardware...\n");

    // Disable IOMMU
    *(volatile u64 *)PA_TO_DM(0xfc000018) &= ~1;

    // Disable all MSIs on Baikal (bus=0, slot=20)
    disableMSI(0xf80a00e0); //func = 0 Baikal ACPI
    disableMSI(0xf80a10e0); //func = 1 Baikal Ethernet Controller
    disableMSI(0xf80a20e0); //func = 2 Baikal SATA AHCI Controller
    disableMSI(0xf80a30e0); //func = 3 Baikal SD/MMC Host Controller
    disableMSI(0xf80a40e0); //func = 4 Baikal PCI Express Glue and Miscellaneous Devices
    disableMSI(0xf80a50e0); //func = 5 Baikal DMA Controller
    disableMSI(0xf80a60e0); //func = 6 Baikal Baikal Memory (DDR3/SPM)
    disableMSI(0xf80a70e0); //func = 7 Baikal Baikal USB 3.0 xHCI Host Controller

    // Stop HPET timers
	//*(volatile u64 *)PA_TO_DM(BPCIE_BAR2 + BPCIE_HPET_BASE + 0x10) &= ~(1UL << 0);  //General Configuration Register
	/*
	u64 NUM_TIM_CAP = *(volatile u64 *)PA_TO_DM(BPCIE_BAR2 + BPCIE_HPET_BASE) & 0x1F00;
	for (u64 N = 0; N <= NUM_TIM_CAP; N++) {
		*(volatile u64 *)PA_TO_DM(BPCIE_BAR2 + BPCIE_HPET_BASE + (0x20*N) + 0x100) &= ~(1UL << 2); //Timer N Configuration and Capabilities Register
	}
	*/
    uart_write_str("kexec: Reconfiguring VRAM...\n");

    configure_vram();
	
    uart_write_str("kexec: Resetting GPU...\n");

    // Softreset GPU
    *(volatile u64 *)PA_TO_DM(0xe48086d8) = 0x15000000; // Halt CP blocks
    *(volatile u64 *)PA_TO_DM(0xe4808234) = 0x50000000; // Halt MEC
    *(volatile u64 *)PA_TO_DM(0xe480d048) = 1; // Halt SDMA0
//  *(volatile u64 *)PA_TO_DM(0xe480d248) = 1; // Halt SDMA1 eeply
	*(volatile u64 *)PA_TO_DM(0xe480d848) = 1; // Halt SDMA1
	*(volatile u64 *)PA_TO_DM(0xe480c300) = 0; // Halt RLC
	
	*(volatile u64 *)PA_TO_DM(0xe480c1a8) &= ~0x180000; // CP_INT_CNTL_RING0 eeply

//  *(volatile u64 *)PA_TO_DM(0xe4808020) |= 0x10003; // Softreset GFX/CP/RLC
	*(volatile u64 *)PA_TO_DM(0xe4808020) |= 0x30005; // Softreset GFX/CP/RLC eeply
	
//    udelay(150);
//  *(volatile u64 *)PA_TO_DM(0xe4808020) &= ~0x10003;
	*(volatile u64 *)PA_TO_DM(0xe4808020) &= ~0x30005; //eeply
//    udelay(150);
    *(volatile u64 *)PA_TO_DM(0xe4800e60) |= 0x00100140; // Softreset SDMA/GRBM
//    udelay(150);
    *(volatile u64 *)PA_TO_DM(0xe4800e60) &= ~0x00100140;
//    udelay(150);

    // Enable audio output
    *(volatile u64 *)PA_TO_DM(0xe4805e00) = 0x154;
    *(volatile u64 *)PA_TO_DM(0xe4805e04) = 0x80000000;
    *(volatile u64 *)PA_TO_DM(0xe4805e18) = 0x154;
    *(volatile u64 *)PA_TO_DM(0xe4805e1c) = 0x80000000;
    *(volatile u64 *)PA_TO_DM(0xe4805e30) = 0x154;
    *(volatile u64 *)PA_TO_DM(0xe4805e34) = 0x80000000;
    *(volatile u64 *)PA_TO_DM(0xe4813404) = 1;
    *(volatile u64 *)PA_TO_DM(0xe481340c) = 1;

//     // Set pin caps of pin 2 to vendor defined, to hide it
//     *(volatile u64 *)PA_TO_DM(0xe4805e18) = 0x101;
//     *(volatile u64 *)PA_TO_DM(0xe4805e1c) = 0xf00000;
//     *(volatile u64 *)PA_TO_DM(0xe4805e18) = 0x120;
//     *(volatile u64 *)PA_TO_DM(0xe4805e1c) = 0xf00000;
//     // Set pin caps of pin 3 to !HDMI
//     *(volatile u64 *)PA_TO_DM(0xe4805e30) = 0x121;
//     *(volatile u64 *)PA_TO_DM(0xe4805e34) = 0x10;
    // Set pin configuration default
    *(volatile u64 *)PA_TO_DM(0xe4805e00) = 0x156;
    *(volatile u64 *)PA_TO_DM(0xe4805e04) = 0x185600f0;
    *(volatile u64 *)PA_TO_DM(0xe4805e18) = 0x156;
    *(volatile u64 *)PA_TO_DM(0xe4805e1c) = 0x500000f0;
    *(volatile u64 *)PA_TO_DM(0xe4805e30) = 0x156;
    *(volatile u64 *)PA_TO_DM(0xe4805e34) = 0x014510f0;

    uart_write_str("kexec: About to relocate and jump to kernel\n");

    ((jmp_to_linux_t)thunk_copy)(
            DM_TO_ID(nix_info.linux_image),
            DM_TO_ID(bp_lo),
            DM_TO_ID(pml4_base),
            (uintptr_t)&gdt_ptr
            );
    // should never reach here
    uart_write_str("kexec: unreachable (?)\n");
}

// Hook for int icc_query_nowait(u8 icc_msg[0x7f0])
int hook_icc_query_nowait(u8 *icc_msg)
{
    kern.printf("hook_icc_query_nowait called\n");
    
    // We need reset bt/wifi, so disable it, we re-enable it when the kernel boot
    //In alternative we can re-enable it here, but sometimes that give problems.. 
    kern.wlanbt(0x2);

    fix_acpi_tables((void*)PA_TO_DM(0xe0000), 0xe0000);

    kern.printf("ACPI tables fixed\n");

    // Transition to BSP and halt other cpus
    // smp_no_rendevous_barrier is just nullsub, but it is treated specially by
    // smp_rendezvous. This is the easiest way to do this, since we can't assume
    // we're already running on BSP. Since smp_rendezvous normally waits on all
    // cpus to finish the callbacks, we just never return...
    kern.smp_rendezvous(kern.smp_no_rendevous_barrier,
                        cpu_quiesce_gate,
                        kern.smp_no_rendevous_barrier, NULL);

    // should never reach here
    kern.printf("hook_icc_query_nowait: unreachable (?)\n");
    return 0;
}
