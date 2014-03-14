#!/bin/sh

. /lib/functions/network.sh
. /lib/functions/net_update_actions.sh

static_domains="www.qq.com t.cn"

valid_gateway()                                       
{                                  
	local arp_entry                                                                                                                
	local wan_type
	wan_type="$(uci get network.wan.proto)"
	if [[ "$wan_type" = "pppoe" ]];then
		return 0
	fi
     
	###:TODO
	network_get_gateway wan_gw wan
	if [[ "$?" -eq 0 ]];then
		arp_entry=$(cat /proc/net/arp | grep $wan_gw 2> /dev/null)
		if [[ "$?" -eq "0" ]]; then
			echo $arp_entry | grep "00:00:00:00:00:00" &> /dev/null
			if [[ "$?" -ne "0" ]];then               
				return 0                          
			fi                    
		fi                                                                                                               
	fi

	return 1                                                                          
}

get_wlan_relay_status()
{
	###FIXME: 
	### 1, These codes what test wlan status should be move to /etc/hotplug.d.
	### Unfortunately, our system does't support reply wlan-relay-mode status in hotplug!
	### 2, In case of the unstable of wireless-rely mode, we do not use ping to test the network,
	### just let it go.

	wlan_st=$(uci get wireless.@wifi-iface[1].network 2>/dev/null)
	if [[ "${wlan_st:-?}" = "wan" ]]; then
		local wlan_iface=$(uci get network.wan.ifname)
		wan_link=$(lua -e "require 'hcwifi';print(hcwifi.get(\"$wlan_iface\",'status'))")
		return $wan_link
	fi
	
	return 0
}

chk_inet_state_by_wget()
{
	local rv=""

	wget -O /dev/null http://www.baidu.com/ &> /dev/null
	rv=$?
	
	return $rv
}


#Init inet state
netdown_action
netup_action

net_get_agreement
if [[ $? -eq 0 ]];then
	while [[ true ]];
	do
		netdown_action	
		sleep 1
		net_get_agreement
		if [[ "$?" -ne 0 ]];then
			break
		fi
	done
fi
	
while [[ true ]];
do
	local net_state="inet_down"

	is_relay="$(lua -e 'local net=require "luci.controller.api.network"; print(net.is_bridge())')"
	if [[ "$is_relay" = "true" ]]; then
		get_wlan_relay_status
		if [[ $? -eq 1 ]]; then
			net_state="inet_up"
		fi
	else
		valid_gateway
		if [[ "$?" -eq 0 ]];then
			for cur in $static_domains
			do
				ping $cur -c 1 -W 2 &>/dev/null
				if [[ $? -eq 0 ]]; then
					net_state="inet_up"
					break
				fi
			done
			if [[ "$net_state" = "inet_down" ]]; then
				chk_inet_state_by_wget
				if [[ "$?" -eq 0 ]];then
					net_state="inet_up"
				fi
			fi
		fi
	fi

	if [[ "$net_state" = "inet_up" ]]; then
		netup_action	
	else
		netdown_action	
	fi

	sleep 5

done
