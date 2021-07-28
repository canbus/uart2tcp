#include <stdio.h>
#include <pthread.h> 
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib")

SOCKET sclient = NULL;

int tcpSend(const char *buf,int len)
{
    if(sclient != NULL){
        printf("tcp tx:%s\n",buf);
        return send(sclient,buf,len,0);
    }
    else
        return -1;
}
void *tcpRx(void *args)
{
    char revdata[200];
    while(1)
	{
		int ret = recv(sclient,revdata,255,0);
		if(ret > 0)
		{
			revdata[ret] = 0x00;
			printf("tcp rx:");
			printf(revdata);
			printf("\n");
		}
	}
}
int tcpInit(char *ip,int port)
{
	WORD sockVision = MAKEWORD(2,2);
	WSADATA wsadata;
	struct sockaddr_in serAddr;
	char senddata[255];
	char revdata[255];
	int ret = -1;
	if(WSAStartup(sockVision,&wsadata) != 0)
	{
		printf("WSAStartup fail\n");
		return 0;
	}

	sclient = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sclient == INVALID_SOCKET)
	{
		printf("socket fail\n");
		return 0;
	}

	serAddr.sin_family = AF_INET;
	// serAddr.sin_port = htons(8888);
	// serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    serAddr.sin_port = htons(port);
	serAddr.sin_addr.S_un.S_addr = inet_addr(ip);
	if( connect(sclient,(SOCKADDR *)&serAddr,sizeof(serAddr)) == SOCKET_ERROR )
	{
		printf("socket connect fail\n");
		return 0;
	}
    printf("tcp init done\n");

    {
        pthread_t tcppid;
	    pthread_create(&tcppid, NULL, tcpRx, NULL); 
    }
	// while(1)
	// {
	// 	strcpy(senddata,"hello");
	// 	send(sclient,senddata,strlen(senddata),0);
	// 	if(strcmp(senddata,"bye") == 0)
	// 		break;
	// 	ret = recv(sclient,revdata,255,0);
	// 	if(ret > 0)
	// 	{
	// 		revdata[ret] = 0x00;
	// 		printf("rx:");
	// 		printf(revdata);
	// 		printf("\n");
	// 	}
	// }

	// closesocket(sclient);
	// WSACleanup();

	//system("pause");
	return 0;
}
