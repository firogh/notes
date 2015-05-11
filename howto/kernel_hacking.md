---
date: 2015-04-25
title: Linux kernel hacking 攻略 
category: howto
---

#git
[gittutorial - A tutorial introduction to Git](http://git-scm.com/docs/gittutorial)

#linux source code
[Working with linux-next](https://www.kernel.org/doc/man-pages/linux-next.html)
要add git://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git不是https.

我之前改了好多bug基于linus的分支, 提交后被告知别人已改. 后来Julia告诉我要基于linux-next开发.

我的linux next remote 就叫next, 基于其创建开发分支.

	git branch now next/master
	git pull next master:now

# kernel hacking tips
kernel patch
manuplation of page table
add new system_call
workqueue


# consideration of kernel hacking
## memory alloction
## The player -- the context of ruuning
## synchroniation

