




Function
=========
input
======
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


output
======
dev_queue_xmit()->:
qdisc_run()__qdisc_run()->qdisc_restart()->dev_hard_start_xmit()
dev_hard_start_xmit()->:
dev_gso_segment()->skb_gso_segment()->ptyep_base.inet_gso_segment()->inet_protos.tcp_tso_segment()
dev->hard_start_xmit()=e100_xmit_frame()
async:net_tx_action()->qdisc_run()
