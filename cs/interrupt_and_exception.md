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
