
#include "def.h"
#include "sys.h"
#include "err.h"
//ucosii���ڴ����ṹ�����ǽ���������ռ�ͨ���ڴ����ṹ������
//static OS_MEM *MboxMem = NULL;
//static char MboxMemoryArea[TOTAL_MBOX_NUM * sizeof(struct LWIP_MBOX_STRUCT)];
const u32_t NullMessage;//�����ָ��Ͷ�ݵ�����
//����ϵͳʹ�õĳ�ʱ������ָ��ṹ
//struct sys_timeouts global_timeouts;
//��ϵͳ�����½�������صı�������

#define LWIP_MAX_TASKS 4 	//�����ں���ഴ�����������
#define LWIP_STK_SIZE  512	//ÿ������Ķ�ջ�ռ�
OS_STK  LWIP_STK_AREA[LWIP_MAX_TASKS][LWIP_STK_SIZE];

//�������ܣ�����ģ����ʼ�������������Ҫ���ڴ�ռ�ĳ�ʼ��
//�Լ�ϵͳ��ʱ������ָ��ĳ�ʼ�����ں�����ʼ��ʱ��������������
void sys_init(void)
{
    u8_t   Err;
  //  MboxMem = OSMemCreate( (void*)MboxMemoryArea, 
  //                      TOTAL_MBOX_NUM, sizeof(sys_mbox_t), &Err );
    //global_timeouts.next =NULL;
}
//�������ܣ�����һ���ź������ź�����ʼֵΪcount
//ֱ�ӵ���ϵͳ�еĺ�������ź�������
//����ֵ��ָ���ź�����ָ�룬������ʧ�ܣ�����Ϊ0
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
  LWIP_ASSERT("[Sys_arch]sem != NULL", sem != NULL);

  *sem = OSSemCreate((u16_t)count);
  LWIP_ASSERT("[Sys_arch]Error creating sem", *sem != NULL);
  if(*sem != NULL) {
    return ERR_OK;
  }
   
  return ERR_MEM;
}

//�������ܣ�ɾ��һ���ź���
void sys_sem_free(sys_sem_t *sem)
{
  u8_t Err;
  // parameter check 
  LWIP_ASSERT("sem != NULL", sem != NULL);

  OSSemDel(*sem, OS_DEL_ALWAYS, &Err);
	
  if(Err != OS_ERR_NONE)
  {
    //add error log here
    Printf("[Sys_arch]free sem fail\n");
  }

  *sem = NULL;
}




//�������ܣ��ͷ�һ���ź���
void sys_sem_signal(sys_sem_t *sem)
{
  u8_t Err;
  LWIP_ASSERT("sem != NULL", sem != NULL);

  Err = OSSemPost(*sem);
  if(Err != OS_ERR_NONE)
  {
        //add error log here
        Printf("[Sys_arch]:signal sem fail\n");
  }
  
  LWIP_ASSERT("Error releasing semaphore", Err == OS_ERR_NONE);
}

//�������ܣ��������̣��ȴ�һ���ź����ĵ��������timeout��Ϊ0�������������ʱ�����Ϊ
//��صĺ��������������һֱ������ֱ���յ��ź���
//����ֵ�����timeout��Ϊ0���򷵻�ֵΪ�ȴ����ź����ĺ���������������ڹ涨��ʱ����
//û�еȵ��ź������򷵻�ֵΪSYS_ARCH_TIMEOUT������ź����ڵ��ú���ʱ�Ѿ����ã�����
//���ᷢ���κ���������������ֵ��ʱ������0
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
  u8_t Err;
  u32_t wait_ticks;
  u32_t start, end;
  LWIP_ASSERT("sem != NULL", sem != NULL);

  if (OSSemAccept(*sem))		  // ����Ѿ��յ�, �򷵻�0 
  {
	  //Printf("debug:sem accept ok\n");
	  return 0;
  }   
  
  wait_ticks = 0;
  if(timeout!=0){
	 wait_ticks = (timeout * OS_TICKS_PER_SEC)/1000;
	 if(wait_ticks < 1)
		wait_ticks = 1;
	 else if(wait_ticks > 65535)
			wait_ticks = 65535;
  }

  start = sys_now();
  OSSemPend(*sem, (u16_t)wait_ticks, &Err);
  end = sys_now();
  
  if (Err == OS_NO_ERR)
		return (u32_t)(end - start);		//���ȴ�ʱ������Ϊtimeout/2
  else
		return SYS_ARCH_TIMEOUT;
  
}


//�������ܣ�����һ�����䣬���������ɵ���Ϣ��Ϊsize����������Ϣ�ı�����һ��ָ�룬
//��ָ������Ϣ�ľ���λ�ã���ʱ�û���Ҫ�����ں��о���������ʽ�ĺ꣺_MBOX_SIZE
//��Щ���Ĭ��ֵΪ0����ʾ�˸�������Ĵ�С����Ȼ�û����Ժ���size��ֱ��ʹ���Լ���
//Ĭ�ϴ�С����ʱû�б�Ҫ�ٶ���_MBOX_SIZE��ʽ�ĺ�
err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
  LWIP_ASSERT("mbox != NULL", mbox != NULL);
  LWIP_UNUSED_ARG(size);

  mbox->ucos_queue = OSQCreate( &(mbox->mbox_msg_entris[0]), MAX_MSG_IN_MBOX );
  LWIP_ASSERT("Error creating queue", mbox->ucos_queue != NULL);
  if(mbox->ucos_queue == NULL) {
    return ERR_MEM;
  }
  memset(mbox->mbox_msg_entris, 0, sizeof(void *)*MAX_MSG_IN_MBOX);
  
  return ERR_OK;
}

