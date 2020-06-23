/****************************************************************************
* Copyright (C), 2011 �ܶ�Ƕ��ʽ������ www.ourstm.net
*
* �������� �ܶ���STM32������V3�ϵ���ͨ��           
* QQ: 9191274, ������sun68, Email: sun68@163.com 
* �Ա����̣�ourstm.taobao.com  
*
* �ļ���: bsp.c
* ���ݼ���:
*       �������ṩ��Ӳ��ƽ̨�ĳ�ʼ��
		
*
* �ļ���ʷ:
* �汾��  ����       ����    ˵��
* v0.2   2011-09-06 sun68  �������ļ�
*
*/
#include "includes.h"
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_exti.h"
#include "misc.h"
#include "demo.h"
//#include "BUTTON.h"
//#include "uip.h"
//#include "uip_arp.h"
//#include "tapdev.h"	   
#include "enc28j60.h"	
#include  <stdarg.h>
void InitNet(void);	
void GPIO_Configuration(void);
void RCC_Configuration(void);
//void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...);
void Printf(unsigned char *Data,...);
char *itoa(int value, char *string, int radix);
char *itoh(unsigned int value, char *string, int radix);
//void tp_Config(void);
//u16 TPReadX(void);
//u16 TPReadY(void);
void NVIC_Configuration(void);
//extern void FSMC_LCD_Init(void); 
//extern void SPI_Flash_Init(void);
extern void Delay(__IO uint32_t nCount);
extern void SPI1_Init(void);

/****************************************************************************
* ��    �ƣ�void RCC_Configuration(void)
* ��    �ܣ�ϵͳʱ������Ϊ72MHZ�� ����ʱ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void RCC_Configuration(void){
  SystemInit();
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);			   //���ù���ʹ��
}

/****************************************************************************
* ��    �ƣ�void GPIO_Configuration(void)
* ��    �ܣ�ͨ��IO������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/  
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 |RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                         RCC_APB2Periph_GPIOE, ENABLE);
  	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				     //LED1����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_3;		 //LED2, LED3����
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;					 //SST25VF016B SPIƬѡ
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_7;		 //PB12---VS1003 SPIƬѡ��V2.1) 
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //PB7---������оƬXPT2046 SPI Ƭѡ
  
  /* ��ֹSPI1�����ϵ������豸 */
  GPIO_SetBits(GPIOB, GPIO_Pin_7);						     //������оƬXPT2046 SPI Ƭѡ��ֹ  
  GPIO_SetBits(GPIOB, GPIO_Pin_12);						     //VS1003 SPIƬѡ��V2.1)��ֹ 
  GPIO_SetBits(GPIOC, GPIO_Pin_4);						     //SST25VF016B SPIƬѡ��ֹ  

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	         	 	//ENC28J60��������ж����� 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   	 		//�ڲ���������
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
}

