---
layout: post
date: 2015-01-03
title: Kernel debug method
category: kernel
---
#BUG
drivers/net/wireless/libertas/cfg.c:1115 lbs_associate() error: potential null dereference 'cmd'.  (kzalloc returns null)
drivers/net/wireless/rtlwifi/efuse.c:378 read_efuse() error: potential null dereference 'efuse_word'.  (kmalloc returns null)
drivers/staging/rtl8192e/rtllib_rx.c:1496 rtllib_rx() error: we previously assumed 'ieee' could be null (see line 1464)
vers/media/media-device.c:146 __media_device_enum_links() warn: check that 'pad' doesn't leak information (struct has a hole after 'index')
drivers/media/media-device.c:167 __media_device_enum_links() warn: check that 'link' doesn't leak information (struct has a hole after 'index')

##BUG type
oops

##BUG made by me
=print_signal_info wrong pritk parameters position
        printk(KERN_NOTICE "K %d : %d -> %s %d %s %d\n", sig, q->info.si_code,
                ss[2], ss[3], task_tgid_vnr(r_t), task_tgid_vnr(r_p));
* Watch compile warning info can be avoid of this bug.
kernel/signal.c: In function '__print_signal_info':
kernel/signal.c:1207:5: warning: format '%d' expects argument of type 'int', but argument 5 has type 'char *' [-Wformat]
kernel/signal.c:1207:5: warning: format '%s' expects argument of type 'char *', but argument 6 has type 'pid_t' [-Wformat]

=spin_lock(sighand) invoke down_sem and cond_resched...
	__send_signal()
{
	get_cmdline -> get_user_pages
}
* Carefully, investgate th invoke path.
* config CONFIG_DEBUG_ATOMIC_SLEEP on
* how to get from bug report to the source of bug

#Debug
##Debugging support in the kernel

##Debugging by Printing
Marco
ratelimit
console level
Print by qurey:proc ioctl

##Debugging by Watching
strace

##Debug tools and 
gdb /usr/src/linux/vmlinux /proc/kcore
objdump -s vmlinux
kdb
./scripts/decodecode < Oops.txt

##Skill
=Generating signl assembly source file!
make kernel/sched.s V=1

##If an page oops close to zero, for example 0xfffffff4
It maybe ERR_PTR(-12);

1. 踩内存

2. recursive stack,PC_MEMLOCK 

3. deadlock, __GFP_FS

4. NULL poniter reference, functions adn varibles

##ASM
move <-
j long jump
b short jump
a0, a1...99% is parameters.

##From ASM to c language
* expand inline function
* locate __asm__() 内嵌汇编, 能快速定位代码! 但很少! slhc_uncompress()
* 找常量!
* loop codes formate!
+ef8:   00a01021    move    v0,a1
 efc:   88440003    lwl a0,3(v0)
 f00:   24450004    addiu   a1,v0,4                                                                                                           
 f04:   98440000    lwr a0,0(v0)                                                                                                               
 f08:   00641821    addu    v1,v1,a0                                                                                                            
 f0c:   0064202b    sltu    a0,v1,a0                                                                                                             
+f10:   14a7fff9    bne a1,a3,ef8 <slhc_uncompress+0x444>                                                                                         
 f14:   00831821    addu    v1,a0,v1

