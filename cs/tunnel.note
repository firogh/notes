
#Basic concepts
=VPN is created by establishing a virtual point-to-point connection through the use of
1. dedicated connections(permanent)
2. virtual tunneling protocols
traffic encryptions and authentication
+ connection(tunnel) + secure

#Tunnel proctocol
=pptp
data-link header | PPP | IP ...
data-link header | IP | GRE | PPP | IP...
rfc2637
http://pptpclient.sourceforge.net/diagrams.phtml 
=pppd -> /dev/ppp
/dev/ppp (see ppp_init) 下发密码, 加密之类的.
ppp_netdev_ops (see ppp_create_interface)
ppp_wirte->ppp_xmit_process->ppp_send_frame->ppp_push->|pch->chan->ops->start_xmit()=pppoe_xmit->__pppoe_xmit

* init
pptp -> pptp server -> generate PPP connection and PPP0(interface)
* input
eth -> ppptp(remove gre header)->PPP header-> pty(device) -> PPP0(interface) -> 
* output
-> PPP0(interface) -> PPP header -> pty -> PPPT(add gre header) -> raw socket -> eth


=shadowsocks
user -> iptables -> shadowsocks(redir.c (struct server)->(struct remote)) -> server

ssh redsocks
user -> iptables -> redsocks -> ssh -> server

=pppoe
http://blog.csdn.net/osnetdev/article/details/8958058
&pppoe_chan_ops;

+ppp header 
is added in ppp_start_xmit to ppp_send_frame NO ppp_hdr struct!
ppp-wan->ndo_start_xmit()=ppp_start_xmit()->ppp_xmit_process()->ppp_send_frame()
->
{
	__skb_push(skb, sizeof(*ph));
    skb_reset_network_header(skb);
	//add pppoe_hdr
    dev_queue_xmit(skb); 
}


#PPP 
http://www.cnitblog.com/liaoqingshan/archive/2013/06/13/52906.html
=type1
Use LCP(similar to MAC) and NCP(similar to LLC) to trans data over EIA/TIA  V.35 V.21

=type2 PPPoE
PPPOE是在以太网链路上通过发现阶段打通一条PPP链接，再进行PPP协议。
