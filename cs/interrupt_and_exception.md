---
tags: [ cs ] 
title: Interrupt and exception
date: 2017-04-03T13:02:12+08:00 
category: cs
---

# Reference
[Obtain sys_call_table on amd64 ](https://www.exploit-db.com/papers/13146/)

#Interrupt
## init
native_init_IRQ()->  set_intr_gate(i, irq_entries_start...
irq_entries_start define in arch/x86/kernel/entry_32.S

##Interrupt Context terminology
* in_irq
in __irq_enter I found preempt_count_add(HARDIRQ_OFFSET) only in here!
+irq disable 部设计preempt_cout

* in_softirq
should rename to in_softirq_or_bh_disable()
local_bh_disable use SOFTIRQ_DISABLE_OFFSET  (2 * SOFTIRQ_OFFSET)
__do_softirq use __local_bh_disable_ip(_RET_IP_, SOFTIRQ_OFFSET), damn it!

* in_interrupt
一种是irq路径上的do_softirq, 即in_serving_softirq
+local_bh_disbale()
//not in softirq context!!!
+local_bh_enable()
硬中断呢?
do_IRQ,肯定是.
local_irq_disable,不是, 但是no operation with preempt_count() 这种情况x86 in_interrupt是无感的.use irqs_disabled to check
[Question about in_interrupt() semantics with regard to softirqs]()

#FAQ
##When to save irq rather than just disable irq
local_irq_disable() used in the code path that never disabled interrupts.
local_irq_save(flags) used in the code path that already disabled interrupts.

##what about irq nested?
http://lwn.net/Articles/380937/

# Triggering a #GP exception
[Exceptions](http://wiki.osdev.org/Exceptions)
if you do lidt in userspace program, you will receive SIGSEGV with si_code 128(somewhere of kernel).
But with the dmesg traps: int0x80[15066] general protection ip:4000c7 sp:7ffc8706cdf0 error:0 in int0x80[400000+1000] form do_general_protection.
Privilege instructions in V3a chapter 5 Protection
.data
    .quad msg 

msg:
    .ascii "Hello, world!\n"
    len = . - msg 
saved_idt:
        .long 0,0 

.text
    .global _start

_start:
    movl  $4, %eax
    movl  $1, %ebx  
    movl  $msg, %ecx 
    sidt  saved_idt
    lidt  saved_idt  ;===============> crashed at here
    movl  $len, %edx 
    int   $0x80
    
    movl  $1, %eax
    xorl  %ebx, %ebx 
    int   $0x80
