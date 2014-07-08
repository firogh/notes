#!/bin/sh

HWF_INFO_GATHER_TIME_INTERVAL="120"
HWF_INFO_GATHER_DB_PATH=/tmp/data/hwf_info_gather_db
HWF_INFO_GATHER_TGZ_PATH=/tmp/data/hwf_info_gather_tgz
HWF_HEALTH_CHK_LOG_PATH=/tmp/data/hwf_health_chk
HWF_HEALT_CHK_VERSION="a1"
cause=""
hwf_health_chk_process_list="nginx dnsmasq fcgi-cgi netifd inet_chk.sh cmagent ubusd hotplug2"

hwf_clean_kpanic_tgzs()
{
	local tgzs=""

	tgzs=$(ls -t $HWF_INFO_GATHER_TGZ_PATH | grep 'kpanic-.*.tgz' | sed '1,4d')

	for tgz in $tgzs; do
		rm -rf $HWF_INFO_GATHER_TGZ_PATH/$tgz
	done
}

hwf_check_panic_log()
{
	local MAC="$(source /lib/platform.sh;tw_get_mac)"
	local timestamp="$(date +%Y%m%d%H%M%S)"
	local pending_panic_files=""
	local version=""
	local name=""

	mkdir -p $HWF_INFO_GATHER_TGZ_PATH
	[[ $? -ne 0 ]] && return 1

	mkdir -p $HWF_INFO_GATHER_DB_PATH
	[[ $? -ne 0 ]] && return 1

	[[ -e /tmp/data/kpanic.log ]] && {

		hwf_clean_kpanic_tgzs

		version=$(grep -i 'HiWiFi firmware version' /tmp/data/kpanic.log | sed '2,$d' | awk '{print $4}')
		timestamp=$(grep -i 'Panic Log Begin' /tmp/data/kpanic.log  | sed '2,$d' | sed 's/[^0-9]//g')
		name="$HWF_HEALT_CHK_VERSION-$version-kpanic-$timestamp-$MAC"

		mv /tmp/data/kpanic.log  /tmp/data/$name.log
		[[ $? -ne 0 ]] && return 1

		cd /tmp/data
		tar -czf $HWF_INFO_GATHER_TGZ_PATH/$name.tgz $name.log
		[[ $? -ne 0 ]] && return 1

		mv /tmp/data/$name.log $HWF_INFO_GATHER_DB_PATH
		[[ $? -ne 0 ]] && return 1
	}

	pending_panic_files=$(ls $HWF_INFO_GATHER_TGZ_PATH | grep "kpanic-.*.tgz")
	if [[ $? -ne 0 ]];then
		return 0
	fi

	for file in $pending_panic_files;do
		( source /lib/functions/hwf_upload_info_routine.sh; hwf_upload_info_data $HWF_INFO_GATHER_TGZ_PATH/$file )
	done
}

hwf_clean_old_coredumps()
{
	local curr_version=$(awk '{print $1}' /etc/.build)

	ls /tmp/data/ | grep '.*\.core\b' &> /dev/null
	[[ $? -eq 0 ]] && {
		for core in $(ls /tmp/data/ |  grep '.*\.core\b'); do
			version=$(echo $core | grep '.*\.core\b' | awk -F '.'  '{print $(NF-3)"."$(NF-2)"."$(NF-1)}')
			if [ "$version" != "$curr_version" ]
			then
				rm -f /tmp/data/$core
			fi
		done
	}
}

hwf_clean_coredumps()
{
	local coredumps=""

	coredumps=$(ls -t /tmp/data | grep '.*\.core\b' | sed '1,4d')

	for core in $coredumps; do
		rm -rf /tmp/data/$core
	done
}

#date +%Y%m%d%H%M%S -d "@$unixdate"
hwf_check_coredump()
{
	hwf_clean_old_coredumps

	ls /tmp/data | grep '.*\.core\b' &> /dev/null
	[[ $? -eq 0 ]] && { cause=$(ls -t /tmp/data | grep '.*\.core\b' | awk  -F "." 'BEGIN{num=1;str=""}{if (num <= 3 && !match(str, $1)){str=str$1"-";num++}} END{print str"core-"}');return 0;}

	return 1
}

hwf_health_chk_process_is_numb_routine()
{
	local chk_command="$1"

	if ! eval $chk_command; then
		return 0
	fi

	return 1
}

hwf_health_chk_process_dnsmasq_is_numb()
{
	hwf_health_chk_process_is_numb_routine  'nslookup hwftestdnsmasq.localhost localhost &> /dev/null'

	return $?
}

hwf_health_chk_process_netifd_is_numb()
{
	hwf_health_chk_process_is_numb_routine  'ubus call network.device status &> /dev/null'

	return $?
}

hwf_health_chk_process_nginx_is_numb()
{
	hwf_health_chk_process_is_numb_routine 'curl -o /dev/null  http://tw/ &> /dev/null'

	return $?
}

hwf_health_chk_process_fcgi_cgi_is_numb()
{
	return 1
}

hwf_health_chk_process_inet_chk_sh_is_numb()
{
	return 1
}

hwf_health_chk_process_cmagent_is_numb()
{
	return 1
}

hwf_health_chk_process_ubusd_is_numb()
{
	return 1
}

hwf_health_chk_process_hotplug2_is_numb()
{
	return 1
}

