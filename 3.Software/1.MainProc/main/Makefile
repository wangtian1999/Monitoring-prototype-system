
CROSS_COMPILE=arm-linux-
CC=$(CROSS_COMPILE)gcc 
CFLAGS= -c -g
LDFLAGS= -lpthread -I /home/linux/Desktop/sqlite/include/  -L /home/linux/Desktop/sqlite/lib/ -lsqlite3
#LDFLAGS= -lpthread

OBJS=main.o data_global.o pthread_transfer.o \
	 pthread_client_request.o pthread_buzzer.o pthread_led.o\
	 pthread_sqlite.o \
	 pthread_refresh.o pthread_sms.o pthread_rfid.o pthread_fan.o linuxuart.o 

monitor_obj :$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	mv *o ./obj
$(OBJS):%.o:%.c
	$(CC) $(CFLAGS) $< -o $@

install:
	sudo cp monitor_obj /opt/4412/rootfs/

.PHONY:clean
clean:
	rm *.o monitor_obj -rf 

