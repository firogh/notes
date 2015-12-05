---
tags: kernel
title: Console and TTY
date: 2015-12-05 14:06:29
category: kernel
---

# Reference

# Contents
[What is the difference between shell, console, and terminal?](http://superuser.com/questions/144666/what-is-the-difference-between-shell-console-and-terminal)
What does console do in kernel or u-boot?
Linux console?
Computer terminal: keyboard + dispaly
Terminal emulator:

# get a glance on u-boot
start.S:board_init_r
init_sequence_f: ->init_baud_rate; serial_init; console_init_f
serial_init ->&eserial1_device->start=eserial##port##_init->NS16550_init: UART divisor init.
Firstly, the struct of serial define some input and output funtion.
It's basic function of serial, put or get!
console_init_f: just gd->have_console = 1;
init_sequence_r: stdio_init_tables,initr_serial, stdio_add_devices, console_init_r,
initr_serial: just register &eserial1_device to serial_devices
stdio_add_devices: drv_system_init, serial_stdio_init 
drv_system_init: register default serial dev to devs.list.
serial_stdio_init: register &eserial1_device to devs.list. Duplicate, but serial dev "eserial0"  and system "serial".
console_init_r: console_doenv ->console_setfile:stdio_devices[file(0/1/2)] = dev; actually, dev is "serial", but they may be KBD!
看来console的真正作用就是在serial和kbd中选择一个, 可能多选iomux? 
main_loop:cli_loop: getc!

# kernel part
* onset
start_kernel or setup_arch(arm)->parse_early_param->do_early_param->p->setup_func()= setup_early_printk->register_console
nucleus
* early_printk
# just print
console_drivers->vt_console_driver->serial8250_console->NULL
printk-> ...->__call_console_drivers-> console_drivers->write = vt_console_print 
{
	//保存到screen buf, vga_con也什么不做啊.
	scr_writew((vc->vc_attr << 8) + c, (unsigned short *)vc->vc_pos);

}
# What about console?
start_kerenl-> 
{
	// All about vga console
	set_arch-> conswitchp = &vga_con; or conswitchp = &dummy_con; 
	console_init->
	{
		con_init->
		{
			// vc->vc_sw->con_putcs is DUMMY
			//内存映射64KB or 32KB的VGA区域. 启动VGA
			conswitchp->con_startup = vgacon_startup ->vga_vram_base = VGA_MAP_MEM(vga_vram_base, vga_vram_size);
			con_driver_map[0~MAX_NR_CONSOLES] = conswitchp; //空间换时间
			// 核心内容!
			for (currcons = 0; currcons < MIN_NR_CONSOLES; currcons++) {
				// 给vc_cons[currcons].d分配内存
				vc_cons[currcons].d = vc = kzalloc(sizeof(struct vc_data), GFP_NOWAIT);
				INIT_WORK(&vc_cons[currcons].SAK_work, vc_SAK);
				//初始化vc_cons[currcons].d
				tty_port_init(&vc->port);
				// 继续初始化, 主要是确定screenbuf size
				visual_init(vc, currcons, 1);
				// 给vc_screenbuf分配内存
				vc->vc_screenbuf = kzalloc(vc->vc_screenbuf_size, GFP_NOWAIT);
				vc_init(vc, vc->vc_rows, vc->vc_cols,
					currcons || !vc->vc_sw->con_save_screen);
			}

			[    0.000000] Console: colour VGA+ 80x25
			register_console(&vt_console_driver);//vt_console can use vgacon writing.
		}
		serial8250_console_init->register_console(&serial8250_console) to console_drivers; exclusive_console.
		[    0.000000] console [tty0] enabled
	}
	module_init(serial8250_init);??
}
* fbcon
register_framebuffer-> do_take_over_console ->
{
	do_register_con_driver->csw->con_startup();registered_con_driver
	do_bind_con_driver ->
	{
		[    3.882220] Console: switching to colour dummy device 80x25
		[    4.720732] Console: switching to colour frame buffer device 170x48
	}
}

# What about tty
* onset
console_init->tty_ldisc_begin->tty_register_ldisc(N_TTY, &tty_ldisc_N_TTY);
N_TTY:[Serial Drivers by Alessandro Rubini](http://www.linux.it/~rubini/docs/serial/serial.html)
fs_initcall:chr_dev_init->drivers/tty/tty_io.c: tty_init->
{
	
	/* /dev/tty0 = /dev/console console_ops */
	/* /dev/tty = the /dev/ttyN whererun echo /dev/tty tty_ops */

	//"/dev/tty",
	cdev_init(&tty_cdev, &tty_fops);
	"/dev/console"
	cdev_init(&console_cdev, &console_fops);
	vty_init->
	{
		//"dev/tty0"	
		cdev_init(&vc0_cdev, console_fops);	
		//"/dev/ttyN"
		tty_register_driver->tty_register_device(_attr) ->tty_cdev_add-> cdev_init(&driver->cdevs[index], &tty_fops);
	}
}
device_init:serial8250_init-> 
{
	// In this function we decide "/dev/ttyS*"
	// dmesg |grep Serial
	// [    0.696341] Serial: 8250/16550 driver, 32 ports, IRQ sharing enabled
	// serial8250.c -> tty_io.c
	serial8250_reg.nr = UART_NR;
	ret = uart_register_driver(&serial8250_reg);
	tty_driver set to uart_driver by uart_register_driver ->
	{
		drv->state = kzalloc		//uart_state
		normal->driver_state    = drv; //args struct uart_driver *drv = &serial8250_reg
		tty_set_operations(normal, &uart_ops); 
		struct tty_port *port = &state->port
		tty_port_init(port);
		port->ops = &uart_port_ops; //tty_port
		// We register "/dev/ttyS*" files here.
		static struct uart_driver serial8250_reg = {
			.owner                  = THIS_MODULE,
			.driver_name            = "serial",
			.dev_name               = "ttyS",
			.major                  = TTY_MAJOR,
			.minor                  = 64,
			.cons                   = SERIAL8250_CONSOLE,
		};
		retval = tty_register_driver(normal); -> register_chrdev_region(dev, driver->num, driver->name) //32, ttyS?*? should be tty_ops

	}
	serial8250_register_ports(&serial8250_reg, &serial8250_isa_devs->dev);
}
* nucleus
start_kernel->rest_init->kernel_init->kernel_init_freeable->sys_open((const char __user *) "/dev/console", O_RDWR, 0)->...->
tty_open->
{
	//This index should be Ctrl + Alt + Fn, Dio!
	// tty_struct is corresponding virtual console, or just console??
	tty_lookup_driver->console_device->c->device(c, index)
	tty_init_dev->
	{
		//So tty_struct->ops = tty_driver->ops = & uart_ops
		alloc_tty_struct->tty->ops = driver->ops;
		tty_driver_install_tty(driver, tty_struct)-> tty_standard_install->driver->ttys[tty->index] = tty;
	}

	tty->ops->open(tty, filp)->//ops = &uart_ops
	{
		uart_ops->open = uart_open->
		{
			uart-> tty struct
			struct uart_driver *drv = (struct uart_driver *)tty->driver->driver_state;
			struct uart_state *state = drv->state + line; //uart_state
			tty->driver_data = state;
		}
	}
}
tty_write->ld->ops->write=n_tty_write->(tty_struct tty->ops->write)=uart_write->
{
	struct uart_state *state = tty->driver_data;
	port = state->uart_port;
	circ = &state->xmit;
	memcpy(circ->buf + circ->head, buf, c);
	uart_start->__uart_start->(uart_port->ops->start_tx(port)); //&uart_port_ops ?? uart_ops??
}


# device files

# Backup
./drivers//tty/vt/vt.c:3042:	    register_chrdev_region(MKDEV(TTY_MAJOR, 0), 1, "/dev/vc/0") < 0)
./drivers//tty/vt/vc_screen.c:644:	if (register_chrdev(VCS_MAJOR, "vcs", &vcs_fops))
./drivers//tty/tty_io.c:3377:		error = register_chrdev_region(dev, driver->num, driver->name);
./drivers//tty/tty_io.c:3414:	unregister_chrdev_region(dev, driver->num);
./drivers//tty/tty_io.c:3430:	unregister_chrdev_region(MKDEV(driver->major, driver->minor_start),
./drivers//tty/tty_io.c:3607:	    register_chrdev_region(MKDEV(TTYAUX_MAJOR, 0), 1, "/dev/tty") < 0)
./drivers//tty/tty_io.c:3613:	    register_chrdev_region(MKDEV(TTYAUX_MAJOR, 1), 1, "/dev/console") < 0)
./drivers//tty/pty.c:841:	    register_chrdev_region(MKDEV(TTYAUX_MAJOR, 2), 1, "/dev/ptmx") < 0)

