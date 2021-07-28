#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <pthread.h>  
#include <time.h>
#include <windows.h>//使用Sleep的头

#include "cJSON.h"

typedef struct 
{
    uint8_t port[20];
    uint32_t bps;
}COMCFG;
typedef struct 
{
    uint8_t ip[20];
    uint32_t port;
}TCPCFG;
typedef struct 
{
    uint8_t device[100];
    uint8_t key[100];
}NLCFG;

typedef struct 
{
    COMCFG comcfg;
    TCPCFG tcpcfg;
    NLCFG  nlcfg;
}CONFIG;

CONFIG config = {0};

int createJsonAndSend2Tcp(int type,uint8_t val[],uint8_t len)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root,"t",type);
    if(type == 1){
        cJSON_AddStringToObject(root,"device",config.nlcfg.device);
        cJSON_AddStringToObject(root,"key",config.nlcfg.key);
        cJSON_AddStringToObject(root,"ver","v1.1");
    }else if(type == 3){
        if(val[0] != (uint8_t)0xDD)
            goto END;
        int kind = val[5];
        int sensorVal = val[7] + val[6] * 256;
        printf("type:%d value:%d\n",kind,sensorVal);
        cJSON_AddNumberToObject(root,"datatype",1);
        cJSON *datas = cJSON_CreateObject();
        switch(kind){
            case 1://温度湿度传感器
                cJSON_AddNumberToObject(datas,"temperature",val[6]);
                cJSON_AddNumberToObject(datas,"humidity",val[7]);
                break;
            case 3://火焰
                cJSON_AddNumberToObject(datas,"flame",sensorVal);
                break;
            case 4://可燃气体
                cJSON_AddNumberToObject(datas,"fuelgas",sensorVal);
                break;
            case 6://luminance
                cJSON_AddNumberToObject(datas,"luminance",sensorVal);
                break;
        }
        cJSON_AddItemToObject(root,"datas",datas);
        cJSON_AddNumberToObject(root,"msgid",123);
    }
    char *str = cJSON_PrintUnformatted(root);
    tcpSend(str,strlen(str));
    free(str);
    
END:
    free(root);
}

void *counter3(void *args){
    int count = 0;
    while(TRUE){
        count ++;
        if(count > 5){
            tcpSend("$#AT#",5);
            Sleep(1000 * 10);
        }else{
            createJsonAndSend2Tcp(1,NULL,0);
            Sleep(1000 * 3);
        }
        
    }
}

int main()
{
    uint8_t jsonCfg[200]="{\"com\":{\"port\":\"COM1\",\"bps\":115200},\"tcp\":{\"IP\":\"117.78.1.201\",\"port\":8700},\"NL\":{\"device\":\"deviceID\",\"key\":\"keyVal\"}}";
    FILE *cfg = fopen("cfg.ini","r");
    if(cfg != NULL){
        fread(jsonCfg,sizeof(uint8_t),1024,cfg);
        fclose(cfg);
    }else{
        fclose(cfg);
        cfg = fopen("cfg.ini","w");
        if(cfg != NULL){
            fwrite(jsonCfg,sizeof(uint8_t),strlen(jsonCfg),cfg);
            fclose(cfg);
        }
    }
    printf("load cfg:%s\n",jsonCfg);
    {
        cJSON *root = cJSON_Parse(jsonCfg);
        assert(root);
        cJSON *com = cJSON_GetObjectItem(root,"com");
        assert(com);
        cJSON *port = cJSON_GetObjectItem(com,"port");
        cJSON *bps = cJSON_GetObjectItem(com,"bps");
        assert(port);
        strcpy(config.comcfg.port,port->valuestring);
        config.comcfg.bps = bps->valueint;

        cJSON *tcp = cJSON_GetObjectItem(root,"tcp");
        assert(tcp);
        cJSON *ip = cJSON_GetObjectItem(tcp,"ip");
        port = cJSON_GetObjectItem(tcp,"port");
        strcpy(config.tcpcfg.ip,ip->valuestring);
        config.tcpcfg.port = port->valueint;

        cJSON *lncfg = cJSON_GetObjectItem(root,"NL");
        assert(lncfg);
        cJSON *device = cJSON_GetObjectItem(lncfg,"device");
        cJSON *key = cJSON_GetObjectItem(lncfg,"key");
        strcpy(config.nlcfg.device,device->valuestring);
        strcpy(config.nlcfg.key,key->valuestring);
        free(root);
    }
    {
        pthread_t t3;
	    pthread_create(&t3, NULL, counter3, NULL);  
    }
    tcpInit(config.tcpcfg.ip,config.tcpcfg.port);
    uart_init(config.comcfg.port,config.comcfg.bps);

}