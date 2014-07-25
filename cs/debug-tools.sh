#!/bin/sh
DEBUG_PKG_PATH="/tmp/data/debug_pkg_home"

if [ $# -ne 1 ];then
	exit 1
fi

bin=$(which $1)
rv=$?
if [ $rv -ne 0 ];then
	echo "Can't found $1"
	exit $rv 
fi


dep_libs=$(LD_TRACE_LOADED_OBJECTS=1 $bin | awk -F '.' '{print $1}' | awk -F '/' '{print $NF}')
for lib in $dep_libs
do
	echo $lib
done

if [ ! -e "/etc/.build" ];then
	echo "Use only in openwrt."
	exit 1
fi
rom_version=$(awk '{print $1}' /etc/.build)

dep_libs=$(LD_TRACE_LOADED_OBJECTS=1 $bin | awk '{print $3}')
opkg update
if [ $? -ne 0 ];then
	echo "opkg update failed."
	exit 1
fi

#HC6361,5761
model=$(cat /proc/cmdline | awk '{print $1}'| awk -F '=' '{print $2}')
target="ralink"
if [ $model == "HC6361" ];then
	target="ar71xx"
fi

mkdir -p $DEBUG_PKG_PATH
for lib in $dep_libs
do
	package=$(opkg search "$lib")
	if [ $? -eq 0 ];then
		package=$(echo $package | sed 's/ //g')
		debug_pkg=$package"_"$target"_debug.ipk"
		remote_path=/ROOT/diskb/build/trunk/daily/$model/${target}.release/debug-packages/$debug_pkg
		#private key
		#dropbearkey -t rsa -f /etc/dropbear/dropbear_id_rsa.db
		#public key
		#dropbearkey -y -f /etc/dropbear/dropbear_id_rsa.db
		echo "get $debug_pkg"
		scp -S dbclient -i /etc/dropbear/dropbear_id_rsa.db yongming.yang@10.9.1.243:$remote_path $DEBUG_PKG_PATH
		echo "install $debug_pkg"
		opkg install $DEBUG_PKG_PATH/$debug_pkg

done

echo "finish setting"

