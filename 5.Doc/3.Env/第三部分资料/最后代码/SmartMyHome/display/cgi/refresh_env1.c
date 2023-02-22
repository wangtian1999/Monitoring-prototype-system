#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "cgic.h"
#include "data_global.h"		
#include "sem.h"
#include <sys/ipc.h>
#include <error.h>


#define N 32
#define MONITOR_NUM 1

int shmid;	//共享内存的ID
int semid;	//信号量的ID	
int msgid;  //消息队列的ID
 


struct shm_addr         //共享内存的结构体
{
	char shm_status;
	struct env_info_client_addr sm_all_env_info;
};

struct msg msgcmb;
struct shm_addr *shm_buf;

void temp_refresh();

int cgiMain()
{
	key_t key; //消息队列，共享内存，信号量的key值
	temp_refresh();
	
	if((key = ftok("/tmp",'i')) < 0)
		{
			perror("ftok failed!\n ");
			exit(-1);
		}
	printf("key = %x\n",key);
	semid = semget(key,1,0666);
	if(semid < 0)
	{
		perror("semget");
		exit(1);
	}	
	shmid = shmget(key,N,0666);
	if(shmid == -1)
		{
			perror("shmget");
			exit(1);
		}
	//共享内存映射
	if((shm_buf = (struct shm_addr *)shmat(shmid,NULL,0)) ==(void *)-1)
	{
		perror("fail to shmat\n");
		exit(1);
	}
	
		sem_p(semid,0);
		printf("Content-type: text/html\n\n");

		fprintf(cgiOut,"<HTML>\n");
		fprintf(cgiOut, "<head><meta http-equiv=\"refresh\" content=\"1\"><style><!--body{line-height:50%}--></style></head>");
		fprintf(cgiOut,"<HEAD>\n");
		fprintf(cgiOut,"<TITLE>提示</TITLE></HEAD>\n");
		fprintf(cgiOut,"<BODY bgcolor=\"#666666\">\n");
		if(shm_buf->shm_status == 1)
		{
		   /*	fprintf(cgiOut, "<script>function show(){var date =new Date(); var now = \"\"; now = date.getFullYear()+\"年\"; now = now + (date.getMonth()+1)+\"月\"; \ now = now + date.getDate()+\"日\"; now = now + date.getHours()+\"时\"; now = now + date.getMinutes()+\"分\";now = now + date.getSeconds()+\"秒\"; document.getElementById(\"nowDiv\").innerHTML = now; setTimeout(\"show()\",1000);} </script> \n ");	
		   *///html 时间
			fprintf(cgiOut,"<h2><font face=\"Broadway\"><font color=\"#00FAF0\">Home1 Real-time Environment Info:</font></font></H2>\n ");
			fprintf(cgiOut,"<h2 align=center><font color=\"#cc0033\"><body onload=\"show()\"> <div id=\"nowDiv\"></div></font></h2> \n ");
			fprintf(cgiOut,"<h4>ZIGBEE数据</H4>\n ");
			fprintf(cgiOut,"<h4>Temperature:  %s </H4>\n", shm_buf->sm_all_env_info.monitor_no[shm_buf->shm_status].zigbee_info.temperature);		
			fprintf(cgiOut,"<h4>Humidity:  %0.2f</H4>\n",shm_buf->sm_all_env_info.monitor_no[shm_buf->shm_status].zigbee_info.humidity);
			fprintf(cgiOut,"<h4>A9数据显示部分</H4>\n ");
			fprintf(cgiOut,"<h4>adc:\t %0.2f</h4>\n",shm_buf->sm_all_env_info.monitor_no[shm_buf->shm_status].a9_info.adc);
			fprintf(cgiOut,"<h4>GYPOX:\t%d</h4>\n",shm_buf->sm_all_env_info.monitor_no[shm_buf->shm_status].a9_info.gyrox);
			fprintf(cgiOut,"<h4>GYPOY:\t%d</h4>\n",shm_buf->sm_all_env_info.monitor_no[shm_buf->shm_status].a9_info.gyroy);
			fprintf(cgiOut,"<h4>GYPOZ:\t%d</h4>\n",shm_buf->sm_all_env_info.monitor_no[shm_buf->shm_status].a9_info.gyroz);
			fprintf(cgiOut,"<h4>AACx:\t%d</h4>\n",shm_buf->sm_all_env_info.monitor_no[shm_buf->shm_status].a9_info.aacx);
			fprintf(cgiOut,"<h4>AACy:\t%d</h4>\n",shm_buf->sm_all_env_info.monitor_no[shm_buf->shm_status].a9_info.aacy);
			fprintf(cgiOut,"<h4>AAcz:\t%d</h4>\n",shm_buf->sm_all_env_info.monitor_no[shm_buf->shm_status].a9_info.aacz);
			fprintf(cgiOut,"<h4>A9-RESERVED[0]:\t%d</h4>\n",shm_buf->sm_all_env_info.monitor_no[shm_buf->shm_status].a9_info.reserved[0]);
			fprintf(cgiOut,"<h4>A9-RESERVED[1]:\t%d</h4>\n",shm_buf->sm_all_env_info.monitor_no[shm_buf->shm_status].a9_info.reserved[1]);
		}else{
		fprintf(cgiOut, "<h2><font face=\"Broadway\"><font color=\"#FFFAF0\">Close!</font></font></H2>\n ");	
		}
	
		
		fprintf(cgiOut, "</BODY></HTML>\n");	
	sem_v (semid, 0);
		
	return 0;
		
}
void temp_refresh()
{
	/*key_t key;
	int value;
	msgcmb.type = 1;        //新家
	msgcmb.msgtype = 4;  //温度蜂鸣器控制
	msgcmb.text[0]=4;	 //前两位平台	编号 A9和zigbee--45
	msgcmb.text[1]=5;
	msgcmb.text[2]=0;	// 两位设备号---02-温湿度
	msgcmb.text[3]=2;
	msgcmb.text[4]=0;	 //具体控制命令
	msgcmb.text[5]=0;
	msgcmb.text[6]=0;
	msgcmb.text[7]=3;
		
	if((key = ftok("/tmp",'g')) < 0 )   //消息队列
	{
		perror("ftok");
		return ;
	}
	msgid = msgget(key,IPC_CREAT|IPC_EXCL|0666);
	if(msgid == -1)
	{
		if(errno == EEXIST)
		{
			msgid = msgget(key,0777);
		}else{
			perror("msgget");
			return ;
		}
	}

	value = msgsnd(msgid,&msgcmb,sizeof(msgcmb),0);
	if(value == -1)
	{
		printf("msgsnd is failed!\n");
		return ;
	}
	*/
}