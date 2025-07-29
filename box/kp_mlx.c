#define pr_fmt(fmt) "kp_mlx %s: " fmt, __func__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <trace/events/module.h>
#include <linux/umh.h>
#include <linux/tracehook.h>
#include <linux/task_work.h>
#include <linux/kallsyms.h>
#include <linux/sched.h>
#include <linux/delay.h>

#define MAX_SYMBOL_LEN  64
static char symbol0[MAX_SYMBOL_LEN] = "mlx5_core";
//static char symbol0[MAX_SYMBOL_LEN] = "kp_d";
static char symbol1[MAX_SYMBOL_LEN] = "mlx5e_update_carrier";
//static char symbol2[MAX_SYMBOL_LEN] = "__x64_sys_setdomainname";
static char symbol2[MAX_SYMBOL_LEN] = "async_event";
module_param_string(symbol0, symbol0, sizeof(symbol0), 0644);
module_param_string(symbol1, symbol1, sizeof(symbol1), 0644);
module_param_string(symbol2, symbol2, sizeof(symbol2), 0644);

static struct callback_head *kp_work_exited;
int kp_task_work_add(struct task_struct *task, struct callback_head *work,
                  enum task_work_notify_mode notify)
{
        struct callback_head *head;

        /* record the work call stack in order to print it in KASAN reports */
        //kasan_record_aux_stack(work);

        do {
                head = READ_ONCE(task->task_works);
                if (unlikely(head == kp_work_exited))
                        return -ESRCH;
                work->next = head;
        } while (cmpxchg(&task->task_works, head, work) != head);

        switch (notify) {
        case TWA_NONE:
                break;
        case TWA_RESUME:
                set_notify_resume(task);
                break;
		/*
        case TWA_SIGNAL:
                set_notify_signal(task);
                break;
		*/
        default:
                WARN_ON_ONCE(1);
                break;
        }
        return 0;
}

static void umh_gcore(struct callback_head *work)
{
	int rv;
	int count = 0;
	struct subprocess_info *sub_info;
	char pid_str[17];
	char *argv[3];
	char *envp[] = {
		"HOME=/",
		"TERM=linux",
		"PATH=/sbin:/bin:/usr/sbin:/usr/bin",
		NULL 
	};
	snprintf(pid_str, 16, "%d", task_pid_nr(current));
	pid_str[16] = '\0';
	argv[0] = "/usr/bin/gcore";
	argv[1] = pid_str;
	argv[2] = NULL;

	//char *argv[] = { "/usr/bin/gcore", pid_str, NULL };

	pr_info("Task %s PID %d state %u ptrace %u parent %s\n",
			current->comm, task_pid_nr(current), current->__state,
		       	current->ptrace, current->parent->comm);

	pr_info("UMH start gcore %s PID %d , argv[0] %s argv[1] %s \n", 
			current->comm, task_pid_nr(current), argv[0], argv[1]);

	sub_info = call_usermodehelper_setup(argv[0], argv, envp, GFP_KERNEL,
					NULL, NULL, NULL);
	if (sub_info == NULL) {
		pr_info("UMH setup gcore failed\n");
		return;
	 }
	rv = call_usermodehelper_exec(sub_info, UMH_WAIT_EXEC);
	//rv = call_usermodehelper_exec(sub_info, UMH_WAIT_PROC);

	pr_info("UMH gcore exec completed with rv %d, taskinfo: PID %d %s state %u ptrace %u parent %s\n",
			rv, task_pid_nr(current), current->comm, current->__state,
		       	current->ptrace, current->parent->comm);

	// Timeout 10 seconds!
	while (count < 250) { 
		count++;
		if (!current->ptrace) {
			msleep(40);
		} else {
			pr_info("After %d ms, GDB finnally ptraced %s PID %d state %u ptrace %u parent %s\n", count * 40,
			current->comm, task_pid_nr(current), current->__state,
		       	current->ptrace, current->parent->comm);
			count = 0;
			break;
		}
	}
	if (count != 0)
		pr_info("10 sconds later, GDB hasn't ptraced %s PID %d state %u ptrace %u parent %s\n",
                        current->comm, task_pid_nr(current), current->__state,
                        current->ptrace, current->parent->comm);
}

struct callback_head gcore_work;
static int queue_gcore_task_work(void)
{
	init_task_work(&gcore_work, umh_gcore);
	kp_task_work_add(current, &gcore_work, TWA_RESUME);
	return 0;
}

static int entry_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{
	WARN_ON(1);
        pr_info("mlx5_core: %d %s\n", task_pid_nr(current), current->comm);
	queue_gcore_task_work();
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
int kp_num = 2;
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
	char *func_name = "work_exited";
	kp_work_exited = (struct callback_head *)kallsyms_lookup_name(func_name);
        if (!kp_work_exited) {
                pr_info("cannot find address for kernel function %s\n",
                        func_name);
                return -EINVAL;
        }

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

