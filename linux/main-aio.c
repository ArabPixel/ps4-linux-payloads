/*
 * main-aio.c — All-In-One PS4 Linux payload
 *
 * Compile-time flags:
 *   -DVRAM_MB_DEFAULT=<n>   default VRAM size in MB (required)
 *   -DVRAM_MB_MIN=<n>       minimum VRAM size in MB
 *   -DVRAM_MB_MAX=<n>       maximum VRAM size in MB
 */

#include <sys/types.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/thr.h>
#include <time.h>
#include <sys/sysctl.h>
#include "sb_detect.h"


#include "aio_types.h"
#include "fw_detect.h"
#include "fw_offsets.h"

void* dlopen(const char*, int);
void* dlsym(void*, const char*);

typedef int (*t_sysctlbyname)(const char *, void *, size_t *, const void *, size_t);
void thr_exit(long *state);

static unsigned long long str_len(const char *s)
{
    unsigned long long len = 0;

    while (s && s[len])
        len++;

    return len;
}

static void log_msg(const char *msg)
{
    unsigned long long len = str_len(msg);

    if (len)
        write(1, msg, len);
    if (!len || msg[len - 1] != '\n')
        write(1, "\n", 1);
}

// Universal kexec blobs
asm("kexec_505:\n.incbin \"kexec-build/505/kexec.bin\"\nkexec_505_end:\n");
asm("kexec_672:\n.incbin \"kexec-build/672/kexec.bin\"\nkexec_672_end:\n");
asm("kexec_700:\n.incbin \"kexec-build/700/kexec.bin\"\nkexec_700_end:\n");
asm("kexec_750:\n.incbin \"kexec-build/750/kexec.bin\"\nkexec_750_end:\n");
asm("kexec_800:\n.incbin \"kexec-build/800/kexec.bin\"\nkexec_800_end:\n");
asm("kexec_850:\n.incbin \"kexec-build/850/kexec.bin\"\nkexec_850_end:\n");
asm("kexec_900:\n.incbin \"kexec-build/900/kexec.bin\"\nkexec_900_end:\n");
asm("kexec_903:\n.incbin \"kexec-build/903/kexec.bin\"\nkexec_903_end:\n");
asm("kexec_960:\n.incbin \"kexec-build/960/kexec.bin\"\nkexec_960_end:\n");
asm("kexec_1000:\n.incbin \"kexec-build/1000/kexec.bin\"\nkexec_1000_end:\n");
asm("kexec_1050:\n.incbin \"kexec-build/1050/kexec.bin\"\nkexec_1050_end:\n");
asm("kexec_1100:\n.incbin \"kexec-build/1100/kexec.bin\"\nkexec_1100_end:\n");
asm("kexec_1102:\n.incbin \"kexec-build/1102/kexec.bin\"\nkexec_1102_end:\n");
asm("kexec_1150:\n.incbin \"kexec-build/1150/kexec.bin\"\nkexec_1150_end:\n");
asm("kexec_1200:\n.incbin \"kexec-build/1200/kexec.bin\"\nkexec_1200_end:\n");
asm("kexec_1250:\n.incbin \"kexec-build/1250/kexec.bin\"\nkexec_1250_end:\n");
asm("kexec_1300:\n.incbin \"kexec-build/1300/kexec.bin\"\nkexec_1300_end:\n");
asm("kexec_1302:\n.incbin \"kexec-build/1302/kexec.bin\"\nkexec_1302_end:\n");
asm("kexec_1350:\n.incbin \"kexec-build/1350/kexec.bin\"\nkexec_1350_end:\n");