/****************************************************************************
* ��    �ƣ�void USART_Config(USART_TypeDef* USARTx,u32 baud)
* ��    �ܣ���������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void USART_Config(USART_TypeDef* USARTx,u32 baud){
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
 
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 , ENABLE);	 		//ʹ�ܴ���1ʱ��

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         		 		//USART1 TX
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		 		//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);		    		 		//A�˿� 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         	 		//USART1 RX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 		//���ÿ�©����
  GPIO_Init(GPIOA, &GPIO_InitStructure);		         	 		//A�˿� 

  USART_InitStructure.USART_BaudRate = 115200;						//����115200bps
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//����λ8λ
  USART_InitStructure.USART_StopBits = USART_StopBits_1;			//ֹͣλ1λ
  USART_InitStructure.USART_Parity = USART_Parity_No;				//��У��λ
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //��Ӳ������
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�շ�ģʽ

  /* Configure USART1 */
  USART_Init(USART1, &USART_InitStructure);							//���ô��ڲ�������   
   /* Enable the USART1 */
  USART_Cmd(USART1, ENABLE);	
  
}
/****************************************************************************
* ��    �ƣ�void BSP_Init(void)
* ��    �ܣ��ܶ����ʼ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/  
void BSP_Init(void)
{  																		   
  RCC_Configuration();  	       //ϵͳʱ�ӳ�ʼ��	
  NVIC_Configuration();		   //�ж�Դ����   
  GPIO_Configuration();			   //GPIO����
  USART_Config(USART1,115200);     //��ʼ������1
 
  SPI1_Init();                     //ENC28J60 SPI ��ʼ��
  //tp_Config();					   //SPI1 ������·��ʼ��    
  //FSMC_LCD_Init();				   //FSMC TFT�ӿڳ�ʼ��  

  /* ��ʾ����Logo */
  //USART_OUT(USART1,"**** (C) COPYRIGHT 2011 �ܶ�Ƕ��ʽ���������� *******\r\n");    	  //�򴮿�1���Ϳ����ַ���
  //USART_OUT(USART1,"*                                                 *\r\n");    	
  //USART_OUT(USART1,"*     �ܶ���STM32���������uIP1.0����̫��ʵ��     *\r\n");  
  //USART_OUT(USART1,"*                                                 *\r\n");    	  	
  //USART_OUT(USART1,"*     MCUƽ̨:STM32F103VET6                       *\r\n");    	
  //USART_OUT(USART1,"*     ��̫��Ӳ��:ENC28J60                         *\r\n");    	  	
  //USART_OUT(USART1,"*     �̼��⣺3.5                                 *\r\n");    	
  //USART_OUT(USART1,"*     ���̰汾: 0.2                               *\r\n");  
  //USART_OUT(USART1,"*     �ܶ�STM32���̣�ourstm.taobao.com            *\r\n");     	   
  //USART_OUT(USART1,"*     �ܶ�STM32��̳��www.ourstm.net  QQ��9191274  *\r\n");   
  //USART_OUT(USART1,"*                                                 *\r\n");    	 	
  //USART_OUT(USART1,"***************************************************\r\n");  
    
 // InitNet();		/* ��ʼ�������豸�Լ�UIPЭ��ջ������IP��ַ */

  /* ����һ��TCP�����˿ں�http�����˿ڣ��˿ں�Ϊ1200��80 */
  //uip_listen(HTONS(1200));
  //uip_listen(HTONS(80));
}


/****************************************************************************
* ��    �ƣ�void NVIC_Configuration(void)
* ��    �ܣ��ж�Դ����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  /* Configure one bit for preemption priority */
  /* ���ȼ��� ˵������ռ���ȼ����õ�λ�����������ȼ����õ�λ��   ��������1�� 7 */    
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* Enable the EXTI2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;				 //�ⲿ�ж�2
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	     //��ռ���ȼ� 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			 //�����ȼ�0  
  //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				 //ʹ��
  NVIC_Init(&NVIC_InitStructure);

  //��������AFIO�ⲿ�ж����üĴ���AFIO_EXTICR1������ѡ��EXTI2�ⲿ�жϵ�����Դ��PE2��
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);     //�ⲿ�ж�����AFIO--ETXI2

  EXTI_InitStructure.EXTI_Line = EXTI_Line1;					  
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			  //�ж�ģʽ
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		  //�½��ش���
  //EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}

/****************************************************************************
* ��    �ƣ�void  OS_CPU_SysTickInit(void)
* ��    �ܣ�ucos ϵͳ����ʱ�ӳ�ʼ��  ��ʼ����Ϊ10msһ�ν���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void  OS_CPU_SysTickInit(void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    INT32U         cnts;
    RCC_GetClocksFreq(&rcc_clocks);		                        //���ϵͳʱ�ӵ�ֵ	 
    cnts = (INT32U)rcc_clocks.HCLK_Frequency/OS_TICKS_PER_SEC;	//���ʱ�ӽ��ĵ�ֵ	
	SysTick_Config(cnts);										//����ʱ�ӽ���	     
}

/****************************************************************************
* ��    �ƣ�void tp_Config(void)
* ��    �ܣ�TFT ���������Ƴ�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
//void tp_Config(void) 
//{ 
//  GPIO_InitTypeDef  GPIO_InitStructure;     
//
//  /* SPI1 ʱ��ʹ�� */
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE); 
// 
//  /* SPI1 SCK(PA5)��MISO(PA6)��MOSI(PA7) ���� */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//�����ٶ�50MHZ
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	        //����ģʽ
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//  /* SPI1 ����оƬ��Ƭѡ�������� PB7 */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//�����ٶ�50MHZ 
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			//�������ģʽ
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
//  
//  
//  /* ����SPI1�����Ϲҽ���4�����裬������ʹ�ô�����ʱ����Ҫ��ֹ����3��SPI1 ���裬 ������������ */  
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;           		//SPI1 SST25VF016BƬѡ 
//  GPIO_Init(GPIOC, &GPIO_InitStructure);
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                //SPI1 VS1003Ƭѡ 
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                 //SPI1 ����ģ��Ƭѡ  
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
//  GPIO_SetBits(GPIOC, GPIO_Pin_4);							//SPI1 SST25VF016BƬѡ�ø� 
//  GPIO_SetBits(GPIOB, GPIO_Pin_12);							//SPI1 VS1003Ƭѡ�ø�  
//  GPIO_SetBits(GPIOA, GPIO_Pin_4);							//SPI1 ����ģ��Ƭѡ�ø� 	 
//																		 
//}

