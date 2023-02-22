#include "data_global.h"
#include "linuxuart.h"



extern pthread_mutex_t mutex_fan;
extern pthread_cond_t cond_fan;


void *pthread_fan(void *arg)
{
	printf("----------%s-------\n",__FUNCTION__);
	int fd = open_port("/dev/ttyUSB0");
	if(fd <0)
	{
		printf("open failed\n");
		return NULL;
	}
	set_com_config(fd,115200,8,'N',1);
	while(1)
	{
		pthread_mutex_lock(&mutex_fan);
		write(fd,fan_cmb,sizeof(fan_cmb));
		pthread_mutex_unlock(&mutex_fan);
		pthread_cond_wait(&cond_fan,&mutex_fan);

	}




}