/* Forward declarations for all blob symbols */
extern char kexec_505[], kexec_505_end[];
extern char kexec_672[], kexec_672_end[];
extern char kexec_700[], kexec_700_end[];
extern char kexec_750[], kexec_750_end[];
extern char kexec_800[], kexec_800_end[];
extern char kexec_850[], kexec_850_end[];
extern char kexec_900[], kexec_900_end[];
extern char kexec_903[], kexec_903_end[];
extern char kexec_960[], kexec_960_end[];
extern char kexec_1000[], kexec_1000_end[];
extern char kexec_1050[], kexec_1050_end[];
extern char kexec_1100[], kexec_1100_end[];
extern char kexec_1102[], kexec_1102_end[];
extern char kexec_1150[], kexec_1150_end[];
extern char kexec_1200[], kexec_1200_end[];
extern char kexec_1250[], kexec_1250_end[];
extern char kexec_1300[], kexec_1300_end[];
extern char kexec_1302[], kexec_1302_end[];
extern char kexec_1350[], kexec_1350_end[];

// Globals set by main() and used by kernel_main()
// kernel_main() runs in kernel context but can still read
// payload globals because the payload's physical memory remains mapped.

static fw_offsets_t *g_fw       = (fw_offsets_t *)0;
static char         *g_kexec_s  = (char *)0;
static char         *g_kexec_e  = (char *)0;


// Helper functions

static fw_offsets_t *find_offsets_by_fw(u16 fw)
{
    u16 norm = normalize_fw_ver(fw);
    for (int i = 0; fw_table[i].fw_ver != 0; i++) {
        if (fw_table[i].fw_ver == norm)
            return &fw_table[i];
    }
    return (fw_offsets_t *)0;
}

/*
 * get_kexec_blob — returns the start/end of the embedded kexec
 * binary according to the normalized firmware version. Switch,
 * to avoid problems with linkers and addresses.
 */
static void get_kexec_blob(u16 norm_fw, char **start, char **end)
{
    switch (norm_fw) {
    case  505: *start = kexec_505;  *end = kexec_505_end;  break;
    case  672: *start = kexec_672;  *end = kexec_672_end;  break;
    case  700: *start = kexec_700;  *end = kexec_700_end;  break;
    case  750: *start = kexec_750;  *end = kexec_750_end;  break;
    case  800: *start = kexec_800;  *end = kexec_800_end;  break;
    case  850: *start = kexec_850;  *end = kexec_850_end;  break;
    case  900: *start = kexec_900;  *end = kexec_900_end;  break;
    case  903: *start = kexec_903;  *end = kexec_903_end;  break;
    case  960: *start = kexec_960;  *end = kexec_960_end;  break;
    case 1000: *start = kexec_1000; *end = kexec_1000_end; break;
    case 1050: *start = kexec_1050; *end = kexec_1050_end; break;
    case 1100: *start = kexec_1100; *end = kexec_1100_end; break;
    case 1102: *start = kexec_1102; *end = kexec_1102_end; break;
    case 1150: *start = kexec_1150; *end = kexec_1150_end; break;
    case 1200: *start = kexec_1200; *end = kexec_1200_end; break;
    case 1250: *start = kexec_1250; *end = kexec_1250_end; break;
    case 1300: *start = kexec_1300; *end = kexec_1300_end; break;
    case 1302: *start = kexec_1302; *end = kexec_1302_end; break;
    case 1350: *start = kexec_1350; *end = kexec_1350_end; break;
    default:   *start = (char *)0;  *end = (char *)0;      break;
    }
}

// Platform-specific helper functions for AIO launch

/*
 * Syscall 153 trampoline.  Args 1-6 go in the standard registers; args 7-8
 * (fw_dump_buf, fw_dump_size_ptr) are pushed on the user stack by the C
 * caller and picked up by the FreeBSD kernel's amd64 syscall entry path.
 * The asm stub itself is identical to before — no stack manipulation needed.
 */
asm("kexec_load:\nmov %rcx, %r10\nmov $153, %rax\nsyscall\nret");

int kexec_load(char *kernel, unsigned long long kernel_size,
               char *initrd,  unsigned long long initrd_size,
               char *cmdline, int vram_mb,
               void *fw_dump_buf, size_t *fw_dump_size_ptr);

int read_file(char *path, char **ptr, unsigned long long *sz)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0) return -1;
    *sz = lseek(fd, 0, SEEK_END);
    *ptr = mmap(NULL, *sz, PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    char *p = *ptr;
    unsigned long long l = *sz;
    lseek(fd, 0, SEEK_SET);
    while (l) {
        unsigned long long chk = read(fd, p, l);
        if (chk <= 0) return -1;
        p += chk;
        l -= chk;
    }
    close(fd);
    return 0;
}

