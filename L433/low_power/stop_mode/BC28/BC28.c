///AT+NSOCL=0
#include "BC28.h"
#include "main.h"
#include "string.h"
char *strx,*extstrx;
char atstr[BUFLEN];
int err;    //全局变量
BC28 BC28_Status;

#define SERVERIP "47.104.238.90"
#define SERVERPORT 1001

void Clear_Buffer(void)//清空串口2缓存
{
    printf(buf_uart2.buf);  //清空前打印信息
    Delay(300);
    buf_uart2.index=0;
    memset(buf_uart2.buf,0,BUFLEN);
}

int BC28_Init(void)
{
    int errcount = 0;
    err = 0;    //判断模块卡是否就绪最重要
    printf("start init bc28\r\n");
    Uart2_SendStr("ATE1\r\n");
    Delay(300);
    printf(buf_uart2.buf);      //打印收到的串口信息
    printf("get back bc28\r\n");
    strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//返回OK
    Clear_Buffer();	
    while(strx==NULL)
    {
        printf("\r\n单片机正在连接到模块...\r\n");
        Clear_Buffer();	
        Uart2_SendStr("AT\r\n");
        Delay(300);
        strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//返回OK
    }
    Uart2_SendStr("AT+CMEE=1\r\n"); //允许错误值
    Delay(300);
    strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//返回OK
    Clear_Buffer();	

    Uart2_SendStr("AT+NBAND?\r\n"); //允许错误值
    Delay(300);
    strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//返回OK
    if(strx)
    {
        printf("======== BAND========= \r\n %s \r\n",buf_uart2.buf);
        Clear_Buffer();
        Delay(300);
    }
    
    Uart2_SendStr("AT+CIMI\r\n");//获取卡号，类似是否存在卡的意思，比较重要。
    Delay(300);
    strx=strstr((const char*)buf_uart2.buf,(const char*)"ERROR");//只要卡不错误 基本就成功
    if(strx==NULL)
    {
        printf("我的卡号是 : %s \r\n",buf_uart2.buf);
        Clear_Buffer();	
        Delay(300);
    }
    else
    {
        err = 1;
        printf("卡错误 : %s \r\n",buf_uart2.buf);
        Clear_Buffer();
        Delay(300);
    }

    Uart2_SendStr("AT+CGATT=1\r\n");//激活网络，PDP
    Delay(300);
    strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//返OK
    Clear_Buffer();	
    if(strx)
    {
        Clear_Buffer();	
        printf("init PDP OK\r\n");
        Delay(300);
    }
    Uart2_SendStr("AT+CGATT?\r\n");//查询激活状态
    Delay(300);
    strx=strstr((const char*)buf_uart2.buf,(const char*)"+CGATT:1");//返1 表明激活成功 获取到IP地址了
    Clear_Buffer();	
    errcount = 0;
    while(strx==NULL)
    {
        errcount++;
        Clear_Buffer();	
        Uart2_SendStr("AT+CGATT?\r\n");//获取激活状态
        Delay(300);
        strx=strstr((const char*)buf_uart2.buf,(const char*)"+CGATT:1");//返回1,表明注网成功
        if(errcount>100)     //防止死循环
        {
            err=1;
            errcount = 0;
            break;
        }
    }


    Uart2_SendStr("AT+CSQ\r\n");//查看获取CSQ值
    Delay(300);
    strx=strstr((const char*)buf_uart2.buf,(const char*)"+CSQ");//返回CSQ
    if(strx)
    {
        printf("信号质量:%s\r\n",buf_uart2.buf);
        Clear_Buffer();
        Delay(300);
    }

    Uart2_SendStr("AT+CEREG?\r\n");
    Delay(300);
    strx=strstr((const char*)buf_uart2.buf,(const char*)"+CEREG:0,1");//返回注册状态
    extstrx=strstr((const char*)buf_uart2.buf,(const char*)"+CEREG:1,1");//返回注册状态
    Clear_Buffer();	
    errcount = 0;
    while(strx==NULL&&extstrx==NULL)//两个返回值都没有
    {
        errcount++;
        Clear_Buffer();
        Uart2_SendStr("AT+CEREG?\r\n");//判断运营商
        Delay(300);
        strx=strstr((const char*)buf_uart2.buf,(const char*)"+CEREG:0,1");//返回注册状态
        extstrx=strstr((const char*)buf_uart2.buf,(const char*)"+CEREG:1,1");//返回注册状态
        if(errcount>100)     //防止死循环
        {
            err=1;
            errcount = 0;
            break;
        }
    }
    return err;
}

