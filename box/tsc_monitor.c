// SPDX-License-Identifier: GPL-2.0-only
/*
 * Here's a sample kernel module showing the use of kprobes to dump a
 * stack trace and selected registers when kernel_clone() is called.
 *
 * For more information on theory of operation of kprobes, see
 * Documentation/trace/kprobes.rst
 *
 * You will see the trace data in /var/log/messages and on the console
 * whenever kernel_clone() is invoked to create a new process.
 */

#define pr_fmt(fmt) "%s: " fmt, __func__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/clocksource.h>
#include <linux/kallsyms.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched/clock.h>
#include <linux/math.h>
/*
#define MAX_SYMBOL_LEN	64
static char symbol[MAX_SYMBOL_LEN] = "clocksource_watchdog";
module_param_string(symbol, symbol, sizeof(symbol), 0644);
*/
/* For each probe you need to allocate a kprobe structure */
/*
static struct kprobe kp = {
	.symbol_name	= symbol,
};
*/

struct cs_uncertainty {
	u64 cs_last;
	u64 wd_last;
	u64 ts;
	u64 accumulated_uncertainty;
	u32 overflow_count; // times of uncertainty overflowing
	u64 interval;
};

struct cs_uncertainty uncertainty;


static char *cs_watchdog = "sgi_rtc";
module_param(cs_watchdog,charp,0660);
struct clocksource *cs_tsc;
struct clocksource *cs_wd;

int suse_detector_interval = 1 * HZ;
static int detector_running = 0;

static struct timer_list detector_timer;
static struct list_head *clocksource_list;
static struct mutex *clocksource_mutex;

static ssize_t xavailable_clocksource_show(void)
{
        struct clocksource *src;
	clocksource_list = (struct list_head *)kallsyms_lookup_name("clocksource_list");
	if (!clocksource_list)
		return 1;

	clocksource_mutex = (struct mutex *)kallsyms_lookup_name("clocksource_mutex");
	if (!clocksource_mutex)
		return 2;

	mutex_lock(clocksource_mutex);
        list_for_each_entry(src, clocksource_list, list) {
		printk("SUSE_DEBUG: %s %ps %llx %x %x %llx %x %x %llx %x %x %x %lx %llx %llx\n", src->name, src->read, src->mask, src->mult, src->shift, src->max_idle_ns,src->maxadj, src->uncertainty_margin, src->max_cycles, src->rating, src->id, src->vdso_clock_mode, src->flags, src->cs_last, src->wd_last);
        }
	mutex_unlock(clocksource_mutex);

        return 0;
}
static inline u64 clocksource_delta(u64 now, u64 last, u64 mask)
{
        u64 ret = (now - last) & mask;

        /*
         * Prevent time going backwards by checking the MSB of mask in
         * the result. If set, return 0.
         */
        return ret & ~(mask >> 1) ? 0 : ret;
}


#define WATCHDOG_MAX_SKEW (100 * NSEC_PER_USEC)
static int clocksource_detector_read(struct clocksource *cs, u64 *cs_now, u64 *wd_now, u64 *ts)
{
	unsigned int nretries;
        u64 wd_end, wd_end2, wd_delta;
        int64_t wd_delay;
	struct clocksource *watchdog =cs_wd;
	int max_cswd_read_retries =3;

        for (nretries = 0; nretries <= max_cswd_read_retries; nretries++) {
                local_irq_disable();
		if (ts)
			*ts = local_clock();
                *wd_now = watchdog->read(watchdog);
                *cs_now = cs->read(cs);
                wd_end = watchdog->read(watchdog);
                wd_end2 = watchdog->read(watchdog);
                local_irq_enable();

                wd_delta = clocksource_delta(wd_end, *wd_now, watchdog->mask);
                wd_delay = clocksource_cyc2ns(wd_delta, watchdog->mult,
                                              watchdog->shift);
                if (wd_delay <= WATCHDOG_MAX_SKEW) {
                        if (nretries > 1 || nretries >= max_cswd_read_retries) {
                                pr_warn("SUSE_DEBUG: timekeeping watchdog on CPU%d: %s retried %d times before success\n",
                                        smp_processor_id(), watchdog->name, nretries);
                        }
                        return 0;
                }
	}
	return 1;
}

