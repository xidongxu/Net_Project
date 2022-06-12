/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_netxduo.c
  * @author  MCD Application Team
  * @brief   NetXDuo applicative file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_netxduo.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "stm32h7xx.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define  APP_CFG_TASK_NETX_PRIO			5u    
#define  APP_CFG_TASK_NETX_STK_SIZE	4096u
static uint64_t AppTaskNetXStk[APP_CFG_TASK_NETX_STK_SIZE / 8];
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define DEFAULT_PORT    	8090

#define IP_ADDR0        	192
#define IP_ADDR1					168
#define IP_ADDR2					0
#define IP_ADDR3					24

#define  MAX_TCP_CLIENTS	1
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
UCHAR data_buffer[4096];
NX_PACKET_POOL pool_0;
NX_IP ip_0;

#define NX_PACKET_POOL_SIZE ((1536 + sizeof(NX_PACKET)) * 20)
ULONG packet_pool_area[NX_PACKET_POOL_SIZE/4 + 4];

ULONG arp_space_area[512 / sizeof(ULONG)];                                                      
ULONG error_counter;

NX_TCP_SOCKET TCPSocket;
TX_SEMAPHORE Semaphore;

#define PRINT_DATA(addr, port, data) do {                                            		\
																						printf("[%lu.%lu.%lu.%lu:%u] -> '%s' \r\n", \
																						(addr >> 24) & 0xff,                      	\
																						(addr >> 16) & 0xff,                      	\
																						(addr >> 8) & 0xff,                       	\
																						(addr & 0xff), port, data);               	\
																				} while(0)

extern TX_THREAD   AppTaskNetXProTCB;
extern TX_THREAD   *netx_thread_ptr;                                               
extern VOID nx_stm32_eth_driver(NX_IP_DRIVER *driver_req_ptr);
static VOID tcp_listen_callback(NX_TCP_SOCKET *socket_ptr, UINT port);

TX_THREAD thread_1;
UCHAR thread_1_stack[1024] = { 0 };
void thread_1_entry(ULONG thread_input);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */
/**
  * @brief  Application NetXDuo Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT MX_NetXDuo_Init(VOID *memory_ptr)
{
  UINT ret = NX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

  /* USER CODE BEGIN MX_NetXDuo_MEM_POOL */
  (void)byte_pool;
  /* USER CODE END MX_NetXDuo_MEM_POOL */

  /* USER CODE BEGIN MX_NetXDuo_Init */
  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 0, thread_1_stack, sizeof(thread_1_stack), 1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
  /* USER CODE END MX_NetXDuo_Init */

  return ret;
}

/* USER CODE BEGIN 1 */
void thread_1_entry(ULONG thread_input)
{
	UINT status;
	UINT ret;
	ULONG socket_state;
	UINT old_priority;

	ULONG source_ip_address;
	NX_PACKET *data_packet;

	UINT source_port;
	ULONG bytes_read;
	
	tx_semaphore_create(&Semaphore, "App Semaphore", 0);

	nx_system_initialize();

	status = nx_packet_pool_create(&pool_0, 
																 "NetX Main Packet Pool",
																 1536,  (ULONG*)(((int)packet_pool_area + 15) & ~15),
																 NX_PACKET_POOL_SIZE);
	printf("nx packet pool create status = %d\r\n", status);
	status = nx_ip_create(&ip_0,
												"NetX IP Instance 0",
												IP_ADDRESS(IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3),
												0xFFFFFF00UL,
												&pool_0, nx_stm32_eth_driver,
												(UCHAR*)AppTaskNetXStk,
												sizeof(AppTaskNetXStk),
												APP_CFG_TASK_NETX_PRIO);
	printf("nx ip create status = %d\r\n", status);
	status = nx_arp_enable(&ip_0, (void *)arp_space_area, sizeof(arp_space_area));
	printf("nx arp enable status = %d\r\n", status);
	status = nx_ip_fragment_enable(&ip_0);
	printf("nx ip enable status = %d\r\n", status);
	status = nx_tcp_enable(&ip_0);
	printf("nx tcp enable status = %d\r\n", status);
	status = nx_udp_enable(&ip_0);
	printf("nx udp enable status = %d\r\n", status);
	status = nx_icmp_enable(&ip_0);
	printf("nx icmp enable status = %d\r\n", status);
	ret = nx_tcp_socket_create(&ip_0, 
														 &TCPSocket, 
														 "TCP Server Socket", 
														 NX_IP_NORMAL, 
														 NX_FRAGMENT_OKAY,
														 NX_IP_TIME_TO_LIVE, 
														 4320, 
														 NX_NULL, 
														 NX_NULL);
	if(ret)
	{
		printf("File: %s in %d is error!\r\n",__FILE__, __LINE__);
	}
	ret = nx_tcp_server_socket_listen(&ip_0, DEFAULT_PORT, &TCPSocket, MAX_TCP_CLIENTS, tcp_listen_callback);
	if(ret)
	{
		printf("File: %s in %d is error!\r\n",__FILE__, __LINE__);
	}
	if(tx_semaphore_get(&Semaphore, TX_WAIT_FOREVER) != TX_SUCCESS)
	{
		
	}
	else
	{
		ret = nx_tcp_server_socket_accept(&TCPSocket, TX_WAIT_FOREVER);
		if (ret)
		{
			printf("File: %s in %d is error!\r\n",__FILE__, __LINE__);
		}
	}
	while(1)
	{
		TX_MEMSET(data_buffer, '\0', sizeof(data_buffer));
		nx_tcp_socket_info_get(&TCPSocket, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &socket_state, NULL, NULL, NULL);
		if(socket_state != NX_TCP_ESTABLISHED)
		{
			ret = nx_tcp_server_socket_accept(&TCPSocket, NX_IP_PERIODIC_RATE);
		}
		if(ret == NX_SUCCESS)
		{
			ret = nx_tcp_socket_receive(&TCPSocket, &data_packet, NX_WAIT_FOREVER);
			if (ret == NX_SUCCESS)
			{
				nx_udp_source_extract(data_packet, &source_ip_address, &source_port);
				nx_packet_data_retrieve(data_packet, data_buffer, &bytes_read);
				PRINT_DATA(source_ip_address, source_port, data_buffer);
				ret =  nx_tcp_socket_send(&TCPSocket, data_packet, NX_IP_PERIODIC_RATE);
				if (ret == NX_SUCCESS)
				{

				}
				nx_packet_release(data_packet);
			}
			else
			{
				nx_tcp_socket_disconnect(&TCPSocket, NX_WAIT_FOREVER);
				nx_tcp_server_socket_unaccept(&TCPSocket);
				nx_tcp_server_socket_relisten(&ip_0, DEFAULT_PORT, &TCPSocket);
			}
		}
		else
		{
			
		}
	}
}

static VOID tcp_listen_callback(NX_TCP_SOCKET *socket_ptr, UINT port)
{
	tx_semaphore_put(&Semaphore);
}   

/* USER CODE END 1 */
