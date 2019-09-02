---
tags: [ kernel ] 
title: Linux device driver
date: 2015-02-27T15:46:12+08:00 
category: cs
---
# Reference
Driver porting: Device model overview: http://lwn.net/Articles/31185/
Documentation/driver-model
The Linux Kernel Driver Model

# Formal causes
## DMA
ULK 3rd

## Mapping
[ioremap() and memremap()](https://lwn.net/Articles/653585/)

# Device tree
* What infomation was included in dtb
HW platform识别，runtime parameter传递以及硬件设备描述
for platform device init see arch/powerpc/platforms/52xx/media5200.c ppc_init
mpc52xx_declare_of_platform_devices
* How does the device tree subsystem read and parse the dts file?
#devres
* devm_kfree
[devm_kfree does not need for fail case of probe function and for remove function.](http://lists.freedesktop.org/archives/dri-devel/2013-May/038943.html)
__device_release_driver
really_probe
* a must use devm_kfree Julia Lawall
 The function at91_dt_node_to_map is ultimately called by the function
> pinctrl_get, which is an exported function.  Since it is possible that this
> function is not called from within a probe function, for safety, the kfree
> is converted to a devm_kfree, to both free the data and remove it from the
> device in a failure situation.
*for noop devm_kmalloc_release 
devm_kmalloc()->dr = alloc_dr(devm_kmalloc_release, size, gfp);
see release_nodes() ->kfree(dr);
