---
tags: [ kernel ] 
layout: post
date: 2015-09-22
title: Linux time subsystem
category: cs
---

# Formal causes
## CPU time

commit 0a71336b6a8858a525007c5b4e0d14ba57f9f315
Author:     Martin Schwidefsky <schwidefsky@de.ibm.com>
AuthorDate: Tue Jan 11 01:40:38 2005 -0800
Commit:     Linus Torvalds <torvalds@ppc970.osdl.org>
CommitDate: Tue Jan 11 01:40:38 2005 -0800
    [PATCH] cputime: introduce cputime
    This patch introduces the concept of (virtual) cputime.  Each architecture
    can define its method to measure cputime.  The main idea is to define a
    cputime_t type and a set of operations on it (see asm-generic/cputime.h).
    Then use the type for utime, stime, cutime, cstime, it_virt_value,
    it_virt_incr, it_prof_value and it_prof_incr and use the cputime operations
    for each access to these variables.  The default implementation is jiffies
    based and the effect of this patch for architectures which use the default
    implementation should be neglectible.

    There is a second type cputime64_t which is necessary for the kernel_stat
    cpu statistics.  The default cputime_t is 32 bit and based on HZ, this will
    overflow after 49.7 days.  This is not enough for kernel_stat (ihmo not
    enough for a processes too), so it is necessary to have a 64 bit type.
