---
tags: net
title: data-link-layer
date: 2015-02-27 15:46:13
category: net
---

#Common concepts
* MTU
This limits the number of bytes of data to 1500(Ethernet II) and 1492(IEEE 802), respectively. 
This characteristic of the link layer is called the MTU, its maximum transmission unit.

* PMTU
/proc/sys/net/ipv4/ip_no_pmtu_disc
0 enable, 1 disable

cat /proc/sys/net/core/warnings

/proc/sys/net/ipv4/tcp_mtu_probing
!0 enable tcp_mtu_probing()
If you are using Jumbo Frames, we recommend setting tcp_mtu_probing = 1 to 
help avoid the problem of MTU black holes. Setting it to 2 sometimes causes performance problems.

net/ipv4/icmp.c
icmp_unreach(
type 3, code 4
icmph->type == ICMP_DEST_UNREACH //3
case ICMP_FRAG_NEEDED //4
icmp_err,


#input
NAPI poll_list net_device
driver intr e100_intr()->__netif_rx_schedule()->__napi_schedule(netdev,nic->napi)->:add napi to poll_list and __raise_softirq_irqoff()
aync:net_rx_action()->netdev->poll()=e100_poll()->e100_rx_clean()...netif_receive_skb()->

Non-NAPI input_pkt_queue skb
driver intr vortex_rx()->netif_rx()->napi_schedule(backlog)->add napi to poll_list and__raise_softirq_irqoff()
async:net_rx_action()->backlog->poll()=process_backlog()->netif_receive_skb()->

后来
netif_receive_skb()->ptype_base.ip_rcv()->NF_INET_PRE_ROUTING->ip_rcv_finish()->
ip_route_input()->ip_route_input_slow():local_input dst.input=ip_local_deliver()
					ip_mkroute_input()->__mkroute_input():dst.input=ip_forward() dst.output=ipoutput()
dst_input()->
ip_local_deliver()->NF_INET_LOCAL_IN->ip_local_deliver_finish()->inet_protos.tcp_v4_rcv()
ip_forward()->NF_INET_FORWARD->ip_forward_finish()->dst_output()见上。

Differences
1 NAPI has not  netif_rx():input_pkt_queue.
2 NAPI and Non-NAPI used different napi->poll 决定本质上的区别。
3 vortex_rx() 多，e100_rx_clean()多！这点可以看出不同优势来。

#output
dev_queue_xmit()->:
qdisc_run()__qdisc_run()->qdisc_restart()->dev_hard_start_xmit()
dev_hard_start_xmit()->:
dev_gso_segment()->skb_gso_segment()->ptyep_base.inet_gso_segment()->inet_protos.tcp_tso_segment()
dev->hard_start_xmit()=e100_xmit_frame()
async:net_tx_action()->qdisc_run()

###System initialization
start_kernel-> parse_early_param irq timers softirq -> rest_init(): kthread
{
	do_basic_setup()  
	{
		driver_init
		sock_init
		do_initcalls()
		{
			net_dev_init: Initializing the Device Handling Layer
			{
				per-CPU 
				proc
				sysfs
				ptype_base
				dst_init
				softirq: net_rx/tx_action
				dev_cpu_callback: CPU hotplug.
			}
		}

	}
	free_init_mem()
	run_init_process()
}

#protocols handler init
For a protocol included statically in the kernel, the initialization function executes at boot time; 
for a protocol compiled as a module, the initialization function
executes when the module is loaded.
The function allocates internal data structures,notifies other subsystems about the protocol’s existence, 
registers files in /proc, and so on. 
A key task is to register a handler in the kernel that handles the traffic for a protocol.

#Capture frames
PF_PACKET
AF_PACKET sockets hand frames directly to dev_queue_xmit

#neigh subsys
arp in ipv4
ndp in ipv6


#Understand __QUEUE_STATE_FROZEN
http://thread.gmane.org/gmane.linux.kernel/709444/focus=714632



