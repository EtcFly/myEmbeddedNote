/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "libc_printf.h"
#include "libc_string.h"
#include "loopback.h"
#include "eth_config.h"
#include "wizchip_dns.h"
#include "wizchip_socket.h"
#include "mqtt_interface.h"
#include "MQTTClient.h"
#include "mqtt_api.h"
#include "bsp_sim7600CE.h"
#include "sfud.h"
#include "ntp.h"
#include "app_lfs.h"
#include "spiffs_app.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
uint8_t gDATABUF[2048];
uint8_t gDATABUF1[2048];
uint8_t gDATABUF2[2048] = "helloworld 9012! ---> test tcp client\r\n";
uint8_t gDATABUF3[2048] = "helloworld 9012! ---> test udp client\r\n";

#include "string.h"

__attribute__((used))static void *tcp_callback(void *arg, uint16_t size, uint16_t *sendbuff_size, uint32_t *total_size)
{
	
	static uint8_t sendBuff[1024];
	uint8_t *str = (uint8_t *)"hell --> &^%djsakdf [time] plesadfsklfkdnczaallckdfsdfjkdgkdjcvmdbsmdfrsfbdn\r\n";
	
	*sendbuff_size = 1024;
	
	*total_size = strlen((const char *)str);
	
	memcpy(sendBuff, str, *total_size);
	return sendBuff;
}

uint8_t dns_buff[512];
__attribute__((used))static void *udp_callback(void *arg, uint16_t size, uint16_t *sendbuff_size, uint32_t *total_size)
{
	static uint8_t sendBuff[1024];
	
	*sendbuff_size = 1024;
	*total_size = size;
	
	memcpy(sendBuff, arg, size);
	return sendBuff;	
}



#define SOCKET_UDP_DNS               5
//#define MAX_MQTT_SEND_BUFF          500U

MQTTMessage msg; 
uint8_t poly_data[100];
uint8_t recv_buff[100];
//MQTTClient client;
//uint8_t sendBuf[MAX_MQTT_SEND_BUFF];
//uint8_t recvBuff[MAX_MQTT_SEND_BUFF];
#include "stdio.h"
void messageHander(MessageData *data)
{
	uint8_t *pstring = 0;
	
	if (data->topicName->cstring == 0 && data->topicName->lenstring.len != 0)
	{
		pstring = (uint8_t *)data->topicName->lenstring.data;
	} 
	
	if (data->topicName->cstring != 0) 
	{
		pstring = (uint8_t *)data->topicName->cstring;
	}
	
	printf("id:%d topic:%.*s poly:%s\n", data->message->id, (data->topicName->lenstring.len), pstring, (char *)data->message->payload);
}
 	volatile int8_t cha;

#include "ff.h"
static uint8_t work[4096];
FATFS fs;
FIL fil;
FILINFO info;




