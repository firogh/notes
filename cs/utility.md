---
tags: cs
title: Programming Utility
date: 2015-02-27 15:46:14
category: cs
---
#Monitor
[Linux Performance Tools](http://www.brendangregg.com/Perf/linux_observability_tools.png)
ltrace
strace
ftrace
lsof



#git 
git send-email --smtp-encryption=tls --smtp-server=smtp.gmail.com --smtp-user=firogm@gmail.com --smtp-server-port=587 --to "kernel-janitors@vger.kernel.org" 0001-x86-Remove-unnecessary-void-pointer-casts.patch

git ls-files -d |xargs -i git checkout {}

git log -S "RTN_UNSPEC" --source --all
* reverse find all the blame from start commit id to now
    git blame --reverse 9e734775f7c22d2f89943ad6c745571f1930105f..   include/linux/netlink.h

* find kernel version of path
    git describe --contains
    git tag --contains <id>

* find commit id of a tag
    git rev-parse v2.6.12-rc2

##revert changes
=checkout all changes

#Makefile

##debug
=Just print echo 
make -s 

=Print shell command
make -n

=Print all variables. Wired-name variable is useful to debug
make -p

=Pirnt a message
$(warning ...)

=Etc
--warn-undefined-variables

#ctags
ctags -R --regex-c="/^SYSCALL_DEFINE[0-9]?\(([a-zA-Z0-9_]*).*/sys_\1/"

#dropbear
=generate private key
dropbearkey -t rsa -f ~/.ssh/id_rsa.db
=yank public key
dropbearkey -y -f ~/.ssh/id_rsa.db
=login in
dbclient -y -i ~/.ssh/id_rsa.db user@ip
=trans file
scp -S dbclient -i ~/.ssh/id_rsa fileName user@ip:/dir

#eval
* create variable name
	__var="name"
	eval "export -- \"$__var=firo\""
	set | grep firo
	__var='name'
	bbb='firo'

#expr
expr "$name" : '\(.*\)\.conf'

#find
* find symbols in object 
	find . -name 'a.out' -exec nm -D {} \; -print
	find . -name '*.o' -print0 | xargs -0 nm -A | egrep ' (i|y)$'
* rm
find . -name ‘your_pattern*’ -exec rm -f {} \;
find . -name ‘your_pattern*’ -delete

#id3tag
id3tag -2 *.mp3
for file in *.mp3;do f=$(echo "$file" |awk -F ' ' '{print $2}' | awk -F '.' '{print $1}'); id3tag -s "$f" "$file";  done
exiftool -json Baccano\!\ 永生之酒\(バッカーノ！\)\ ORIGINAL\ SOUNDTRACK\ SPIRAL\ MELODIES/12.緩みきった官能のダンス.mp3


#indent
indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs *.c *.h

#pipe
find . -type d | while read d; do cnt=$(ls $d | grep tgz | wc -l); echo "$cnt $d"; done | sort -n >stat 

#qemu
qemu -m 512 -kernel bzImage -append “root=/dev/sda” -boot c -hda busybox.img -k en-us

#rdesktop
rdesktop -K -g 1366x700 -r clipboard:PRIMARYCLIPBOARD 192.168.10.200 -r sound:local -u firo -p ""
rdesktop -K -g 1366x700 -r clipboard:CLIPBOAD 192.168.10.200
nmap  -sP 192.168.10.0/24  | grep 'Nmap scan' | awk '{print$5}' | while read line; do echo "connect to $line";  timeout -s 9 2 rdesktop -K -g 1366x700 -r clipboard:CLIPBOARD $line; done

#readelf
readelf -S module.ko

#vim

