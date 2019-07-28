---
tags: [ cs ] 
title: x86 interrupt and exception
date: 2017-04-03T13:02:12+08:00 
category: cs
---


#Interrupt
## init
native_init_IRQ() ->  set_intr_gate(i, irq_entries_start...
irq_entries_start define in arch/x86/kernel/entry_32.S

##Interrupt Context terminology
* interrupt context - in_irq or in_interrupt
irq_enter 
* bh context - in_softirq
local_bh_disable 
__do_softirq -> __local_bh_disable_ip

# Sleep in interrupt
[为什么Linux内核不允许在中断中休眠？](http://wangcong.org/2012/06/01/-e4-b8-ba-e4-bb-80-e4-b9-88linux-e5-86-85-e6-a0-b8-e4-b8-8d-e5-85-81-e8-ae-b8-e5-9c-a8-e4-b8-ad-e6-96-ad-e4-b8-ad-e4-bc-91-e7-9c-a0-ef-bc-9f/)

#When to save irq rather than just disable irq
local_irq_disable() used in the code path that never disabled interrupts.
local_irq_save(flags) used in the code path that already disabled interrupts.

#what about irq nested?
[Prevent nested interrupts when the IRQ stack is near overflowing v2](http://lwn.net/Articles/380937/)
