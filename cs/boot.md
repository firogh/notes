---
tags: [ cs ] 
title: System booting
date: 2015-11-12
category: cs
---

# Debug
efi=debug earlyprintk=efi,keep earlycon=efifb keep_bootcon" and no "quiet" 


# SMP boot
Check SDM v3 BSP and AP Processors
BSP - boot strap processor； AP - application processor
## Core commit
setup_real_mode-> trampoline_header->start = (u64) secondary_startup_64;
commit f37240f16bec91f15ce564515f70a6ca9715ce96
Author: Jarkko Sakkinen <jarkko.sakkinen@intel.com>
Date:   Tue May 8 21:22:43 2012 +0300
    x86, realmode: header for trampoline code
## BSP
### Build time for real_mode_header in arch/x86/realmode/rm/header.S
In pasyms.h, git gud!
pa_trampoline_header = trampoline_header;
pa_trampoline_start = trampoline_start;
pa_startup_32 = startup_32;
pa_startup_64 = startup_64;
### Early init
setup_real_mode-> trampoline_header->start = (u64) secondary_startup_64;  # tr_start
### start_kernel
kernel_init->smp_init->cpu_up->do_cpu_up->_cpu_up->
ap hp threadfn -> bringup_cpu -> __cpu_up -> smp_ops.cpu_up(cpu, tidle) is native_cpu_up
        do_boot_cpu is the core function. It set up the code for APs to run and check cpu_callin_mask.
        start_eip = real_mode_header->trampoline_start;
        initial_code = (unsigned long)start_secondary                   # initial_code
## AP
trampoline_start -> ... -> startup_64 -> tr_start(%rip) is secondary_startup_64 -> initial_code(%rip) is start_secondary
-> cpu_init

# Memory
arch/x86/kernel/head_64.S init_top_pgt

break buffer see alloc_low_buffer
static void __init memory_map_top_down(unsigned long map_start,
                                       unsigned long map_end)
{
        unsigned long real_end, start, last_start;
        unsigned long step_size;
        unsigned long addr;
        unsigned long mapped_ram_size = 0; 

        /* xen has big range in reserved near end of ram, skip it at first.*/
        addr = memblock_find_in_range(map_start, map_end, PMD_SIZE, PMD_SIZE);
        real_end = addr + PMD_SIZE;

        /* step_size need to be small so pgt_buf from BRK could cover it */
        step_size = PMD_SIZE;
        max_pfn_mapped = 0; /* will get exact value next */
        min_pfn_mapped = real_end >> PAGE_SHIFT;
        last_start = start = real_end;



# initrd
related code:
reserve_initrd
subsys_initcall(genhd_device_init);->kobj_map_init{bdev_map.probe.get = base_probe}
subsys_initcall(init_scsi);4->scsi_sysfs_register{autoprobe = 1;} 
rootfs_initcall(populate_rootfs);->{unpack_to_rootfs; 解压initramfs到rootfs}
module_initinit_sd;6->scsi_register_driver ->driver_register->bus_add_driver ->driver_attach ->driver_probe_device-> drv->probe(dev)=sd_probe_async->add_disk -> register_disk -> get_gendisk -> kobj_lookup { bdev_map.probe.get()=base_probe(){request_module}}
# vs initramfs
initrd is image with specific fs type, like ext2, need driver built-in kernel.
initramfs is a cpio, like tar only simpler, populated to rootfs in kernel, with fs type rootfs

# root device

Related code:
root= name_to_dev_t, mount_root in prepare_namespace

# kernel boot process
Documentation/x86/boot.txt
[The Kernel Boot Process](https://manybutfinite.com/post/kernel-boot-process/)
arch/x86/boot/header.S::start_of_setup
arch/x86/boot/main.c::main()
	arch/x86/boot/memory.c::detect_memory()
	arch/x86/boot/memory.c::detect_memory_e820() = boot_params.e820_entries
	arch/x86/boot/pm.c::go_to_protected_mode()
arch/x86/boot/pmjump.S::protected_mode_jump
arch/x86/kernel/compressed/head_64.S::startup_32
arch/x86/kernel/compressed/head_64.S::startup_64
arch/x86/kernel/head_64.S::startup_64
kernel/main.c::start_kernel()
## Reloctaion for decompress
974f221c84b05b1dc2f5ea50dc16d2a9d1e95eda
x86/boot: Move compressed kernel to the end of the decompression buffer
## Horrable commit description on phys_base
commit 1ab60e0f72f71ec54831e525a3e1154f1c092408
Author: Vivek Goyal <vgoyal@in.ibm.com>
Date:   Wed May 2 19:27:07 2007 +0200
    [PATCH] x86-64: Relocatable Kernel Support