//spinlock_t *sd_watchdog_lock;
static int detector_init(void)
{
	int rv = 0;
        struct clocksource *src;
        clocksource_list = (struct list_head *)kallsyms_lookup_name("clocksource_list");
        if (!clocksource_list)
                return 1;

        clocksource_mutex = (struct mutex *)kallsyms_lookup_name("clocksource_mutex");
        if (!clocksource_mutex)
                return 2;
//	sd_watchdog_lock = (spinlock_t *)kallsyms_lookup_name("watchdog_lock");
//	if (!sd_watchdog_lock)
//		return 3;

        mutex_lock(clocksource_mutex);
        list_for_each_entry(src, clocksource_list, list) {
		if (!memcmp(src->name, "tsc", 3)) {
			cs_tsc = src;
			rv |= 0x10;
		//} else if (!memcmp(src->name, "sgi_rtc", 7)) {
		} else if (!memcmp(src->name, cs_watchdog, strlen(cs_watchdog))) {
			cs_wd = src;
			rv |= 0x20;
		}
        }
        mutex_unlock(clocksource_mutex);

	if (!(rv & 0x20))
		printk("SUSE_DEBUG: Can't find clocksource watchdog %s\n", cs_watchdog);

//	spin_lock(&sd_watchdog_lock);
	if (rv == 0x30)
		clocksource_detector_read(cs_tsc, &uncertainty.cs_last, &uncertainty.wd_last, &uncertainty.ts);
//	spin_unlock(&sd_watchdog_lock);
	uncertainty.interval = ~0ULL;

	return rv == 0x30 ? 0: rv;
}

static void tsc_monitor(struct timer_list *t)
{
	u64 cs_now, now, wd_now, delta, ts;
	int rv, interval = suse_detector_interval;
	int64_t wd_nsec, cs_nsec;
	struct clocksource *watchdog = cs_wd, *cs = cs_tsc;
	u32 md;


	rv = clocksource_detector_read(cs_tsc, &cs_now, &wd_now, &ts);
	if (rv) {
		interval = 1;  // next tick
		goto out;
	}
	
	delta = clocksource_delta(wd_now, uncertainty.wd_last, watchdog->mask);
	wd_nsec = clocksource_cyc2ns(delta, watchdog->mult,
                                             watchdog->shift);	

	delta = clocksource_delta(cs_now, uncertainty.cs_last, cs->mask);
	cs_nsec = clocksource_cyc2ns(delta, cs->mult, cs->shift);

	md = cs->uncertainty_margin + watchdog->uncertainty_margin;
	uncertainty.accumulated_uncertainty += abs(cs_nsec - wd_nsec);
	if (uncertainty.accumulated_uncertainty > md) {
		now = local_clock();
		uncertainty.overflow_count++;
		if ((now- uncertainty.ts) < uncertainty.interval) {
			pr_warn("SUSE_DEBUG: CPU%d '%s' %llx - %llx ns, %llu seconds, skew %llu ns, %u times.\n",
				smp_processor_id(), cs->name, uncertainty.ts, now, (now- uncertainty.ts)/1000000000, uncertainty.accumulated_uncertainty, uncertainty.overflow_count);
			pr_warn("SUSE_DEBUG: '%s' wd_nsec: %lld wd_now: %llx wd_last: %llx margin: %d ns\n",
				watchdog->name, wd_nsec, wd_now, uncertainty.wd_last, watchdog->uncertainty_margin);
			pr_warn("SUSE_DEBUG: '%s' cs_nsec: %lld cs_now: %llx cs_last: %llx margin: %d ns\n",
				cs->name, cs_nsec, cs_now, uncertainty.cs_last, cs->uncertainty_margin);
			uncertainty.interval = now- uncertainty.ts;

		}
		uncertainty.wd_last = wd_now;
		uncertainty.cs_last = cs_now;
		uncertainty.ts = now;
		uncertainty.accumulated_uncertainty = 0;
	}
out:
	detector_timer.expires = jiffies + interval;
	add_timer(&detector_timer);
}