/*
 * Write sz bytes from buf to path.  Returns 0 on success, -1 on any error.
 * Used to persist the firmware CPIO to the same USB/HDD location as bzImage.
 */
static int write_file(const char *path, const void *buf, unsigned long long sz)
{
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) return -1;
    const char *p = (const char *)buf;
    while (sz) {
        long long n = write(fd, p, sz);
        if (n <= 0) { close(fd); return -1; }
        p  += (unsigned long long)n;
        sz -= (unsigned long long)n;
    }
    close(fd);
    return 0;
}

/*
 * Firmware CPIO dump buffer.  Filled by sys_kexec via copyout after
 * firmware_extract().  128 KB is a safe upper bound for FW_CPIO_SIZE.
 * In BSS so it does not inflate the binary on disk.
 */
#define FW_DUMP_BUF_SIZE (128 * 1024)
static char   g_fw_dump_buf[FW_DUMP_BUF_SIZE];
static size_t g_fw_dump_size;

// evf_open, evf_cancel, evf_close - some events for reboot
int evf_open(char *);
void evf_cancel(int, unsigned long long, unsigned long long);
void evf_close(int);

void reboot_thread(void *_)
{
    // Wait a bit and then reboot
    nanosleep((const struct timespec *)
              "\1\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", NULL);
    int evf = evf_open("SceSysCoreReboot");
    evf_cancel(evf, 0x4000, 0);
    evf_close(evf);
    kill(1, SIGUSR1);
    
    // Threads must not return, so we exit or hang.
    // A return would cause user thread fatal error
    thr_exit(NULL);
    for(;;);
}

void alert(const char* msg)
{
    static int(*sceSysUtilSendSystemNotificationWithText)(int, const char*);
    if(!sceSysUtilSendSystemNotificationWithText)
    {
        // Try standard path first, then common path
        void* handle = dlopen("/system/common/lib/libSceSysUtil.sprx", 0);
        if(!handle) handle = dlopen("libSceSysUtil.sprx", 0);
        
        if(handle)
            sceSysUtilSendSystemNotificationWithText = dlsym(handle, "sceSysUtilSendSystemNotificationWithText");
    }
    
    if(sceSysUtilSendSystemNotificationWithText)
        sceSysUtilSendSystemNotificationWithText(222, msg);
}

int my_atoi(const char *s)
{
    int ret = 0, neg = 0;
    while (*s == ' ') s++;
    neg = (*s == '-') ? 1 : 0;
    for (; *s; s++) {
        char c = *s;
        if ('0' <= c && c <= '9') { ret *= 10; ret += c - '0'; }
        else break;
    }
    return neg ? -ret : ret;
}

static int pack_kexec_args(int vram_mb, u16 fw_ver, int sb_val)
{
    u32 sb_family = (u32)((sb_val >> 16) & 0xF); 
    
    return (int)(
        ((sb_family & 0xF) << KEXEC_SB_SHIFT) | 
        ((u32)fw_ver << KEXEC_FW_SHIFT)       | 
        ((u32)vram_mb & KEXEC_VRAM_MASK)
    );
}

// kernel_main() — in kernel context after kexec exploit
// Reads g_fw / g_kexec_s / g_kexec_e which main() sets

void kexec(void *f, void *u);

static unsigned long long get_syscall(void)
{
    unsigned int eax, ecx, edx;
    ecx = 0xc0000082;
    asm volatile("rdmsr" : "=a"(eax), "=d"(edx) : "c"(ecx));
    return ((unsigned long long)edx) << 32 | eax;
}

