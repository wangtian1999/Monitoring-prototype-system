//分配资源
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
	   mutex_process,
	   mutex_cloud,
	   mutex_fan,
	   mutex_tube,
	   mutex_buzzer;

extern pthread_cond_t cond_client_request,
	   cond_refresh,
	   cond_sqlite,
	   cond_fan,
	   cond_process,
	   cond_cloud,
	   cond_tube,
	   cond_buzzer;


extern pthread_t id_client_request,
		  id_refresh,
		  id_sqlite,
		  id_fan,
		  id_process,
		  id_cloud,
		  id_tube,
		  id_buzzer;

extern int msgid;  //消息队列的ID
extern int shmid;  //共享内存的ID
extern int semid;  //信号量的ID



int main(int argc, char * argv [])
{
	pthread_mutex_sum();     //初始化锁

	signal(SIGINT,&release_pthread_resource);  //等待接收信号，信号处理函数,若信号终止，则回调这个函数

	pthread_cond_sum();       //初始化各种条件变量

	pthread_create_sum();    //线程初始化

	pthread_join_sum();       //回收线程
	return 0;
}




void pthread_mutex_sum()
{
	pthread_mutex_init(&mutex_client_request,NULL);
	pthread_mutex_init(&mutex_fan,NULL);
	pthread_mutex_init(&mutex_refresh,NULL);
	pthread_mutex_init(&mutex_sqlite,NULL);
	pthread_mutex_init(&mutex_process,NULL);
	pthread_mutex_init(&mutex_cloud,NULL);
	pthread_mutex_init(&mutex_tube,NULL);
	pthread_mutex_init(&mutex_buzzer,NULL);

}
void pthread_cond_sum()
{

	pthread_cond_init(&cond_client_request,NULL);
	pthread_cond_init(&cond_refresh,NULL);
	pthread_cond_init(&cond_fan,NULL);
	pthread_cond_init(&cond_sqlite,NULL);
	pthread_cond_init(&cond_process,NULL);
	pthread_cond_init(&cond_cloud,NULL);
	pthread_cond_init(&cond_tube,NULL);
	pthread_cond_init(&cond_buzzer,NULL);	
}

void pthread_create_sum()
{
	pthread_create(&id_client_request,NULL,pthread_client_request,NULL);
	pthread_create(&id_refresh,NULL,pthread_refresh,NULL);
	pthread_create(&id_sqlite,NULL,pthread_sqlite,NULL);
	pthread_create(&id_process,NULL,pthread_process,NULL);
	pthread_create(&id_cloud,NULL,pthread_cloud,NULL);
	pthread_create(&id_tube,NULL,pthread_tube,NULL);
	pthread_create(&id_fan,NULL,pthread_fan,NULL);
	pthread_create(&id_buzzer,NULL,pthread_buzzer,NULL);	
}
//等待线程退出
void pthread_join_sum()
{
	pthread_join(id_client_request,NULL);       printf("pthread-1---exit\n");
	pthread_join(id_refresh,NULL);				printf("pthread-2---exit\n");
	pthread_join(id_sqlite,NULL);				printf("pthread-3---exit\n");
	pthread_join(id_cloud,NULL);				printf("pthread-4---exit\n");
	pthread_join(id_process,NULL);				printf("pthread-5---exit\n");
	pthread_join(id_tube,NULL);					printf("pthread-6---exit\n");
	pthread_join(id_buzzer,NULL);				printf("pthread-7---exit\n");
	pthread_join(id_fan,NULL);					printf("pthread-8---exit\n");
}

//释放线程资源
void release_pthread_resource(int signo)
{
	//释放锁和条件变量
	pthread_mutex_destroy(&mutex_client_request);
	pthread_mutex_destroy(&mutex_refresh);
	pthread_mutex_destroy(&mutex_sqlite);
	pthread_mutex_destroy(&mutex_fan);
	pthread_mutex_destroy(&mutex_process);
	pthread_mutex_destroy(&mutex_cloud);
	pthread_mutex_destroy(&mutex_tube);
	pthread_mutex_destroy(&mutex_buzzer);

	pthread_cond_destroy(&cond_client_request);
	pthread_cond_destroy(&cond_refresh);
	pthread_cond_destroy(&cond_sqlite);
	pthread_cond_destroy(&cond_fan);
	pthread_cond_destroy(&cond_process);
	pthread_cond_destroy(&cond_cloud);
	pthread_cond_destroy(&cond_tube);
	pthread_cond_destroy(&cond_buzzer);

	pthread_cond_init(&cond_client_request,NULL);
	pthread_cond_init(&cond_refresh,NULL);
	pthread_cond_init(&cond_sqlite,NULL);
	pthread_cond_init(&cond_process,NULL);
	pthread_cond_init(&cond_fan,NULL);
	pthread_cond_init(&cond_cloud,NULL);
	pthread_cond_init(&cond_tube,NULL);
	pthread_cond_init(&cond_buzzer,NULL);	



	//分离线程资源 （回收僵尸进程）
	pthread_detach(id_client_request);
	pthread_detach(id_refresh);
	pthread_detach(id_sqlite);
	pthread_detach(id_cloud);
	pthread_detach(id_process);
	pthread_detach(id_fan);
	pthread_detach(id_tube);
	pthread_detach(id_buzzer);


	printf("all pthread is detached\n");

	msgctl(msgid,IPC_RMID,NULL);
	shmctl(shmid,IPC_RMID,NULL);
	semctl(semid,1,IPC_RMID,NULL);
	exit(0);

}













