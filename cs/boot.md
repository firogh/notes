---
tags: [ cs ] 
title: System booting
date: 2015-11-12
category: cs
---

# Reference
[Article: Introducing initramfs, a new model for initial RAM disks](http://linuxdevices.linuxgizmos.com/introducing-initramfs-a-new-model-for-initial-ram-disks-a/)

# Contents
What basic initializations should be perfomed when power on?
How do we load the kernel to memory?
What does kernel do when kernel boot.
MBR vs BIOS?
PC vs embedded
Kernel init 
before start_kernel
start_kernel
reset_init & kernel_init

# p4080 Board
PBL 
1. initialize I2C, SPI, eLBC, eSDHC.
2. Load RCW and boot init commands from above.
3. write date to conf register and memory
# config
CCSRBAR -> CCSR 
default is 0x0_FE00_0000  
CCSRBAR is in CCSR memory. odd. stored in 0(CCSRBAR).
CCSRBAR always points to itself.
holdoff
# u-boot
ft_fixup_cpu -> determine_mp_bootpg(NULL) & fdt_add_mem_rsv

main_loop-> ...->./common/bootm_os.c:410:... do_bootm_states-> 
{
bootm_find_other->bootm_find_ramdisk_fdt->bootm_find_fdt->boot_get_fdt&set_working_fdt_addr(0x0200000)->IMAGE_FORMAT_FIT->
&
boot_fn = bootm_os_get_boot_func(images->os.os)=[IH_OS_LINUX] = 
&
boot_selected_os->boot_fn=do_bootm_linux->boot_body_linux->image_setup_linux->image_setup_libfdt->
board/freescale/corenet_ds/corenet_ds.c ft_board_setup-> ft_cpu_setup->
}

# PPC multicore booting
* There are two possibilites to make secondary cores booting failed.
1. There is a bug in the code executed by Secondary cores
2. The spin table address, passed to croe0, is not correct, so we did not really kick the secondary cores.

CCSRBAR
MPC86xx_MCM_OFFSET
srio_boot_master_release_slave

uboot->
set the cpu-release-addr at funciton ft_fixup_cpu(), I am not sure.

core0 run, 2nd cores holdoff
core0 set BPTR and kick(BRR) 2nd cores, 2nd cores spintable loop in uboot function setup_mp
core0 -> arch/powerpc/cpu/mpc85xx/start.S->_start_e500
board_init_r->cpu_init_r-> setup_mp-> {
__bootpg_addr = (u32)virt_to_phys(&__second_half_boot_page); //Assembly code for put core to spin tablea. Used by __secondary_start_page
__spin_table_addr = (u32)get_spin_phys_addr();// used by __second_half_boot_page 
bootpg = BPTR = __secondary_start_page
plat_mp_up->kick BRR & Boot Space Translation
}
2nd cores holdoff -> __secondary_start_page
kernel->
core0 boot kernel, core0 kickoff 2nd cores spintable, triger 2nd core's spin table by writing  the spin table field with the desired address
2nd cores ePAPR->spintable->addr_l=__early_start->__secondary_start->smp_ops->setup_cpu->cpu_idle
arch/powerpc/kernel/head_fsl_booke.S
U-boot logs:
Reserving MP boot page to 7ffff000^M^M - --   -   MP spin table
address.
SPAG : fdt_fixup_fman_firmware, 567^M^M
^M^M
..r.^M^M
VDBG : ft_cpu_setup, 737, comment fdt_fixup_memory ^M^M
VDBG: Secondary cores are not held in reset.^M^M
Kernel Logs:
smp_85xx_kick_cpu: timeout waiting for core 1 to ack^M^M
smp: failed starting cpu 1 (rc -2)^M^M
smp_85xx_kick_cpu: timeout waiting for core 2 to ack^M^M
smp: failed starting cpu 2 (rc -2)^M^M
smp_85xx_kick_cpu: timeout waiting for core 3 to ack^M^M
smp: failed starting cpu 3 (rc -2)^M^M
Brought up 1 CPUs^M^M
devtmpfs: initialized^M^M

Note that u-boot can't detect  memory above 2G and we need to check
if kernel is accessing at high mem of 2G or 4G.


