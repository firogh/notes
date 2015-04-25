---
date: 2015-04-25
title: 使用mutt收发邮件
category: howto
---

#Reference

昨天用web Gmail给社区的maillist回信, 被退信了. 提示Gmail发送的邮件包含html.
这才想起来社区的mailist只接收:
	No MIME, no links, no compression, no attachments.  Just plain text
我之前发给社区的邮件是用git sendmail发给maillist, 不会有这个问题. 
有点不方便, 今天决定使用mutt.

需要mutt, fetchmail, procmail, msmtp, proxychains(天朝专用), vim/emacs
发一个邮件, 需要这么多程序啊, 以前一直以为一个mutt就够了, 还Gmail方便啊.

mutt的作用类似于胶水, 把其他程序联系起来.
fetchmial收, procmail过滤spam mail, msmtp 用来发送, proxychains 翻x墙,
vim/emacs用来编辑邮件.

具体的配置看wangcong的[使用mutt处理电子邮件](http://wangcong.org/2007/03/09/-e4-bd-bf-e7-94-a8mutt-e5-a4-84-e7-90-86-e7-94-b5-e5-ad-90-e9-82-ae-e4-bb-b6-2/)


* 国内gmail用户要用proxychains 做下代理, 这样就能使用gmail了.
	.muttrc
	set sendmail="proxychains /usr/bin/msmtp"

* wangcong的blog中的配置不太符合习惯:
	set indent_str="<"
今天发邮件的时候, 被Julia指出:
	I have never seen text quoted with <.  Is it an accident?
所以按着习惯和美感应该是
	set indent_str=">"

