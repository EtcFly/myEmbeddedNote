#include "wizchip_socket.h"
#include "wizchip_conf.h"
#include "stm32f1xx_hal.h"
#include "eth_config.h"
#include "w5500.h"



#define SOCKET_UDP_PRINTF            1
#define SOCKET_UDP_PRINTF_PORT       4000



#define W5500_REST   GPIO_PIN_4    //PC4
#define W5500_NSS    GPIO_PIN_4    //PA4
#define W5500_SCLK   GPIO_PIN_5    //PA5
#define W5500_DO     GPIO_PIN_7    //PA7   MOSI
#define W5500_DI     GPIO_PIN_6    //PA6 MISO 
#define W5500_INTn   GPIO_PIN_5    //PC5


#define W5500_NSS_H  HAL_GPIO_WritePin(GPIOA, W5500_NSS, GPIO_PIN_SET)
#define W5500_NSS_L  HAL_GPIO_WritePin(GPIOA, W5500_NSS, GPIO_PIN_RESET)

#define W5500_REST_L HAL_GPIO_WritePin(GPIOC, W5500_REST, GPIO_PIN_RESET)
#define W5500_REST_H HAL_GPIO_WritePin(GPIOC, W5500_REST, GPIO_PIN_SET)

#ifdef USING_SOFT_SPI  

#define W5500_DO_H   HAL_GPIO_WritePin(GPIOA, W5500_DO, GPIO_PIN_SET)
#define W5500_DO_L   HAL_GPIO_WritePin(GPIOA, W5500_DO, GPIO_PIN_RESET)
 
#define W5500_SCLK_H HAL_GPIO_WritePin(GPIOA, W5500_SCLK, GPIO_PIN_SET)
#define W5500_SCLK_L HAL_GPIO_WritePin(GPIOA, W5500_SCLK, GPIO_PIN_RESET)

#define W5500_DINPUT HAL_GPIO_ReadPin(GPIOA, W5500_DI)

#endif

#ifndef USING_SOFT_SPI

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

SPI_HandleTypeDef hspi1;

#endif



//进入临界区
void SPI_CrisEnter(void)
{
    __set_PRIMASK(1);
}
//退出临界区
void SPI_CrisExit(void)
{
    __set_PRIMASK(0);
}

void SPI_CS_Select(void)
{
    W5500_NSS_L;
}
void SPI_CS_Deselect(void)
{
    W5500_NSS_H;
}


//SPI1接口初始化
void SPI1_init(void)
{
    //各管脚的初始化配置
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin =  W5500_REST;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);   

    GPIO_InitStruct.Pin =  W5500_NSS; 
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	

#ifdef USING_SOFT_SPI	
    GPIO_InitStruct.Pin =  W5500_SCLK | W5500_DO;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 

    GPIO_InitStruct.Pin =  W5500_DI; 
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin =  W5500_INTn; 
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
#else	
    __HAL_RCC_SPI1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**SPI1 GPIO Configuration    
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 10;
    hspi1.State = HAL_SPI_STATE_RESET;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		Error_Handler();
	}
#endif

    W5500_NSS_H;
}
//延时函数
void delay_us(uint16_t time)
{
    uint16_t i,j;
    for(i=0;i<time;i++)
    {
        j=32;
        while(j>1)j--;
    }
}

//W5500芯片复位
void W5500_RESET(void)
{
    W5500_REST_L;//RESET置低复位
    HAL_Delay(50);
    W5500_REST_H;//RESET置高  
    HAL_Delay(10);
}

#ifdef USING_SOFT_SPI
//SPI写一字节数据
void SPI_WriteByte(uint8_t Tdata)
{
    uint8_t i,temp;
    temp=Tdata;
    for(i=0;i<8;i++)
    {
        if(temp&0x80)
            W5500_DO_H;
        else
            W5500_DO_L;
        delay_us(1);
        W5500_SCLK_L; 
        temp=(temp<<1);
        W5500_SCLK_H;
    }
}