void BC28_PDPACT(void)//激活场景，为连接服务器做准备
{
    int errcount = 0;
    Uart2_SendStr("AT+CGDCONT=1,\042IP\042,\042HUAWEI.COM\042\r\n");//设置APN
    Delay(300);
    Uart2_SendStr("AT+CGATT=1\r\n");//激活场景
    Delay(300);
    Uart2_SendStr("AT+CGATT?\r\n");//激活场景
    Delay(300);
    strx=strstr((const char*)buf_uart2.buf,(const char*)" +CGATT:1");//注册上网络信息
    Clear_Buffer();	
    while(strx==NULL)
    {
        errcount++;
        Clear_Buffer();
        Uart2_SendStr("AT+CGATT?\r\n");//激活场景
        Delay(300);
        strx=strstr((const char*)buf_uart2.buf,(const char*)"+CGATT:1");//一定要终端入网
        if(errcount>100)     //防止死循环
        {
            errcount = 0;
            break;
        }
    }
    Uart2_SendStr("AT+CSCON?\r\n");//判断连接状态，返回1就是成功
    Delay(300);
    strx=strstr((const char*)buf_uart2.buf,(const char*)"+CSCON:0,1");//注册上网络信息
    extstrx=strstr((const char*)buf_uart2.buf,(const char*)"+CSCON:0,0");//注册上网络信息
    Clear_Buffer();	
    errcount = 0;
    while(strx==NULL&&extstrx==NULL)    //连接不能反馈错误
    {
        errcount++;
        Clear_Buffer();
        Uart2_SendStr("AT+CSCON?\r\n");//
        Delay(300);
        strx=strstr((const char*)buf_uart2.buf,(const char*)"+CSCON:0,1");//
        extstrx=strstr((const char*)buf_uart2.buf,(const char*)"+CSCON:0,0");//
        if(errcount>100)     //防止死循环
        {
            errcount = 0;
            break;
        }
    }

}

void BC28_ConTCP(void)
{
    int errcount = 0;
    Uart2_SendStr("AT+NSOCL=1\r\n");//关闭socekt连接
    Uart2_SendStr("AT+NSOCL=2\r\n");//关闭socekt连接
    Uart2_SendStr("AT+NSOCL=3\r\n");//关闭socekt连接
    Delay(300);
    Clear_Buffer();	
    Uart2_SendStr("AT+NSOCR=STREAM,6,0,1\r\n");//创建一个TCP Socket
    Delay(300);
    strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//返回OK
    while(strx==NULL)
    {
        errcount++;
        strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//返回OK
        if(errcount>10)     //防止死循环
        {
            errcount = 0;
            break;
        }

    }
    Clear_Buffer();

    memset(atstr,0,BUFLEN);
    //AT+NSOCO=1,123.57.41.13,1001
    sprintf(atstr,"AT+NSOCO=1,%s,%d\r\n",SERVERIP,SERVERPORT);
    Uart2_SendStr(atstr);//发送0 socketIP和端口后面跟对应数据长度以及数据
    Delay(300);
    strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//返回OK
    errcount = 0;
    while(strx==NULL)
    {
        errcount++;
        strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//返回OK
        if(errcount>10)     //防止死循环
        {
            errcount = 0;
            break;
        }
    }
    Clear_Buffer();	
}

void BC28_Senddata(uint8_t *len,uint8_t *data)
{
    int errcount=0;
    memset(atstr,0,BUFLEN);
    //AT+NSOSD=1,4,31323334
    sprintf(atstr,"AT+NSOSD=1,%s,%s\r\n",len,data);
    Uart2_SendStr(atstr);//发送0 socketIP和端口后面跟对应数据长度以及数据
    Delay(300);
    strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//返回OK
    while(strx==NULL)
    {
        errcount++;
        strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//返回OK
        if(errcount>100)     //防止死循环
        {
            errcount = 0;
            break;
        }
    }
    Clear_Buffer();	
}

/*
+NSONMI:0,4
AT+NSORF=0,4
0,47.104.238.90,1001,4,31323334,0

OK*/
void BC28_RECData(void)
{
    char i;
    strx=strstr((const char*)buf_uart2.buf,(const char*)"+NSONMI:");//返回+NSONMI:，+NSONMI:1,5
    if(strx)
    {
        BC28_Status.Socketnum=strx[8];//编号
        for(i=0;;i++)
        {
            if(strx[i+10]==0x0D)
                break;
            BC28_Status.reclen[i]=strx[i+10];//长度
        }

        memset(atstr,0,BUFLEN);
        sprintf(atstr,"AT+NSORF=%c,%s\r\n",BC28_Status.Socketnum,BC28_Status.reclen);//长度以及编号
        Uart2_SendStr(atstr);
        Delay(300);
        Clear_Buffer();

    }
    Uart2_SendStr("AT+NSORF=1,100\r\n");
}

