---
tags: cs
title: Programming Utility
date: 2015-02-27 15:46:14
category: cs
---
#git 
git send-email --smtp-encryption=tls --smtp-server=smtp.gmail.com --smtp-user=firogm@gmail.com --smtp-server-port=587 --to "kernel-janitors@vger.kernel.org" 0001-x86-Remove-unnecessary-void-pointer-casts.patch

git ls-files -d |xargs -i git checkout {}

git log -S "RTN_UNSPEC" --source --all
* reverse find all the blame from start commit id to now
    git blame --reverse 9e734775f7c22d2f89943ad6c745571f1930105f..   include/linux/netlink.h

* find kernel version of path
    git describe --contains
    git tag --contains <id>

* find commit id of a tag
    git rev-parse v2.6.12-rc2

##revert changes
=checkout all changes

#Makefile

##debug
=Just print echo 
make -s 

=Print shell command
make -n

=Print all variables. Wired-name variable is useful to debug
make -p

=Pirnt a message
$(warning ...)

=Etc
--warn-undefined-variables
