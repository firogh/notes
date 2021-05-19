---
tags: [ cs ] 
title: x86 interrupt and exception
date: 2017-04-03T13:02:12+08:00 
category: cs
---

# Events
Interrupts: asynonymous(passively received), external
Exception: synonymous(actively detected), internal
Software interrupts: is a trap. int/int3, into, bound.
IPI
[IRQs: the Hard, the Soft, the Threaded and the Preemptible](https://www.youtube.com/watch?v=-pehAzaP1eg)
[How Dealing with Modern Interrupt Architectures can Affect Your Sanity](https://www.youtube.com/watch?v=YE8cRHVIM4E)

# stack management
[x86_64 IST Stacks in kernel](https://www.kernel.org/doc/html/latest/x86/kernel-stacks.html)
6.14.4 Stack Switching in IA-32e Mode
irq_stack_union
## backtrace
commit a2bbe75089d5eb9a3a46d50dd5c215e213790288
x86: Don't use frame pointer to save old stack on irq entry
       /* Save previous stack value */
       movq %rsp, %rsi
...
2:     /* Store previous stack value */
       pushq %rsi
[Firo: end of EOI; x86/dumpstack: make stack name tags more comprehensible](https://lore.kernel.org/patchwork/patch/736894/)

# Concurrency, nested?
## Mask exception
RF in EFLAGS for masking #DB
[Does sti/cli affect software interrupt](https://stackoverflow.com/a/1581729/1025001)

## irq nested?
[Prevent nested interrupts when the IRQ stack is near overflowing v2](http://lwn.net/Articles/380937/)
[对Linux x86-64架构上硬中断的重新认识](http://www.lenky.info/archives/2013/03/2245)
### Firo: clear the flags for PF through interrupt gate
v3a: 6.12.1 Exception- or Interrupt-Handler Procedures
6.12.1.2 Flag Usage By Exception- or Interrupt-Handler Procedure

## synchronization
local_irq_disable() used in the code path that never disabled interrupts.
local_irq_save(flags) used in the code path that already disabled interrupts.

## in_interrupt
383 static inline void tick_irq_exit(void)
384 {
385 #ifdef CONFIG_NO_HZ_COMMON
386         int cpu = smp_processor_id();
387 
388         /* Make sure that timer wheel updates are propagated */
389         if ((idle_cpu(cpu) && !need_resched()) || tick_nohz_full_cpu(cpu)) {
390                 if (!in_interrupt())
391                         tick_nohz_irq_exit();
392         }
393 #endif
394 }
395 
396 /*
397  * Exit an interrupt context. Process softirqs if needed and possible:
398  */
399 void irq_exit(void)
400 {
401 #ifndef __ARCH_IRQ_EXIT_IRQS_DISABLED
402         local_irq_disable();
403 #else
404         lockdep_assert_irqs_disabled();
405 #endif
406         account_irq_exit_time(current);
407         preempt_count_sub(HARDIRQ_OFFSET);
408         if (!in_interrupt() && local_softirq_pending())
409                 invoke_softirq();
410 
411         tick_irq_exit();

# Exceptions
[Exceptions](http://wiki.osdev.org/Exceptions)
related code:
do_nmi do_int3 debug_stack_usage_inc, debug_idt_descr, debug_idt_table,
## Faults — A fault is an exception that can generally be corrected and that, once corrected, allows the program
to be restarted with no loss of continuity. When a fault is reported, the processor restores the machine state to
the state prior to the beginning of execution of the faulting instruction. The return address (saved contents of
the CS and EIP registers) for the fault handler points to the faulting instruction, rather than to the instruction
following the faulting instruction.
## Traps — A trap is an exception that is reported immediately following the execution of the trapping instruction.
Traps allow execution of a program or task to be continued without loss of program continuity. The return
address for the trap handler points to the instruction to be executed after the trapping instruction.
## Aborts — An abort is an exception that does not always report the precise location of the instruction causing
the exception and does not allow a restart of the program or task that caused the exception. Aborts are used to
report severe errors, such as hardware errors and inconsistent or illegal values in system tables.

## Triggering a #GP exception
exception_GP_trigger.S
## Exeception init
Rleated code:
idt_setup_early_traps           #===> idt_table: ist=0; DB, BP
idt_setup_early_pf              #===> idt_table: PF ist=0;
trap_init, idt_setup_traps                 #===> idt_table: ist=0; DE, 0x80 ... etc.
trap_init->cpu_init, idt_setup_ist_traps             #===> idt_table: ist=1; DB, NMI, BP, DF, MC;
x86_init.irqs.trap_init         #===> if !KVM, noop
idt_setup_debugidt_traps        #===> debug_idt_table, check debug stack; INTG; #DB debug; #BP int; check arch/x86/entry/entry_64.S

# Interrupt
If interrupt occured in user mode, then cpu will context swith for potential reschedule.
The Interrupt Descriptor Table (IDT) is a data structure used by the x86 architecture to implement an interrupt vector table. 
## Hardware interrupts
are used by devices to communicate that they require attention from the operating system.
more details in init_IRQ() or set_irq() in driver.
## software interrupt 
more details in trap_init().
* exception or trap
is caused either by an exceptional condition in the processor itself, 
divide zero painc?
* special instruction, for example INT 0x80
or a special instruction in the instruction set which causes an interrupt when it is executed.
## IRQ line number vs interrupt vector
cat /proc/interrupts
            CPU0       CPU1       CPU2       CPU3
   0:         21          0          0          0  IR-IO-APIC    2-edge      timer
v3a Chapter 6 and Check ULK3 Chapter 4 Interrupt vectors
the 0 in /proc/interrupts is a IRQ line number
The 0 for Divide error is a interrupt vector.

## Interrupt init
early_irq_init = alloc NR_IRQS_LEGACY irq_desc; - 16    #===> [    0.000000] NR_IRQS: 65792, nr_irqs: 1024, preallocated irqs: 16
init_IRQ()->x86_init.irqs.intr_init=native_init_IRQ     #===> external interrupt init; 
	pre_vector_init = init_ISA_irqs #===> 1) legacy_pic->init(0); init 8259a; 2) link irq_desc in irq_desc_tree with flow handle and chip.
	idt_setup_apic_and_irq_gates    #===> apic normal(from 32) and system interrupts; 

## affinity
root@snow:/tmp# cat x.sh 
echo 1 > /proc/irq/129/smp_affinity
sudo trace-cmd record -p function_graph --max-graph-depth 70 -g __irq_set_affinity -c -F  ./x.sh
__irq_set_affinity msi_domain_set_affinity intel_ir_set_affinity apic_set_affinity

interrupt balancing
Interrupts not distributed as specified in smp_affinity: https://www.suse.com/support/kb/doc/?id=000018837
De-mystifying interrupt balancing: irqbalance: https://www.youtube.com/watch?v=hjMWVrqrt2U

# IPI
commit 52aec3308db85f4e9f5c8b9f5dc4fbd0138c6fa4
Author: Alex Shi <alex.shi@intel.com>
Date:   Thu Jun 28 09:02:23 2012 +0800
    x86/tlb: replace INVALIDATE_TLB_VECTOR by CALL_FUNCTION_VECTOR
ERROR_APIC_VECTOR               0xfe
RESCHEDULE_VECTOR               0xfd
CALL_FUNCTION_VECTOR            0xfc
CALL_FUNCTION_SINGLE_VECTOR     0xfb
THERMAL_APIC_VECTOR             0xfa
THRESHOLD_APIC_VECTOR           0xf9
REBOOT_VECTOR                   0xf8

# History
[history of interrupts](https://people.cs.clemson.edu/~mark/interrupts.html)
[Another History of interrupts with video](https://virtualirfan.com/history-of-interrupts)
