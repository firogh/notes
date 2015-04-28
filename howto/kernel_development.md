---
date: 2015-04-25
title: Linux kernel 开发攻略 
category: howto
---

#git
[gittutorial - A tutorial introduction to Git](http://git-scm.com/docs/gittutorial)

#linux source code
[Working with linux-next](https://www.kernel.org/doc/man-pages/linux-next.html)
要add git://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git不是https.

我之前改了好多bug基于linus的分支, 提价后被告知别人已改. 后来Julia告诉我要基于linux-next开发.

我的linux next 就叫next, 创建上游最新的开发代码.
	git branch now next/master
	git pull next master:now

