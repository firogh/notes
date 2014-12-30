
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
A host autoconfigure its interfaces in IP version 6.就是自动配IP这个很神.
Creation of Link-Local Addresses
fe80::interface id/10
Duplicate Address Detection
The procedure for detecting duplicate addresses uses Neighbor Solicitation and Advertisement messages
Creation of Global Addresses

Stateful Autoconfiguration mechanism
------------------------------------
DHCPv6


== Explore ==
== Depth exploration ==
== Lateral exploration ==
Packet size issue
-----------------
link layer must have an MTU of 1280 bytes  otherwise supports fragmention and reassembly.

Uper-layer checksum
-------------------
The transport layer or other upper layer used src and dst address to make up a pseudo-header
which in it's checksum compution.



== Vertical exploration ==





