static void clocksource_start_detector(void)
{
	timer_setup(&detector_timer, tsc_monitor, 0);
	detector_timer.expires = jiffies + suse_detector_interval;
	add_timer(&detector_timer);
	detector_running =  1;
}

static void clocksource_stop_detector(void)
{
	if (!detector_running)
		return;
	del_timer_sync(&detector_timer);
	detector_running = 0;
}

static int clock_proc_show(struct seq_file *m, void *v)
{
	struct clocksource *src;
	u64 ts_nsec;

	ts_nsec = local_clock();
        clocksource_list = (struct list_head *)kallsyms_lookup_name("clocksource_list");
        if (!clocksource_list)
                return 1;

        clocksource_mutex = (struct mutex *)kallsyms_lookup_name("clocksource_mutex");
        if (!clocksource_mutex)
                return 2;

        mutex_lock(clocksource_mutex);
	local_irq_disable();
        list_for_each_entry(src, clocksource_list, list) {
                seq_printf(m, "%llu %s %llu %u %u %llx %x %x %llx %x %x %x %lx %llu %llu\n", local_clock(), src->name, src->read(src), src->mult, src->shift, src->max_idle_ns,src->maxadj, src->uncertainty_margin, src->max_cycles, src->rating, src->id, src->vdso_clock_mode, src->flags, src->cs_last, src->wd_last);
        }
	local_irq_enable();

        mutex_unlock(clocksource_mutex);
	return 0;
}

static int clock_proc_init(void)
{
	if (!proc_create_single("suse_debug_clocksource_show", 0, NULL, clock_proc_show))
		return -ENOMEM;

	return 0;
}


static int __init monitor_init(void)
{
	int rv;

	pr_info("SUSE_DEBUG: Loading. Watchdog: %s\n", cs_watchdog);

	rv = xavailable_clocksource_show();
	if (rv) {
		printk("SUSE_DEBUG: Error failed to resolve clocksource symbols%d\n", rv);
		return -ENOENT;
	}

	if (clock_proc_init()) {
		printk("SUSE_DEBUG: Error failed to proc entry%d\n", rv);
		return -ENOMEM;
	}

	rv = detector_init();
	if (rv) {
		remove_proc_entry("suse_debug_clocksource_show", NULL);

                printk("SUSE_DEBUG: Error failed to init detector %d\n", rv);
                return -ENOENT;
        }
	
	clocksource_start_detector();


	pr_info("SUSE_DEBUG: Loaded.\n");
/*
	kp.pre_handler = handler_pre;
	// kp.post_handler = handler_post;
	ret = register_kprobe(&kp);
	if (ret < 0) {
		pr_err("register_kprobe failed, returned %d\n", ret);
		return ret;
	}
	pr_info("Planted kprobe at %p\n", kp.addr);
	my_kretprobe.kp.symbol_name = func_name;
        ret = register_kretprobe(&my_kretprobe);
        if (ret < 0) {
                pr_err("register_kretprobe failed, returned %d\n", ret);
                return ret;
        }
        pr_info("Planted return probe at %s: %p\n",
                        my_kretprobe.kp.symbol_name, my_kretprobe.kp.addr);
*/
	return 0;
}

static void __exit monitor_exit(void)
{
	remove_proc_entry("suse_debug_clocksource_show", NULL);
	clocksource_stop_detector();
/*
	unregister_kprobe(&kp);
	pr_info("kprobe at %p unregistered\n", kp.addr);
	        unregister_kretprobe(&my_kretprobe);
        pr_info("kretprobe at %p unregistered\n", my_kretprobe.kp.addr);

        pr_info("Missed probing %d instances of %s\n",
                my_kretprobe.nmissed, my_kretprobe.kp.symbol_name);
	*/
	pr_info("SUSE_DEBUG: Unloaded.\n");
}

module_init(monitor_init)
module_exit(monitor_exit)
MODULE_LICENSE("GPL");