//�������ܣ��ͷ����䣬��������ͷ�ʱ�����л���������Ϣ����˵�����û�Ӧ�ó������
//�û�Ӧ�����н������������
void sys_mbox_free(sys_mbox_t *mbox)
{
  /* parameter check */
  u8_t Err;
  LWIP_ASSERT("mbox != NULL", mbox != NULL);


  OSQFlush(mbox->ucos_queue);
	
  OSQDel(mbox->ucos_queue, OS_DEL_ALWAYS, &Err);
	
  if(Err != OS_ERR_NONE)
  {
    //add error log here
    Printf("[Sys_arch]free Mbox queue fail\n");
  }

  mbox->ucos_queue= NULL;
}


//�������ܣ������䷢��һ����Ϣ��������Ͷ��������������������ֱ�����ͳɹ�
void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
    LWIP_ASSERT("mbox != NULL", mbox != NULL);
	
    if (msg == NULL)
    	msg = (void*)&NullMessage;//�����ָ��Ͷ�ݵ�����
	while (OSQPost(mbox->ucos_queue, msg) == OS_Q_FULL)
		   OSTimeDly(2);
}

//�������ܣ����������䷢��һ����Ϣ�����ɹ��򷵻�ERR_OK������������ˣ��򷵻�ERR_MEM
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
    LWIP_ASSERT("mbox != NULL", mbox != NULL);
	
    if (msg == NULL)
    	msg = (void*)&NullMessage;//�����ָ��Ͷ�ݵ�����

    if (OSQPost(mbox->ucos_queue, msg) != OS_NO_ERR)
    {
        return ERR_MEM;
    }
    
    return ERR_OK;
}

//�������ܣ���һ������ȴ���Ϣ�����timeout��Ϊ0�����ʶ�ȴ���������������ʱ
//���ȴ��ɹ����򷵻�ֵ��ʶ�ȴ��ĺ���������Ϊ0����ʾ������һֱ�ȴ���ֱ���յ���Ϣ
//data����ΪNULL����ʱ����ʶ�����н��յ���������Ϣ���뱻������
u32_t
sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
	u8_t Err;
	u32_t wait_ticks;
	u32_t start, end;
    void *Data;
    LWIP_ASSERT("mbox != NULL", mbox != NULL);
	LWIP_ASSERT("*msg != NULL", msg != NULL);
	
    Data = OSQAccept(mbox->ucos_queue, &Err);
	if (Data != NULL)		
	{
        if (Data == (void*)&NullMessage)
        {
            *msg = NULL;
        }
        else
        {
            *msg = Data;
        }
		return 0;
	}
	
	wait_ticks = 0;
	if(timeout!=0){
	   wait_ticks = (timeout * OS_TICKS_PER_SEC)/1000;
	   if(wait_ticks < 1)
  	      wait_ticks = 1;
       else if(wait_ticks > 65535)
  	          wait_ticks = 65535;
	}

    Data = OSQPend(mbox->ucos_queue, (u16_t)wait_ticks, &Err);

	if (Data != NULL)		
	{
        if (Data == (void*)&NullMessage)
        {
            *msg = NULL;
        }
        else
        {
            *msg = Data;
        }
	}
    
	if (Err == OS_NO_ERR)
		return timeout/2;       //���ȴ�ʱ������Ϊtimeout/2
	else
	    return SYS_ARCH_TIMEOUT;
}

//�������ܣ����Դ�һ�������ȡ��Ϣ���ú��������������̣���������������ʱ��
//��ȡ�ɹ�������0��������������SYS_MBOX_EMPTY
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
	INT8U err;
	void *Data;
	LWIP_ASSERT("mbox != NULL", mbox != NULL);
	LWIP_ASSERT("*msg != NULL", msg != NULL);
	
	Data = OSQAccept(mbox->ucos_queue, &err);
	
    if (Data != NULL)		
	{
        if (Data == (void*)&NullMessage)
        {
            *msg = NULL;
        }
        else
        {
            *msg = Data;
        }
		return 0;
	}
	else
	{
        return SYS_MBOX_EMPTY;
    }
}
//���ؽ��̵ĳ�ʱʱ��ṹ�����������ǽ�ϵͳ���еĽ���ʹ��ͬһ����ʱ����

//�������ܣ��½�һ�����̣�������ϵͳ��ֻ�ᱻ����һ��
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
  static u32_t TaskCreateFlag=0;
  u8_t i=0;
  name=name;
  stacksize=stacksize;
  
  while((TaskCreateFlag>>i)&0x01){
    if(i<LWIP_MAX_TASKS&&i<32)
          i++;
    else return 0;
  }
  if(OSTaskCreate(thread, (void *)arg, &LWIP_STK_AREA[i][LWIP_STK_SIZE-1],prio)==OS_NO_ERR){
       TaskCreateFlag |=(0x01<<i); 
	   
  };

  return prio;
}


