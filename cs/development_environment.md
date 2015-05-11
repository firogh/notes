---
tags: kernel
title: The linux development environment
date: 2015-02-27 15:46:12
category: cs
---
# Reference
[Programming tool](https://en.wikipedia.org/wiki/Programming_tool)
[Utility software](https://en.wikipedia.org/wiki/Utility_software)

# Development environment
## Source code editor
* vim
easymotion

## Code navigation tools
* ctags
ctags -R --regex-c="/^SYSCALL_DEFINE[0-9]?\(([a-zA-Z0-9_]*).*/sys_\1/"
* cscope
* GNU global
* LXR Cross Referencer
* Google code search
* How to find hook functions of a struct ops
For example, all start_xmit of ppp_channel_ops, 
pppol2tp_chan_ops, pppol2tp_xmit,
pptp_chan_ops pptp_xmit
pppoe_chan_ops pppoe_xmit
or grep -nr 'ppp_channel_ops' /path/to/{l2tp, pptp, pppoe}
* locate codes in complex program source files .
Bubble locate, begin with the fundermental system api for example socket(), bind(), connect()...then raise

## Revision control
* [git](http://git-scm.com/docs)
* diff patch

## Build

### Build automation
* Make

###Compiler
* gcc

### Linker
* GNU ld
* gold
###Kernel build
* build arm on x86
 make ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnu- drivers/pcmcia/sa11xx_base.o


## Reverse engineering
* readelf
* objdump
* tcpdump
###Manipulate ELF
ELF header, readelf -h
Program header table, readelf -l
Segments, readelf --segments 
Sections, readelf --sections
Section header table, readelf -S 
.symtab, Symbol table store index of string
.strtab, String table is a array of all strings split by \0
.hash, utiliy for quire symbols
.shstrtab, Section header string table store section name for ELF,like .text, .data...

##Mail
mutt
fetchmail
fetchmail -d0 -vk pop.gmail.com
msmtp
/home/firo/.msmtprc
procmail


## Test
* kvm 
* supermin

###Kernel modification test
git status  | grep 'modified:' | while read line; do file=$(echo $line | awk '{print $2}'); echo $file; make ${file/%.c/.o};done

## Debug
More details in debugging note
#Git 

git ls-files -d |xargs -i git checkout {}

git log -S "RTN_UNSPEC" --source --all
* reverse find all the blame from start commit id to now
    git blame --reverse 9e734775f7c22d2f89943ad6c745571f1930105f..   include/linux/netlink.h

* find kernel version of path
    git describe --contains
    git tag --contains <id>

* find commit id of a tag
    git rev-parse v2.6.12-rc2
#git pull
git pull {repo} {remotebranchname}:{localbranchname}
git pull origin xyz:xyz
git pull next master:now

#git sendmail
shadowsocks
/etc/proxychains.conf
/home/firo/.gitconfig
proxychains git send-email --to dbaryshkov@gmail.com --cc rmk+kernel@arm.linux.org.uk --cc linux-pcmcia@lists.infradead.org  0001-Drivers-pcmcia-Fix-memroy-leak-in-sa11xx_drv_pcmcia_.patch

git send-email --smtp-encryption=tls --smtp-server=smtp.gmail.com --smtp-user=firogm@gmail.com --smtp-server-port=587 --to "kernel-janitors@vger.kernel.org" 0001-x86-Remove-unnecessary-void-pointer-casts.patch

#git diff
git -c core.whitespace=tab-in-indent diff --check



#Make
## useful usage
make cynthia
make -q : need rebuild?

##debug
* Just print echo 
make -s 
* Print shell command
make -n
* Print all variables. not really execute. Wired-name variable is useful to debug
make -p
* Pirnt a message
$(warning ...)
* Etc
--warn-undefined-variables

##menuconfig
* if no .config, every config tools make a .config from scrach!

* oldconfig just for new moduels patch merged in trunk. This no any relations to .config.old and /boot/config.x.y.z

* make localmodconfig will reduce many unused kernel config.

* make bzImage  #kmods will not build that configured with M! 
* make modules_install INSTALL_MOD_PATH=/home/firo/kmods
###make ARCH=arm menuconfig

##build signle kernel module
make menuconfig
make oldconfig && make prepare
make -C $(pwd) M=/home/firo/linux/fs/ext3 modules V=1

#GCC
## useful various options
-E -S -c 
-I -L -l
-ansi
-fsyntax-only 
-Werror
-save-temps
-H: show all header used
-v
-Wall
-fPIC: for shared library
-Q: print function and statisc?
-Wextra: 用处不大.
-U: undefine a preprocessor macro.
-pipe: make compilation faster, replace xx.S with |
-Wpadded: we can rearrange the fields of structure to make the structure smaller
-time
-x c: c language
-: stdin

## asmlinkage
However, for C functions invoked from assembly code, 
we should explicitly declare the function's calling convention, 
because the parameter passing code in assembly side has been fixed. 


##[Generating optimized code](http://www.stlinux.com/devel/debug/jtag/build?q=node/82)

##Symbol table
* System.map less 
When you compile the kernel
nm vmlinux
readelf -s 
* /proc/kallsyms
