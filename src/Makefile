MODULE_NAME := netfilter
RESMAN_CORE_OBJS := main.o
RESMAN_GLUE_OBJS := netFilter.o dealConf.o netLink.o
$(MODULE_NAME)-objs := $(RESMAN_CORE_OBJS) $(RESMAN_GLUE_OBJS)

obj-m := netfilter.o

PWD := $(shell pwd)
KVER ?= $(shell uname -r)
KDIR := /lib/modules/$(KVER)/build

defalut:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	cp netfilter.ko netfilter.tmp
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	mv netfilter.tmp netfilter.ko
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
