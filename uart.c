#include <Windows.h>
#include <stdio.h>
 
HANDLE hCom;
 
int uart_init(char *port,int bps)
{
  char comx[20]="\\\\.\\";//"\\\\.\\COM1"
  strcat(comx,port);
  hCom = CreateFile(comx, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                    OPEN_EXISTING, 0, NULL);

  if (hCom == INVALID_HANDLE_VALUE) {
    printf("open COM fail!\n");
    return FALSE;
  } else {
    printf("open COM sucess\n");
  }

  SetupComm(hCom, 1024, 1024);  //输入缓冲区和输出缓冲区的大小都是1024

  /*********************************超时设置**************************************/
  COMMTIMEOUTS TimeOuts;
  //设定读超时
  TimeOuts.ReadIntervalTimeout = MAXDWORD;  //读间隔超时
  TimeOuts.ReadTotalTimeoutMultiplier = 0;  //读时间系数
  TimeOuts.ReadTotalTimeoutConstant = 0;    //读时间常量
  //设定写超时
  TimeOuts.WriteTotalTimeoutMultiplier = 1;  //写时间系数
  TimeOuts.WriteTotalTimeoutConstant = 1;    //写时间常量
  SetCommTimeouts(hCom, &TimeOuts);          //设置超时

  /*****************************************配置串口***************************/
  DCB dcb;
  GetCommState(hCom, &dcb);
  dcb.BaudRate = bps;//9600;        //波特率为9600
  dcb.ByteSize = 8;           //每个字节有8位
  dcb.Parity = NOPARITY;      //无奇偶校验位
  dcb.StopBits = ONESTOPBIT;  //一个停止位
  SetCommState(hCom, &dcb);

  DWORD wCount;  //实际读取的字节数
  BOOL bReadStat;

  char str[100] = {0};

  while (1) {
    // PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR); //清空缓冲区
    bReadStat = ReadFile(hCom, str, sizeof(str), &wCount, NULL);

    if (!bReadStat) {
      printf("read uart fail");
      return FALSE;
    } else {
      str[wCount] = 0;
      if (wCount > 0) {
        printf("com rx:%s\n", str);
		    //WriteFile(hCom,"rx ok",5,&wCount,NULL);
        createJsonAndSend2Tcp(3,str,wCount);
      }
    }
    Sleep(100);
  }

  CloseHandle(hCom);
}