#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "cgic.h"



int cgiMain()
{

	unsigned short temp = 0;
	int fd;
	double result = 0;
	unsigned char buff[2];
	fd = open("/dev/temp0",O_RDWR);
	
	while(1){
		
        printf("open device success.\r\n");
        read(fd, buff, sizeof(buff));
        temp=((unsigned short)buff[1])<<8;
        temp|=(unsigned short)buff[0];
        if(temp & 0x0800)
        {
             temp &= 0xF000;
             result=0.0625*((double)(~temp+1));
        }else{
             result=0.0625*((double)temp);
        }
		printf("temperature is %4f \r\n", result);
        sleep(2);
		}
	printf("Content- type: text/html\n\n");
	fprintf(cgiOut,"<HTML>\n");
	fprintf(cgiOut,"<HEAD>\n");
	fprintf(cgiOut,"<TITLE>提示</TITLE></HEAD>\n");
	fprintf(cgiOut,"<BODY>\n");
	fprintf(cgiOut,"success!=%d",temp);
	fprintf(cgiOut,"</BODY>\n");
	fprintf(cgiOut,"</HEAD>\n");
	fprintf(cgiOut,"</HTML>\n");
	close(fd);
	return 0;
}
	