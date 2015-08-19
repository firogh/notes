---
tags: kernel
title: Power management
date: 2015-02-27 15:46:12
category: kernel
---

# Reference
[The cpuidle subsystem](https://lwn.net/Articles/384146/)cpuidle—Do nothing, efficiently
[Hardware Specifications 9.4](http://www.marvell.com/embedded-processors/armada-xp/assets/HW_MV78460_OS.PDF)
[Functional Specifications 34] (http://www.marvell.com/embedded-processors/armada-xp/assets/ARMADA-XP-Functional-SpecDatasheet.pdf)
Documentation/power
Controlling Processor C-State Usage in Linux
[Chapter 11. Power Management](http://doc.opensuse.org/documentation/html/openSUSE_114/opensuse-tuning/cha.tuning.power.html)

C-states: idle core power state


# Introdution
电源管理都涉及到那些内容?
PMU, CPU, 
In hardware layer, we need [PMU](https://en.wikipedia.org/wiki/Power_Management_Unit) to complete power management.
What is the relation of PMU and CPU?
armadaxp的PMU与CPU物理上与逻辑上是独立的.
The Power Management functions are provided by two power manager units:
The Device Power Management Unit (DEV_PMU)
The Multiprocessor Power Management Service Unit (MP_PMU).
PMU省电模式:




# kernel cpuidle subsystem
include/linux/cpuidle.h
struct cpuidle_driver
struct cpuidle_state
struct cpuidle_device used by ladder or menu



device_initcall -> armadaxp_init_cpuidle -> cpuidle_register_driver  cpuidle_register_device
cpu_idle -> cpuidle_idle_call
