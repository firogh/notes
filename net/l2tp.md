---
layout: post
date: 2014-12-28
title:  L2tp in linux kernel
category: kernel
---

#common
IP -> UDP -> L2TP -> PPP

#source files
net/l2tp/
├── Kconfig
├── l2tp_core.c
├── l2tp_core.h
├── l2tp_debugfs.c
├── l2tp_eth.c
├── l2tp_ip.c
├── l2tp_netlink.c
├── l2tp_ppp.c
└── Makefile

include/uapi/linux/ppp_defs.h

#Functions
l2tp_recv_common ...recv_skb=pppol2tp_recv()
l2tp_xmit_core



