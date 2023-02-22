#include "data_global.h"
#include "linuxuart.h"



extern pthread_mutex_t mutex_process;
extern pthread_cond_t  cond_process;

void * pthread_process(void * arg)
{
	printf("--------%s-------------\n",__FUNCTION__);
	int fd;
	char temp_buf[7]={0};
	fd = open_port("/dev/ttyUSB0");
	if(fd < 0)
	{
		perror("open failed\n");
		return NULL;
	}
	set_com_config(fd,115000,8,'N',1);
	while(1)
	{
		pthread_mutex_lock(&mutex_process);
		write(fd,temp_cmb,QUEUE_MSG_LEN);
		read(fd,temp_buf,sizeof(temp_buf));
		printf("temp_buf is %s\n",temp_buf);
		pthread_mutex_unlock(&mutex_process);
		pthread_cond_wait(&cond_process,&mutex_process);

	}




}