hwf_health_chk_process_is_numb()
{
	local process=$1

	process=${process//-/_}
	process=${process//./_}
	if hwf_health_chk_process_"$process"_is_numb; then
		sleep 5
		if hwf_health_chk_process_"$process"_is_numb; then
			return 0
		fi
	fi

	return 1
}

hwf_health_chk_process_is_die()
{
	local target_process=$1

	if ! pidof $target_process > /dev/null; then
		sleep 5
		if ! pidof $target_process > /dev/null; then
			return 0
		fi
	fi

	return 1
}

hwf_health_chk_process()
{
	local process=$1

	if hwf_health_chk_process_is_die $process ; then
		cause=$cause"$process"die-
		return 0
	elif hwf_health_chk_process_is_numb $process ; then
		cause=$cause"$process"numb-
		return 0
	fi

	return 1
}

hwf_health_chk_memory()
{
	local high=$(free | grep -i 'Mem'  | awk '{print $2 * 0.95}')
	local used=$(free | grep -i 'buffers:' | awk '{print $3}')

	if [ $used -gt $high ]; then
		cause="mm-"
		return 1
	fi

	return 1
}

hwf_health_chk_processes()
{
	cause=""
	local rv=1

	hwf_health_chk_memory
	if [ $? -eq 0 ];then
		rv=0
	fi

	hwf_check_coredump
	if [ $? -eq 0 ];then
		rv=0
	fi

	for process in $hwf_health_chk_process_list; do
		hwf_health_chk_process $process
		[[ $? -eq 0 ]] && {
			rv=0
		}
	done

	return $rv
}

hwf_clean_info_tgzs()
{
	local tgzs=""

	tgzs=$(ls -t $HWF_INFO_GATHER_TGZ_PATH | grep -v 'kpanic-.*.tgz' | sed '1,4d')

	for tgz in $tgzs; do
		rm -rf $HWF_INFO_GATHER_TGZ_PATH/$tgz
	done
}

try_restart_processes()
{
	local process=""

	mkdir -p $HWF_HEALTH_CHK_LOG_PATH
	rm -f $HWF_HEALTH_CHK_LOG_PATH/*
	for process in $hwf_health_chk_process_list;
	do
		if hwf_health_chk_process $process; then
			case "$process" in
				"nginx")
				/etc/init.d/nginx restart &> $HWF_HEALTH_CHK_LOG_PATH/nginx.log
				;;
				"netifd")
				/etc/init.d/network restart &> $HWF_HEALTH_CHK_LOG_PATH/netifd.log
				;;
				"dnsmasq")
				/etc/init.d/dnsmasq restart &>  $HWF_HEALTH_CHK_LOG_PATH/dnsmasq.log
				;;
				"fcgi-cgi")
				/etc/init.d/fcgi-cgi start &> $HWF_HEALTH_CHK_LOG_PATH/dnsmasq.log
				;;
				"inet_chk.sh")
				/etc/init.d/inet_chk start &> $HWF_HEALTH_CHK_LOG_PATH/inet_chk.log
				;;
				"cmagent")
				/etc/init.d/cmagent restart &> $HWF_HEALTH_CHK_LOG_PATH/cmagent.log
				;;
				"ubusd")
				/etc/init.d/ubus restart &>  $HWF_HEALTH_CHK_LOG_PATH/ubus.log
				;;
				"hotplug2")
				/etc/init.d/boot restart &> $HWF_HEALTH_CHK_LOG_PATH/hotplug2.log
				;;
			esac
		fi
	done

}

#Conditions which trigger info-gather
#1. There is kpanic.log in /tmp/data
#2. There is *.core file in /tmp/data
#3. Ether of dnsmasq nginx fcgi-cgi netifd die or didn't response.
main()
{
	local run_state=""
	local next_stamp="$(date +%s)"
	local collected=""
	local collect_stamp=""
	local old_cause=""
        local MAC="$(source /lib/platform.sh;tw_get_mac)"
        local rom_veriosn=""
        local stamp_dir=""
	local pid_list=""
	local instance_num=""

        if [ -e "/etc/.build" ]; then
                rom_version=$(awk '{if (NR == 1){print $1}}' /etc/.build)
                #The max len of string rom_veriosn is 32.
                rom_version=${rom_version:0:32}
        fi

	run_state="$(cat /tmp/state/hwf_health_chk_run_state 2> /dev/null )"
	if [[ "$run_state" == "start" ]];then
		hwf_check_panic_log

                pid_list=$(pidof "hwf_health_chk.sh")
                instance_num=$(echo $pid_list | wc -w)
                [ "$instance_num" -gt 1 ] && { exit 1; }

		while :
		do
			sleep 10
			collected=1
			stamp_dir=""

			/sbin/diskspace_chk.sh 2000 &> /dev/null

			hwf_clean_coredumps

			hwf_health_chk_processes
			if [[ $? -eq 0 -a "$(date +%s)" -gt "$next_stamp" ]];then
				touch /tmp/data/healthchk_tmp
				if [ $? -eq 0 ];then
					rm -f /tmp/data/healthchk_tmp

					collected=0
					old_cause=$cause
					collect_stamp="$(date +%Y%m%d%H%M%S)"
					stamp_dir=${HWF_HEALT_CHK_VERSION}"-"$rom_version"-"$old_cause$collect_stamp"-"$MAC

					hwf_clean_info_tgzs
					/sbin/hwf_info_gather.sh "pre" $stamp_dir
					next_stamp=$(expr $(date +%s) + $HWF_INFO_GATHER_TIME_INTERVAL)
				fi
			fi

			try_restart_processes

			if [ "$collected" -eq 0 ];then

				/sbin/hwf_info_gather.sh "post" $stamp_dir

				( source /sbin/hwf_info_upload.sh; hwf_upload_info )
			fi

		done
	fi

	#During shutdown, we want hwf_health_chk calm down.
	sleep 5
}

main
