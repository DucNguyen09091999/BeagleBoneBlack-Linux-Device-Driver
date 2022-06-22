
#make -C ${KDIR} M=$(PWD) clean
#beaglebone black /arm
KDIR := /home/ducnguyen/Working-Space/beagle-bone-black/bb-kernel/KERNEL
CDIR := /home/ducnguyen/Working-Space/beagle-bone-black/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin
PREFIX := arm-linux-gnueabihf-

#Host/pc
obj-m := chrdev.o
HDIR := /lib/modules/`uname -r`/build

all :
	make ARCH=arm CROSS_COMPILE=${CDIR}/${PREFIX} -C ${KDIR} M=$(PWD) modules
	${CDIR}/${PREFIX}gcc -o app app.c

clean :
	make ARCH=arm CROSS_COMPILE=${CDIR}/${PREFIX} -C ${KDIR} M=$(PWD) clean
	rm -rf app
help :
	make ARCH=arm CROSS_COMPILE=${CDIR}/${PREFIX} -C ${KDIR} M=$(PWD) help
	
host:
	make -C $(HDIR) M=$(PWD) modules
	gcc -o app app.c