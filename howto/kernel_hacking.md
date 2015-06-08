---
date: 2015-04-25
title: Linux kernel hacking 攻略 
category: howto
---


#git
[gittutorial - A tutorial introduction to Git](http://git-scm.com/docs/gittutorial)
# linux source code
[Working with linux-next](https://www.kernel.org/doc/man-pages/linux-next.html)
要add git://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git不是https.
我之前改了好多bug基于linus的分支, 提交后被告知别人已改. 后来Julia告诉我要基于linux-next开发.
我的linux next remote 就叫next, 基于其创建开发分支.

	git branch now next/master
	git pull next master:now
# Fix kernel mistakes
内核至今如此优秀就是因为, 成千上万前赴后继的开发者笔耕不缀的结果.
别当豆包不当干粮, 虽然, 你不能设计出很牛的算法, or 什么子系统,
但是内核还有很多问题有待解决, 正所谓老虎也要打, 苍蝇也要拍; 千里之堤毁于蚁穴.
我提过好些这种patch Dan Carpenter都告诉我Linus已经改了.
## kernel newbie上有个list
[KernelJanitors/Todo](http://kernelnewbies.org/KernelJanitors/Todo)
* Style fix
如:
remove_wait_queue(entry->wait_address,&entry->wait);
remove_wait_queue(entry->wait_address, &entry->wait);
## Smatch
smatch这个工具是Dan写的主要就是为了找到内核的小问题, 基本用法:

        make CHECK="~/path/to/smatch/smatch -p=kernel" C=1 \
                bzImage modules | tee warns.txt
smatch 会产生好多可疑的问题, 细心寻找吧.
你找到了, 那么就是修改了.
记住在smatch之前一点要git pull next master:now一下, 保证checkout到了now的分支.
## Coccinelle
Coccinelle是 Julia Lawall 写的静态检测工具.very nice.
自己研究吧
Documentation/coccinelle.txt
http://pagesperso-systeme.lip6.fr/Julia.Lawall/tutorial.pdf
## 生成patch
先修改.
之后git add
在commit 之前你要 git log --online path/to/modification/file
看下提交的titile:

	git log  --oneline drivers/base/firmware_class.c
	5455c8c firmware: Fix memory leak in error path
	e0fd9b1 firmware: use const for remaining firmware names
	f9692b2 firmware: fix possible use after free on name on a
主义冒号:后一定要有一个空格!
你会还是看下Document下的submitting的文档.
之后commit:
先是类似上面的一行oneline 简要说明
空一行.
之后具体描述下.
commit后, 生成patch
如果你只提交了一次:

	git format-patch HEAD^..HEAD
这个命令就ok了.
如果commit多次, 自己斟酌两次diff的commit id了, HEAD^ 和HEAD都是commit id.
这样就生成了patch, 一般叫做0001-xxx-ooo.patch之类的
如我这个, 已被接收:

	0001-firmware-Fix-memory-leak-in-error-path.patch
commit id 是5455c8c3284a63e2673d1be7f040fb245cbf9be9

## 发送patch
天朝用户自己打梯子吧proxychains shadowsocks.
基本步骤是
先编译一下:
make path/to/modification/file.o
之后
./scripts/checkpatch.pl
之后 get maintainer
./scripts/get_maintainer.pl

发送的时候, 发给维护者 --cc其他人, 还有cc 一个mailist, 如果没有合适的list的话,
就cc到linux-kernel@vger.kernel.org, 这是个开放的list, 有合适的list了, 就不要cc它了.

	proxychains git send-email --to ming.lei@canonical.com --cc gregkh@linuxfoundation.org --cc kernel-janitors@vger.kernel.org 0001-firmware-Fix-memory-leak-in-error-path.patch
免不了你要返工重新修改.这时候, 新生成的patch这样:

	git format-patch --subject-prefix="PATCH v2" HEAD^..HEAD
改几次就v几.
和社区交流的时候, 要注意礼貌, 而且要感谢别人对你的patch做出的建议,
没有人的时间是被猪拱来的.

这些都是比较简单(代码量上)的patch, 要想提交深度的还需要对某方面的深度.
基本上这就完了, 你的真正的patch就给社区了.
# 进阶decent
这个是昨天晚上改drivers代码时候, 看到TODO的注释猛然想到的.

	grep -nr 'FIXME' --include="*.c"  ./ | tee fixmek.log
	grep -nr 'TODO' --include="*.c" ./ | tee todok.log
	wc -l fixmek.log 
有6000多个.
<iframe frameborder="no" border="0" marginwidth="0" marginheight="0" width=330 height=86 src="http://music.163.com/outchain/player?type=2&id=32063363&auto=1&height=66"></iframe>
# Deeply involved
http://vger.kernel.org/~davem/net_todo.html
这个列表上的基本都过期了....哎
往深了走不是难事, 主要是意识到内核, 不是什么神秘的东西, 选好一个方向
简单看看概念见我的<如何学习> 就可以实践.
必须要意识到, 动手比看再多概念管用, 我就是之前看了太多, 还发展出一个哲学
体系出来:-) 确实随着现代信息科学给社会带来的巨大变化, 哲学也必须要更新.
才能更好的服务于人.
这几天就在, 找内核哪里还不完善, 自己能补上, 今天6号了.
#patch formate advices
* Julia Lawall Sorry to be picky, 
but normally people put a space after the colon.  Also,
the subject line could be shorter: Remove unneeded cast.  
The description part of the subject doesnt have to be unique, 
just the whole thing, asfter the [PATCH] part.
* Dan Carpenter
Otherwise your patch was fine, btw.  Other improvements.
Don't put "Drivers:" in the subject.
On Wed, Apr 22, 2015 at 09:10:50PM +0800, Firo Yang wrote:
> From: Firo Yang <firogm@gmail.com>
Don't include this line.  We can get it from you email address.

Include everyone from the ./scripts/get_maintainer.pl output except
don't include linux-kernel@vger.kernel.org if there is another mailing
list there already.
* To find  patch prefix

	git log --oneline  path/to/file.c
* 更新patch 要加v几

	git format-patch --subject-prefix="PATCH v2" xxx..ooo
* 提交多个patch 要手动生成一个[PATCH 0/N], 这个0就是要写简要描述的.

	proxychains git send-email --subject "[PATCH v2 0/15] Remove unneeded casts of memory-alloc function return values" --thread --compose --confirm=compose --to firogm@gmail.com *.patch
* Dave Miller偏好
@@ -325,13 +325,15 @@ static inline void empty_child_dec(struct key_vector *n)
  static struct key_vector *leaf_new(t_key key, struct fib_alias *fa)
  {
-       struct tnode *kv = kmem_cache_alloc(trie_leaf_kmem, GFP_KERNEL);
-       struct key_vector *l = kv->kv;
+       struct tnode *kv;
+       struct key_vector *l;
Dave Miller usually prefers it if variables are ordered from longest to shortest.  
So you should probably have l defined first, and then kv.
