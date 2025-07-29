#define pr_fmt(fmt) "kp_dummpy %s: " fmt, __func__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>

#define MAX_SYMBOL_LEN  64
//static char symbol[MAX_SYMBOL_LEN] = "module_load";
static char symbol[MAX_SYMBOL_LEN] = "x86_pmu_stop";
module_param_string(symbol, symbol, sizeof(symbol), 0644);

/* For each probe you need to allocate a kprobe structure */
static struct kprobe kp = {
        .symbol_name    = symbol,
};

/* kprobe pre_handler: called just before the probed instruction is executed */
static int __kprobes handler_pre(struct kprobe *p, struct pt_regs *regs)
{
#ifdef CONFIG_X86
        pr_info("<%s> p->addr = 0x%p, ip = %lx, flags = 0x%lx\n",
                p->symbol_name, p->addr, regs->ip, regs->flags);
#endif
        return 0;
}

static int __init kprobe_init(void)
{
        int ret;
        kp.pre_handler = handler_pre;
        //kp.post_handler = handler_post;

        ret = register_kprobe(&kp);
        if (ret < 0) {
                pr_err("register_kprobe failed, returned %d\n", ret);
                return ret;
        }
        pr_info("Planted kprobe at %p\n", kp.addr);

        return 0;
}

static void __exit kprobe_exit(void)
{
        unregister_kprobe(&kp);
        pr_info("kprobe at %p unregistered\n", kp.addr);

}

module_init(kprobe_init)
module_exit(kprobe_exit)
MODULE_LICENSE("GPL");
