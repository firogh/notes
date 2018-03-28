---
tags: [ kernel ] 
title: An introduction to File system in Linux operating system
date: 2015-02-27T15:46:12+08:00 
category: cs
---

# Reference
[VFS notes](http://www.fieldses.org/~bfields/kernel/vfs.txt)
[Linux: Replacing atime With relatime][1]
[Why is Linux's filesystem designed as a single directory tree?][2]
[Design a file system](https://stackoverflow.com/questions/5376116/design-a-file-system)

[0]: http://yarchive.net/comp/linux/everything_is_file.html
[1]: http://web.archive.org/web/20110427023154/http:/kerneltrap.org/node/14148
[2]: https://unix.stackexchange.com/questions/93960/why-is-linuxs-filesystem-designed-as-a-single-directory-tree/
[3]: https://unix.stackexchange.com/questions/265620/on-unix-systems-why-do-we-have-to-explicitly-open-and-close-files-to-be/265621

# Everything is a file
The whole point with "everything is a file" is not that you have some
random filename (indeed, sockets and pipes show that "file" and "filename"
have nothing to do with each other), but the fact that you can use common
tools to operate on different things. - [Linus][0]


# File and directory
[What is the meaning/purpose of finding the “foundations of mathematics”?](https://math.stackexchange.com/a/854656/38596)

# File operations - onset, nucleus, coda
[On Unix systems, why do we have to explicitly `open()` and `close()` files to be able to `read()` or `write()` them?][3]
## onset - the preparing open()
Prepare the object for processing.
* Name to inode.

* make dentry
d_alloc() alloc new dentry
parent_entry->d_inode->i_op->lookup=ext2_lookup()-> ext2_iget() -> use raw_inode make inode->
set inode to dentry

#Hacks
I suddenly find that a good way to understand fs in kernel is to manupulate a small and complete fs, like ramfs or tmpfs.

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
## Init Rootfs 
init_mount_tree
and sys_chroot

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

# FAQ
## forbid ln
hard link to directory, recursive
hard link to file accross partitions, may led confilict with inode number.

## stick bit
chmod +t
1777
只有owner 和root才能删除这个文件, 用于/tmp

## time of file
### atime
perhaps the most stupid Unix design idea of all times," adding: "[T]hink about this a bit: 'For every file that is read from the disk, lets do a ... write to the disk! And, for every file that is already cached and which we read from the cache ... do a write to the disk!'" He further emphasized the performance impact thus:
