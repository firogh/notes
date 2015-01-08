vlan
====
net/8021q/


== Structure ==
Vlans are a way to split up a layer2 broadcasting domain, VLANs allow you to create multiple separated networks with only a single switch.
In a vlan-capable network there are 2 types of connections : "access" connections and "trunk" connections
Vlan packet

== Understand ==
Vlan packet
===========
Preamble 56 alternating bits | SFD10101011 | dst mac | src mac | TPID 0x8100 | TCI:PCP DEI VID| Ether type 0x86DD ipv6 ...|CRC FCS
802.1Q/Vlan header
----------
TPID is the same as 0x86DD just a Ether type 0x8100  | PCP 3bis  DEI 1bit VID 12bis

Access connection
=================
An access connection looks like a normal connection to an ethernet switch, 
only that switch will only forward your packets within the same vlan, so they will not be able to reach ports that are in a different vlan.
For access ports, the switch will add (or overwrite) this tag value on any incoming(it means transfer out of host) packet before forwarding

Trunk connnection
================
"Trunk" ports can communicate with multiple vlans, but you need to send special packets that contain 
both the packet and an indication in what vlan they are to be forwarded.
For trunk ports, the value is supposed to be present. If it is not, the value of the "native vlan" will be added.

Split up a layer2 broadcasting domain
=====================================
vlan 什么都别想, 你总的有个vlan吧. 对上来就创建一个vlan-device.
vconfig add eth0 vid
for trunk routing between the different vlans we need ip_forward



== Depth exploration ==
802.1q standard





== SOURCE ==
initialize
vlan_proto_init()
这个函数最重要的是映射了iocctl函数， 因为接下来的所有操作都要用到ioctl。

application
vconfig eth0 1
vlan_ioctl_handler()->register_vlan_device():
alloc net_device.
vlan_setup()函数非常重要，设置dev->priv_flags 为 802.1q！tx queue 为0.
设置open函数为vlan_dev_open;
register_vlan_dev(）把这个设备注册到内核。






