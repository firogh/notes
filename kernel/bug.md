---
tags: kernel
title: bug
date: 2015-02-27 15:46:12
category: kernel
---

This note is used for keep the linux kernel bug found by myself.
The formation of this note is below.
NO. + found time
bug file + location
descriptions

0 3/9/2012
mm/mmap.c arch_get_unmapped_area 
Invoke tow timesfunction find_vma for unchanged variable vma.It is not bug!!!Just for interesting

1 9/15/2012
pci_root_buses 



drivers/net/wireless/libertas/cfg.c:1115 lbs_associate() error: potential null dereference 'cmd'.  (kzalloc returns null)
drivers/net/wireless/rtlwifi/efuse.c:378 read_efuse() error: potential null dereference 'efuse_word'.  (kmalloc returns null)
drivers/staging/rtl8192e/rtllib_rx.c:1496 rtllib_rx() error: we previously assumed 'ieee' could be null (see line 1464)
vers/media/media-device.c:146 __media_device_enum_links() warn: check that 'pad' doesn't leak information (struct has a hole after 'index')
drivers/media/media-device.c:167 __media_device_enum_links() warn: check that 'link' doesn't leak information (struct has a hole after 'index')


#BUG type
oops

#BUG made by me
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


