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
uint8 buffer[2048];/*����һ��2KB�Ļ���*/
int main(void)
{
  uint16 port=6000;/*����һ���˿ڲ���ʼ��*/
  uint16 len=0;
//  uint32 t;	
  uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};/*����Mac����*/
  uint8 lip[4]={192,168,1,111};/*����lp����*/
  uint8 sub[4]={255,255,255,0};/*����subnet����*/
  uint8 gw[4]={192,168,1,1};/*����gateway����*/
  uint8 ip[4];
  
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

       switch(getSn_SR(0))/*��ȡsocket0��״̬*/
       {
         case SOCK_INIT:/*socket��ʼ�����*/
           listen(0);/*��TCPģʽ���������������������*/
   
           break;
         case SOCK_ESTABLISHED:/*socket���ӽ���*/
		     
           if(getSn_IR(0) & Sn_IR_CON)
           {
              setSn_IR(0, Sn_IR_CON);/*Sn_IR�ĵ�0λ��1*/
            }
           len=getSn_RX_RSR(0);/*lenΪ�ѽ������ݵĴ�С*/
           if(len>0)
           {
		      //t=IINCHIP_READ(Sn_SR(0));
			  memset(buffer,0x00,len+1);
             // printf("0x%d\r\n",t);
              recv(0,buffer,len);/*W5200��������Sever������*/
			  printf("%s\r\n",buffer);
              send(0,buffer,len);/*W5200��Server��������*/
//	          send(0,str1,100);/*W5200��Server��������*/
            }
           break;
         case SOCK_CLOSE_WAIT:/*socket�ȴ��ر�״̬*/
           break;
         case SOCK_CLOSED:/*socket�ر�*/
           socket(0,Sn_MR_TCP,port,Sn_MR_ND);/*��socket0��һ���˿�*/
           break;
      }
  }
}

