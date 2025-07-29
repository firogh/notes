#define pr_fmt(fmt) "pte_page %s: " fmt, __func__
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <asm/pgtable_64_types.h>
#include <linux/pgtable.h>
#include <asm-generic/memory_model.h>
#include <asm/pgtable.h>


#define ENTRY_MASK              (~0xfff0000000000fffULL)
static int __init kprobe_init(void)
{
        struct mm_struct *mm = current->mm;
        unsigned long addr =  0xffff800000000000;
        int i = 0, pfn = 0;
        unsigned long entry;
        struct page *p;

        for (i = pgd_index(addr); i < 512; i++) {
                entry = pgd_val(*(mm->pgd + i));
                if (entry  != 0) {
                        pr_info("l5 paging: %d mm: %px  pgd: %px index: %d pgd entry: %lx\n", 
				pgtable_l5_enabled(), current->mm, mm->pgd, i, entry);
                        pfn = (entry &ENTRY_MASK) >> 12;
                        break;
		}
        }

        if (pfn) {
                p = pfn_to_page(pfn);
                pr_info("pfn: %d page flags: %lx maping %px\n", pfn, p->flags, p->mapping);
                p->flags = 0;
                p->mapping = 1;
                pr_info("modified pfn: %d page flags: %lx maping %px\n", pfn, p->flags, p->mapping);
        }

        return 0;
}

static void __exit kprobe_exit(void)
{
        pr_info("pte_page loaded\n");

}

module_init(kprobe_init)
module_exit(kprobe_exit)
MODULE_LICENSE("GPL");
