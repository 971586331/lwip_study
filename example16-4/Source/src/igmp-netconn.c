
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/dhcp.h"
#include "lwip/inet.h"

#include "igmp-netconn.h"

#if LWIP_NETCONN

extern struct netif enc28j60_netif;

static void igmp_netconn_thread(void *arg)
{
	//struct netconn *conn;
	//struct ip_addr serveraddr;
	//u32_t err,wr_err;
	err_t err;
	//int strlen = 0;
	struct netconn *conn;
	struct ip_addr local_addr,group_addr,remote_addr;
   
	// while(enc28j60_netif.dhcp->state != DHCP_BOUND)	//DHCP�Ƿ�����ЧIP��ַ
	// 			OSTimeDly(10);					//�ȴ���ֱ��IP��ַ��Ч

	
    local_addr = enc28j60_netif.ip_addr;        //����IP��ַ
	
	IP4_ADDR(&group_addr,233,0,0,6);			//�ಥ��ַ
	IP4_ADDR(&remote_addr,192,168,2,10);		//������ַ
	 
	conn=netconn_new(NETCONN_UDP);		//�½�UDP���͵����ӽṹ
	netconn_bind(conn,NULL,9090);			//���ڱ��ض˿�9090��
	//����ಥ��
	Printf("Join group\n");
	netconn_join_leave_group(conn,&group_addr,&local_addr,NETCONN_JOIN);
	Printf("Join group ok\n");
	while(1)
	{
	struct netbuf *inbuf = NULL;
	err = netconn_recv(conn, &inbuf); 		//�����ϵȴ���������
	Printf("recv data group\n");
	if(err == ERR_OK) 				//������Ч
	{								//�����ݷ�������remote_addr��8080�˿�
		netconn_sendto(conn,inbuf,&remote_addr,8080);
		netbuf_delete(inbuf);			//ɾ������
	 }
	}
	netconn_delete(conn);

}

void igmp_netconn_init()
{
  sys_thread_new("dhcp_netconn_thread", igmp_netconn_thread, NULL, DEFAULT_THREAD_STACKSIZE, TCPIP_THREAD_PRIO+1);
}

#endif /* LWIP_NETCONN*/
