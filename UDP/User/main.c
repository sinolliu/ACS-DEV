#include "stm32f10x.h"
#include "mcu_int.h"
#include "config.h"
#include "device.h"
#include "spi2.h"
#include "socket.h"
#include "w5500.h"
#include "24c16.h"
#include "ult.h"
#include "md5.h"
#include <stdio.h>

extern uint8 txsize[];
extern uint8 rxsize[];

uint8 buffer[2048];/*定义一个2KB的缓存*/

int main()
{
  uint8 pc_ip[4]={192,168,1,101};/*配置服务器的IP地址*/
  uint16 pc_port=6000;/*定义计算机的一个端口并初始化*/
  uint16 len=0;

  uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};/*定义Mac变量*/
  uint8 lip[4]={192,168,1,111};/*定义lp变量*/
  uint8 sub[4]={255,255,255,0};/*定义subnet变量*/
  uint8 gw[4]={192,168,1,1};/*定义gateway变量*/
  uint8 ip[4];
  //uint8 test[10] = "test";
//  uint32 counter=0;
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
    switch(getSn_SR(0))/*获取socket 0的状态*/
      {
         case SOCK_UDP:/*socket 的套接字初始化完成*/
           //setSn_IR(0, Sn_IR_RECV);
          // counter++;
           //sprintf(test, "test:%03d\r\n", counter);
//           sendto(0, test,10, pc_ip, pc_port);
           Delay_ms(100);
           if(getSn_IR(0) & Sn_IR_RECV)
          {
            setSn_IR(0, Sn_IR_RECV);/*Sn_IR的第0位置1*/
          }
           if((len=getSn_RX_RSR(0))>0)
           { 
              recvfrom(0,buffer, len, pc_ip,&pc_port);/*W5200接收计算机发送来的数据*/
			  printf("%s\r\n",buffer);
              sendto(0,buffer,len, pc_ip, pc_port);/*W5200把接收到的数据发送给计算机*/
			  
            }
           break;
         case SOCK_CLOSED:/*socket 关闭*/
           socket(0,Sn_MR_UDP,7000,0);/*初始化socket 0的套接字*/
           break;
       }
  }
}