/****************************************************************************
* ��    �ƣ�unsigned char SPI_WriteByte(unsigned char data) 
* ��    �ܣ�SPI1 д����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/  
//unsigned char SPI_WriteByte(unsigned char data) 
//{ 
// unsigned char Data = 0; 
//
//  //�ȴ����ͻ�������
//  while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET); 
//  // ����һ���ֽ�  
//  SPI_I2S_SendData(SPI1,data); 
//
//   //�ȴ��Ƿ���յ�һ���ֽ� 
//  while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==RESET); 
//  // ��ø��ֽ�
//  Data = SPI_I2S_ReceiveData(SPI1); 
//
//  // �����յ����ֽ� 
//  return Data; 
//}  

/****************************************************************************
* ��    �ƣ�void SpiDelay(unsigned int DelayCnt) 
* ��    �ܣ�SPI1 д��ʱ����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/  
//void SpiDelay(unsigned int DelayCnt)
//{
// unsigned int i;
// for(i=0;i<DelayCnt;i++);
//}

/****************************************************************************
* ��    �ƣ�u16 TPReadX(void) 
* ��    �ܣ�������X�����ݶ���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/  
//u16 TPReadX(void)
//{ 
//   u16 x=0;
//   TP_CS();	                        //ѡ��XPT2046 
//   SpiDelay(10);					//��ʱ
//   SPI_WriteByte(0xd0);				//����X���ȡ��־
//   SpiDelay(10);					//��ʱ
//   x=SPI_WriteByte(0x00);			//������ȡ16λ������ 
//   x<<=8;
//   x+=SPI_WriteByte(0x00);
//   SpiDelay(10);					//��ֹXPT2046
//   TP_DCS(); 					    								  
//   x = x>>3;						//��λ�����12λ����Ч����0-4095
//   return (x);
//}
/****************************************************************************
* ��    �ƣ�u16 TPReadY(void)
* ��    �ܣ�������Y�����ݶ���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
//u16 TPReadY(void)
//{
//   u16 y=0;
//   TP_CS();	                        //ѡ��XPT2046 
//   SpiDelay(10);					//��ʱ
//   SPI_WriteByte(0x90);				//����Y���ȡ��־
//   SpiDelay(10);					//��ʱ
//   y=SPI_WriteByte(0x00);			//������ȡ16λ������ 
//   y<<=8;
//   y+=SPI_WriteByte(0x00);
//   SpiDelay(10);					//��ֹXPT2046
//   TP_DCS(); 					    								  
//   y = y>>3;						//��λ�����12λ����Ч����0-4095
//   return (y);
//}


/*******************************************************************************
*	��������InitNet
*	��  ��:
*	��  ��:
*	����˵������ʼ������Ӳ����UIPЭ��ջ�����ñ���IP��ַ
************************************************************/
void InitNet(void)
{	//uip_ipaddr_t ipaddr;
//	myip[0]=192; myip[1]=168; myip[2]=1; myip[3]=15;
//	routeip[0]=192; routeip[1]=168; routeip[2]=1; routeip[3]=1;
//	netmask[0]=255; netmask[1]=255; netmask[2]=255; netmask[3]=0;
//	TCP_S_Link=0;                            //TCP�������Ϳͻ��˽�����־
//
//	tapdev_init();                     		 //ENC28J60��ʼ��
//	
//	USART_OUT(USART1,"uip_init\n\r");
//	uip_init();								 //UIPЭ��ջ��ʼ��
//
//	USART_OUT(USART1,"uip ip address : %d,%d,%d,%d\r\n",myip[0],myip[1],myip[2],myip[3]);
//	uip_ipaddr(ipaddr, myip[0],myip[1],myip[2],myip[3]);		 //����IP��ַ
//	uip_sethostaddr(ipaddr);
//
//	USART_OUT(USART1,"uip route address : %d,%d,%d,%d\r\n",routeip[0],routeip[1],routeip[2],routeip[3]);
//	uip_ipaddr(ipaddr, routeip[0],routeip[1],routeip[2],routeip[3]);		 //����Ĭ��·����IP��ַ
//	uip_setdraddr(ipaddr);
//
//	USART_OUT(USART1,"uip net mask : %d,%d,%d,%d\r\n",netmask[0],netmask[1],netmask[2],netmask[3]);
//	uip_ipaddr(ipaddr, netmask[0],netmask[1],netmask[2],netmask[3]);		 //������������
//	uip_setnetmask(ipaddr);

}

