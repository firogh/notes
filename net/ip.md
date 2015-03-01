---
layout: post
date: 2014-12-28
title: ipv4
category: net
---

#Reference
[What’s wrong with IPv4 and Why we are moving to IPv6](http://www.tecmint.com/ipv4-and-ipv6-comparison/)
#ipv4 address
* A类 0*******，
范围00000000~01111111 即0~127
 10.0.0.0~10.255.255.255
* B类 10******，
范围10000000~10111111 即128~191
 172.16.0.0~172.31.255.255
* c类 110***** 
范围11000000~11011111 即192~223
192.168.0.0~192.168.255.255
* D类
范围224~239用于组播
* E类
范围240~255 用于科学试验

#ip frag and assembly



#What is?
NAPT66
== Structure ==
IPv6 packet 
IPv6 address

== Understand ==
IPv6 packet
==========
Fixed header 
------------

Extension header
---------------
Hop-by-Hop Options Header:this option header is used to carry optional information that must be examined
by every node along a packet's delivery path.

Destination Options Header:it is used to carry optional information

Routing Header:the routing header  is used by an ipv6 source to list some intermidate nodes to be 
visted on the way to a packet's destination.

Fragment Header: the fragment header is used by an ipv6 source to send to a packet larger than would fit 
in the path MTU to it's destination

Authentication header 
Encapsulating Security Payload header
Upper-layer header

IPv6 address
============
Addressing model:
----------------
Unicast:Link-local fe80::interface id/10, ::, ::1, Global unicast address, IPv6 Addresses with Embedded IPv4 Addresses
Anycast:for dns or something else
Multicast

Text Representation
-------------------
The preferred form
Simplicity form: Groups of zeroes:: and leading zeros ommitted
Dotted-quad notation
Prefix for network address.

Stateless Address Autoconfiguration
-----------------------------------
