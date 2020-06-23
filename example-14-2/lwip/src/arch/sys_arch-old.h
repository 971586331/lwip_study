#ifndef __SYS_ARCH_H__
#define __SYS_ARCH_H__

#include    "includes.h"

//#define TOTAL_MBOX_NUM  20	//�����ں��ܹ�ʹ�õ����������Ŀ
#define MAX_MSG_IN_MBOX 100	//ÿ����������ܹ���ŵ���Ϣ��Ŀ

//�����ں�ʹ�õ�����Ľṹ
struct LWIP_MBOX_STRUCT{
	OS_EVENT * ucos_queue;						//����ucos�еĶ��л�����ʵ��
	void     *mbox_msg_entris[MAX_MSG_IN_MBOX];//�����д����Ϣ��ָ��
};


//����LwIP�ڲ�ʹ�õ���������
typedef struct LWIP_MBOX_STRUCT sys_mbox_t;  //ϵͳ��������ָ��
typedef OS_EVENT * sys_sem_t;                  //ϵͳ�ź�������ָ��
typedef INT8U     sys_thread_t;				  //ϵͳ�����ʶ

/* �ź�NULL, ����NULL ���� */ 
#define SYS_MBOX_NULL  (sys_mbox_t)NULL
#define SYS_SEM_NULL   (sys_sem_t)NULL

#define sys_sem_valid(sema) ((*sema) != NULL)
#define sys_sem_set_invalid(sema) (*sema = NULL)

/* let sys.h use binary semaphores for mutexes */
#define LWIP_COMPAT_MUTEX 1

#define sys_mbox_valid(mbox) ((mbox != NULL) && ((mbox)->ucos_queue != NULL))
#define sys_mbox_set_invalid(mbox) ((mbox)->ucos_queue = NULL)


#endif
