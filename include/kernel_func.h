#ifndef __KERNEL_FUNC_H_
#define __KERNEL_FUNC_H_

#if defined(__i386__)
#define mb() asm volatile("lock; addl $0,0(%%esp)" :: : "memory")
#define wmb() asm volatile("lock; addl $0,0(%%esp)" :: : "memory")
#define rmb() asm volatile("lock; addl $0,0(%%esp)" :: : "memory")
#define cpu_relax() asm volatile("rep; nop" :: : "memory");
#define CPUINFO_PROC "model name"
#ifndef __NR_perf_event_open
#define __NR_perf_event_open 336
#endif
#endif

#if defined(__x86_64__)
#define mb() asm volatile("mfence" :: : "memory")
#define wmb() asm volatile("sfence" :: : "memory")
#define rmb() asm volatile("lfence" :: : "memory")
#define cpu_relax() asm volatile("rep; nop" :: : "memory");
#define CPUINFO_PROC "model name"
#ifndef __NR_perf_event_open
#define __NR_perf_event_open 298
#endif
#endif

#ifdef __powerpc__
#include "../../arch/powerpc/include/uapi/asm/unistd.h"
#define mb() asm volatile("sync" :: : "memory")
#define wmb() asm volatile("sync" :: : "memory")
#define rmb() asm volatile("sync" :: : "memory")
#define CPUINFO_PROC "cpu"
#endif

#ifdef __s390__
#define mb() asm volatile("bcr 15,0" :: : "memory")
#define wmb() asm volatile("bcr 15,0" :: : "memory")
#define rmb() asm volatile("bcr 15,0" :: : "memory")
#endif

#ifdef __sh__
#if defined(__SH4A__) || defined(__SH5__)
#define mb() asm volatile("synco" :: : "memory")
#define wmb() asm volatile("synco" :: : "memory")
#define rmb() asm volatile("synco" :: : "memory")
#else
#define mb() asm volatile("" :: : "memory")
#define wmb() asm volatile("" :: : "memory")
#define rmb() asm volatile("" :: : "memory")
#endif
#define CPUINFO_PROC "cpu type"
#endif

#ifdef __hppa__
#define mb() asm volatile("" :: : "memory")
#define wmb() asm volatile("" :: : "memory")
#define rmb() asm volatile("" :: : "memory")
#define CPUINFO_PROC "cpu"
#endif

#ifdef __sparc__
#ifdef __LP64__
#define mb()                                                                   \
  asm volatile("ba,pt %%xcc, 1f\n"                                             \
               "membar #StoreLoad\n"                                           \
               "1:\n" ::                                                       \
                   : "memory")
#else
#define mb() asm volatile("" :: : "memory")
#endif
#define wmb() asm volatile("" :: : "memory")
#define rmb() asm volatile("" :: : "memory")
#define CPUINFO_PROC "cpu"
#endif

#ifdef __alpha__
#define mb() asm volatile("mb" :: : "memory")
#define wmb() asm volatile("wmb" :: : "memory")
#define rmb() asm volatile("mb" :: : "memory")
#define CPUINFO_PROC "cpu model"
#endif

#ifdef __ia64__
#define mb() asm volatile("mf" :: : "memory")
#define wmb() asm volatile("mf" :: : "memory")
#define rmb() asm volatile("mf" :: : "memory")
#define cpu_relax() asm volatile("hint @pause" :: : "memory")
#define CPUINFO_PROC "model name"
#endif

#ifdef __arm__
/*
 * Use the __kuser_memory_barrier helper in the CPU helper page. See
 * arch/arm/kernel/entry-armv.S in the kernel source for details.
 */
#define mb() ((void (*)(void))0xffff0fa0)()
#define wmb() ((void (*)(void))0xffff0fa0)()
#define rmb() ((void (*)(void))0xffff0fa0)()
#define CPUINFO_PROC "Processor"
#endif

#ifdef __aarch64__
#define mb() asm volatile("dmb ish" :: : "memory")
#define wmb() asm volatile("dmb ishst" :: : "memory")
#define rmb() asm volatile("dmb ishld" :: : "memory")
#define cpu_relax() asm volatile("yield" :: : "memory")
#endif

#ifdef __mips__
#define mb()                                                                   \
  asm volatile(".set	mips2\n\t"                                                \
               "sync\n\t"                                                      \
               ".set	mips0"                                                    \
               : /* no output */                                               \
               : /* no input */                                                \
               : "memory")
#define wmb() mb()
#define rmb() mb()
#define CPUINFO_PROC "cpu model"
#endif

#ifdef __arc__
#define mb() asm volatile("" :: : "memory")
#define wmb() asm volatile("" :: : "memory")
#define rmb() asm volatile("" :: : "memory")
#define CPUINFO_PROC "Processor"
#endif

#ifdef __metag__
#define mb() asm volatile("" :: : "memory")
#define wmb() asm volatile("" :: : "memory")
#define rmb() asm volatile("" :: : "memory")
#define CPUINFO_PROC "CPU"
#endif

#ifdef __xtensa__
#define mb() asm volatile("memw" :: : "memory")
#define wmb() asm volatile("memw" :: : "memory")
#define rmb() asm volatile("" :: : "memory")
#define CPUINFO_PROC "core ID"
#endif

#define barrier() asm volatile("" :: : "memory")

#ifndef cpu_relax
#define cpu_relax() barrier()
#endif

#define ACCESS_ONCE(x) (*(volatile typeof(x) *)&(x))

#endif
