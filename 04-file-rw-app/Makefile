# obj-m = fileop.o
# EXTRA_CFLAGS = -Wall
# KDIR = /lib/modules/`uname -r`/build

# all:
# 	make -C $(KDIR) M=`pwd` modules
# clean:
# 	make -C $(KDIR) M=`pwd` clean


EXTRA_CFLAGS = -Wall
obj-m += fileop.o

BBB_KERNEL := /home/ducnguyen/Working-Space/beagle-bone-black/bb-kernel/KERNEL/

TOOLCHAIN := /home/ducnguyen/Working-Space/beagle-bone-black/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin

PREFIX := arm-linux-gnueabihf-

HOST_KERN_DIR := /lib/modules/`uname -r`/build

all :
	make ARCH=arm CROSS_COMPILE=$(TOOLCHAIN)/$(PREFIX) -C $(BBB_KERNEL) M=$(shell pwd) modules
	$(TOOLCHAIN)/$(PREFIX)gcc -o interapp interapp.c

clean :
	make -C $(BBB_KERNEL) M=$(shell pwd) clean
	rm -rf interapp
host:
	make -C $(HOST_KERN_DIR) M=$(shell pwd) modules
	$(CC) -o interapp interapp.c