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

uint8 buffer[2048];/*����һ��2KB�Ļ���*/

int main()
{
  uint8 pc_ip[4]={192,168,1,101};/*���÷�������IP��ַ*/
  uint16 pc_port=6000;/*����������һ���˿ڲ���ʼ��*/
  uint16 len=0;

  uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};/*����Mac����*/
  uint8 lip[4]={192,168,1,111};/*����lp����*/
  uint8 sub[4]={255,255,255,0};/*����subnet����*/
  uint8 gw[4]={192,168,1,1};/*����gateway����*/
  uint8 ip[4];
  //uint8 test[10] = "test";
//  uint32 counter=0;
  RCC_Configuration(); /* ���õ�Ƭ��ϵͳʱ��*/
  GPIO_Configuration();/* ����GPIO*/
  NVIC_Configuration();/* ����Ƕ���ж�����*/
  
  Systick_Init(72);/* ��ʼ��Systick����ʱ��*/
  USART1_Init(); /*��ʼ������ͨ��:115200@8-n-1*/
  at24c16_init();/*��ʼ��eeprom*/
  printf("W5500 EVB initialization over.\r\n");
  
  Reset_W5500();/*Ӳ����W5500*/
  WIZ_SPI_Init();/*��ʼ��SPI�ӿ�*/
  printf("W5500 initialized!\r\n");  
 
  setSHAR(mac);/*����Mac��ַ*/
  setSUBR(sub);/*������������*/
  setGAR(gw);/*����Ĭ������*/
  setSIPR(lip);/*����Ip��ַ*/
  
    //Init. TX & RX Memory size of w5500
  sysinit(txsize, rxsize); /*��ʼ��8��socket*/
  
  setRTR(2000);/*�������ʱ��ֵ*/
  setRCR(3);/*����������·��ʹ���*/
  

  getSIPR (ip);
  printf("IP : %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]);
  getSUBR(ip);
  printf("SN : %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]);
  getGAR(ip);
  printf("GW : %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]);
  printf("Network is ready.\r\n");
  
  while(1)
  {
    switch(getSn_SR(0))/*��ȡsocket 0��״̬*/
      {
         case SOCK_UDP:/*socket ���׽��ֳ�ʼ�����*/
           //setSn_IR(0, Sn_IR_RECV);
          // counter++;
           //sprintf(test, "test:%03d\r\n", counter);
//           sendto(0, test,10, pc_ip, pc_port);
           Delay_ms(100);
           if(getSn_IR(0) & Sn_IR_RECV)
          {
            setSn_IR(0, Sn_IR_RECV);/*Sn_IR�ĵ�0λ��1*/
          }
           if((len=getSn_RX_RSR(0))>0)
           { 
              recvfrom(0,buffer, len, pc_ip,&pc_port);/*W5200���ռ����������������*/
			  printf("%s\r\n",buffer);
              sendto(0,buffer,len, pc_ip, pc_port);/*W5200�ѽ��յ������ݷ��͸������*/
			  
            }
           break;
         case SOCK_CLOSED:/*socket �ر�*/
           socket(0,Sn_MR_UDP,7000,0);/*��ʼ��socket 0���׽���*/
           break;
       }
  }
}
