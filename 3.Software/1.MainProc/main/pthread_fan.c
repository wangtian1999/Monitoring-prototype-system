#include "data_global.h"
#include "linuxuart.h"



extern pthread_mutex_t mutex_fan;
extern pthread_cond_t cond_fan;


void *pthread_fan(void *arg)
{
	printf("----------%s-------\n",__FUNCTION__);
	int fd = open_port("/dev/ttyUSB1");
	if(fd <0)
	{
		printf("open failed\n");
		return NULL;
	}
	set_com_config(fd,115200,8,'N',1);
	while(1)
	{
		pthread_mutex_lock(&mutex_fan);
		pthread_cond_wait(&cond_fan,&mutex_fan);
		printf("fan success!\n");
		if(fan_cmb[0] == 0x60)
		{
			write(fd,"11",2);
		}else if(fan_cmb[0] == 0x61)
		{
			write(fd,"00",2);
		}else {
			printf("fan input error!\n");
		}
		pthread_mutex_unlock(&mutex_fan);

	}




}
