#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>
#include <sys/sem.h>
#include <unistd.h>
#include "sem.h"
#include "cgic.h"
#include "data_global.h"



#define N 32

key_t key;
int msgid;
char sto_no[2];
struct msg msg_buf;

char recive_phone[12];

int cgiMain()
{
	memset(&msg_buf,0,sizeof(msg_buf));
	cgiFormStringNoNewlines("recive_phone",recive_phone,12);
	if((key = ftok("/tmp",'g')) < 0)
	{
		perror("msgget");
		exit(1);
	}
	strcpy(msg_buf.text,recive_phone);
	msg_buf.type = 1;
	msg_buf.msgtype = 10;
	msgsnd(msgid,&msg_buf,sizeof(msg_buf),0);
	
	cgiHeaderContentType("text/html");
	fprintf(cgiOut,"<html><head>\n");
	fprintf(cgiOut,"<title> My CGi</title></head>\n");
	fprintf(cgiOut,"<body>\n");
	fprintf(cgiOut,"<H2>setting send tel message successful</H2>\n");
	
  /*fprintf(cgiOut,"<meta http-equiv=\"refresh\" content=\"1;url=   \">");*/
	fprintf(cgiOut,"</body></html>\n");
	return 0;
}
	