//SPI读取一字节数据
uint8_t SPI_ReadByte(void)
{
    uint8_t i,j=0;  
    for(i=0;i<8;i++)
    {
        W5500_SCLK_L; 
        j=(j<<1);
        if(W5500_DINPUT==GPIO_PIN_SET)
            j=j|0x01;
        delay_us(1);        
        W5500_SCLK_H;
    }
    return j;
}
#else


//SPI写一字节数据
void SPI_WriteByte(uint8_t Tdata)
{
	if (HAL_SPI_Transmit(&hspi1,&Tdata, 1, 100) == HAL_OK)
	{
		
	}
}

//SPI读取一字节数据
uint8_t SPI_ReadByte(void)
{
	uint8_t tmp;
	if (HAL_SPI_Receive(&hspi1,&tmp, 1, 100) == HAL_OK)
	{
		return tmp;
	}
	return 0x00;
}

#endif


wiz_NetInfo gWIZNETINFO = { //.mac = {0x34, 0x97, 0xf6, 0x91, 0xd7, 0x0e},
							.mac = {0x10, 0x22, 0x33, 0x44, 0x55, 0x66},
                            .ip =  {192, 168, 13, 21},
                            .sn =  {255,255,255,0},
                            .gw =  {192, 168,13,1},
                            .dns = {114,114,114,114},
                            .dhcp = NETINFO_DHCP};

//注册TCP通讯相关的回调函数
void RegisterFunction(void)
{  

  reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);    //注册临界区回调函数

#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
  reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);//注册片选回调函数
#elif _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_
  reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);  
#else
   #if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SIP_) != _WIZCHIP_IO_MODE_SIP_
      #error "Unknown _WIZCHIP_IO_MODE_"
   #else
      reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect); 
   #endif
#endif
  reg_wizchip_spi_cbfunc(SPI_ReadByte, SPI_WriteByte);    //SPI读写字节回调函数
}

//初始化芯片参数
void ChipParametersConfiguration(void)
{
  uint8_t tmp;
  uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};
    if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1)
        {
            printf("WIZCHIP Initialized fail.\r\n");
            while(1);
        }

    do{
        if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1)
            {
            printf("Unknown PHY Link stauts.\r\n");
            }
      }while(tmp == PHY_LINK_OFF);
}

#include "string.h"
//初始化网络通讯参数,mac,ip等
void NetworkParameterConfiguration(void)  //Intialize the network information to be used in WIZCHIP

{

  uint8_t tmpstr[6];

  ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
  ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);
  ctlwizchip(CW_GET_ID,(void*)tmpstr);
