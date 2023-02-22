#交叉编译工具链
#CROSS_COMPILE = arm-none-linux-gnueabi-

CC=$(CROSS_COMPILE)gcc

#编译时的参数，可用于调试
CFLAGS= -c -g
#需要链接的库
LDFLAGS= -lpthread        				


OBJS +=  main.o  data_global.o pthread_process.o   linuxuart.o
OBJS += pthread_client_request.o pthread_buzzer.o pthread_tube.o\
		pthread_sqlite.o pthread_refresh.o pthread_cloud.o pthread_fan.o

main_frame :$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	mv  *o ./obj
$(OBJS):%.o:%.c
	$(CC) $(CFLAGS) $< -o $@

install:
	sudo cp main_frame    ~/source/rootfs/ 

.PHONY:clean
clean:
	rm ./obj/*.o  *.o  main_frame -rf 
