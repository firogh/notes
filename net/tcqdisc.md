---
tags: net
title: tcqdisc
date: 2015-02-27 15:46:13
category: net
---
#Bibliography
http://tldp.org/HOWTO/Traffic-Control-HOWTO/intro.html
lartc.rog
http://ace-host.stuart.id.au/russell/files/tc/



#parts-variants-sd-al
Shaping: Shapers delay packets to meet a desired rate.
Scheduling: Schedulers arrange and/or rearrange packets for output.
Classifying: Classifiers sort or separate traffic into queues.
Policing: Policers measure and limit traffic in a particular queue.
Dropping: Dropping discards an entire packet, flow or classification.
Marking: Marking is a mechanism by which the packet is altered.




#MOST_WANTED
=conflict tc qidsc del with softnet_data->softnet_data
 [PATCH] pkt_sched: Destroy gen estimators under rtnl_lock().
http://thread.gmane.org/gmane.linux.network/102444/focus=102592
After synchronize_rcu() in dev_deactivate() we are sure any qdisc_run(),
from dev_queue_xmit() or net_tx_action() can only see and lock noop_qdisc.
This was happened in dev_deactivate_many()

=difference between synchronize_net and  synchronize_rcu?
http://article.gmane.org/gmane.linux.network/196309/match=net_device+dismantle
In this patch, we replace synchronize_rcu with synchronize_net().





#FAQ
#tc filter add dev eth0 parent 10:0 protocol ip prio 1 u32 match ip src 4.3.2.1/32 match ip sport 80 0xffff flowid 10:1
#How dose parameter sport works in  network qdisc?
[root@leander]# tc filter add               \ (1)
>                  dev eth0                 \ (2)
>                  parent 1:0               \ (3)
>                  protocol ip              \ (4)
>                  prio 5                   \ (5)
>                  u32                      \ (6)
>                  match ip port 22 0xffff  \ (7)
>                  match ip tos 0x10 0xff   \ (8)
>                  flowid 1:6               \ (9)
>                  police                   \ (10)
>                  rate 32000bps            \ (11)
>                  burst 10240              \ (12)
>                  mpu 0                    \ (13)
>                  action drop/continue       (14)
http://www.tldp.org/HOWTO/html_single/Traffic-Control-HOWTO/#c-filter
=user
 u32_parse_opt 
{
	-> parse_selector ->...-> parse_ip
	struct nlmsghdr *n
	rta = NLMSG_TAIL(n)
	rta->type = TCA_U32_SEL

}
=kernel

NETLINK_ROUTE -> RTM_NEWTFILTER: see tc_filter_init -> tc_ctl_tfilter->(tp->ops->change = u32_change in net/sched/cls_u32.c) 

tcf_exts_validate: init police and action of this shel tc command, 
put sel in tc_u_knode;
tc_u_knode insert in tc_u_hnode
root is tcf_proto 入殓 by prior.

final
tcf_proto -> tc_u_hnode -> tc_u_knode -> sel
也就是用户太的selector没变存到内核中了.


enqueue -> filter_list ->u32->classify() this classify is implement by u32!
tcf_proto_ops->.kind = "u32", .classify   =   u32_classify,
police and action invoke in tcf_action_exec , act register by tcf_register_action.



#tcf_chain
tcf_proto insert tc filter chain of qdisc or class by prior

#ll_cache: linker layer cache

#TCA_U32_CLASSID in u32_set_parms
filter classid and flowid is the same meaning in russell tc doc

#TCA_KIND in filter is u32...register_tcf_proto_ops



#for qdisc
RTM_NEWQDISC tc_modify_qdisc

