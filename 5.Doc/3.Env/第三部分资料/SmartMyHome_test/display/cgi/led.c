#include <stdio.h>
#include <string.h>
#include "cgic.h"

int cgiMain()
{
	
	printf("Content-type: text/html\n\n");

	fprintf(cgiOut,"<HTML>\n");
	fprintf(cgiOut,"<HEAD>\n");
	fprintf(cgiOut,"<TITLE>提示</TITLE></HEAD>\n");
	fprintf(cgiOut,"<BODY>\n");
	fprintf(cgiOut,"cmb success!");
	fprintf(cgiOut,"</BODY>\n");
	fprintf(cgiOut,"</HEAD>\n");
	fprintf(cgiOut,"</HTML>\n");

}
