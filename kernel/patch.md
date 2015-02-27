---
tags: kernel
title: patch
date: 2015-02-27 15:46:12
category: kernel
---
static int crashlog_die_notify(struct notifier_block *self, unsigned long val, void *ptr)
{
   struct die_args *args;

   args = (struct die_args *)ptr;

   if (args) {
       printk(KERN_EMERG "DIE: val(%lu) str(%s) err(%ld) trapnr(%d) signr(%d)\n",
               val, args->str, args->err, args->trapnr, args->signr);
   } else {
       printk(KERN_EMERG "DIE: val(%lu)\n", val);
   }
   printk(KERN_EMERG "TASK: pid(%d) comm(%s)\n", current->pid, current->comm);

   return 0;
}