void kernel_main(void)
{
    // kernel_base calculate
    unsigned long long kernel_base = get_syscall() - g_fw->xfast_syscall;

    // Disable write-protect to patch kernel
    asm volatile("cli\nmov %%cr0, %%rax\nbtc $16, %%rax\nmov %%rax, %%cr0"
                 : : : "rax");

    *(char *)(kernel_base + g_fw->patch1) = 0x07;
    *(char *)(kernel_base + g_fw->patch2) = 0x07;
    // pstate set before shutdown (needed for PS4 Pro)
    *(char *)(kernel_base + g_fw->pstate)  = 0x03;

    asm volatile("mov %%cr0, %%rax\nbts $16, %%rax\nmov %%rax, %%cr0\nsti"
                 : : : "rax");

    unsigned long long early_printf = kernel_base + g_fw->printf_off;
    unsigned long long kmem_alloc   = kernel_base + g_fw->kmem_alloc;
    unsigned long long kernel_map   = kernel_base + g_fw->kernel_map;

    // Kernel memory alloc and copy kexec blob
    char *new_kexec = ((char *(*)(unsigned long long, unsigned long long))
                       kmem_alloc)(*(unsigned long long *)kernel_map,
                                   g_kexec_e - g_kexec_s);
    for (int i = 0; g_kexec_s + i != g_kexec_e; i++)
        new_kexec[i] = g_kexec_s[i];

    // Launch kexec blob
    ((void (*)(void *, void *))new_kexec)((void *)early_printf, NULL);
}

// VRAM default values

#ifndef VRAM_MB_DEFAULT
#define VRAM_MB_DEFAULT 1024
#endif
#ifndef VRAM_MB_MIN
#define VRAM_MB_MIN 32
#endif
#ifndef VRAM_MB_MAX
#define VRAM_MB_MAX 4609
#endif
#ifndef HDD_BOOT_PATH
#define HDD_BOOT_PATH "/data/linux/boot/"
#endif
#ifndef HDD_SECOND_BOOT_PATH
#define HDD_SECOND_BOOT_PATH "/user/system/boot/"
#endif

int get_sb_id() {
    static t_sysctlbyname p_sysctlbyname = NULL;

    if (!p_sysctlbyname) {
        // Resolve it from libkernel.sprx which is always loaded
        void* handle = dlopen("libkernel.sprx", 0); 
        if (!handle) handle = dlopen("/system/common/lib/libkernel.sprx", 0);
        
        p_sysctlbyname = (t_sysctlbyname)dlsym(handle, "sysctlbyname");
    }

    if (p_sysctlbyname) {
        uint32_t id = 0;
        size_t len = sizeof(id);
        // Call the resolved function pointer
        if (p_sysctlbyname("hw.sce_subsys_subid", &id, &len, NULL, 0) == 0) {
            return (int)id;
        }
    }
    return -1;
}

const char* GetSouthbridgeName(int val) {
    // Sistro's hex patterns (0xXXYYZZ)
    switch (val & 0xFFFFFF) {
        case 0x10100: return "Aeolia A0";
        case 0x10200: return "Aeolia A1";
        case 0x10300: return "Aeolia A2";
        case 0x20100: return "Belize A0";
        case 0x20200: return "Belize B0";
        case 0x30100: return "Baikal A0";
        case 0x30200: return "Baikal B0";
        case 0x30201: return "Baikal B1";
        case 0x40100: return "Belize2 A0";
        default:      return "Unknown Southbridge";
    }
}

// main() — user-space entry pointas