//    //串口打印出参数
    printf("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
    printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],
            gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
    printf("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
    printf("GAR: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
    printf("SUB: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
    printf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);
    printf("======================\r\n");

}

uint8_t destip[4] = 	{192, 168, 13, 118};
#include "stdio.h"
int fputc(int ch, FILE *f)
{
   if (getSn_SR(SOCKET_UDP_PRINTF) == SOCK_UDP)
   {
	   wizchip_sendto(SOCKET_UDP_PRINTF, (uint8_t *)&ch, 1, destip, SOCKET_UDP_PRINTF_PORT);
   }
	return ch;
}

int getc(FILE *f)
{
   uint16_t port = SOCKET_UDP_PRINTF_PORT;
   int ch = -1;
   if (getSn_SR(SOCKET_UDP_PRINTF) == SOCK_UDP)
   {
	   uint16_t size;
	   
	   size = getSn_RX_RSR(SOCKET_UDP_PRINTF);
	   if (size > 0) {
		   wizchip_recvfrom(SOCKET_UDP_PRINTF, (uint8_t *)&ch, 1, destip, &port);
	   }
   }
   return ch;
}

void W5500_ChipInit(void)
{
    W5500_RESET();
    RegisterFunction();
    ChipParametersConfiguration();
    if (wizchip_socket(SOCKET_UDP_PRINTF, Sn_MR_UDP, SOCKET_UDP_PRINTF_PORT, 0X00) != SOCKET_UDP_PRINTF)
    {
		while (1) {; }
    }
	
    NetworkParameterConfiguration();
} 

void eth_init(void)
{
	SPI1_init();
	W5500_ChipInit();
}

/************************************* TCP client ************************************************************/
int32_t eth_tcpClient(uint8_t sn, uint8_t* buf, uint8_t* destip, uint16_t destport)
{
   int32_t ret; // return value for SOCK_ERRORs
   uint16_t size = 0;

   // Destination (TCP Server) IP info (will be connected)
   // >> loopback_tcpc() function parameter
   // >> Ex)
   //	uint8_t destip[4] = 	{192, 168, 0, 214};
   //	uint16_t destport = 	5000;

   // Port number for TCP client (will be increased)
   static uint16_t any_port = 	50000;

   // Socket Status Transitions
   // Check the W5500 Socket n status register (Sn_SR, The 'Sn_SR' controlled by Sn_CR command or Packet send/recv status)
   switch(getSn_SR(sn))
   {
      case SOCK_ESTABLISHED :
         if(getSn_IR(sn) & Sn_IR_CON)	// Socket n interrupt register mask; TCP CON interrupt = connection with peer is successful
         {
#ifdef _LOOPBACK_DEBUG_
			printf("%d:Connected to - %d.%d.%d.%d : %d\r\n",sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
			setSn_IR(sn, Sn_IR_CON);  // this interrupt should be write the bit cleared to '1'
         }

         //////////////////////////////////////////////////////////////////////////////////////////////
         // Data Transaction Parts; Handle the [data receive and send] process
         //////////////////////////////////////////////////////////////////////////////////////////////
		 if((size = getSn_RX_RSR(sn)) > 0) // Sn_RX_RSR: Socket n Received Size Register, Receiving data length
         {
			if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE; // DATA_BUF_SIZE means user defined buffer size (array)
			ret = wizchip_recv(sn, buf, size); // Data Receive process (H/W Rx socket buffer -> User's buffer)

			if(ret <= 0) return ret; // If the received data length <= 0, receive failed and process end
         }
		 
//		while(size != DATA_BUF_SIZE)
//		{
			ret = wizchip_send(sn, buf, DATA_BUF_SIZE); // Data send process (User's buffer -> Destination through H/W Tx socket buffer)
			if(ret < 0) // Send Error occurred (sent data length < 0)
			{
				wizchip_close(sn); // socket close
				return ret;
			}
//			sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
//		} 
		 //////////////////////////////////////////////////////////////////////////////////////////////
         break;

      case SOCK_CLOSE_WAIT :
#ifdef _LOOPBACK_DEBUG_
         //printf("%d:CloseWait\r\n",sn);
#endif
         if((ret=wizchip_disconnect(sn)) != SOCK_OK) return ret;
#ifdef _LOOPBACK_DEBUG_
         printf("%d:Socket Closed\r\n", sn);
#endif
         break;

      case SOCK_INIT :
#ifdef _LOOPBACK_DEBUG_
    	 printf("%d:Try to connect to the %d.%d.%d.%d : %d\r\n", sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
    	 if( (ret = wizchip_connect(sn, destip, destport)) != SOCK_OK) return ret;	//	Try to TCP connect to the TCP server (destination)
         break;

      case SOCK_CLOSED:
    	  wizchip_close(sn);
    	  if((ret=wizchip_socket(sn, Sn_MR_TCP, any_port++, 0x00)) != sn){
         if(any_port == 0xffff) any_port = 50000;
         return ret; // TCP socket open with 'any_port' port number
        } 
#ifdef _LOOPBACK_DEBUG_
    	 //printf("%d:TCP client loopback start\r\n",sn);
         //printf("%d:Socket opened\r\n",sn);
#endif
         break;
      default:
         break;
   }
   return 1;
}

/************************************* TCP Servel ************************************************************/
int32_t eth_tcpServel(uint8_t sn, uint8_t* buf, uint16_t port, 
					void *(*callback)(void *arg, uint16_t size, uint16_t *sendbuff_size, uint32_t *total_size))
{
   int32_t ret;
   uint16_t size = 0;
   uint16_t sentsize=0;
   uint32_t total_size;
   uint8_t *pSendBuff = 0;

#ifdef _LOOPBACK_DEBUG_
   uint8_t destip[4];
   uint16_t destport;
#endif

   switch(getSn_SR(sn))
   {
      case SOCK_ESTABLISHED :
         if(getSn_IR(sn) & Sn_IR_CON)
         {
#ifdef _LOOPBACK_DEBUG_
			getSn_DIPR(sn, destip);
			destport = getSn_DPORT(sn);

			printf("%d:Connected - %d.%d.%d.%d : %d\r\n",sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
			setSn_IR(sn,Sn_IR_CON);
         }
		 if((size = getSn_RX_RSR(sn)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
         {
			if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
			ret = wizchip_recv(sn, buf, size);

			if(ret <= 0) return ret;      // check SOCKERR_BUSY & SOCKERR_XXX. For showing the occurrence of SOCKERR_BUSY.
			
			if (0 != callback)
			{
				pSendBuff = (*callback)((void *)buf, size, &sentsize, &total_size);
				if (0 != pSendBuff && sentsize > 0) {
					while (total_size >= sentsize) {
						ret = wizchip_send(sn, pSendBuff, sentsize);
						if(ret < 0)	{
							wizchip_close(sn);
							return ret;
						}
						
						total_size -= sentsize;
						pSendBuff += sentsize;
					}
					
					if (total_size < sentsize) {
						ret = wizchip_send(sn, pSendBuff, total_size);
						if(ret < 0)	{
							wizchip_close(sn);
							return ret;
						}	
					}
				}
			}
		}
         break;
      case SOCK_CLOSE_WAIT :
#ifdef _LOOPBACK_DEBUG_
         //printf("%d:CloseWait\r\n",sn);
#endif
         if((ret = wizchip_disconnect(sn)) != SOCK_OK) return ret;
#ifdef _LOOPBACK_DEBUG_
         printf("%d:Socket Closed\r\n", sn);
#endif
         break;
      case SOCK_INIT :
#ifdef _LOOPBACK_DEBUG_
    	 printf("%d:Listen, TCP server loopback, port [%d]\r\n", sn, port);
#endif
         if( (ret = wizchip_listen(sn)) != SOCK_OK) return ret;
         break;
      case SOCK_CLOSED:
#ifdef _LOOPBACK_DEBUG_
         //printf("%d:TCP server loopback start\r\n",sn);
#endif
         if((ret = wizchip_socket(sn, Sn_MR_TCP, port, 0x00)) != sn) return ret;
#ifdef _LOOPBACK_DEBUG_
         //printf("%d:Socket opened\r\n",sn);
#endif
         break;
      default:
         break;
   }
   return 1;
}


/************************************* UDP Servel************************************************************/
int32_t eth_udpServel(uint8_t sn, uint8_t* buf, uint16_t port, 
					void *(*callback)(void *buff, uint16_t rxSize, 
						 uint16_t *sentSize, uint32_t *totalSize))
{
   int32_t  ret;
   uint32_t total_size;
   uint16_t size, sentsize;
   uint8_t  destip[4];
   uint16_t destport;
   uint8_t *psendBuff = 0;
	
   switch(getSn_SR(sn))
   {
      case SOCK_UDP :
         if((size = getSn_RX_RSR(sn)) > 0)
         {
            if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
            ret = wizchip_recvfrom(sn, buf, size, destip, (uint16_t*)&destport);
            if(ret <= 0)
            {
#ifdef _LOOPBACK_DEBUG_
               printf("%d: recvfrom error. %ld\r\n",sn,ret);
#endif
               return ret;
            }
			
			if (0 != callback) {
				psendBuff = callback(buf, ret, &sentsize, &total_size);
				while (total_size >= sentsize) {
				   ret = wizchip_sendto(sn, psendBuff, sentsize, destip, destport);
				   if(ret < 0)
				   {
#ifdef _LOOPBACK_DEBUG_
					  printf("%d: sendto error. %ld\r\n",sn,ret);
#endif
					  return ret;
				   }
				   total_size -= sentsize; // Don't care SOCKERR_BUSY, because it is zero.
				   psendBuff += sentsize;
				}
				
				if (total_size < sentsize) {
				   ret = wizchip_sendto(sn, psendBuff, total_size, destip, destport);
				   if(ret < 0)
				   {
#ifdef _LOOPBACK_DEBUG_
					  printf("%d: sendto error. %ld\r\n",sn,ret);
#endif
					  return ret;
				   }
				}
			}
         }
         break;
      case SOCK_CLOSED:
#ifdef _LOOPBACK_DEBUG_
         //printf("%d:UDP loopback start\r\n",sn);
#endif
         if((ret = wizchip_socket(sn, Sn_MR_UDP, port, 0x00)) != sn)
            return ret;
#ifdef _LOOPBACK_DEBUG_
         printf("%d:Opened, UDP loopback, port [%d]\r\n", sn, port);
#endif
         break;
      default :
         break;
   }
   return 1;
}

/************************************* UDP Client************************************************************/
int32_t eth_udpClient(uint8_t sn, uint8_t* buf, uint32_t len, uint8_t *destip, uint16_t port)
{
   int32_t  ret;
   uint16_t size;
   uint16_t destport = 0x00;

   switch(getSn_SR(sn))
   {
      case SOCK_UDP :
		  
		 if((size = getSn_RX_RSR(sn)) > 0)
		 {
			if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
			ret = wizchip_recvfrom(sn, buf, size, destip, (uint16_t*)&destport);
			if(ret <= 0)
			{
#ifdef _LOOPBACK_DEBUG_
			   printf("%d: recvfrom error. %ld\r\n",sn,ret);
#endif
			   return ret;
			}

		 }
		 
		 if (destport != port)
		 {
			destport = port;
		 }
		 
		while (len >= DATA_BUF_SIZE) {
		   ret = wizchip_sendto(sn, buf, DATA_BUF_SIZE, destip, destport);
		   if(ret < 0)
		   {
#ifdef _LOOPBACK_DEBUG_
			  printf("%d: sendto error. %ld\r\n",sn,ret);
#endif
			  return ret;
		   }
		   len -= DATA_BUF_SIZE; // Don't care SOCKERR_BUSY, because it is zero.
		   buf += DATA_BUF_SIZE;
		}
		
		if (len < DATA_BUF_SIZE) {
		   ret = wizchip_sendto(sn, buf, len, destip, destport);
		   if(ret < 0)
		   {
#ifdef _LOOPBACK_DEBUG_
			  printf("%d: sendto error. %ld\r\n",sn,ret);
#endif
			  return ret;
		   }
		}
			  
         break;
      case SOCK_CLOSED:
#ifdef _LOOPBACK_DEBUG_
         //printf("%d:UDP loopback start\r\n",sn);
#endif
         if((ret = wizchip_socket(sn, Sn_MR_UDP, port, 0x00)) != sn)
            return ret;
#ifdef _LOOPBACK_DEBUG_
         printf("%d:Opened, UDP loopback, port [%d]\r\n", sn, port);
#endif
         break;
      default :
         break;
   }
   return 1;
}


