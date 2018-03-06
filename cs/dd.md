---
tags: [ kernel ] 
title: Linux device driver
date: 2015-02-27T15:46:12+08:00 
category: cs
---
#Bibilography
Driver porting: Device model overview: http://lwn.net/Articles/31185/
/Documentation/driver-model
The Linux Kernel Driver Model
# Contents
Why abstruction? 
# sysfs
kernfs_drain
## 3.14
root      3000  0.0  0.0 7241420 13352 ?       Sl   06:19   0:00 vi_config_replicator
root     12783  0.0  0.0  21980  1704 ?        S    06:16   0:00 /usr/bin/socat TCP-LISTEN:16509,fork TCP:10.0.2.16:16509
root     30718  0.0  0.0   4224   364 ttyS3    D    06:52   0:00 cat /sys/class/uio/uio3/name
root     30719  0.0  0.0   6676   644 ttyS3    D    06:52   0:00 modprobe -r uio_dma_proxy
root     30723  0.0  0.0   4408   636 ttyS3    S+   06:52   0:00 grep -e modprobe\|cat
[host:/]$ cat /proc/30718/stack
[<ffffffff8a0656bf>] msleep+0x2f/0x40
[<ffffffff8a3fc1dc>] name_show+0x1c/0x50
[<ffffffff8a3b5b30>] dev_attr_show+0x20/0x60
[<ffffffff8a219b44>] sysfs_kf_seq_show+0xc4/0x170
[<ffffffff8a21ce63>] kernfs_seq_show+0x23/0x30
[<ffffffff8a1c68cd>] seq_read+0xcd/0x3c0
[<ffffffff8a21d805>] kernfs_fop_read+0xf5/0x160
[<ffffffff8a1a31b7>] vfs_read+0x97/0x160
[<ffffffff8a1a3c96>] SyS_read+0x46/0xc0
[<ffffffff8a5c2909>] system_call_fastpath+0x16/0x1b
[<ffffffffffffffff>] 0xffffffffffffffff
[host:/]$ cat /proc/30719/stack
[<ffffffff8a21ca3c>] kernfs_addrm_finish+0x8c/0xf0
[<ffffffff8a21cca1>] kernfs_remove_by_name_ns+0x51/0xa0
[<ffffffff8a21ab71>] remove_files.isra.1+0x41/0x80
[<ffffffff8a21aeb7>] sysfs_remove_group+0x47/0xa0
[<ffffffff8a21af43>] sysfs_remove_groups+0x33/0x50
[<ffffffff8a3b50ef>] device_remove_attrs+0x6f/0x80
[<ffffffff8a3b5d19>] device_del+0x119/0x1c0
[<ffffffff8a3b5dde>] device_unregister+0x1e/0x70
[<ffffffff8a3b5eac>] device_destroy+0x3c/0x50
[<ffffffff8a3fcabd>] uio_release_complete+0x7d/0xd0
[<ffffffff8a3fcbf7>] uio_work+0xe7/0x110
[<ffffffff8a3fcc5e>] uio_unregister_device+0x3e/0x50
[<ffffffffa028b5ea>] remove+0x1a/0x50 [uio_dma_proxy]
[<ffffffff8a3bb517>] platform_drv_remove+0x17/0x20
[<ffffffff8a3b9a59>] __device_release_driver+0x69/0xd0
[<ffffffff8a3ba428>] driver_detach+0xb8/0xc0
[<ffffffff8a3b96a5>] bus_remove_driver+0x55/0xe0
[<ffffffff8a3ba87c>] driver_unregister+0x2c/0x50
[<ffffffff8a3bbc12>] platform_driver_unregister+0x12/0x20
[<ffffffffa028c44b>] destroy_platform_drivers+0x5f/0x7b [uio_dma_proxy]
[<ffffffffa028c4f7>] cleanup+0x15/0x21 [uio_dma_proxy]
[<ffffffff8a0cb7e2>] SyS_delete_module+0x142/0x1b0
[<ffffffff8a5c2909>] system_call_fastpath+0x16/0x1b
[<ffffffffffffffff>] 0xffffffffffffffff
and    
0xffffffff8121ca37 <+135>:   callq  0xffffffff815b6ad0 <wait_for_completion>
   0xffffffff8121ca3c <+140>:   nopl   0x0(%rax)
