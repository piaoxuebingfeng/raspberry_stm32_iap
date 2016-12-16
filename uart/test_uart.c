#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <asm/termios.h>

#include "serial.h"

#define DEV_NAME  "/dev/ttyS0"
#define FLIE_NAME "USART.bin"

int main (int argc, char *argv[])
{
	int fd;
	int len, i,ret;
    char buf[] = "hello ZLG!";
	char download_buf[20];
	//char sendbuf[1024*41]="";
	char *sendbufp;
	FILE *pfile;
	long filesize;
	printf("%s\n",buf);
	fd = open(DEV_NAME, O_RDWR | O_NOCTTY);
    if(fd < 0) {
                perror(DEV_NAME);
                return -1;
        }
	ret = set_port_attr (
                         	fd,  
                          	B115200,          // B1200 B2400 B4800 B9600 .. B115200
                         	8,           	  // 5, 6, 7, 8
                          	"1",    	  //  "1", "1.5", "2"
                          	'N',              // N(o), O(dd), E(ven)
                          	150,		  //VTIME
                          	255 );		  //VTIME

	if(ret < 0) {
		printf("set uart arrt faile \n");
		exit(-1);
	}
	printf("%s\n",buf);
//-----------------------get filesize-----------------------
	pfile = fopen(FLIE_NAME,"rb");
	if(pfile == NULL)
	{
		printf("file not found\n");
		exit(1);
	}
	fseek(pfile,0,SEEK_END);
	filesize = ftell(pfile);
	fclose(pfile);
	printf("the filesize is %ld Bytes!\n",filesize);
//-----------------------------------------------------------

	sprintf(download_buf,"downloadlen:%d\n",filesize);
	len = write(fd, download_buf, sizeof(download_buf));
	if (len < 0) 
	{
		printf("write data error \n");
		return -1;
	}
	usleep(200*1000);
	
	pfile= fopen(FLIE_NAME,"rb");
	fseek(pfile,0,SEEK_SET);
	sendbufp = (char *)malloc(1024*sizeof(char));

	for(i=0;filesize>0;i++)
	{
		if(filesize > 1024)
		{
			fread(sendbufp,1,1024,pfile);
			len = write(fd, sendbufp, 1024);
			if (len < 0) 
			{
				printf("write data error \n");
				return -1;
			}
			filesize=filesize-1024;	
		}
		else
		{
			fread(sendbufp,1,filesize,pfile);
			len = write(fd, sendbufp, filesize);
			if (len < 0) 
			{
				printf("write data error \n");
				return -1;
			}
			filesize=filesize-filesize;
		}
		printf("the filesize is %ld Bytes!\n",filesize);
		fseek(pfile,0,SEEK_CUR);
		usleep(200*1000);
	}


//	len = read(fd, buf, sizeof(buf));
//    if (len < 0) {
//                printf("read error \n");
//                return -1;
//    }
//	printf("%s \n", buf);

	free(sendbufp);
	fclose(pfile);
	return(0);
}