### How to caculate cpu time usage
http://www.ilinuxkernel.com/files/Linux_CPU_Usage_Analysis.pdf
CPU time =user+system+nice+idle+iowait+irq+softirq+Stl
%us =(User time + Nice time)/CPU time * 100
%sy =(System time + Hard Irq time +SoftIRQ time)/CPU time * 100
#### vmstat.c
        duse = *cpu_use + *cpu_nic;
        dsys = *cpu_sys + *cpu_xxx + *cpu_yyy;
        didl = *cpu_idl;
        diow = *cpu_iow;
        dstl = *cpu_zzz;
        Div = duse + dsys + didl + diow + dstl;
        if (!Div) Div = 1, didl = 1;
        divo2 = Div / 2UL;
        printf(w_option ? wide_format : format,
               running, blocked,
               unitConvert(kb_swap_used), unitConvert(kb_main_free),
               unitConvert(a_option?kb_inactive:kb_main_buffers),
               unitConvert(a_option?kb_active:kb_main_cached),
               (unsigned)( (unitConvert(*pswpin  * kb_per_page) * hz + divo2) / Div ),
               (unsigned)( (unitConvert(*pswpout * kb_per_page) * hz + divo2) / Div ),
               (unsigned)( (*pgpgin                * hz + divo2) / Div ),
               (unsigned)( (*pgpgout               * hz + divo2) / Div ),
               (unsigned)( (*intr                  * hz + divo2) / Div ),
               (unsigned)( (*ctxt                  * hz + divo2) / Div ),
               (unsigned)( (100*duse                    + divo2) / Div ),
               (unsigned)( (100*dsys                    + divo2) / Div ),
               (unsigned)( (100*didl                    + divo2) / Div ),
               (unsigned)( (100*diow                    + divo2) / Div ),
               (unsigned)( (100*dstl                    + divo2) / Div )


### CPU time STEAL 
[iostat - What does the 'steal' field mean?](https://unix.stackexchange.com/questions/264958/iostat-what-does-the-steal-field-mean)
Author:     Martin Schwidefsky <schwidefsky@de.ibm.com>
AuthorDate: Tue Jan 11 01:40:38 2005 -0800
Commit:     Linus Torvalds <torvalds@ppc970.osdl.org>
CommitDate: Tue Jan 11 01:40:38 2005 -0800
    [PATCH] cputime: introduce cputime
...
    The third thing that gets introduced by this patch is an additional field
    for the /proc/stat interface: cpu steal time.  An architecture can account
    cpu steal time by calls to the account_stealtime function.  The cpu which
    backs a virtual processor doesn't spent all of its time for the virtual
    cpu.  To get meaningful cpu usage numbers this involuntary wait time needs
    to be accounted and exported to user space.

### CPU time iowait
check io.log

###CPU time nice
Documentation/admin-guide/pm/cpufreq.rst
commit b9170836d1aa4ded7cc1ac1cb8fbc7867061c98c
Refs: v2.6.12-rc5-248-gb9170836d1aa
Author:     Dave Jones <davej@redhat.com>
AuthorDate: Tue May 31 19:03:47 2005 -0700
Commit:     Dave Jones <davej@redhat.com>
CommitDate: Tue May 31 19:03:47 2005 -0700

    [CPUFREQ] Conservative cpufreq governer

    A new cpufreq module, based on the ondemand one with my additional patches
    just posted.  This one is more suitable for battery environments where its
    probably more appealing to have the cpu freq gracefully increase and decrease
    rather than flip between the min and max freq's.

    N.B. Bruno Ducrot pointed out that the amd64's "do have unacceptable latency
    between min and max freq transition, due to the step-by-step requirements
    (200MHz IIRC)"; so AMD64 users would probably benefit from this too.

``ignore_nice_load``
        If set to 1 (default 0), it will cause the CPU load estimation code to
        treat the CPU time spent on executing tasks with "nice" levels greater
        than 0 as CPU idle time.
        This may be useful if there are tasks in the system that should not be
        taken into account when deciding what frequency to run the CPUs at. 
        Then, to make that happen it is sufficient to increase the "nice" level
        of those tasks above 0 and set this attribute to 1.


# TSC
[Pitfalls of TSC usage](http://oliveryang.net/2015/09/pitfalls-of-TSC-usage/)
[The trouble with the TSC](https://lwn.net/Articles/388188/)

## Genus-differentia definition
interrupt, clock event,  clock source, tick, timer, timekeeping, gettimeofday.

## unites
cputime = jiffies under HZ (FIXME)
sum_exec_runtime nanoseconds
clockt = jiffies but under USER_HZ

## 功用定义
获取时间
timer
sleep ?
update sched info

## 发生定义/设计   etymology, history Operational definition
timekeeping/计时:时刻．
clock source: 时间之源, 表针之力.
clock event: 闹铃之力, 经过tick展现能力.
tick: timer的中断事件叫tick, tick device产生tick, 可以说tick决定了clock_event event_handler进而决定了, clock_event行为.
tickless: 
for timer: use HW timer one shot, set next.
for update time: in above HW timer, not good 
for sched: for priority distributed in time slice, use timer.
dynamic tick/no HZ:
No HZ in idle 
No HZ while only 1 process running for HPC.
tick devies 就是clock event包了层虎皮.
tick broadcast framework:  based on tick device
clock: 可记录时间, 表盘.利用timekeeping
timer: use clock. what about timer_list?
timer_list: do in softirq
* timekeeping aspect
onset: start_kernel -> timekeeping_init & time_init &(rest_init-> kernel_init-> \
kernel_init_freeable->do_basic_setup->do_initcalls--device_initcall(init_clocksource_sysfs))
nucleus:used by clock see init_posix_timers and update_wall_time and sys_time
coda:

* clock source aspect

* tick device layer aspect
per_cpu(tick_cpu_device, cpu)
onset: time_init->mach->init_time->...clockevents_register_device->tick_check_new_device
nucleus: a38x_timer_interrupt->a38x_clkevt.event_handler

* tick broadcast framework
onset:

* clock aspect
k_clock
onset:init_posix_timers & init_posix_cpu_timers
nucleus: 用户-> posix_clock->timepkeeping->clock_socurce

* timer aspect
onset: open_softirq(HRTIMER_SOFTIRQ, run_hrtimer_softirq); 
nucleus: common_timer_set
hrtimer_interrupt | (tick_handle_periodic-> tick_periodic-> update_process_times->run_local_timers->hrtimer_run_queues)->__run_hrtimer->timer.function = posix_timer_fn;(set in common_timer_set)

计算机概念
system clock CLOCK_REALTIME CLOCK_MONOTONIC
time.h time_t=tm=timeval=timespec calendar time  clock_t

clock event -> tick device-> handle function
clock event 相较于timer更为抽象, timer 是一种用途,timer 通过clock event来实现功能, 
clock event 通过timer来体现自己, timer面向使用者的onset, 
而clock event 则是偏向于nucleus and coda.
timer: 标记不能立即执行的变化.
clock source: 用一些整型抽象一个过程, 对于时间来说, 简直是完美的抽象.
* kernel requirement
linux的时间子系统要求硬件timer提供下面两种能力：
一是free running的counter，此外需要能够在指定的counter值上产生中断的能力。
