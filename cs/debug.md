---
tags: cs
title: debug
date: 2015-02-27 15:46:14
category: cs
---
#How to debug segmention fault
* coredump + gdb
    bt
    x/100a


#Gdb usage 
=x/100a

=thread apply all bt full

#Collect debug assist info, reference source
/proc (specially /proc/sys/)

#How to know library dependencies of a bin
LD_TRACE_LOADED_OBJECTS=1 git 
ldd /usr/bin/git
#ldd code tools
LD_TRACE_LOADED_OBJECTS=1  /sbin/netifd  | awk -F '.' '{print $1}'


#Backtrace or stacktrace
=how to find a backtrace
* gdb bt(Strongly, recommand), break continue bt
* backtrace()
* read soucecode
* print log 
=Exit bug
* Use atexit() register a stackdump() function
* gdb, bt
=Runtime function backtrace
* gdb set brakepoint, then bt it
* backtrace() function
=Timer backtrace
* Just funny, the foolish of me. 


=backtrace() funciton code example
* compile with -rdynamic
void print_stacktrace()
{
    int size = 16;
    void * array[16];
    int stack_num = backtrace(array, size);
    char ** stacktrace = backtrace_symbols(array, stack_num);
    for (int i = 0; i < stack_num; ++i)
    {
        printf("%s\n", stacktrace[i]);
    }
    free(stacktrace);
}

#Debug c++
=demangle c++ symbols 
* c++filt command
* addr2line -f -C -a xxx -e ooo
=read symbol

##tips
#signal
This is just a hiwifi wonderful kernel patch #931
echo 1 > /proc/sys/kernel/debug_signal

#demagling
/home/build/x/ab/mips-openwrt-linux-addr2line -C -f -e /data/logs/hwf-health-chk/debug-root/HC6361/0.9005.5384s/debugfs/tmp/data/usr/bin/aria2c  0x00607188
aria2::ZeroBtMessage::~ZeroBtMessage()



name.c:2445: REQUIRE((((source) != ((void *)0)) && (((const isc__magic_t *)(source))->magic == ((('D') << 24 | ('N') << 16 | ('S') << 8 | ('n')))))) failed, back trace
*#0 0x7fba521689e7 in ??
*#1 0x7fba5216893a in ??
*#2 0x7fba52a76c8c in ??
*#3 0x40a239 in ??
*#4 0x40a2da in ??
*#5 0x40a498 in ??
*#6 0x40f70a in ??
*#7 0x7fba5218b10b in ??
*#8 0x7fba50feb52a in ??
*#9 0x7fba5006b77d in ??

=debugme
* #define debugme(fmt, args...) do {FILE *fdebug=fopen("/d.log", "a+"); time_t tdebug; time(&tdebug);fprintf(fdebug,"%s", asctime(localtime(&tdebug)));fprintf(fdebug,"%s,%d:", __FUNCTION__, __LINE__);fprintf(fdebug, fmt, ##args);fclose(fdebug);} while(0)


#ssgoagbroad-start
the principle of debug is start at the begin.
accept()函数不会用, 如果早点在accept打log就看到了.

#smartqos custom qdisc - self inferrence
要自己推测除几种可能, 之后按着思路去找, 不能汪洋大海, 乱砍.

#find the key ponit
custom qdisc

#Collect signal info for debug
= Who and what is singal?
* kernel stuff
* old IPC ghost then sysV IPC.
* SIGKILL can't be blocked. SIGTERM can be installed a new sigaction.
* kallsymbol

= name, who
* for siganl shell command, we need to konw script's name, maybe the ppid and pid.
* for binary execution, we need ppid and pid name.

= action of receiver's correspond signal
* Ignore should be ommitted.


#define debugme(fmt, args...) \
	do { \
		FILE *fdebug = fopen("/d.log", "a+"); \
		time_t tdebug; time(&tdebug); \
		fprintf(fdebug,"%s", asctime(localtime(&tdebug))); \
		fprintf(fdebug,"%s,%d:", __FUNCTION__, __LINE__); \
		fprintf(fdebug, fmt, ##args);fclose(fdebug); \
	} while(0)


	


#define debugme(fmt, args...) do{FILE *fdebug=fopen("/d.log","a+");time_t tdebug;time(&tdebug);char *s= asctime(localtime(&tdebug));fprintf(fdebug    ,"%.*s,%s,%d:", strlen(s) - 1, s, __FUNCTION__, __LINE__);fprintf(fdebug,fmt,##args);fclose(fdebug);}while(0)


#locate codes
* Bubble locate, begin with the fundermental system api for example socket(), bind(), connect()...then raise

#trouble shooting
* diff with good


