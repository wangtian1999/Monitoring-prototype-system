#CROSS_COMPILE = arm-none-linux-gnueabi-
CC=$(CROSS_COMPILE)gcc
AR=$(CROSS_COMPILE)ar
RANLIB=$(CROSS_COMPILE)ranlib
LIBS=-L./ -lcgic

MODULE_NAME = led_drv
MODULE_NAME1 = buzzer_drv
MODULE_NAME2 = adc_drv
MODULE_NAME3 = mpu6050_drv

ifeq ($(KERNELRELEASE), )

NFSBOA_CGIBIN=~/source/rootfs/boa/cgi-bin

all: libcgic.a login.cgi led_drv led.cgi buzzer.cgi  buzzer_drv\
	  refresh_env1.cgi	phone.cgi fen.cgi adc_drv mpu6050_drv
	

libcgic.a: cgic.o cgic.h
	rm -f libcgic.a
	$(AR) rc libcgic.a cgic.o
	$(RANLIB) libcgic.a
led.cgi: led.o libcgic.a
	$(CC) led.o -o led.cgi ${LIBS}
buzzer.cgi:buzzer.o libcgic.a
	$(CC) buzzer.o -o buzzer.cgi ${LIBS}
login.cgi: login.o libcgic.a
	$(CC) login.o -o login.cgi ${LIBS}
refresh_env1.cgi: refresh_env1.o libcgic.a
	$(CC) refresh_env1.o -o refresh_env1.cgi ${LIBS}	
phone.cgi:	phone.o libcgic.a
	$(CC) phone.o -o phone.cgi ${LIBS}	
fen.cgi: fen.o  libcgic.a
	$(CC) fen.o -o fen.cgi ${LIBS}	
	
	
CUR_DIR = $(shell pwd)

KERNEL_DIR = /home/wu/linux-3.14/linux-3.14-fs4412/

buzzer_drv:
	make -C $(KERNEL_DIR) M=$(CUR_DIR) modules
led_drv: 
	make -C $(KERNEL_DIR) M=$(CUR_DIR) modules	
mpu6050_drv:
	make -C $(KERNEL_DIR) M=$(CUR_DIR) modules
adc_drv:
	make -C $(KERNEL_DIR) M=$(CUR_DIR) modules
install:
#	sudo cp *.cgi $(NFSBOA_CGIBIN)
	cp *.ko ~/source/rootfs/drv/
	
	sudo cp *.cgi /boa/cgi-bin/ 
.PHONY:clean	
clean:
	
	make -C  $(KERNEL_DIR) M=$(CUR_DIR) clean
	rm -f *.ko *.a *.cgi
else

obj-m += $(MODULE_NAME).o
obj-m += $(MODULE_NAME1).o
obj-m += $(MODULE_NAME2).o
obj-m += $(MODULE_NAME3).o
endif
