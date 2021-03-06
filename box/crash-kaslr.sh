#!/bin/bash
# PHYSICAL_ALIGN [=0x1000000] 
# find_random_virt_addr -> 
# return random_addr * CONFIG_PHYSICAL_ALIGN + minimum;
# That's why we I used 
match=$( file $1 | grep "ELF" )
if [[ !  -z  $match  ]]; then
    vmlinux=$1
    core=$2
else
    vmlinux=$2
    core=$1
fi
base=0x$( nm $vmlinux | grep " linux_banner$" | cut -f1 -d" " )
#echo "base:" $base
i=$base
rip=0x$(
    while [[ $(($i)) -lt 0 ]]; do
        printf "rd 0x%x 2\n" $(($i));
        i=$(($i + 0x1000000));  # CONFIG_PHYSICAL_ALIGN=0x1000000 ? 
				# 16MB see RANDOMIZE_BASE
        y=$(printf "%x" $i)
    done | crash --minimal $vmlinux $core | grep "Linux version" | cut -f1 -d:
)
#echo "RIP:" $rip
off="auto"
if [[ !  -z  "$rip"  ]]; then
    off=$(printf "0x%x" $((rip - base)))
fi
#echo "off: " $off
crash $vmlinux $core --kaslr=$off
