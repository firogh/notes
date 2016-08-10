---
tags: [ kernel ] 
title: Kernel
date: 2015-02-27T15:46:12+08:00 
category: cs
---


# Data structures
* u32 __u32
__u32 is used for user-space. declare a variabe used by icotl. qosmark.
u32 is used for kernel.
deatils in ldd3e chapter 10

# Panic
kernel/kernel/panic.c

##disk 
subsys_initcall 4 genhd_device_init with base_probe{ request_module()}
module_init 6 ->init_sd->sync_schedule_domain(sd_probe_async

##cmdline
root= name_to_dev_t, mount_root in prepare_namespace
如果/init不能 sys_access, 则prepare_namespace,切换到真正的root=指定的设备上设备在sd_probe上初始化了.
systemd负责挂在文件系统, 切换.

# The principle of kernel & driver backport
将高本版kernel的feature移植到低版本的kernel的过程就是backport.
1. 尽量保持与mainline的代码一致
2. 做好取舍不要引入太多patch. 
## 开发流程
理清所引入feature的代码. 主要数据结构以及功能流程.

# ARM
## smp_ops
Machine: Marvell Armada XP Development Board
MACHINE_START(_type,_name)
__section__(".arch.info.init")
arch/arm/tools/mach-types
armada_xp_db            MACH_ARMADA_XP_DB       ARMADA_XP_DB            3036
### bootargs
debug initcall_debug
### printk
# old questions
What is platform driver?
find source code of config
How to print backtrace by kernel oops, and why it's not exact?
What is abi
why get_unaligned_be16?
 _THIS_IP_ vs __FUNCTIONS__

# kernel modules
sudo depmod -n -a -F /boot/System.map-4.3.5-300.fc23.x86_64  4.3.5-300.fc23.x86_64 > /tmp/m.log

# kernel patches
The cxgb3_*_send() functions return NET_XMIT_ values, which are
positive integers values. So don't treat positive return values
as an error.
67f1aee6f45059fd6b0f5b0ecb2c97ad0451f6b3
