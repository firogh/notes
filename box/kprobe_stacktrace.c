/*
 * NOTE: This example is works on x86 and powerpc.
 * Here's a sample kernel module showing the use of kprobes to dump a
 * stack trace and selected registers when _do_fork() is called.
 *
 * For more information on theory of operation of kprobes, see
 * Documentation/kprobes.txt
 *
 * You will see the trace data in /var/log/messages and on the console
 * whenever _do_fork() is invoked to create a new process.
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/perf_event.h>
#include <asm/unwind.h>

#define MAX_SYMBOL_LEN	64
static char symbol[MAX_SYMBOL_LEN] = "_do_fork";
module_param_string(symbol, symbol, sizeof(symbol), 0644);

/* For each probe you need to allocate a kprobe structure */
static struct kprobe kp = {
	.symbol_name	= symbol,
};

static inline int kp_callchain_store(struct perf_callchain_entry_ctx *ctx, u64 ip)
{
        if (ctx->nr < ctx->max_stack && !ctx->contexts_maxed) {
                struct perf_callchain_entry *entry = ctx->entry;
                entry->ip[entry->nr++] = ip;
                ++ctx->nr;
                return 0;
        } else {
                return -1; /* no more room, stop walking the stack */
        }    
}

static int kp_callchain(struct perf_callchain_entry_ctx *entry, struct pt_regs *regs)
{
	struct unwind_state state;
	unsigned long addr;

	if (kp_callchain_store(entry, regs->ip))
		return 0;

	for (unwind_start(&state, current, regs, NULL); !unwind_done(&state);
             unwind_next_frame(&state)) {
                addr = unwind_get_return_address(&state);
                if (!addr || kp_callchain_store(entry, addr))
                        return 2;
        } 

	return 1;
}
/* kprobe pre_handler: called just before the probed instruction is executed */
static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
	struct perf_callchain_entry_ctx ctx;
	struct perf_callchain_entry *entry;
	int i = 0, rv = 0;

	pr_info("<%s> pre_handler: p->addr = 0x%p, ip = %lx, flags = 0x%lx\n",
		p->symbol_name, p->addr, regs->ip, regs->flags);

	entry = kmalloc(256, GFP_KERNEL);
	if (!entry)
		return 0;

	ctx.entry     = entry;
        ctx.max_stack = 24;
        ctx.nr        = entry->nr = 0;
        ctx.contexts       = 0;
        ctx.contexts_maxed = false;
	rv = kp_callchain(&ctx,regs);

	if (rv) {
		i = entry->nr;
		printk("=> %d ", i);
		for (i = 0; i < entry->nr; i++) {
			printk("%llx %pF", entry->ip[i], (void *)entry->ip[i]);
		}
		printk("\n");
	}

	kfree(entry);
	/* A dump_stack() here will give a stack backtrace */
	return 0;
}

/*
 * fault_handler: this is called if an exception is generated for any
 * instruction within the pre- or post-handler, or when Kprobes
 * single-steps the probed instruction.
 */
static int handler_fault(struct kprobe *p, struct pt_regs *regs, int trapnr)
{
	pr_info("fault_handler: p->addr = 0x%p, trap #%dn", p->addr, trapnr);
	/* Return 0 because we don't handle the fault. */
	return 0;
}

static int __init kprobe_init(void)
{
	int ret;
	kp.pre_handler = handler_pre;
	kp.post_handler = NULL;//handler_post;
	kp.fault_handler = handler_fault;

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
