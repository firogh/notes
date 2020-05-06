---
tags: [ kernel ] 
title: An introduction to File system in Linux operating system
date: 2015-02-27T15:46:12+08:00 
category: cs
---


# References
[hellofs](https://github.com/accelazh/hellofs/blob/master/super.c)
[Linux Filesystems API summary](https://www.kernel.org/doc/html/latest/filesystems/api-summary.html)
[Linux File Systems in 21 days 45 minutes](https://download.samba.org/pub/samba/cifs-cvs/ols2007-fs-tutorial-smf.pdf)

# Buffer cache
getblk: ULK2 14.2.3 The getblk( ) Function

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

# rootfs 
init_rootfs
init_mount_tree and sys_chroot and rootfs_mount
do_basic_setup->rootfs_initcall(populate_rootfs);

# tmpfs
[关于 tmpfs](http://wangcong.org/2012/02/17/-e5-85-b3-e4-ba-8e-tmpfs/)
fs/ramfs,  tmpfs mm/shmem.c

# Sysfs

# Procfs
##How many parts does procfs has?
* process-specific read only /proc/<pid>/
* process tuneable interface. /proc/<pid>/
* kernel sysctl, importanta kernel tuneable files. /proc/sys/kernel net vm fs...
* Read only kernel infomation. /proc
##I think, important directory need to know.
/proc/sys/kernel

# btrfs 
btrfs_mount 
btrfs_fill_super
setup_bdi