echo 2 > /proc/sys/vm/firo_uio
echo 0 > /sys/kernel/debug/tracing/tracing_on
echo function_graph > /sys/kernel/debug/tracing/current_tracer
modprobe uio_dma_proxy
: > /sys/kernel/debug/tracing/trace
cat /sys/class/uio/uio0/name &
echo 1 > /sys/kernel/debug/tracing/tracing_on
modprobe -r uio_dma_proxy &
sleep 1
echo 0 > /sys/kernel/debug/tracing/tracing_on
# Device tree
* What infomation was included in dtb
HW platform识别，runtime parameter传递以及硬件设备描述
for platform device init see arch/powerpc/platforms/52xx/media5200.c ppc_init
mpc52xx_declare_of_platform_devices
* How does the device tree subsystem read and parse the dts file?
#Why DDM?
The device model was originally intended to make power management tasks easier 
* I/o Architecture
1 Expansion hardware. 
Bus includes internal bus/system bus(PCI,ISA,SBus,I2C) and external bus(ieee1394/firewire,USB) and both(SCSI).
2 Communication with peripherals
I/O ports
I/O memory mapping
Interrupts
hrough the maintenance of a representation of the host system's hardware structure.
# linux 设备驱动模型
分为三层：
从结构上看，每一层都有对应的数据结构。
1 设备驱动层，驱动开发工程师，也就是码农们，设计的akashadevice  akashadriver 结构体。
2 驱动程序核心层 ，是一个通用层，内核中的标准结构，device device_driver bus_type.
3 Kobject核心层，也是一个通用层， kobject kobj_type kset 组成。
其实下面还有一个sys文件系统层。
从操作上看，也是分为三类，初始化 注册 注销
1 初始化，是局限于每一层的。
2 注册和注销就是贯穿三层的。
，我们知道其实总线就是铁轨，而各种设备就是北京东站，大连站。
我们人呢，就是铁轨上跑的数据。driver还用说就是火车了呗。
Bus总线这类设备比较特殊,需要内核对总线支持，即总线的核心层；
另一部分在外设的驱动程序中，负责把驱动挂接到总线，如pci_driver, usb_driver, i2c_driver。记住，这是说的总线的驱动。和挂接在总线上的设备驱动不同。
就好像open系统调用和驱动程序中file_operations中xxx_open的关系一样。
作为中间传输用途的总线设备的驱动是在内核当中，已经写好了，不用像各种总线上的设备一样，再写什么专门服务pci，i2c，spi等
总线设备的驱动，这里感谢下内核，又轻松了不少。pci之类的总线的，驱动就是match probe remove的函数，
不用专门封装成一个结构体，像我们遇到的block char设备，他们才是真中的外设。总之总线要单独看。
作为一个外设的驱动是很不容易除了要完成外设本身功能的部分还要处理好和总线的关系。对于pci总线上设备驱动就是由pci_driver来调和的。
kobject核心层的注册有点小技巧，他是先在作为参数kobj结构体中指定了，这个结构体属于哪里kset，之后在注册函数里，在kset里把kobj链入。
PS：设备注册和驱动注册是分开的。当设备注册后通过udev开始寻找驱动的过程。驱动是通过pci_device_id和相应设备建立关联的，不是major 和minor设备号。
可别高乱了。
# Sys 文件系统
bus，总线树跟踪每个总线上的每个设备。实质是保存驱动，设备在device中，用链接指明。
class, 类树按功能分类设备。
device，干货，其他的目录都指向这里。
# Kobject
清0，init，set name，get，put
在sys中是dir，attr 是文件
# kobj_type
release(),
# Kset
添加kobject到kset（指针，add）
# Bus, device, driver.
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
