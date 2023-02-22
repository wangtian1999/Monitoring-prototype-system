#include <stdio.h>
#include <string.h>
#include "cgic.h"


#define N 64

char name[N] ={1};
char password[N] = {2};

int cgiMain()
{
	cgiFormStringNoNewlines("username",name,N);
	cgiFormStringNoNewlines("password",password,20);
	cgiHeaderContentType("text/html");
	fprintf(cgiOut,"<HTML>\n");
	fprintf(cgiOut,"<HEAD>\n");
	fprintf(cgiOut,"<TITLE>登录提示</TITLE></HEAD>\n");
	if((strcmp(name,"chuangke") == 0)&&(strcmp(password,"123") == 0))
	{
		fprintf(cgiOut,"<body>");
		fprintf(cgiOut,"<H1>%s</H1>","login ok!\n");
		fprintf(cgiOut,"<H2>Welcome to %s</H2>",name);
		fprintf(cgiOut,"<meta http-equiv=\"refresh\" content=\"5;url=http://192.168.0.103/function.html\">");
		fprintf(cgiOut,"</body>");
	}else{
		fprintf(cgiOut,"<body>");
		fprintf(cgiOut,"error,please wait for back");
		fprintf(cgiOut,"<meta http-equiv=\"refresh\" content=\"5;url=http://192.168.0.103/login.html\">");
		fprintf(cgiOut,"</body>");
	}




	return 0;
}

