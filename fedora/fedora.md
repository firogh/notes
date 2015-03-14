---
tags: fedora
title: nautilus
date: 2015-02-27 15:46:11
category: fedora
---

#Nautilus
* Sidebar
Edit below file to rename dir in place menu of sidebar.
vim ~/.config/user-dirs.dirs


#xdg 
* Specification
http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html


#Raspberry pi

* Remote viewer
vinagre

#fedora 22
* Check dup and error
yum check

* after distro-sync 22 yum/dnf still recognize 21
yum check must output fedor-relase/repos-21 something like, remove it.

* tty loop login
disable selinux
authconfig update all

* kernel parameter
audit=0 selinux=0  
rhgb  redaht graphical boot, show picture
quiet: show little message
init=/bin/bash

* boot stop at update UTMP
disable NetworkManager



