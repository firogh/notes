---
layout: post
date: 2014-12-28
title: Interrupts of linux kernel
category: kernel
---

#Context terminology
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

[Deal PF_MEMALLOC in softirq](http://thread.gmane.org/gmane.linux.kernel/1152658)







