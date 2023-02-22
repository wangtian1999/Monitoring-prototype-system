
#include "data_global.h"
#include <strings.h>
#include "linuxuart.h"

int usb1_fd;
int card;

extern struct env_info_client_addr  sm_all_env_info;

void *pthread_rfid(void *arg)
{

	//打开设备文件
	int home_id = 1;
	usb1_fd = open_port("/dev/ttyUSB0");
	if(usb1_fd == -1){
		printf("open rfid failed.\n");
	}
	set_com_config(usb1_fd,115200,8,'N',1);

	while(1){
		char buf1[8];
		usleep(500000);
		read(usb1_fd,&buf1,sizeof(buf1));
		usleep(500000);
		printf("buf1 ===== %s\n",buf1);

		int a,b,c,d;
		a = buf1[0]-48;
		b = buf1[1]-48;
		c = buf1[3]-48;
		d = buf1[4]-48;
		card = a*1000 + b*100 + c*10 + d;
		printf("card = %d\n",card);

	}
	
	close(usb1_fd);
}
