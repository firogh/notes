---
tags: [ howto ]
title: Use crash utility to examine the running system
date: 2018-9-24
category: howto
---
# Prepare
## Kernel pages info
[Overview of Kernel Packages](https://docs.fedoraproject.org/en-US/fedora/rawhide/system-administrators-guide/kernel-module-driver-configuration/Manually_Upgrading_the_Kernel/)

## Enbale debuginfo
/etc/yum.repos.d/fedora-updates.repo
/etc/yum.repos.d/fedora.repo
or 
dnf --enablerepo=debug install kernel-debuginfo

## locate vmlinux
rpm -ql kernel-debuginfo | grep vmlinux


# Start to use crash 
sudo crash /usr/lib/debug/lib/modules/4.18.9-200.fc28.x86_64/vmlinux

# p
p *(void**)0xffff887f7ee24bb0@2

## help
crash help list

## do some practices
crash> p file_systems
file_systems = $2 = (struct file_system_type *) 0xffffffffad2ab380
crash> list file_system_type.next -s file_system_type.name 0xffffffffad2ab380
ffffffffad2ab380
  name = 0xffffffffad11b3ce "sysfs"
ffffffffad212700
  name = 0xffffffffad0e37b9 "rootfs"
ffffffffad2b7460
  name = 0xffffffffad0a65af "ramfs"
ffffffffad2a2b40
  name = 0xffffffffad0e3e67 "bdev"
...
crash> struct file_system_type ffffffffad2a2b40
struct file_system_type {
  name = 0xffffffffad0e3e67 "bdev", 
  fs_flags = 0, 
  mount = 0xffffffffac2e4e60, 
  kill_sb = 0xffffffffac2a8fd0, 
  owner = 0x0, 
  next = 0xffffffffad2ab080, 
  fs_supers = {
    first = 0xffff9f9d5600a0e8
  }, 
  s_lock_key = {<No data fields>}, 
  s_umount_key = {<No data fields>}, 
  s_vfs_rename_key = {<No data fields>}, 
  s_writers_key = 0xffffffffad2a2b78, 
  i_lock_key = {<No data fields>}, 
  i_mutex_key = {<No data fields>}, 
  i_mutex_dir_key = {<No data fields>}
}
crash> mod -s e1000e
     MODULE       NAME                        SIZE  OBJECT FILE
ffffffffc0304700  e1000e                    282624  /usr/lib/debug/usr/lib/modules/4.18.9-200.fc28.x86_64/kernel/drivers/net/ethernet/intel/e1000e/e1000e.ko.debug 
crash> struct module ffffffffc0304700
struct module {
  state = MODULE_STATE_LIVE, 
  list = {
    next = 0xffffffffc02c4208, 
    prev = 0xffffffffc033f648
  },
...
