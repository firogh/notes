---
tags: [ kernel ] 
title: An introduction to File system in Linux operating system
date: 2015-02-27T15:46:12+08:00 
category: cs
---

# ref
[Linux Filesystems in 21 days 45 minutes](https://www.samba.org/samba/samba/ftp/cifs-cvs/ols2006-fs-tutorial-smf.pdf)
Documentation/filesystems/vfs.txt

# Init rootfs 
init_mount_tree
and sys_chroot

# ?? mkfs.ext2
e2fsprog

# inode
## inode_hashtable
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
## no hashtable
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

# mount
do_new_mount
{
	vfs_kern_mount
	{
		root = mount_fs -> ramfs_mount -> ramfs_fill_super
		{
			ramfs_get_inode
			sb->s_root = d_make_root(inode);
		}
		mnt->mnt.mnt_root = root;
	}
	do_add_mount
	{
		lock_mount
		{
			mnt = lookup_mnt(path);
			retry until !mnt, then create new mp
		}
		graft_tree->attach_recursive_mnt
		{
			get_mountpoint->d_set_mounted
			mnt_set_mountpoint
			commit_tree -> child mnt.mnt_hash -> mount_hashtable
		}

	}
}
## LQO
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

# open
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
