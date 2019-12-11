---
tags: [ kernel ] 
title: An introduction to File system in Linux operating system
date: 2015-02-27T15:46:12+08:00 
category: cs
---

# rootfs 
init_mount_tree
and sys_chroot

# fsck
man systemd-fsck-root.service

# mkfs
e2fsprog

# block size
blockdev --getbsz /dev/sda1 
4096
sudo tune2fs -l /dev/sda1 | grep size

# LQO
如果是很多基本概念没有可以看看，因为就是个中规中矩的教科书，从历史到单机存储方方面面，如果写过store，就没必要了，可以关心rise lab 或者peloton lab他们的项目和publication，我曾经想弄存储，最后觉得最后都是玩电路了，感觉自己玩不下去，就放弃了。整体的工业设计可以找找emc的人写的书

# ACL
man chmod
## T vs t
只有owner 和root才能删除这个文件, 用于/tmp
touch /tmp/firo; ls -l /tmp/firo
-rw-rw-r-- 1 firo firo 0 Sep 24 21:20 /tmp/firo
chmod +t /tmp/firo; ls -l /tmp/firo 
-rw-rw-r-T 1 firo firo 0 Sep 24 21:20 /tmp/firo
chmod +x /tmp/firo; ls -l /tmp/firo 
-rwxrwxr-t 1 firo firo 0 Sep 24 21:20 /tmp/firo

# Linus on atime
perhaps the most stupid Unix design idea of all times," adding: "Think about this a bit: 'For every file that is read from the disk, lets do a ... write to the disk! And, for every file that is already cached and which we read from the cache ... do a write to the disk!'" He further emphasized the performance impact thus:
[Linux: Replacing atime With relatime](http://web.archive.org/web/20110427023154/http:/kerneltrap.org/node/14148)

## Linus on  Everything is a file
The whole point with "everything is a file" is not that you have some
random filename (indeed, sockets and pipes show that "file" and "filename"
have nothing to do with each other), but the fact that you can use common
tools to operate on different things. - [Linus](http://yarchive.net/comp/linux/everything_is_file.html)

## Hard Link
hard link to directory, recursive
hard link to file accross partitions, may led confilict with inode number.

# Sysfs
sysfs is strongly depend on driver module, just rmmod tg3 then /sys/class/net/enp9s0 went away!
[hierarchy of sysfs](https://lwn.net/Articles/604413/)
## 4.10 +
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

# Procfs
##How many parts does procfs has?
* process-specific read only /proc/<pid>/
* process tuneable interface. /proc/<pid>/
* kernel sysctl, importanta kernel tuneable files. /proc/sys/kernel net vm fs...
* Read only kernel infomation. /proc
##I think, important directory need to know.
/proc/sys/kernel
