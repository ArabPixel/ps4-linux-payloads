/*
 * ps4-kexec - a kexec() implementation for Orbis OS / FreeBSD
 *
 * Copyright (C) 2015-2016 shuffle2 <godisgovernment@gmail.com>
 * Copyright (C) 2015-2016 Hector Martin "marcan" <marcan@marcan.st>
 *
 * This code is licensed to you under the 2-clause BSD license. See the LICENSE
 * file for more information.
 */

#ifndef KERNEL_H
#define KERNEL_H

#include "types.h"
#include "reboot.h"

#define PAGE_SIZE 0x4000
#define PAGE_MASK (PAGE_SIZE - 1)

#define PML4SHIFT 39
#define PDPSHIFT 30
#define PDRSHIFT 21
#define PAGE_SHIFT 12

#define KVADDR(l4, l3, l2, l1) ( \
	((unsigned long)-1 << 47) | \
	((unsigned long)(l4) << PML4SHIFT) | \
	((unsigned long)(l3) << PDPSHIFT) | \
	((unsigned long)(l2) << PDRSHIFT) | \
	((unsigned long)(l1) << PAGE_SHIFT))

#define PA_TO_DM(x) (((uintptr_t)x) | kern.dmap_base)
#define DM_TO_ID(x) (((uintptr_t)x) & (~kern.dmap_base)) // XXX

typedef u64 vm_paddr_t;
typedef u64 vm_offset_t;
typedef u64 vm_size_t;
typedef void * vm_map_t;
typedef char vm_memattr_t;
typedef void * pmap_t;

typedef void (*smp_rendezvous_callback_t)(void *);

struct sysent_t {
    int sy_narg;
    void *sy_call;
    u16 sy_auevent;
    void *sy_systrace_args_func;
    int sy_entry;
    int sy_return;
    int sy_flags;
    int sy_thrcnt;
};

struct ksym_t {
    // two parameters related to kaslr (they are not symbols)
    uintptr_t kern_base;
    uintptr_t dmap_base;

    int (*printf)(const char *fmt, ...);
    int (*snprintf)(const char *fmt, ...);
    int (*copyin)(const void *uaddr, void *kaddr, size_t len);
    int (*copyout)(const void *kaddr, void *uaddr, size_t len);
    int (*copyinstr)(const void *uaddr, void *kaddr, size_t len, size_t *done);

    void **kernel_map;
    void *kernel_pmap_store;
    vm_offset_t (*kmem_alloc_contig)(vm_map_t map, vm_size_t size, int flags,
                                     vm_paddr_t low, vm_paddr_t high,
                                     unsigned long alignment,
                                     unsigned long boundary,
                                     vm_memattr_t memattr);
    void (*kmem_free)(vm_map_t, vm_offset_t, vm_size_t);
    vm_paddr_t (*pmap_extract)(pmap_t pmap, vm_offset_t va);
    void (*pmap_protect)(pmap_t pmap, u64 sva, u64 eva, u8 pr);

    struct sysent_t *sysent;

    void (*sched_pin)(void);
    void (*sched_unpin)(void);
    void (*smp_rendezvous)(smp_rendezvous_callback_t,
                           smp_rendezvous_callback_t,
                           smp_rendezvous_callback_t, void *);
    // yes...it is misspelled :)
    void (*smp_no_rendevous_barrier)(void *);
    void *icc_query_nowait;
    void *Starsha_UcodeInfo;
    int (*gpu_devid_is_9924)();
    void *(*gc_get_fw_info)();
    void *eap_hdd_key;
    void *edid;
    void (*wlanbt)(unsigned int state);
    int (*kern_reboot)(int magic);
    void(*set_gpu_freq)(unsigned int num, unsigned int freq);
    void(*set_pstate)(unsigned int val);
    void(*update_vddnp)(unsigned int val);
    void(*set_cu_power_gate)(unsigned int val);
    void *pstate_before_shutdown;
};

extern struct ksym_t kern;

static inline int curcpu(void)
{
    int cpuid;
    // TODO ensure offsetof(struct pcpu, pc_cpuid) == 0x34 on all fw
    asm volatile("mov %0, gs:0x34;" : "=r" (cpuid));
    return cpuid;
}

// Assign a working printf function to this to debug the symbol resolver
extern int (*early_printf)(const char *fmt, ...);

void *kernel_resolve(const char *name);

void *kernel_alloc_contig(size_t size);
void kernel_free_contig(void *addr, size_t size);

void kernel_remap(void *start, void *end, int perm);

void kernel_syscall_install(int num, void *call, int narg);
int kernel_hook_install(void *target, void *hook);

int kernel_init(void *early_printf);

#endif
