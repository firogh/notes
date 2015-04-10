---
tags: cs
title: Programming Utility
date: 2015-02-27 15:46:14
category: cs
---

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
