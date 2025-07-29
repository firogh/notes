obj-m += kp_mlx.o kp_d.o #corrupt_page.o 
#clear_flags.o #ipvs.o #kp_ifaddr.o #susekp.o #kaperf.o #rdpmc.o kp_watchdog.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
