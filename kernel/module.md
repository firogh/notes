---
---
tags: kernel
title: modules
date: 2015-02-27 15:26:35
category: kernel
---
layout: post
date: 2014-12-28
title: 
category: kernel
---

#pre-require
modules.alias
http://doc.opensuse.org/documentation/html/openSUSE_113/opensuse-reference/cha.udev.html

http://blog.chinaunix.net/uid-22954220-id-4380202.html
运行时, 插入u盘也是这样.

2. 内核自己加载比如缺少模块的时候 网络协议, fs
https://unix.stackexchange.com/questions/90027/what-is-the-sequence-loading-linux-kernel-module-on-startup-how-priority-is-set/90037#90037

3 rc 里面smartqos之类的.

##Load module into kernel
vmlinux.lds.h linker scipts include helper macros.
