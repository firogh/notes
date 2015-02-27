---
tags: net
title: conntrack
date: 2015-02-27 15:46:13
category: net
---
#type-variants-sd-al


#parts-variants-sd-al
=parts
*the relationship of the packet to that connection. 
*connection tracking converts a packet to a "tuple"representing the interesting parts of the packet, before searching for
  bindings or rules which match it.

SNAT
192.168.199.132 g.cn
g.cn 192.168.199.1 

DNAT
192.168.199.132 g.cn
192.168.199.1 192.168.199.1
https://www.ietf.org/rfc/rfc3489.txt
symmetric nat, 端口不复用, 访问同一个服务器.
