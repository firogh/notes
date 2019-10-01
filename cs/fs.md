---
tags: [ kernel ] 
title: An introduction to File system in Linux operating system
date: 2015-02-27T15:46:12+08:00 
category: cs
---

# ref
[Linux Filesystems in 21 days 45 minutes](https://www.samba.org/samba/samba/ftp/cifs-cvs/ols2006-fs-tutorial-smf.pdf)
Documentation/filesystems/vfs.txt
[Why does open before read/write?](https://unix.stackexchange.com/a/265680/16335)

# Init rootfs 
init_mount_tree
and sys_chroot

# ?? mkfs.ext2
e2fsprog

# VFS and Ops
## Principles
Layering
Abstraction
Object-oriented programming 
Performance: translation cache: decache
Onset/nuclus/coda: KISS
## tasks
mount
content operations: i_fop
dirent operations: i_op lookup, create, remove
## app
file descriptor
1. cache the translation from full path to inode
2. onset-operation cache; 
## vfs
* lookup interpreatation
decach layer dentry name interpretation cache 
inode-> lookup
* create
func: lookup_open
lookup -> inode-> create
* remove
unlink
* content operations - read/write
file: operation cache
## fs
inode layer

# FS and Ops
## Tasks
Alloc an inode
Remove an inode
Read the content of an inode
Write contents of an inode

## Principles
Layering
Performance: page cache
## Thinking over
Backdev info
FS format
inode content space index
## FS
Read the first block
buffer head:hard drive locations and sizes; memory addr
alloc an inode: linked to parent and mark parent dirty in memory, then write to disk
alloc spare space: record the locations of the spare space in inode
write the dirty space: user the buffer head record the locations 
* page cache 
address_space a_ops
page size vs block size
VMA: process map a whole page and use vma to descirbe it.
but if write overflow? the adjacent  buffer head will corrupt.
* block io
buffer head: unite 512

### FS block size
blockdev --getbsz /dev/sda1 
4096
sudo tune2fs -l /dev/sda1 | grep size

## Hard drive driver
Parameters: locations and size
Read content from disk
Write content form disk

# buffer head
__getblk_slow
grow_buffers
grow_dev_page
alloc_page_buffers

# FS format
fs-sb; fs-inode space; Spare space
The hierarchy of inode is mantained linked lists.

# inode_hashtable
ext2_iget
{
	inode = iget_locked(sb, ino);
	{
		head = inode_hashtable + hash(sb, ino)
		if !inode && alloc_inode(sb)
		then
			hlist_add_head(&inode->i_hash, head);
		fi
	}

}
# no hashtable
ramfs_get_inode
{
	new_inode->new_inode_pseudo-> alloc_inode
	still inode_sb_list_add
}

# dentry
d_alloc()
To get the full path call dentry_path_raw(filp->f_path.dentry,buf,buflen).

# struct path
get the meaning of this struct by looking d_path().It includes the two endpoints of a path which are mnt and dentry.
[Upcoming API change: struct path](https://lwn.net/Articles/206758/)

# Mount
Check mount.log

## LQO
mount->mnt_mountpoint  和 mount ->mnt_mp->m_dentry;

Check follow_managed
* for DCACHE_MOUNTED in get_mountpoint
Amnt: Protect the mountpoint hashtable with mount_lock - 3895dbf8985f656675b5bde610723a29cbce3fa7
* monutpoint
get rid of full-hash scan on detaching vfsmounts - 84d17192d2afd52aeba88c71ae4959a015f56a38
* d_mounted -> DCACHE_MOUNTED 
    The mounted state of a dentry is only used to speculatively take a look in the
    mount hash table if it is set -- before following the mount, vfsmount lock is
    taken and mount re-checked without races.
fs: dcache remove d_mounted - 5f57cbcc02cf18f6b22ef4066bb10afeb8f930ff
如果是很多基本概念没有可以看看，因为就是个中规中矩的教科书，从历史到单机存储方方面面，如果写过store，就没必要了，可以关心rise lab 或者peloton lab他们的项目和publication，我曾经想弄存储，最后觉得最后都是玩电路了，感觉自己玩不下去，就放弃了。整体的工业设计可以找找emc的人写的书

# Open
sys_open ->
    path_init()
    1. get start point current->fs{root, pwd}
    2. link_path_walk() to the parent dentry of last entry vim in /sbin/vim. deal with diff mounted fs(ext2 and windows ntfs)
    do_last()
    1. dentry,look_dcache(), if not in dcache, d_alloc(); d_op = sb->s_dentry_op
    2. file, if not open, file->f_op = inode->i_fop

## open flags
Check open_flags.c

# write
sys_write -> file->f_op->write()...

# file mode bits
man chmod
## T vs t
只有owner 和root才能删除这个文件, 用于/tmp
touch /tmp/firo; ls -l /tmp/firo
-rw-rw-r-- 1 firo firo 0 Sep 24 21:20 /tmp/firo
chmod +t /tmp/firo; ls -l /tmp/firo 
-rw-rw-r-T 1 firo firo 0 Sep 24 21:20 /tmp/firo
chmod +x /tmp/firo; ls -l /tmp/firo 
-rwxrwxr-t 1 firo firo 0 Sep 24 21:20 /tmp/firo
## ?? X

# Design a filesystem
[Design a file system](https://stackoverflow.com/questions/5376116/design-a-file-system)
[Why is Linux's filesystem designed as a single directory tree?][2]
[2]: https://unix.stackexchange.com/questions/93960/why-is-linuxs-filesystem-designed-as-a-single-directory-tree/
## Set vs directory
[What is the meaning/purpose of finding the “foundations of mathematics”?](https://math.stackexchange.com/a/854656/38596)
## Linus on atime
perhaps the most stupid Unix design idea of all times," adding: "Think about this a bit: 'For every file that is read from the disk, lets do a ... write to the disk! And, for every file that is already cached and which we read from the cache ... do a write to the disk!'" He further emphasized the performance impact thus:
[Linux: Replacing atime With relatime][1]
[1]: http://web.archive.org/web/20110427023154/http:/kerneltrap.org/node/14148

## Linus on  Everything is a file
The whole point with "everything is a file" is not that you have some
random filename (indeed, sockets and pipes show that "file" and "filename"
have nothing to do with each other), but the fact that you can use common
tools to operate on different things. - [Linus](http://yarchive.net/comp/linux/everything_is_file.html)

## Hard Link
hard link to directory, recursive
hard link to file accross partitions, may led confilict with inode number.

## LQO
[On Unix systems, why do we have to explicitly `open()` and `close()` files to be able to `read()` or `write()` them?][3]
[3]: https://unix.stackexchange.com/questions/265620/on-unix-systems-why-do-we-have-to-explicitly-open-and-close-files-to-be/265621
