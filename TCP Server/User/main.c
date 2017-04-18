#include "mcu_int.h"
#include "config.h"
#include "device.h"
#include "spi2.h"
#include "socket.h"
#include "w5500.h"
#include "24c16.h"
#include "ult.h"
#include "md5.h"
#include "string.h"
#include <stdio.h>


extern uint8 txsize[];
extern uint8 rxsize[];
//extern const	uint8 str1; 
uint8 buffer[2048];/*定义一个2KB的缓存*/
int main(void)
{
  uint16 port=6000;/*定义一个端口并初始化*/
  uint16 len=0;
//  uint32 t;	
  uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};/*定义Mac变量*/
  uint8 lip[4]={192,168,1,111};/*定义lp变量*/
  uint8 sub[4]={255,255,255,0};/*定义subnet变量*/
  uint8 gw[4]={192,168,1,1};/*定义gateway变量*/
  uint8 ip[4];
  
  RCC_Configuration(); /* 配置单片机系统时钟*/
  GPIO_Configuration();/* 配置GPIO*/
  NVIC_Configuration();/* 配置嵌套中断向量*/
  
  Systick_Init(72);/* 初始化Systick工作时钟*/
  USART1_Init(); /*初始化串口通信:115200@8-n-1*/
  at24c16_init();/*初始化eeprom*/
  printf("W5500 EVB initialization over.\r\n");
  
  Reset_W5500();/*硬重启W5500*/
  WIZ_SPI_Init();/*初始化SPI接口*/
  printf("W5500 initialized!\r\n");  
 
  setSHAR(mac);/*配置Mac地址*/
  setSUBR(sub);/*配置子网掩码*/
  setGAR(gw);/*配置默认网关*/
  setSIPR(lip);/*配置Ip地址*/
  
  //Init. TX & RX Memory size of w5500
  sysinit(txsize, rxsize); /*初始化8个socket*/
  
  setRTR(2000);/*设置溢出时间值*/
  setRCR(3);/*设置最大重新发送次数*/
  

  getSIPR (ip);
  printf("IP : %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]);
  getSUBR(ip);
  printf("SN : %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]);
  getGAR(ip);
  printf("GW : %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]);
  printf("Network is ready.\r\n");

  while(1)
  {

       switch(getSn_SR(0))/*获取socket0的状态*/
       {
         case SOCK_INIT:/*socket初始化完成*/
           listen(0);/*在TCP模式下向服务器发送连接请求*/
   
           break;
         case SOCK_ESTABLISHED:/*socket连接建立*/
		     
           if(getSn_IR(0) & Sn_IR_CON)
           {
              setSn_IR(0, Sn_IR_CON);/*Sn_IR的第0位置1*/
            }
           len=getSn_RX_RSR(0);/*len为已接收数据的大小*/
           if(len>0)
           {
		      //t=IINCHIP_READ(Sn_SR(0));
			  memset(buffer,0x00,len+1);
             // printf("0x%d\r\n",t);
              recv(0,buffer,len);/*W5200接收来自Sever的数据*/
			  printf("%s\r\n",buffer);
              send(0,buffer,len);/*W5200向Server发送数据*/
//	          send(0,str1,100);/*W5200向Server发送数据*/
            }
           break;
         case SOCK_CLOSE_WAIT:/*socket等待关闭状态*/
           break;
         case SOCK_CLOSED:/*socket关闭*/
           socket(0,Sn_MR_TCP,port,Sn_MR_ND);/*打开socket0的一个端口*/
           break;
      }
  }
}

