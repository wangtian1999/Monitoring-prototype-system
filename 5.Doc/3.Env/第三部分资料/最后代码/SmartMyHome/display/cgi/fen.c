#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "cgic.h"
#include "data_global.h"		

char fen[1] = {'0'};

struct msg msgcmb;
int msgid;  //消息队列的ID
key_t key;   //消息队列，共享内存，信号量的key值
int cgiMain(){
	
	int value;
	cgiFormString("fen",fen,2);
	msgcmb.type = 1;        //新家
	msgcmb.msgtype = 3;  //风扇控制
	msgcmb.text[0]=5;	 //前两位平台	编号 zigbee--50
	msgcmb.text[1]=0;
	msgcmb.text[2]=0;	// 两位设备号---01-fen
	msgcmb.text[3]=1;
	msgcmb.text[4]=0;	 //具体控制命令
	msgcmb.text[5]=0;
	msgcmb.text[6]=0;
	if(strncmp(fen,"1",1))
		{
				msgcmb.text[7]=1;
		}else{
				msgcmb.text[7]=0;
		}
		
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
	value = msgsnd(msgid,&msgcmb,sizeof(msgcmb),0);
	if(value == -1)
	{
		printf("msgsnd is failed!\n");
		return -1;
	}
	printf("Content- type: text/html\n\n");
	fprintf(cgiOut,"<HTML>\n");
	fprintf(cgiOut,"<HEAD>\n");
	fprintf(cgiOut,"<TITLE>提示</TITLE></HEAD>\n");
	fprintf(cgiOut,"<BODY>\n");
	fprintf(cgiOut,"<H2>%s--\n</H2>",fen);
	fprintf(cgiOut,"cmb success!");
	fprintf(cgiOut,"</BODY>\n");
	fprintf(cgiOut,"</HEAD>\n");
	fprintf(cgiOut,"</HTML>\n");
	
	return 0;
}
