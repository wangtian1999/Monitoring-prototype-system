#include "data_global.h"
#include "leddrv.h"

extern pthread_mutex_t mutex_led;
extern pthread_cond_t cond_led;

// A9 led 模块线程.
void *pthread_led(void *arg)
{
	printf("pthread_led\n");

	int fd = open("/dev/myled2", O_RDWR);
	if(fd < 0)
	{
		printf("open fd error!\n");
		exit(-1);
	}

	while(1)
	{
		pthread_mutex_lock(&mutex_led);
		pthread_cond_wait(&cond_led, &mutex_led);
		printf("led_cmb = %x\n", led_cmb[0]);
		if(led_cmb[0] == 0x41)
		{
			ioctl(fd, MY_LED_ON, 2);
			ioctl(fd, MY_LED_ON, 3);
			ioctl(fd, MY_LED_ON, 4);
			ioctl(fd, MY_LED_ON, 5);
		} else if(led_cmb[0] == 0x40){
			ioctl(fd, MY_LED_OFF, 2);
			ioctl(fd, MY_LED_OFF, 3);
			ioctl(fd, MY_LED_OFF, 4);
			ioctl(fd, MY_LED_OFF, 5);		
		}else {
			printf("led input error\n");
		}
		pthread_mutex_unlock(&mutex_led);
		printf("***************** success *****************\n");
	}


#if 0
	5.	open(dev_led,  )
	6.	pthread_cond_wait (cond_led,  );
	7.	获取dev_led_mask（控制标志）
	8.	通过ioctl（）控制led
#endif 
}