int main(void)
{
    // Ignore signals to prevent process termination
    struct sigaction sa = { .sa_handler = SIG_IGN, .sa_flags = 0 };
    sigaction(SIGSTOP, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGKILL, &sa, NULL);

    // Firmware detection and blob selection
    u16 fw_ver = get_firmware();
    u16 norm   = normalize_fw_ver(fw_ver);

    g_fw = find_offsets_by_fw(norm);
    if (!g_fw) {
        alert("AIO: Unsupported firmware version - no offset table entry.");
        return 1;
    }

    get_kexec_blob(norm, &g_kexec_s, &g_kexec_e);
    if (!g_kexec_s || g_kexec_s == g_kexec_e) {
        alert("AIO: No kexec blob found for this firmware.");
        return 1;
    }
    // southbridge
    int sb_val = get_sb_id();

    // Linux files loading from USB or HDD
    char *kernel = NULL; unsigned long long kernel_size = 0;
    char *initrd = NULL; unsigned long long initrd_size = 0;
    char *cmdline = NULL; unsigned long long cmdline_size = 0;
    char *vramstr = NULL; unsigned long long vramstr_size = 0;
    int vram_mb = 0;

#define L(name, where, wheresz, is_fatal) \
    if (read_file("/mnt/usb0/" name, where, wheresz) \
     && read_file("/mnt/usb1/" name, where, wheresz) \
     && read_file(HDD_BOOT_PATH name, where, wheresz) \
     && read_file(HDD_SECOND_BOOT_PATH name, where, wheresz) \
     && is_fatal) { \
        alert("Failed to load file: " name "\nChecked paths:\n" \
                "/mnt/usb0/" name "\n/mnt/usb1/" name "\n" \
                HDD_BOOT_PATH name "\n" HDD_SECOND_BOOT_PATH name); \
        return 1; \
    }

    L("bzImage",            &kernel,  &kernel_size,  1);
    L("initramfs.cpio.gz",  &initrd,  &initrd_size,  1);
    L("bootargs.txt",       &cmdline, &cmdline_size,  0);

    if (cmdline && cmdline_size) {
        for (int i = 0; i < (int)cmdline_size; i++)
            if (cmdline[i] == '\n') { cmdline[i] = '\0'; break; }
    } else {
        cmdline = "panic=0 clocksource=tsc consoleblank=0 net.ifnames=0 "
                  "radeon.dpm=0 amdgpu.dpm=0 drm.debug=0 "
                  "console=ttyS0,115200n8 console=tty0 "
                  "video=HDMI-A-1:1920x1080@60";
    }

    L("vram.txt", &vramstr, &vramstr_size, 0);
    if (vramstr && vramstr_size) {
        vram_mb = my_atoi(vramstr);
        if (vram_mb < VRAM_MB_MIN || vram_mb > VRAM_MB_MAX)
            vram_mb = VRAM_MB_DEFAULT;
    } else {
        vram_mb = VRAM_MB_DEFAULT;
    }

    // Launch kernel exploit → kernel_main()
    kexec(kernel_main, (void *)0);

    // Launch reboot watchdog thread, then Linux loader
    long x, y;
    struct thr_param thr = {
        .start_func = reboot_thread,
        .arg        = NULL,
        .stack_base = mmap(NULL, 16384, PROT_READ | PROT_WRITE,
                           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0),
        .stack_size = 16384,
        .tls_base   = NULL,
        .tls_size   = 0,
        .child_tid  = &x,
        .parent_tid = &y,
        .flags      = 0,
        .rtp        = NULL,
    };
    thr_new(&thr, sizeof(thr));
    g_fw_dump_size = 0;
    kexec_load(kernel, kernel_size, initrd, initrd_size, cmdline,
               pack_kexec_args(vram_mb, fw_ver, sb_val),
               g_fw_dump_buf, &g_fw_dump_size);

    /*
     * kexec_load() (sys_kexec) returns to userspace after arming the ICC
     * hook but before the actual reboot.  If sys_kexec copied out the
     * firmware CPIO, write it to whichever storage path is reachable —
     * the same set we tried for bzImage so the blobs land next to it.
     */
    if (g_fw_dump_size > 0) {
        log_msg("AIO: writing fw CPIO to storage...");
#define DUMP(dir) \
        if (write_file(dir "ps4fw.cpio", g_fw_dump_buf, g_fw_dump_size) == 0) { \
            log_msg("AIO: fw CPIO written to " dir "ps4fw.cpio"); \
            goto fw_dump_done; \
        }
        DUMP("/mnt/usb0/")
        DUMP("/mnt/usb1/")
        DUMP(HDD_BOOT_PATH)
        DUMP(HDD_SECOND_BOOT_PATH)
#undef DUMP
        log_msg("AIO: fw CPIO write failed on all paths");
fw_dump_done:;
    }

    for (;;);
}
