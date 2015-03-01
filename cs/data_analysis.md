---
tags: cs
title: data_analysis
date: 2015-02-27 15:46:14
category: cs
---

#The process of data analysis
=Data requirements

=Data collection

=Data processing

=Data cleaning

=Exploratory data analysis

=Modeling and algorithms

=Data product

=Communication


#analysis
=Requirements why pppoe reconnect?
* Normal disconnection
+ Case 1: Terminated by ISP
+ Case 2: Interrupted by customer or circumstance.
	-Unplug the network cable willn't influnce pppoe
	Power off restart, need kernel start time
+ Case 3: PPPoE persist
	PPPoE session is lost, but connection state still show established. Finnaly, restart by inetchk ifup
	singal 15
	#TODO: Fix inetchk wrong time to ifup wan.

* Abnormal disconnection
+ Case 4: System error
	Interrupted by inetchk

=Collection
* syslog
+ cmagent
+ pppd

=Processing

