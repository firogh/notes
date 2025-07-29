#define pr_fmt(fmt) "kp_mlx %s: " fmt, __func__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <trace/events/module.h>

#define MAX_SYMBOL_LEN  64
static char symbol0[MAX_SYMBOL_LEN] = "kp_d";
static char symbol1[MAX_SYMBOL_LEN] = "mlx5e_update_carrier";
static char symbol2[MAX_SYMBOL_LEN] = "kernel_clone";
//static char symbol2[MAX_SYMBOL_LEN] = "async_event";
module_param_string(symbol0, symbol0, sizeof(symbol0), 0644);
module_param_string(symbol1, symbol1, sizeof(symbol1), 0644);
module_param_string(symbol2, symbol2, sizeof(symbol2), 0644);

static int entry_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{
//      WARN_ON(1);
        pr_info("kernel clone\n");
        return 0;
}

static struct kretprobe rp1 = {
        .entry_handler  = entry_handler,
        .kp.symbol_name = symbol2
};
static struct kretprobe rp2 = {
        .entry_handler  = entry_handler,
        .kp.symbol_name = symbol1
};

int kp_initialized = 0;
int kp_num = 1;
struct kretprobe *rps[2] = {&rp1, &rp2};
static void trace_module_load_hit(void *ignore, struct module *mod)
{
        int ret;

        pr_info("Loading module %s\n", mod->name);

        if (strcmp(mod->name, symbol0))
                return;

        ret = register_kretprobes(rps, kp_num);
        if (ret < 0) {
                pr_err("register_kretprobe returned %d\n", ret);
                return;
        }
        pr_info("Loading module %s\n", mod->name);
        kp_initialized = 1;
        return;
}

static void register_tracepoints(struct tracepoint *tp, void *ignore)
{
        if (!strcmp(tp->name, "module_load"))
                WARN_ON(tracepoint_probe_register(tp, trace_module_load_hit, NULL));
}
static void unregister_tracepoints(struct tracepoint *tp, void *ignore)
{
        if (!strcmp(tp->name, "module_load"))
                tracepoint_probe_unregister(tp, trace_module_load_hit, NULL);
}

static int __init kprobe_init(void)
{
        for_each_kernel_tracepoint(register_tracepoints, NULL);
        pr_info("Planted kprobe \n");
        return 0;
}

static void __exit kprobe_exit(void)
{
        if (kp_initialized)
                unregister_kretprobes(rps, kp_num);
        for_each_kernel_tracepoint(unregister_tracepoints, NULL);
        pr_info("kprobe unregistered\n");
}

module_init(kprobe_init)
module_exit(kprobe_exit)
MODULE_LICENSE("GPL");

