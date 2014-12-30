ICMPv4
====

Structure
=========
协议本身，是一种没有特定实体的信息。 如果一味寻找其实体，是很可笑的做法。
这不代表协议实体不存在，只是不容易察觉，必须放到特定的环境下。
Beginning	icmp_init

Function
========
Transfer	host/router-ip	ip header + icmp header + data			peer/router-ip
				receive:strut icmp_control icmp_pointers[]
				send:	icmp_send()			


ICMPv6
======

== Sturcture ==
IPv6 use ICMPv6 to report errors encountered in processing packet then process
Perform other internet-layer functions, e.g ping
ICMPv6 packet


== Understand ==
ICMPv6 packet
=============
General format: type code checksum 
--------------
Error messages
--------------
Destination unreachable 
Packet Too Big 
Time Exceeded 
Parameter Problem 

Informational Messages
---------------------
Echo Request Message
Echo Reply Message



== Depth exploration ==

== Lateral exploration ==
Determinate a source address
--------------------------
Compute checksum with pesudo ipv6 header
----------------------------------------

== Vertical exploration ==


== Apply ==

