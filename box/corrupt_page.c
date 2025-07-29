#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <asm-generic/percpu.h>


static int __init clear_page_flags_init(void)
{
    unsigned long addr;
    struct page *page;

    unsigned long start_addr = __per_cpu_offset[0];
    unsigned long end_addr = __per_cpu_offset[1];     // Set virtual end address
                                                       //
    pr_info("SUSE clear_page_flags_init %lx %lx %lx\n", (unsigned long) __per_cpu_offset, start_addr, end_addr);
    for (addr = start_addr; addr < end_addr; addr += PAGE_SIZE) {
//    for (addr = start_addr; addr < 0; addr += PAGE_SIZE) {
        page = virt_to_page((void *)addr);  // Convert virtual to page struct
        if (!page)
            continue;

        printk("SUSE Cleared flags for page at virtual address: 0x%lx %lx %lx", addr, page->flags, (unsigned long)page->mapping);
        page->flags = 0;  // Clear the flags
        page->mapping = 1;
        printk("  %lx %lx\n", page->flags, (unsigned long)page->mapping);
    }

    return 0;
}

static void __exit clear_page_flags_exit(void)
{
    pr_info("SUSE Module exited\n");
}

module_init(clear_page_flags_init);
module_exit(clear_page_flags_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Firo Yang");
