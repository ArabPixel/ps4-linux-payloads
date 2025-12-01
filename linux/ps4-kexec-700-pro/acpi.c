/*
 * ps4-kexec - a kexec() implementation for Orbis OS / FreeBSD
 *
 * Copyright (C) 2015-2016 shuffle2 <godisgovernment@gmail.com>
 * Copyright (C) 2015-2016 Hector Martin "marcan" <marcan@marcan.st>
 *
 * This code is licensed to you under the 2-clause BSD license. See the LICENSE
 * file for more information.
 */

#include "types.h"

#ifdef TESTING
# include <stdio.h>
# include <sys/mman.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <string.h>
#else
# include "kernel.h"
# include "string.h"
# define printf kern.printf
#endif

#define SIG32(s0, s1, s2, s3) (s0 | (s1 << 8) | (s2 << 16) | (s3 << 24))
#define PSIG32(s) (u8)s, (u8)(s >> 8), (u8)(s >> 16), (u8)(s >> 24)

#define PACKED __attribute__((packed))

#define PCI_DEVFN(slot, func)   ((((slot) & 0x1f) << 3) | ((func) & 0x07))

struct RSDP {
    u64 sig;
    u8 checksum;
    u8 oemid[6];
    u8 rev;
    u32 rsdt_addr;
    u32 length;
    u64 xsdt_addr;
    u8 ext_checksum;
    u8 rsvd[3];
} PACKED;

struct SDTH {
    u32 sig;
    u32 length;
    u8 rev;
    u8 checksum;
    u8 oem_id[6];
    u8 oem_tid[8];
    u32 oem_rev;
    u8 creator_id[4];
    u32 creator_rev;
} PACKED;

struct RSDT {
    struct SDTH hdr;
    u32 table_addr[];
} PACKED;

struct XSDT {
    struct SDTH hdr;
    u64 table_addr[];
} PACKED;

struct FADT {
    struct SDTH hdr;
    u32 facs;
    u32 dsdt;
    // more stuff...
} PACKED;

struct ivhd_entry4 {
    u8 type;
    u16 devid;
    u8 flags;
} PACKED;

struct ivhd_header {
    u8 type;
    u8 flags;
    u16 length;
    u16 devid;
    u16 cap_ptr;
    u64 mmio_phys;
    u16 pci_seg;
    u16 info;
    u32 efr_attr;
} PACKED;

struct IVRS {
    struct SDTH hdr;
    u32 IVinfo;
    u8 reserved[8];
    struct ivhd_header hd_hdr;
    struct ivhd_entry4 hd_entries[3];
} PACKED;

// We have enough space to use the second half of the 64KB table area
// as scratch space for building the tables
#define BUFFER_OFF 0x8000

#define P2M(p) (((u64)(p)) - phys_base + map_base)
#define M2P(p) ((((void*)(p)) - map_base) + phys_base)
#define B2P(p) ((((void*)(p)) - buf_base) + phys_base)

#define ALIGN(s) p = (void*)((u64)(p + s - 1) & (-s))
#define PADB(s) p += (s)
#define ALLOCB(s) ({void *tmp=p; PADB(s); tmp;})
#define ALLOC(t) (t)ALLOCB(sizeof(t))
#define COPYB(sz, s) ({void *tmp=p; memcpy(p, s, (sz)); p += (sz); tmp;})
#define COPYT(s) COPYB(((struct SDTH*)s)->length, s)
#define COPYTP(s) COPYT(P2M(s))
#define COPY(t, s) ({void *tmp=p; *(t*)p = *(t*)s; p += sizeof(t); (t*)tmp;})
#define COPYP(t, s) COPY(t, P2M(s))

static void rsdp_checksum(struct RSDP *rsdp) {
    rsdp->checksum = rsdp->ext_checksum = 0;

    u8 sum = 0;
    for (int i = 0; i < 20; i++)
        sum += ((u8*)rsdp)[i];
    rsdp->checksum = -sum;
    sum = 0;
    for (int i = 0; i < sizeof(*rsdp); i++)
        sum += ((u8*)rsdp)[i];
    rsdp->ext_checksum = -sum;
}

static void table_checksum(void *table) {
    struct SDTH *hdr = table;
    hdr->checksum = 0;
    u8 sum = 0;
    for (int i = 0; i < hdr->length; i++)
        sum += ((u8*)table)[i];
    hdr->checksum = -sum;
}

#define IVHD_FLAG_ISOC_EN_MASK          0x08
#define IVHD_DEV_ALL                    0x01
#define IVHD_DEV_SELECT                 0x02
#define IVHD_DEV_SELECT_RANGE_START     0x03
#define IVHD_DEV_RANGE_END              0x04

#define ACPI_DEVFLAG_SYSMGT1            0x10
#define ACPI_DEVFLAG_SYSMGT2            0x20

