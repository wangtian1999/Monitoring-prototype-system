#include "data_global.h"


//定义锁
pthread_mutex_t mutex_client_request,
				mutex_refresh,
				mutex_sqlite,
				mutex_process,
				mutex_cloud,
				mutex_tube,
				mutex_fan,
				mutex_buzzer;

pthread_cond_t cond_client_request,
				cond_refresh,
				cond_sqlite,
				cond_process,
				cond_fan,
				cond_cloud,
				cond_tube,
				cond_buzzer;


pthread_t id_client_request,
		  id_refresh,
		  id_sqlite,
		  id_fan,
		  id_process,
		  id_cloud,
		  id_tube,
		  id_buzzer;


int msgid;  //消息队列的ID
int shmid;  //共享内存的ID
int semid;  //信号量的ID

key_t key;   //消息队列，共享内存，信号量的key值
key_t shm_key;
key_t sem_key;

char recive_phone[12] = {0};  //接受手机号
char center_phone[12] = {0};	//发送手机号

char *message ="Makeru is my home,growing up to see everyone!.\n";

struct env_info_client_addr sm_all_env_info;


//消息队列的一个函数
int send_msg_queue(long type,unsigned char text)
{
	struct msg msgbuf;
	msgbuf.type = 1;
	msgbuf.msgtype = type;
	msgbuf.text[0] = text;

	if(msgsnd(msgid,&msgbuf,sizeof(msgbuf) - sizeof(long),0) == -1)
	{
		printf("failed to msgsnd type2\n");
		exit(-1);
	}
	return 0;
}



