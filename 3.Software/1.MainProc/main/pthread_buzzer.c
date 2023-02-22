#include "data_global.h"
#include "mybeep.h"

extern pthread_mutex_t mutex_buzzer;
extern pthread_cond_t cond_buzzer;

//:A9蜂鸣器控制线程.
void *pthread_buzzer(void *arg)
{
	printf("pthread_buzzer\n");

	int fd = open("/dev/mybeep", O_RDWR);
	if(fd < 0)
	{
		printf("open fd error!\n");
		exit(-1);
	}

	while(1)
	{

		pthread_mutex_lock(&mutex_buzzer);
		pthread_cond_wait(&cond_buzzer, &mutex_buzzer);
		printf("buzzer_cmb = %x\n", buzzer_cmb[0]);
		if(buzzer_cmb[0] == 0x51)
		{
			ioctl(fd, MY_BEEP_ON);
		}else if(buzzer_cmb[0] == 0x50){
			ioctl(fd, MY_BEEP_OFF);
		}else {
			printf("buzzer input error\n");
		}
		pthread_mutex_unlock(&mutex_buzzer);
		sleep(2);
		printf("***************** success *****************\n");
	}
	

#if 0	
	1.	open(dev_buzzer,  )
	2.	pthread_cond_wait (cond_buzzer,  );
	3.	获取dev_buzzer_mask(控制标志)
	4.	通过ioctl（）控制buzzer
#endif 
}