/****************************************************************************
* ��    �ƣ�void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...)
* ��    �ܣ���ʽ�������������
* ��ڲ�����USARTx:  ָ������
			Data��   ��������
			...:     ��������
* ���ڲ�������
* ˵    ������ʽ�������������
        	"\r"	�س���	   USART_OUT(USART1, "abcdefg\r")   
			"\n"	���з�	   USART_OUT(USART1, "abcdefg\r\n")
			"%s"	�ַ���	   USART_OUT(USART1, "�ַ����ǣ�%s","abcdefg")
			"%d"	ʮ����	   USART_OUT(USART1, "a=%d",10)
* ���÷������� 
****************************************************************************/
void Printf(unsigned char *Data,...){ 
   
	const char *s;
	unsigned char* temp;
    int d;
    char buf[16];
    va_list ap;
    va_start(ap, Data);
	while(*Data!=0){				                          //�ж��Ƿ񵽴��ַ���������
		if(*Data==0x5c){									  //'\'
			switch (*++Data){
				case 'r':							          //�س���
					USART_SendData(USART1, 0x0d);	   

					Data++;
					break;
				case 'n':							          //���з�
					USART_SendData(USART1, 0x0a);	
					Data++;
					break;
				
				default:
					Data++;
				    break;
			}
			
			 
		}
		else if(*Data=='%'){									  //
	        //first, we escape length control
      	    temp = (Data + 1);
			while(*temp >= '0' && *temp <= '9') {temp++;};
			Data = temp - 1;

			switch (*++Data){				
				case 's':										  //�ַ���
                	s = va_arg(ap, const char *);
                	for ( ; *s; s++) {
                    	USART_SendData(USART1,*s);
						while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);
                	}
					Data++;
                	break;
            	case 'd':										  //ʮ����
				case 'u':
                	d = va_arg(ap, int);
                	itoa(d, buf, 10);
                	for (s = buf; *s; s++) {
                    	USART_SendData(USART1,*s);
						while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);
                	}
					Data++;
                	break;
				case 'x':										  //ʮ������
                	d = va_arg(ap, int);
                	itoh(d, buf, 16);
					
                	for (s = buf; *s; s++) {
                    	USART_SendData(USART1,*s);
						while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);
                	}
					Data++;
                	break;
				default:
					Data++;
				    break;
			}		 
		}
		else USART_SendData(USART1, *Data++);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);
	}
}

/******************************************************
		��������ת�ַ�������
        char *itoa(int value, char *string, int radix)
		radix=10 ��ʾ��10����	��ʮ���ƣ�ת�����Ϊ0;  

	    ����d=-379;
		ִ��	itoa(d, buf, 10); ��
		
		buf="-379"							   			  
**********************************************************/
char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 1000000000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */

/******************************************************
		��������תʮ���ַ�������
        char *itoh(unsigned int value, char *string, int radix)
		radix=16 ��ʾ��16����	��ʮ�����ƣ�ת�����Ϊ0;  
							   			  
**********************************************************/
char *itoh(unsigned int value, char *string, int radix)
{
    unsigned int  i, d;
    int     flag = 0;
	int     index = 32;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 16)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
		*ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    for (i = 0xF0000000; i > 0; i = i >> 4)
    {
        d = value & i;
		index = index - 4;
        d = d >> index;

        if (d || flag)
        {
		    //for last and only last hex, we add a '0'
		    if(i == 0x0F && flag == 0)
			{
			    *ptr++ = 0x30;
			}
		
			if(d < 10)
		    {
                *ptr++ = (char)(d + 0x30);
			}else
			{
				*ptr++ = (char)(d - 10 + 'a');
			}
            //value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */
/******************* (C) COPYRIGHT 2011 �ܶ�STM32 *****END OF FILE****/

