---
tags: [ cs ] 
title: x86 interrupt and exception
date: 2017-04-03T13:02:12+08:00 
category: cs
---

# References
[history of interrupts](https://people.cs.clemson.edu/~mark/interrupts.html)
[Another History of interrupts with video](https://virtualirfan.com/history-of-interrupts)
Interrupts: asynonymous(passively received), external
Exception: synonymous(actively detected), internal
Software interrupts: is a trap. int/int3, into, bound.
[为什么Linux内核不允许在中断中休眠](http://wangcong.org/2012/06/01/-e4-b8-ba-e4-bb-80-e4-b9-88linux-e5-86-85-e6-a0-b8-e4-b8-8d-e5-85-81-e8-ae-b8-e5-9c-a8-e4-b8-ad-e6-96-ad-e4-b8-ad-e4-bc-91-e7-9c-a0-ef-bc-9f/)
[IRQs: the Hard, the Soft, the Threaded and the Preemptible](https://www.youtube.com/watch?v=-pehAzaP1eg)
[How Dealing with Modern Interrupt Architectures can Affect Your Sanity](https://www.youtube.com/watch?v=YE8cRHVIM4E)
## Mask exception
RF in EFLAGS for masking #DB
SS & SP

# Form
## Interrupt Context terminology
* interrupt context - in_irq or in_interrupt
irq_enter 
* bh context - in_softirq
local_bh_disable 
__do_softirq -> __local_bh_disable_ip

## Recusively disbale irq
local_irq_disable() used in the code path that never disabled interrupts.
local_irq_save(flags) used in the code path that already disabled interrupts.

## irq nested?
[Prevent nested interrupts when the IRQ stack is near overflowing v2](http://lwn.net/Articles/380937/)
[对Linux x86-64架构上硬中断的重新认识](http://www.lenky.info/archives/2013/03/2245)

# Exceptions
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
## kernel handler
do_error_trap

# x86
## Interrupt vector
arch/x86/include/asm/irq_vectors.h
## Why do timer interrupt and #DE share the same vector 0.
cat /proc/interrupts
            CPU0       CPU1       CPU2       CPU3
   0:         21          0          0          0  IR-IO-APIC    2-edge      timer
v3a Chapter 6
0, DE, Divide Error, Fault, No DIV and IDIV instructions.
Check ULK3 Chapter 4 Interrupt vectors and Linux 技术内幕 7.2.1中断号
the 0 in /proc/interrupts is a IRQ line number
The 0 for Divide error is a interrupt vector.
## An example of threaded irq
mei_me_probe
ps axjf | grep irq | grep mei
    2   499     0     0 ?           -1 S        0   0:00  \_ [irq/126-mei_me]
## Does sti/cli affect software interrupt
https://stackoverflow.com/a/1581729/1025001
## IPI
commit 52aec3308db85f4e9f5c8b9f5dc4fbd0138c6fa4
Author: Alex Shi <alex.shi@intel.com>
Date:   Thu Jun 28 09:02:23 2012 +0800
    x86/tlb: replace INVALIDATE_TLB_VECTOR by CALL_FUNCTION_VECTOR
 73 #define ERROR_APIC_VECTOR               0xfe
 74 #define RESCHEDULE_VECTOR               0xfd
 75 #define CALL_FUNCTION_VECTOR            0xfc
 76 #define CALL_FUNCTION_SINGLE_VECTOR     0xfb
 77 #define THERMAL_APIC_VECTOR             0xfa
 78 #define THRESHOLD_APIC_VECTOR           0xf9
 79 #define REBOOT_VECTOR                   0xf8

## Practices
* Triggering a #GP exception
exception_GP_trigger.S
[Exceptions](http://wiki.osdev.org/Exceptions)
if you do lidt in userspace program, you will receive SIGSEGV with si_code 128(somewhere of kernel).
But with the dmesg traps: int0x80[15066] general protection ip:4000c7 sp:7ffc8706cdf0 error:0 in int0x80[400000+1000] form do_general_protection.
Privilege instructions in V3a chapter 5 Protection
* Obtain sys_call_table on amd64
https://www.exploit-db.com/papers/13146/

# Material
## irq and exeception init
type: gate_desc; desc_struct
file: arch/x86/kernel/idt.c
start_kernel
{
        setup_arch
        {
                idt_setup_early_traps           #===> idt_table: ist=0; DB, BP
                ...
                idt_setup_early_pf              #===> idt_table: PF ist=0;
        }
        ...
        trap_init
        {
                idt_setup_traps                 #===> idt_table: ist=0; DE, 0x80 ... etc.
                ...
                cpu_init
                {
                        load_current_idt
                        ...
                        ist stacks init - exception_stacks
                        t->x86_tss.ist[v] points to top of each stack.
                }
                idt_setup_ist_traps             #===> idt_table: ist=1; DB, NMI, BP, DF, MC;
                x86_init.irqs.trap_init         #===> if !KVM, noop
                idt_setup_debugidt_traps        #===> debug_idt_table, check debug stack; INTG; #DB debug; #BP int; check arch/x86/entry/entry_64.S
                                # idtentry debug                  do_debug                has_error_code=0        paranoid=1 shift_ist=DEBUG_STACK
                                # idtentry int3                 do_int3                 has_error_code=0        paranoid=1 shift_ist=DEBUG_STACK

        }
        early_irq_init = alloc NR_IRQS_LEGACY irq_desc; - 16    #===> [    0.000000] NR_IRQS: 65792, nr_irqs: 1024, preallocated irqs: 16
        init_IRQ()->x86_init.irqs.intr_init=native_init_IRQ     #===> external interrupt init; 
        {
                pre_vector_init = init_ISA_irqs #===> 1) legacy_pic->init(0); init 8259a; 2) link irq_desc in irq_desc_tree with flow handle and chip.
                idt_setup_apic_and_irq_gates    #===> apic normal(from 32) and system interrupts; 
        }
        softirq_init
        local_irq_enable
}
### Interrupt and exception stack
https://www.kernel.org/doc/Documentation/x86/kernel-stacks
TSS v3a 6.12
6.14.4 Stack Switching in IA-32e Mode
#### Exception - page_fault
Kernel doesn't change the inherit TSS stack
#### interrupt irq_entries_start
Kernel change it irq_stack_union in ".macro interrupt"
SWAPGS 要手动切换.
ENTER_IRQ_STACK old_rsp=%rdi
movq    %rsp, \old_rsp          #==========> old_rsp should be %rdi, check https://sourceware.org/binutils/docs/as/Macro.html
movq    \old_rsp, PER_CPU_VAR(irq_stack_union + IRQ_STACK_SIZE - 8)
movq    PER_CPU_VAR(irq_stack_ptr), %rsp
#### IST exception
6.14.5 Interrupt Stack Table
Documentation/x86/kernel-stacks
Why Debug stack is double page? Spare IST pointer.

### Link to process stack from irq stack
commit a2bbe75089d5eb9a3a46d50dd5c215e213790288
x86: Don't use frame pointer to save old stack on irq entry
       /* Save previous stack value */
       movq %rsp, %rsi
...
2:     /* Store previous stack value */
       pushq %rsi
[Firo: end of EOI; x86/dumpstack: make stack name tags more comprehensible](https://lore.kernel.org/patchwork/patch/736894/)

## LQO
### do_nmi do_int3 debug_stack_usage_inc, debug_idt_descr, debug_idt_table,
load_debug_idt
commit 42181186ad4db986fcaa40ca95c6e407e9e79372
Author: Steven Rostedt <srostedt@redhat.com>
Date:   Fri Dec 16 11:43:02 2011 -0500

    x86: Add counter when debug stack is used with interrupts enabled
### clear the flags for PF through interrupt gate
v3a: 6.12.1 Exception- or Interrupt-Handler Procedures
6.12.1.2 Flag Usage By Exception- or Interrupt-Handler Procedure
### rbx in page_fault?
### x86_64 kernel don't use trap gate?
Yes
### Paranoid?
Documentation/x86/entry_64.txt