int main(void)
{
  /* USER CODE BEGIN 1 */
	int32_t ret;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/
  (void)ret;
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  bsp_sim7600_init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  extern wiz_NetInfo gWIZNETINFO;
  char *s = "www.boda-its.com";
  uint8_t dns_ipv4[4];
  uint8_t destip[4] = {192,168,13,118};
//	uint16_t destport = 	5000;
//	uint8_t buf[128];
	(void)destip;

//  loopback_tcpc(1, buf, destip, destport);
	eth_init();
	DNS_init(SOCKET_UDP_DNS, dns_buff);
 
  
  printf("dns init is ok\n");
  if (DNS_run(gWIZNETINFO.dns, (uint8_t *)s, dns_ipv4) > 0)
  {
	  printf("host name: %s ---> ipv4:%d.%d.%d.%d\r\n", s, dns_ipv4[0], dns_ipv4[1], dns_ipv4[2], dns_ipv4[3]);
  }	else if (DNS_run(gWIZNETINFO.dns, (uint8_t *)s, dns_ipv4) > 0){
	  printf("host name: %s ---> ipv4:%d.%d.%d.%d\r\n", s, dns_ipv4[0], dns_ipv4[1], dns_ipv4[2], dns_ipv4[3]);
  } else {
	printf("timeout!\n");
  }
  
  uint8_t serverIp[4] = {114, 55, 56, 64};

  msg.dup = 0;
  msg.id = 10;
  msg.payload = "etcfly mqtt test!";
  msg.payloadlen = strlen((char *)msg.payload);
  msg.qos = QOS1;
  msg.retained = 0;
  if (mqtt_connect(serverIp, 20, 7, &msg) == 0)
  {
	printf("mqtt link ok!\n");
  }
  

  const char *poly = (const char *)"today is monday! ! test ok!";
  memcpy(poly_data, poly, strlen(poly));
  if (mqtt_publish(QOS0, 0, 0, 0X1560, (uint8_t *)"mqtt_test", poly_data) == 0)
  {
	printf("mqtt publish success!\n");
  }
  
  if (mqtt_subscribe("$Path/sub", 1, messageHander) == 0)
  {
	printf("mqtt subscribe success!\n");
  }

  sfud_init(); 
  FRESULT res;
  
 
  res = f_mkfs("0:", FM_ANY, 4096, work, sizeof work);
  {
	  if (res) printf("mkfs file system err! %d\n", res);
	  else printf("mkfd file system ok!\n"); 
  }
//  
//  res = f_mount(&fs, "0:", 0);
//  if (res == FR_OK) {
////	  res = f_mkfs("0:", FM_FAT32, 0, work, sizeof(work));

//  } else {
//	 while (1);
//  }

//  char *s_write = "test Again by 190906 in Nor Flash MX25L256";
//  if (res) printf("fatfs init fail %d!\n", res);
//  else { 
//	 printf("fatfs init ok!\n");
//	 f_mkdir("0:/NorFlash");
//	 res = f_open(&fil, "0:/NorFlash/EtcFly.txt", FA_WRITE| FA_OPEN_ALWAYS);
//	 if (res) printf("file write | open fail errCode:%d!\n", res);
//	 else {
//		UINT returnNum;
//		res = f_write(&fil, s_write, libc_strlen(s_write), &returnNum);
//		if (res == FR_OK) { printf("write file success! fileSize :%d write num is %d!\n", f_size(&fil), returnNum); }
//		else {printf("write file fail!\n");}
//		
//		f_close(&fil);
		
//		static DIR dir;
//		static char path[20] = "/";
//		if (f_opendir(&dir, path) == FR_OK)
//		{
//			while (f_readdir(&dir, &info) == FR_OK)
//			{
//				if (info.fname[0] == '\0')
//				{
//					break;
//				} else 	{ 
//					if (info.fattrib & AM_ARC) {	printf("file Name :%s fileSize:%d\n", info.fname, info.fsize);
//					} 
//					if (info.fattrib & AM_DIR) {
//						printf("Directory Name:%s DirectorySize:%d\n", info.fname, info.fsize);
//					}
//				}
//			} 
//		} else printf("open dir fail!\n");
//		
//		res = f_open(&fil, "0:/NorFlash/EtcFly.txt", FA_WRITE|FA_READ);
//		if (res) printf("read open fail! --- errcode:%d\n", res);
//		else {
//			uint8_t *fileBuff =0;
//			fileBuff = malloc(100);
//			if (fileBuff == 0)printf("malloc memory fail!\n");
//			else printf("malloc memory ok!\n");
//			
//			FILINFO infor;
//			f_stat("0:/NorFlash/EtcFly.txt", &infor);
//			res = f_read(&fil, fileBuff, infor.fsize, &returnNum);
//			if (res) printf("read file fail! --- errcode:%d\n", res);
//			else {
//				printf("read file:%s read size:%d file:%s\n", "EctFly.txt", returnNum, fileBuff);
//				f_close(&fil);
//			}
//			free(fileBuff);
//		}
//	}
//  }
//  printf("/*******************************fatfs write/read speed test *******************************/");
//  static uint8_t write_buff[512];
//  uint32_t fileSize, timStart, timEnd;
//  if (f_open(&fil, "0:/hello.txt", FA_WRITE| FA_READ | FA_OPEN_ALWAYS) == FR_OK)
//  {
//	  UINT bw;
//	  printf("fatfs begin test:%d\n", timStart = HAL_GetTick());
//	 for (uint16_t i = 0; i < 2*1024; i++)
//	 {
//		if (f_write(&fil, write_buff, 512, &bw) != FR_OK || bw != 512){
//		    printf("write fail index:%d bw:%d\n", i, bw);
//			break;
//		}
//	 }
//     printf("fatfs end test:%d\n", timEnd = HAL_GetTick());
//	 f_sync(&fil);
//	 f_stat("0:/hello.txt", &info);
//	 fileSize = info.fsize;
//	 printf("fileName:%s fileSize:%d writeSpeed:%d\n", info.fname, info.fsize, (int)(fileSize*1.0f*1000/(timEnd-timStart)));
//  }
//  
//  UINT bw;
//  if (f_lseek(&fil, 0) == FR_OK)
//  {
//	printf("seek fatfs ok!\n");
//  }
//  printf("fatfs begin test:%d\n", timStart = HAL_GetTick());
//  for (uint16_t i=0; i<2*1024; i++)
//  {
//	  if (f_read(&fil, write_buff, 512, &bw) != FR_OK || bw != 512)
//	  {
//		  printf("read fail index:%d bw:%d\n", i, bw);
//		  break;
//	  }
//  }
//  printf("fatfs end test:%d\n", timEnd = HAL_GetTick());
//  f_sync(&fil);
//  fileSize = info.fsize;
//  f_stat("0:/hello.txt", &info);
//  printf("fileName:%s fileSize:%d readSpeed:%d\n", info.fname, info.fsize, (int)(fileSize*1.0f*1000/(timEnd-timStart)));
//  f_close(&fil);
//  printf("/*******************************fatfs write/read speed test end*******************************/");	
//  
//  ntpclient_init();
//  printf("littlefs test begin.....\n");
//  lfs_test();
  
  
  spiffs_test();
  while (1)
  {
    /* USER CODE END WHILE */
		/* Loopback Test */
		// TCP server loopback test
//		if( (ret = loopback_tcps(0, gDATABUF, 1000)) < 0) {
//	//		printf("SOCKET ERROR : %ld\r\n", ret);
//		}
//		if( (ret = loopback_tcpc(1, gDATABUF, destip, 2000)) < 0) {
//	//		printf("SOCKET ERROR : %ld\r\n", ret);
//		}
//    // UDP server loopback test
//		if( (ret = loopback_udps(2, gDATABUF, 3000)) < 0) {
//	//		printf("SOCKET ERROR : %ld\r\n", ret);
//		}
    /* USER CODE BEGIN 3 */
//	  eth_tcpServel(0, gDATABUF, 1120, tcp_callback);
//	  eth_udpServel(1, gDATABUF1, 1130, udp_callback);
//	  eth_tcpClient(2, gDATABUF2, destip, 2000);	
      void MQTTRun(void* parm);
	  extern MQTTClient s_client;
	  MQTTRun((void *)&s_client);
	  	  
	HAL_Delay(1);
	do_ntp_client();
//	tim++;
//	if (tim == 15000)
//	{
//		tim = 0;
//		if (sim7600ce_isPwrUp() == 0)
//		{
//		uint8_t *simSend = (uint8_t *)"AT+NETOPEN\r";
//		if (sim7600ce_send(simSend, libc_strlen((const char *)simSend)) == 0) {
//			printf("sim7600 send ok!\n");
//		}
//		}	
//	}

	if ((cha = getchar()) != -1)
	{
		if (cha >= '0' && cha <= '9') {
			sim7600ce_run(cha - '0');
		}
		printf("%c", cha);
	}
	
	uint8_t len;
	len = sim7600ce_recv(recv_buff, sizeof(recv_buff));
	if (len > 0)
	{
		uint8_t ch;
		printf("recv:");
		for (uint8_t i = 0; i < len; i++)
		{
			ch = recv_buff[i];
			if ('\r' == ch)
				printf("%s", "<\\r>");
			else if ('\n' == ch) {
				printf("%s", "<\\n>");
			}
			else
				printf("%c", ch);
		}
		printf("\n");
	} 
	

//	  wizchip_sendto(SOCKET_UDP_DNS, (uint8_t *)"hello wrold\n", sizeof("hello wrold\n"), destip, 5000);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
