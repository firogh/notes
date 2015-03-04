---
tags: net
title: tcp
date: 2015-02-27 15:46:14
category: net
---

== Structure ==
TCP packet
Connection
Data receiver and transfer

== Understand ==
TCP packet
===========
TCP header
----------

TCP options
----------


== Apply ==
Create TCP options
==================
tcp_syn_build_options()

TCP input
=========

Receive ack
-----------
tcp_ack()
记录ack的数据大小mss or tcp abc
update snd_wl1 and snd_una
slow path update mtu mss tcp_skb_cb.sacked



Tcp output
==========
Active send data
----------------
tcp_sendpage()/tcp_sendmsg()->tcp_write_xmit()/tcp_push_one()->tcp_transmit_skb

Timer expiring retransmiter
--------------
tcp_retransmiter_timer()...->tcp_transmit_skb()

reponse for receiving an ACK
----------------------------
tcp_data_snd_check()->tcp_write_xmit()


