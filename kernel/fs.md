---
tags: kernel
title: filesystem
date: 2015-02-27 15:46:12
category: kernel
---

#VFS
##Common concepts
* VFS:Common fs interface plus fs anstraction layer! 
## Unix fs-related abstractions: file, directory entries, inodes, and mount points.
* Filesytem: a hierarchial storage of data adhering to a specific structure.
In unix, fs are mounted at a specific mount point in global hierarchy known as a namespace.
* file: and ordered string of bytes.
* directory: analogous to a folder and usally contains related files.
* dentry: Each component of a path is called a directory entry.
* inode: Unix separate the concept of a file from any associated infomation(file metadata) inode!
* supreblock: a data structure containing information about the filesystem as a whole.

## VFS primary objects: superblock, inode, dentry, file.
* supreblock: a specific mounted filesystem.
* inode: unit for fs. all the information needed by the kernel to manipulate a file or directory. 
* dentry: accelerate access fs, dcache! a single component of a path, certainly include *regular* file.
VFS treats directory as non-exist *normal* file. directory and dentry is not the same at all, different congitive level!
In VFS, file, dentry, and inode all represents dir and file and others, in others words struct file can stand for a dir!
* file: an open normal file as associated with a process.

##Step of how to use ext2 fs
* Register a fs type in kernel.
struct file_system_type: basic fs driver codes!

* mkfs.ext2
* shell mount -> sys_mount -> file_system_type->mount => root dentry and dentry->d_sb!
* sys_open -> 
    path_init()
    1. get start point current->fs{root, pwd}
    2. link_path_walk() to the parent dentry of last entry vim in /sbin/vim. deal with diff mounted fs(ext2 and windows ntfs)
    do_last()
    1. dentry,look_dcache(), if not in dcache, d_alloc(); d_op = sb->s_dentry_op
    2. file, if not open, file->f_op = inode->i_fop
* sys_write ->
    file->f_op->write()...

##How make dentry
d_alloc() alloc new dentry
parent_entry->d_inode->i_op->lookup=ext2_lookup()-> ext2_iget() -> use raw_inode make inode->
set inode to dentry

#Procfs
##How many parts does procfs has?
* process-specific read only /proc/<pid>/
* process tuneable interface. /proc/<pid>/
* kernel sysctl, importanta kernel tuneable files. /proc/sys/kernel net vm fs...
* Read only kernel infomation. /proc
##I think, important directory need to know.
/proc/sys/kernel
