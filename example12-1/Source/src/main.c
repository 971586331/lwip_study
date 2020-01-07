#include "includes.h"

#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_exti.h"
#include "misc.h"
#include <stdio.h>	 
#include  <stdarg.h>
#include "demo.h"

static  OS_STK App_TaskStartStk[APP_TASK_START_STK_SIZE];
static  void App_TaskStart(void* p_arg);

static  OS_STK App_TaskStartStk2[APP_TASK_START_STK_SIZE];
static  void App_TaskStart2(void* p_arg);

//extern void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...);
extern void Printf(unsigned char *Data,...);
//extern functions	
//extern void SPI1_Init(void);
extern void lwip_demo(void *pdata);


unsigned int system_tick_num = 0;
unsigned int sys_now(void)
{
    return system_tick_num;
}

/****************************************************************************
* ��    �ƣ�int main(void)
* ��    �ܣ��������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
int main(void)
{
   CPU_INT08U os_err; 
   /* ��ֹ�����ж� */
   CPU_IntDis();
   /* ucosII ��ʼ�� */
   OSInit(); 
   
   /* Ӳ��ƽ̨��ʼ�� */
   BSP_Init(); 
   LED1_ON();
   LED2_ON();
   LED3_ON();

   //printf("we are in loop1\n");
                                 
   //���������� ���ȼ����  ���������������һ����;��Ϊ���Ժ�ʹ��ͳ������
   os_err = OSTaskCreate((void (*) (void *)) App_TaskStart,               		    //ָ����������ָ��
                          (void *) 0,												//����ʼִ��ʱ�����ݸ�����Ĳ�����ָ��
               		     (OS_STK *) &App_TaskStartStk[APP_TASK_START_STK_SIZE - 1],	//���������Ķ�ջ��ջ��ָ��   �Ӷ����µݼ�
                         (INT8U) 1);								//�������������ȼ�
   OSTaskCreate((void (*) (void *)) App_TaskStart2,               		    //ָ����������ָ��
                          (void *) 0,												//����ʼִ��ʱ�����ݸ�����Ĳ�����ָ��
              		     (OS_STK *) &App_TaskStartStk2[APP_TASK_START_STK_SIZE - 1],	//���������Ķ�ջ��ջ��ָ��   �Ӷ����µݼ�
                         (INT8U) APP_TASK_START_PRIO+1);								//�������������ȼ�          
   //lwip_init_task();
   OSTimeSet(0);			 //ucosII�Ľ��ļ�������0    ���ļ�������0-4294967295  
   OSStart();                //����ucosII�ں�   
   return (0);

}
/****************************************************************************
* ��    �ƣ�static  void App_TaskStart(void* p_arg)
* ��    �ܣ���ʼ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
static  void App_TaskStart(void* p_arg)
{
 
  (void) p_arg;
   //��ʼ��ucosIIʱ�ӽ���
   OS_CPU_SysTickInit();
   //printf("we are in open system tick\n");                            
   //ʹ��ucos ��ͳ������
#if (OS_TASK_STAT_EN > 0)
   
   OSStatInit();                			//----ͳ�������ʼ������                                 
#endif
   Printf("****************zzz******************************\r\n");
   //App_TaskCreate();						//��������������
   lwip_demo(NULL);	  //��ʼ���ںˣ�����LwIP���

   /*never run*/
   Printf("****************zzz******************************\r\n");
   while (1)
   {  	  	    
	  OSTimeDlyHMSM(0, 0, 2, 0);
	  Printf("****************zzz******************************\r\n");
	  //printf("we are in open system tick\n"); 
   }
}

static void App_TaskStart2(void* p_arg)
{
  static int tmp = 0;
  (void) p_arg;
  
   Printf("****************sss******************************\r\n");
   while (1)
   {  	  	    
	  OSTimeDlyHMSM(0, 0, 4, 20);
	  if(tmp %2 == 0)
	  {
	  	 LED1_ON();
		 LED2_ON();
		 LED3_ON();
	  }
	  else
	  {
	  	 LED1_OFF();
		 LED2_OFF();
		 LED3_OFF();

	  }
	  tmp++;

	  Printf("***Led Task running***\r\n");
	  //printf("we are in open system tick\n"); 
   }



}
__asm void EnableInt(unsigned int data)
{

    MRS     R0, PRIMASK
    CPSID   I
    BX      LR
}
__asm unsigned int DisableInt(void)
{

    MSR     PRIMASK, R0;
    BX      LR
}

/*
*********************************************************************************************************
*********************************************************************************************************
*                                          uC/OS-II APP HOOKS
*********************************************************************************************************
*********************************************************************************************************
*/

#if (OS_APP_HOOKS_EN > 0)
/*
*********************************************************************************************************
*                                      TASK CREATION HOOK (APPLICATION)
*
* Description : This function is called when a task is created.
*
* Argument : ptcb   is a pointer to the task control block of the task being created.
*
* Note     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void App_TaskCreateHook(OS_TCB* ptcb)
{
}

/*
*********************************************************************************************************
*                                    TASK DELETION HOOK (APPLICATION)
*
* Description : This function is called when a task is deleted.
*
* Argument : ptcb   is a pointer to the task control block of the task being deleted.
*
* Note     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void App_TaskDelHook(OS_TCB* ptcb)
{
   (void) ptcb;
}

/*
*********************************************************************************************************
*                                      IDLE TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskIdleHook(), which is called by the idle task.  This hook
*               has been added to allow you to do such things as STOP the CPU to conserve power.
*
* Argument : none.
*
* Note     : (1) Interrupts are enabled during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 251
void App_TaskIdleHook(void)
{
    //Printf("****************App_TaskIdleHook******************************\r\n");
}
#endif

/*
*********************************************************************************************************
*                                        STATISTIC TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskStatHook(), which is called every second by uC/OS-II's
*               statistics task.  This allows your application to add functionality to the statistics task.
*
* Argument : none.
*********************************************************************************************************
*/

void App_TaskStatHook(void)
{
    
	//Printf("****************App_TaskStatHook******************************\r\n");
}

/*
*********************************************************************************************************
*                                        TASK SWITCH HOOK (APPLICATION)
*
* Description : This function is called when a task switch is performed.  This allows you to perform other
*               operations during a context switch.
*
* Argument : none.
*
* Note     : 1 Interrupts are disabled during this call.
*
*            2  It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                   will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the
*                  task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/

#if OS_TASK_SW_HOOK_EN > 0
void App_TaskSwHook(void)
{
}
#endif

/*
*********************************************************************************************************
*                                     OS_TCBInit() HOOK (APPLICATION)
*
* Description : This function is called by OSTCBInitHook(), which is called by OS_TCBInit() after setting
*               up most of the TCB.
*
* Argument : ptcb    is a pointer to the TCB of the task being created.
*
* Note     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 204
void App_TCBInitHook(OS_TCB* ptcb)
{
   (void) ptcb;
}
#endif
#endif
