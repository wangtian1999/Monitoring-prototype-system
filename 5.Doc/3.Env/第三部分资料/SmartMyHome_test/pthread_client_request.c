//网页端 接受   客户端的请求
#include "data_global.h"


extern int msgid;  //消息队列的ID
extern key_t key;   //消息队列，共享内存，信号量的key值

extern pthread_mutex_t  mutex_client_request,
	   mutex_fan,
	   mutex_refresh,
	   mutex_sqlite,
	   mutex_process,
	   mutex_cloud,
	   mutex_tube,
	   mutex_buzzer;

extern pthread_cond_t cond_client_request,
	   cond_refresh,
	   cond_sqlite,
	   cond_process,
	   cond_fan,
	   cond_cloud,
	   cond_tube,
	   cond_buzzer;

pthread_t 	id_client_request,
			id_refresh,
			id_sqlite,
			id_process,
			id_cloud,
			id_tube,
			id_buzzer;

struct msg msgbuf;

void *pthread_client_request(void * arg)
{
	int i = 0;
	if((key = ftok("/tmp",'g')) < 0 )   //消息队列
	{
		perror("ftok");
		exit(-1);
	}
	msgid = msgget(key,IPC_CREAT|IPC_EXCL|0666);
	if(msgid == -1)
	{
		if(errno == EEXIST)
		{
			msgid = msgget(key,0777);
		}else{
			perror("msgget");
			exit(1);
		}
	}
	printf("pthread_client_request!\n");
	while(1){
		bzero(&msgbuf,sizeof(msgbuf));
		printf("wait from client request...\n");
		msgrcv(msgid,&msgbuf,sizeof(msgbuf),1,0);
		printf("GET %ldL msg\n",msgbuf.msgtype);
		switch(msgbuf.msgtype){

		case 1:
			pthread_mutex_lock(&mutex_tube);
			printf("----start  tube------\n");
			memcpy(tube_cmb,msgbuf.text,QUEUE_MSG_LEN);
			printf("Tube_cmb\n");
			for(i=0; i<9; i++)
			{
				printf("%d  ",tube_cmb[i]);
			}
			puts("");
			pthread_mutex_unlock(&mutex_tube);
			pthread_cond_signal(&cond_tube);
			break;
		case 2:
			pthread_mutex_lock(&mutex_buzzer);
			printf("----start  Beep------\n");
			memcpy(buzzer_cmb,msgbuf.text,QUEUE_MSG_LEN);
			for(i=0; i<9; i++)
			{
				printf("%d  ",tube_cmb[i]);
			}
			puts("");
			pthread_mutex_unlock(&mutex_buzzer);
			pthread_cond_signal(&cond_buzzer);
			break;
		case 3:
			break;
		case 4:
			break;
		case 10:
			break;				
		default:
			printf("input error!\n");
			break;
		}
	}	

}

