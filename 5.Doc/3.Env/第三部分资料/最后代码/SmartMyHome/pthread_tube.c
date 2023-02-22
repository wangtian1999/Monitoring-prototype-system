#include "data_global.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>



extern pthread_mutex_t  	mutex_tube;
extern pthread_cond_t      	cond_tube;

void *pthread_tube(void *arg)
{
	int value;
	int i;
	printf("pthread_len");
	int fd[4];
	while(1){
		pthread_mutex_lock(&mutex_tube);
		pthread_cond_wait(&cond_tube,&mutex_tube);
		printf("printf_msgbuf:*********");
		printf("Led_cmb---1\n");
		for(i=0; i<9; i++)
		{
				printf("%d  ",tube_cmb[i]);
		}
		puts("");
		pthread_mutex_unlock(&mutex_tube);
		}
}







