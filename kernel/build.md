---
tags: kernel
title: build
date: 2015-02-27 15:46:12
category: kernel
---
#config
=if no .config, every config tools make a .config from scrach!

=oldconfig just for new moduels patch merged in trunk. This no any relations to .config.old and /boot/config.x.y.z

=make localmodconfig will reduce many unused kernel config.


#kernel image [Y]
make bzImage  #kmods will not build that configured with M! 

#modules [M]
 #In same dir!
 #cuse waw confiured with N, it was to build by make modules!. fuse with M that build by make modules!
make modules	#build kmoes with M!

make modules_install INSTALL_MOD_PATH=/home/firo/kmods


#build signle kernel module
make menuconfig
make oldconfig && make prepare
make -C $(pwd) M=/home/firo/linux/fs/ext3 modules V=1

#src-tree
include/linux: share with userspace
include/net: kernel stuff
