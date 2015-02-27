---
tags: net
title: bonding
date: 2015-02-27 15:46:13
category: net
---
== Structure ==
Aggregating multiple network interfaces into a single logical "bonded" interface.
Bonded interface
Link monitoring

== Understand ==
Aggregating 
===========
network initilization scripts
----------------------------
sysconfig:suse
initscirpts:redhat
interfaces:debian

sysfs
-----

ifenslave
--------

Bonded interface
================
mode
----
balance-rr or 0
active-backup or 1
balance-xor or 2
broadcast or 3
802.3ad or 4

xmit_hash_policy
----------------
layer2 layer2+3 layer3+4

Link monitoring
===============
监视slave device
ARP monitoring
--------------
发送arp query
MII monitoring
------------
driver
mii register
ethtool

== Explore ==

Depth Exploration
=================

Lateral Exploration
===================
Vlan support
-----------

Vertical Exploration
====================


== Apply ==
======







