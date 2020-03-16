---
tags: [ kernel ] 
date: 2015-04-25
title: Yet another guide on the way to linux kernel hacking
category: cs
---

# Hacking goals
## Trade time for space
search_exception_tables
# define _ASM_EXTABLE_HANDLE(from, to, handler)                 \
        .pushsection "__ex_table","a" ;                         \
        .balign 4 ;                                             \
        .long (from) - . ;                                      \
        .long (to) - . ;                                        \
        .long (handler) - . ;                                   \
        .popsection

# Hacking area
mm
https://kernsec.org/wiki/index.php/Kernel_Self_Protection_Project
[Variable-length arrays and the max() mess](https://lwn.net/Articles/749064/)
[Sub-system Update: Kernel Self-Protection Project - Kees Cook, Google](https://www.youtube.com/watch?v=wlqjQe3vDx8&list=PLbzoR-pLrL6rOT6m50HdJFYUHyvA9lurI&index=11&t=0s)
## Kernel refactoring
https://linuxtv.org/wiki/index.php/Development:_Hints_for_Refactoring_Existing_Drivers
[Kernel Recipes 2017 - Refactoring the Linux Kernel - Thomas Gleixner](https://www.youtube.com/watch?v=mxxicJZ8cis)
## Feature
some results or progress information for PFRA
## Write your own kernel
[Writing an OS in Rust](http://os.phil-opp.com/)
[BrokenThorn Entertainment Operating System Development Series](http://www.brokenthorn.com/Resources/OSDevIndex.html)

# Build
* if no .config, every config tools make a .config from scrach!
* oldconfig just for new moduels patch merged in trunk. This no any relations to .config.old and /boot/config.x.y.z
* make localmodconfig will reduce many unused kernel config.
* make bzImage  #kmods will not build that configured with M! 
* make modules_install INSTALL_MOD_PATH=/home/firo/kmods
##build signle kernel module
make menuconfig
make oldconfig && make prepare
make -C $(pwd) M=/home/firo/linux/fs/ext3 modules V=1 
make ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnu- drivers/pcmcia/sa11xx_base.o

# Git repos
[Git tree maintenance](https://lwn.net/Articles/572068/)
## next tree - 2.6.1x ~ latest tree; please rebase your patch against this tree before send it to upstream
[Working with linux-next](https://www.kernel.org/doc/man-pages/linux-next.html)
git add git://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git
## tglx tree - 2.4 - 2.6.1x
origin	git://git.kernel.org/pub/scm/linux/kernel/git/tglx/history.git (fetch)
origin	git://git.kernel.org/pub/scm/linux/kernel/git/tglx/history.git (push)
## history tree - 0 - 2.4
origin	https://git.kernel.org/pub/scm/linux/kernel/git/history/history.git/ (fetch)
origin	https://git.kernel.org/pub/scm/linux/kernel/git/history/history.git/ (push)
git checkout -b 240p 2.4.0-prerelease

# Blogs
http://www.joelfernandes.org/linuxinternals/
https://paulmck.livejournal.com/
## Traning
[Unreliable Guide To Hacking The Linux Kernel](https://www.kernel.org/doc/htmldocs/kernel-hacking/index.html)
[Bootlin linux kernel labs](https://linux-kernel-labs.github.io/master/index.html)

# Mailing list archives
[The Linux Kernel Archives](https://www.kernel.org/lore.html)
Online: https://lore.kernel.org/lists.html
Git: https://git.kernel.org/pub/scm/public-inbox/
[LKML archives on lore.kernel.org](https://lwn.net/Articles/758034/)
and marc.info

# Source code navigator
Just make tags; make cscope
['Source code reading' related sites](https://www.gnu.org/software/global/links.html)

# KVM
[Building a KVM host machine.](https://blacks3pt3mb3r.wordpress.com/linux-stuffz/264-2/)
[Speeding up kernel development with QEMU](https://lwn.net/Articles/660404/)
[How to Build A Custom Linux Kernel For Qemu](http://mgalgs.github.io/2015/05/16/how-to-build-a-custom-linux-kernel-for-qemu-2015-edition.html)
[Rapid kernel development with dracut and Qemu](https://morbidrsa.github.io/2017/04/19/rapid-kernel-development-with-dracut-and-qemu.html)
## SUSE
zypper --root /home/firo/ws/suse addrepo http://download.opensuse.org/distribution/leap/15.0/repo/oss/ foss
https://en.opensuse.org/Package_repositories
zypper --root /home/firo/ws/suse install kernel-default-devel

## Build minimal bootable rootfs - fedora
./etc/yum.repos.d/
./etc/yum.repos.d/fedora-updates-testing.repo
./etc/yum.repos.d/fedora.repo
./etc/yum.repos.d/fedora-updates.repo
./etc/yum.repos.d/fedora-cisco-openh264.repo
can be gotten by supermin --prepare bash -o /tmp/supermin.d
or just copy from you host to some place like /home/firo/kernel/k/testfs/
sudo dnf --releasever=27 --installroot=/home/firo/kernel/k/testfs/ --setopt=reposdir=/home/firo/kernel/k/testfs/etc/yum.repos.d install dnf udev passwd
## How to build a minimal kernel for testing?
[tiny config @ kernel.org][5]
[3 attempts to reduce the configurations][6]
[Fedora equivalent of debootstrap](7)
Then enable following config option
CONFIG_CHR_DEV_SG
Some ftrace stuff
CONFIG_SLUB
CONFIG_KASAN
## [Kernel.org: Build a tiny kernel](https://tiny.wiki.kernel.org/)
## make initrd for NFS
[Upgrade kernel and initrd in Linux](http://migueleonardortiz.com.ar/linux/upgrade-kernel-and-initrd-in-linux/2067)
sudo chroot suse mkinitrd -m 'nfs nfsv3 nfsv4 iwlwifi' -A -D wlp4s0

## KVM and NFS
qemu-system-x86_64 -nographic -enable-kvm  -kernel ./bzImag  -append ' console=ttyS0 ip=dhcp root=/dev/nfs nfsroot=192.168.0.104:/home/firo/kernel/k/testfs,nfsvers=3,tcp rw nfsrootdebug debug  raid=noautodetect selinux=0 enforcing=0 '
dnf install nfs-utils
cat /etc/exports # For more details, man exports
/home/firo/kernel/k/testfs 127.0.0.1(rw,sync,fsid=0,no_root_squash)
systemctl start nfs-server.service
systemctl status nfs-server.service 
● nfs-server.service - NFS server and services
   Loaded: loaded (/usr/lib/systemd/system/nfs-server.service; disabled; vendor preset: disabled)
   Active: active (exited) since Sat 2018-03-17 17:52:29 CST; 4s ago
### Test the nfs
sudo mount -t nfs localhost://home/firo/kernel/k/testfs /mnt
if touch prermission denied, add no_all_squash to/etc/exports
## NFS errors
[   54.600121] NFS: sending MNT request for 10.0.2.2:/buildarea1/firo/ima/export/dist
[   54.600121] NFS: failed to create MNT RPC client, status=-101
[   54.600121] NFS: unable to mount server 10.0.2.2, error -101
CONFIG_E100 CONFIG_E1000 ...and IP_PNP and DHCP BOOTP RARP
try: nfsvers=3,tcp and 192.168.0.104 or 10.0.2.2 are mandatory!
## NFS vers
rpcinfo -t localhost nfs 
program 100003 version 3 ready and waiting
program 100003 version 4 ready and waiting
rpcinfo -p | grep nfs
    100003    3   tcp   2049  nfs
    100003    4   tcp   2049  nfs
    100227    3   tcp   2049  nfs_acl

# Submitting patch
## Patch prefix
	git log --oneline  path/to/file.c
## Resending the patch
[First revision, version, second revision](https://meta.stackexchange.com/questions/314212/why-is-it-called-initial-revision-if-its-not-a-revision)
[Versioning one patch revision](https://kernelnewbies.org/Outreachyfirstpatch)
[For example, if you're sending the second revision of a patch, you should use [PATCH v2]](https://kernelnewbies.org/PatchTipsAndTricks)
[use PATCHv2 (or PATCHv3 and so on) in the subject lines instead of PATCH ... To update the subject lines, add the -v 2 (or -v 3, etc) options to git format-patch](https://kernelnewbies.org/PatchPhilosophy)
[Finally, to send your new patch series as a reply to the previous one, first look up the Message-Id of the cover letter (or the one-and-only patch) in your previous patch series, and then pass that to the --in-reply-to= option of either git format-patch or git send-email.](https://kernelnewbies.org/PatchPhilosophy)
[Patch v2, v3, ... Changes, tags](https://kernelnewbies.org/PatchTipsAndTricks)
## Patch in series with a cover
	proxychains git send-email --subject "[PATCH v2 0/15] Remove unneeded casts of memory-alloc function return values" --thread --compose --confirm=compose --to firogm@gmail.com *.patch
## Dave S Miller's perferences
@@ -325,13 +325,15 @@ static inline void empty_child_dec(struct key_vector *n)
  static struct key_vector *leaf_new(t_key key, struct fib_alias *fa)
  {
-       struct tnode *kv = kmem_cache_alloc(trie_leaf_kmem, GFP_KERNEL);
-       struct key_vector *l = kv->kv;
+       struct tnode *kv;
+       struct key_vector *l;
Dave Miller usually prefers it if variables are ordered from longest to shortest.  So you should probably have l defined first, and then kv.
## Julia Lawall Sorry to be picky, 
but normally people put a space after the colon.  Also,
the subject line could be shorter: Remove unneeded cast.  
The description part of the subject doesnt have to be unique, 
just the whole thing, asfter the [PATCH] part.
## Dan Carpenter
Otherwise your patch was fine, btw.  Other improvements.
Don't put "Drivers:" in the subject.
On Wed, Apr 22, 2015 at 09:10:50PM +0800, Firo Yang wrote:
> From: Firo Yang <firogm@gmail.com>
Don't include this line.  We can get it from you email address.
Include everyone from the ./scripts/get_maintainer.pl output except
don't include linux-kernel@vger.kernel.org if there is another mailing
list there already.
## Kubecek
│17:31:57 mkubecek | For the record, once the commit is in net or net-next tree, there is no chance to tweak its commit message. │ vtsironis_ho
│17:32:26   alesak | mkubecek: ok, thanks for the clarification                                                                  │ wpreston
│17:33:19 mkubecek | These trees do not rebase and some people (like me) would appreciate if other subsystem trees didn't        │ Zara
│                  | either.                                                                                                     │ zuzka
│17:34:20 mkubecek | We could avoid spurious git_sort failures and commits like kernel-source 174731527683 

# git mutt patch
[Greg: How to Apply a Patch to the Linux Kernel Stable Tree](https://www.youtube.com/watch?v=6zUVS4kJtrA)
## newbies! what to do?
[Peter Zijlstra: From DOS to kernel hacking](https://lwn.net/Articles/286244/)
[KernelJanitors/Todo](http://kernelnewbies.org/KernelJanitors/Todo)
[linux-wanking@vger.kernel.org](https://lwn.net/Articles/284099/)
## Smatch
        make CHECK="~/path/to/smatch/smatch -p=kernel" C=1 \
                bzImage modules | tee warns.txt
## Coccinelle
