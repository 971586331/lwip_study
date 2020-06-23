
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/dhcp.h"
#include "lwip/inet.h"

#include "dhcp-netconn.h"

#if LWIP_NETCONN
#define MAX_BUFFER_LEN 256
char sendbuf[MAX_BUFFER_LEN];

extern struct netif enc28j60_netif;

static void dhcp_netconn_thread(void *arg)
{
	struct netconn *conn;
	struct ip_addr serveraddr;
	u32_t err,wr_err;
	int strlen = 0;
   
	while(enc28j60_netif.dhcp->state != DHCP_BOUND)	//DHCP�Ƿ�����ЧIP��ַ
				OSTimeDly(10);					//�ȴ���ֱ��IP��ַ��Ч

	Printf("ip_addr = %d \n", enc28j60_netif.ip_addr);
	
	IP4_ADDR(&serveraddr,192,168,2,10); 			//���������IP��ַ
	
	while(1)
	{
	   conn=netconn_new(NETCONN_TCP);			//����TCP���ӽṹ
	   err=netconn_connect(conn,&serveraddr,8080);	//���ӷ��������˿ں�8080
	 
	   if(err==ERR_OK) {							//���ӳɹ�
		   Printf("Connection OK \n"); 		//��ӡ��Ϣ
		   do
		   {
		      strlen = sprintf(sendbuf,"A LwIP client Using DHCP Address: %s\r\n", \
			  	ipaddr_ntoa((ip_addr_t *)&(enc28j60_netif.ip_addr)));
			  
			  wr_err=netconn_write(conn,sendbuf, strlen, NETCONN_NOCOPY);
			  OSTimeDly(100);
		   }while(wr_err==ERR_OK);
	   }
	   Printf("Connection failed \n");
	   netconn_close(conn); 						//�ر�����
	   netconn_delete(conn);						//ɾ�����ӽṹ
	}

}

void dhcp_netconn_init()
{
  sys_thread_new("dhcp_netconn_thread", dhcp_netconn_thread, NULL, DEFAULT_THREAD_STACKSIZE, TCPIP_THREAD_PRIO+1);
}

#endif /* LWIP_NETCONN*/
