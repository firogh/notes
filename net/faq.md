#first_device 用途？
subsys 在前, device在后.

#What is bridge?
linux bridge 
netdev_rx_handler_register(dev, br_handle_frame, p);
__netif_receive_skb -> rx_handler=br_handle_frame,

and generic concept: hub, switch?
hub: layer 1, bradcast, exclusive share, 报文可被侦听.
switch: layer 2,  mac port route, CAM table in linux bridge module!
switch with vlan: layer 3, 因为vlan之间的报文转发需要路由, 所以是layer层技术.

#What is the type in ip link?

net_poll

napi

dev_queue_xmit_nit

watchdog

dma

###dev_open
#ifconfig eth0 up
先打开一个socket(af->af, type, 0);
socket的ioctl()->sock_ioctl()->dev_ioctl()->SIOCSIFFLAGS-> dev_ifsioc()->dev_change_flags()->IFF_UP->dev_open()->

socket有一个自己的虚拟文件系统，会分配一个inode和file结构，并在这个file结构上赋值socket_file_ops

sys_socket()->sock_map_fd()->sock_attach_fd()->sock_attach_fd()->init_file()

#ip link set eth0 up
RTM_SETLINK rtnl_setlink ->do_setlink->dev_change_flags -> ..->__dev_open->ndo_open

#开机启动...
内核中如果Networking support  --->Networking options  --->IP: kernel level autoconfiguration选中，就可以根据命令行传入的参数或者dhcp获取配置网络各个参数，让网络进入使能状态。
具体调用来说就是net/ipv4/ipconfig.c  ip_auto_config -> ic_setup_if -> ic_devinet_ioctl(SIOCSIFADDR, &ir) -> devinet_ioctl(SIOCSIFADDR) ,接下来就跟命令配置ip地址一样的过程了。


#pci device scan
  linux启动过程中pci总线初始化主要包括2部分，pci控制器的注册和pci设备的枚举，
pci总线和其他总线一个很重要的区别就是pci总线的枚举，在启动过程中遍历pci总线树上所有可能的dev func，
记录下所有存在的设备的vendor id  设备名等，这个是做为后面pci设备驱动初始化中注册pci设备驱动需要匹配的重要依据，类似于platform驱动。



#__LINK_STATE_LINKWATCH_PENDING,

netif_carrier_on
https://www.kernel.org/doc/Documentation/networking/operstates.txt
http://tools.ietf.org/html/rfc2863

#/sys/class/net/p4p1/*
net/core/net-sysfs.c
NETDEVICE_SHOW_RW
net_class_attrs
 _name##_show

#ip link
dev_get_flags

That is voaltile
if (netif_running(dev)) {
5249                 if (netif_oper_up(dev))
5250                         flags |= IFF_RUNNING;
5251                 if (netif_carrier_ok(dev))
5252                         flags |= IFF_LOWER_UP;
5253                 if (netif_dormant(dev))
5254                         flags |= IFF_DORMANT;
5255         }


#netlink

socket: NETLINK_ROUTE/NETLINK_FIB_LOOKUP/NETLINK_GENERIC -> netlink_create_kernel(specific input)

send -> rtnetlink_rcv
recive <- kernel.

#Qdisc change
[PATCH net-next 0/5] net_sched: Adjust qdisc_change() for command "#tc qdisc change/replace ..."

