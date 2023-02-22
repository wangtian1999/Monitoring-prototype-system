#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include "data_global.h"


void release_pthread_resource(int signo);
void pthread_mutex_sum();
void pthread_cond_sum();
void pthread_create_sum();
void pthread_join_sum();

//定义锁
extern pthread_mutex_t mutex_client_request,
	   mutex_refresh,
	   mutex_sqlite,
	   mutex_transfer,
	   mutex_sms,
	   mutex_buzzer,
	   mutex_led,
	   mutex_fan;

//定义条件变量
extern pthread_cond_t  cond_client_request,
	   cond_refresh,
	   cond_sqlite,
	   cond_transfer,
	   cond_sms,
	   cond_buzzer,
	   cond_led,
	   cond_fan;

//定义线程
extern pthread_t  id_client_request,
	   id_refresh,
	   id_sqlite,
	   id_transfer,
	   id_sms,
	   id_buzzer,
	   id_led,		
	   id_fan,
	   id_rfid;		

extern int msgid; 	//消息队列的ID
extern int shmid; 	//共享内存的ID
extern int semid; 	//信号量的ID


int main(int argc, const char *argv[])
{
	pthread_mutex_sum(); 	//初始化锁

	signal (SIGINT, release_pthread_resource); 	//等待接收信号

	pthread_cond_sum(); 	//初始化各种条件变量

	pthread_create_sum(); 	//线程初始化

	pthread_join_sum(); 	//等待线程退出　回收线程

	return 0;
}

//初始化锁
void pthread_mutex_sum()
{
	pthread_mutex_init(&mutex_client_request,NULL);
	pthread_mutex_init(&mutex_refresh,NULL);
	pthread_mutex_init(&mutex_sqlite,NULL);
	pthread_mutex_init(&mutex_transfer,NULL);
	pthread_mutex_init(&mutex_sms,NULL);
	pthread_mutex_init(&mutex_buzzer,NULL);
	pthread_mutex_init(&mutex_led,NULL);
	pthread_mutex_init(&mutex_fan,NULL);
}

//初始化各种条件变量
void pthread_cond_sum()
{
	pthread_cond_init(&cond_client_request,NULL);
	pthread_cond_init(&cond_refresh,NULL);
	pthread_cond_init(&cond_sqlite,NULL);
	pthread_cond_init(&cond_transfer,NULL);
	pthread_cond_init(&cond_sms,NULL);
	pthread_cond_init(&cond_buzzer,NULL);                                       
	pthread_cond_init(&cond_led,NULL);
	pthread_cond_init(&cond_fan,NULL);
}



//线程初始化
void pthread_create_sum()
{
	pthread_create(&id_client_request,NULL,pthread_client_request,NULL);
	pthread_create(&id_refresh,NULL,pthread_refresh,NULL);  			
	pthread_create(&id_sqlite,NULL,pthread_sqlite,NULL);  			
	pthread_create(&id_transfer,NULL,pthread_transfer,NULL); 
	pthread_create(&id_sms,NULL,pthread_sms,NULL);  	
	pthread_create(&id_buzzer,NULL,pthread_buzzer,NULL);	 
	pthread_create(&id_led,NULL,pthread_led,NULL);  	
	pthread_create(&id_fan,NULL,pthread_fan,NULL);  	
	pthread_create(&id_rfid,NULL,pthread_rfid,NULL);  	
}

//等待线程退出
void pthread_join_sum()
{
	pthread_join(id_client_request,NULL);   printf ("pthread1 exit\n");
	pthread_join(id_refresh,NULL);          printf ("pthread2 exit\n");
	pthread_join(id_sqlite,NULL); 			printf ("pthread3 exit\n");
	pthread_join(id_transfer,NULL);			printf ("pthread4 exit\n");
	pthread_join(id_sms,NULL);				printf ("pthread5 exit\n");
	pthread_join(id_buzzer,NULL);			printf ("pthread6 exit\n");
	pthread_join(id_led,NULL);				printf ("pthread7 exit\n");
	pthread_join(id_fan,NULL);				printf ("pthread8 exit\n");
	pthread_join(id_rfid,NULL);				printf ("pthread9 exit\n");
}

//释放线程资源
void release_pthread_resource(int signo)
{
	//释放锁和条件变量
	pthread_mutex_destroy (&mutex_client_request);   
	pthread_mutex_destroy (&mutex_refresh);   
	pthread_mutex_destroy (&mutex_sqlite);    
	pthread_mutex_destroy (&mutex_transfer);   
	pthread_mutex_destroy (&mutex_sms);   
	pthread_mutex_destroy (&mutex_buzzer);   
	pthread_mutex_destroy (&mutex_led);   
	pthread_mutex_destroy (&mutex_fan);   

	pthread_cond_destroy (&cond_client_request);
	pthread_cond_destroy (&cond_refresh);
	pthread_cond_destroy (&cond_sqlite);
	pthread_cond_destroy (&cond_transfer);
	pthread_cond_destroy (&cond_sms);
	pthread_cond_destroy (&cond_buzzer);
	pthread_cond_destroy (&cond_led);
	pthread_cond_destroy (&cond_fan);

	//分离线程资源　（回收僵尸进程）
	pthread_detach(id_client_request);
	pthread_detach(id_refresh);
	pthread_detach(id_sqlite);
	pthread_detach(id_transfer);
	pthread_detach(id_sms);
	pthread_detach(id_buzzer);
	pthread_detach(id_led);
	pthread_detach(id_fan);
	pthread_detach(id_rfid);

	printf("all pthread is detached\n");

	msgctl (msgid, IPC_RMID, NULL);
	shmctl (shmid, IPC_RMID, NULL);
	semctl (semid, 1, IPC_RMID, NULL);

	exit(0);
}



