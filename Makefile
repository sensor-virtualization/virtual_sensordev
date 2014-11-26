obj-m += virtual_sensordev.o
PWD := $(shell pwd)

CC := $(CROSS_COMPILE)gcc

all:
	$(MAKE) -C $(KDIR) M=${shell pwd} modules

clean:
	$(MAKE) -C $(KDIR) M=${shell pwd} clean || true
	-rm .v* *.o *.ko *.mod.c *.mod.o modules.order Module.symvers || true