static void *build_ivrs(struct IVRS *ivrs) {
    memset(ivrs, 0, sizeof(*ivrs));

    ivrs->hdr.sig = SIG32('I', 'V', 'R', 'S');
    ivrs->hdr.length = sizeof(*ivrs);
    ivrs->hdr.rev = 1;
    memcpy(ivrs->hdr.oem_id, "F0F   ", 6);
    memcpy(ivrs->hdr.oem_tid, "PS4KEXEC", 8);
    ivrs->hdr.oem_rev = 0x20161225;
    memcpy(ivrs->hdr.creator_id, "KEXC", 4);
    ivrs->hdr.creator_rev = 0x20161225;
    ivrs->IVinfo = 0x00203040;

    struct ivhd_header *hdr = &ivrs->hd_hdr;
    hdr->type = 0x10;
    hdr->flags = /*coherent | */(1 << 5) | IVHD_FLAG_ISOC_EN_MASK;
    hdr->length = sizeof(ivrs->hd_hdr) + sizeof(ivrs->hd_entries);
    hdr->devid = PCI_DEVFN(0, 2);
    hdr->cap_ptr = 0x40; // from config space + 0x34
    hdr->mmio_phys = 0xfc000000;
    hdr->pci_seg = 0;
    hdr->info = 0; // msi msg num? (the pci cap should be written by software)
    // HATS = 0b10, PNBanks = 2, PNCounters = 4, IASup = 1
    hdr->efr_attr = (2 << 30) | (2 << 17) | (4 << 13) | (1 << 5);

    struct ivhd_entry4 *entries = &ivrs->hd_entries[0];
    // on fbsd, all aeolia devfns have active entries except memories (func 6)
    //   not sure if this is just because it wasn't in use when i dumped it?
    // all entries are r/w
    // IntCtl = 0b01 and IV = 1 are set for all entries (irqs are forwarded)
    // apcie has SysMgt = 0b11 (others are 0b00). (device-initiated dmas are translated)
    // Modes:
    //   4 level:
    //     apcie
    //   3 level:
    //     all others

    // the way to encode this info into the IVHD entries is fairly arbitrary...
    entries[0].type = IVHD_DEV_SELECT;
    entries[0].devid = PCI_DEVFN(20, 0);
    entries[0].flags = ACPI_DEVFLAG_SYSMGT1 | ACPI_DEVFLAG_SYSMGT2;

    entries[1].type = IVHD_DEV_SELECT_RANGE_START;
    entries[1].devid = PCI_DEVFN(20, 1);
    entries[1].flags = 0;
    entries[2].type = IVHD_DEV_RANGE_END;
    entries[2].devid = PCI_DEVFN(20, 7);
    entries[2].flags = 0;

    table_checksum(ivrs);
    return ivrs + 1;
}

void fix_acpi_tables(void *map_base, u64 phys_base)
{
    void *buf_base = map_base + 0x8000;
    void *p = buf_base;
    memset(buf_base, 0, 0x8000);

    printf("Fixing ACPI tables at 0x%llx (%p)\n", phys_base, map_base);

    struct RSDP *rsdp = COPYP(struct RSDP, phys_base);
    printf("RSDT at 0x%x\n", rsdp->rsdt_addr);
    printf("XSDT at 0x%llx\n", rsdp->xsdt_addr);

    struct RSDT *rsdt_src = P2M(rsdp->rsdt_addr);
    struct RSDT *rsdt = COPYTP(rsdp->rsdt_addr);
    rsdp->rsdt_addr = B2P(rsdt);

    PADB(0x30); // this gives us space for new tables

    struct XSDT *xsdt = COPYTP(rsdp->xsdt_addr);
    rsdp->xsdt_addr = B2P(xsdt);

    PADB(0x60);

    struct FADT *fadt = NULL;

    int cnt = (rsdt_src->hdr.length - sizeof(*rsdt)) / 4;
    int i;
    for (i = 0; i < cnt; i++) {
        struct SDTH *hdr = P2M(rsdt_src->table_addr[i]);
        printf("%c%c%c%c at 0x%x\n", PSIG32(hdr->sig), rsdt_src->table_addr[i]);
        switch (hdr->sig) {
            case SIG32('F', 'A', 'C', 'P'):
            {
                fadt = (void*)hdr;
                printf("FACS at 0x%x\n", fadt->facs);
                printf("DSDT at 0x%x\n", fadt->dsdt);
                // Sony puts the FACS before the FADT, unaligned, which is
                // noncompliant, but let's keep it there
                u8 *facs = COPYB(64, P2M(fadt->facs));
                fadt = (void*)(hdr = COPYT(hdr));
                fadt->facs = B2P(facs);
                PADB(0x38);
                break;
            }
            case SIG32('S', 'S', 'D', 'T'):
            {
                // Put the DSDT before the SSDT
                if (fadt) {
                    PADB(0xf0);
                    u8 *dsdt = COPYTP(fadt->dsdt);
                    fadt->dsdt = B2P(dsdt);
                    PADB(0x174);
                    table_checksum(fadt);
                } else {
                    printf("ERROR: no FADT yet?\n");
                }
                hdr = COPYT(hdr);
                break;
            }
            default:
                hdr = COPYT(hdr);
        }
        table_checksum(hdr);
        xsdt->table_addr[i] = rsdt->table_addr[i] = B2P(hdr);
    }

    xsdt->table_addr[i] = rsdt->table_addr[i] = B2P(p);
    i++;
    p = build_ivrs(p);

    rsdt->hdr.length = sizeof(*rsdt) + 4 * i;
    xsdt->hdr.length = sizeof(*xsdt) + 8 * i;

    rsdp_checksum(rsdp);
    table_checksum(rsdt);
    table_checksum(xsdt);
    memcpy(map_base, buf_base, p - buf_base); 
}


#ifdef TESTING

int main(int argc, char **argv)
{
    int fd;
    void *base;

    fd = open(argv[1], O_RDWR);
    base = mmap(NULL, 0x10000, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    fix_acpi_tables(base, 0xe0000);
    return 0;
}

#endif
