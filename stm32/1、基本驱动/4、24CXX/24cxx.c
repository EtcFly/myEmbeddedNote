/**************************************************************************************************************************
*@fileName: 24cxx.c
*@platform: MDK 5.10.0.0 + STM32F103C8T6
*@Version:  V1.0.0
*@describe: eeprom 24cxx相关驱动
*@modification
*==========================================================================================================================
* version    Author   Data                                            Details
*--------------------------------------------------------------------------------------------------------------------------
* V1.0.0      Wy     190222                                           Create File
===========================================================================================================================
**************************************************************************************************************************/


#include "stm32f10x.h"
#include "24cxx.h"
#include "bsp_i2c_gpio.h"


/***************************************************************************************************************************
*@funcName: uint8_t ee_CheckOk(void)
*@describe: eeprom自检
*@input:    None
*@return:   0:表示正常  1:表示不正常
****************************************************************************************************************************/
static uint8_t bsp_InitEEP(void)
{
	if (i2c_CheckDevice(EEP_DEVICE_ADDR) == 0) 	{
		return 0;
	}else {	  /* 失败后发送I2C总线停止信号 */
		i2c_Stop();
		return 1;
	} 
} 


/***************************************************************************************************************************
*@funcName: void eeprom_24cxx_init(void)
*@describe: 24cxx的硬件初始化
*@input:    None
*@return:   None
****************************************************************************************************************************/
static void eeprom_24cxx_init(void)
{
  while (bsp_InitEEP());
}


///***************************************************************************************************************************
//*@funcName: void eeprom_24cxx_writeByte(uint8_t devAddr, uint8_t byte, uint32_t addr)
//*@describe: 写入1Byte数据到EEPROM
//*@input:
//     @devAddr: 设备地址
//     @byte: 写入的字节内容
//     @addr: 写入的地址
//*@return: None
//****************************************************************************************************************************/
//static uint8_t eeprom_24cxx_writeByte(uint8_t devAddr, uint8_t byte, uint32_t addr)
//{
//  uint8_t ret;

//	i2c_Start();							
//	i2c_SendByte(devAddr);				
//	i2c_WaitAck();
//	i2c_SendByte((uint8_t)((addr>>8)&0xff));
//	i2c_WaitAck();
//	i2c_SendByte((uint8_t)(addr&0xff));
//	i2c_WaitAck(); 

//	i2c_SendByte(byte);
//	ret=i2c_WaitAck();
//  
//	i2c_Stop();
//  return ret;
//}

///***************************************************************************************************************************
//*@funcName: uint8_t eeprom_24cxx_readByte(uint8_t devAddr, uint32_t addr)
//*@describe: 读取EEPROM指定位置的1Byte内容
//*@input:
//     @devAddr: 设备地址
//     @addr: 写入数据的地址
//*@return: 
//     @retV: 读取的1Byte内容
//****************************************************************************************************************************/
//static uint8_t eeprom_24cxx_readByte(uint8_t devAddr, uint32_t addr)
//{
//   uint8_t retV;

//	i2c_Start();										//启动I2C

//	i2c_SendByte(devAddr);						//发送器件地址
//	i2c_WaitAck();
//	i2c_SendByte((uint8_t)((addr>>8)&0xff));
//	i2c_WaitAck();        
//	i2c_SendByte((uint8_t)((addr)&0xff));
//	i2c_WaitAck();

//	i2c_Start();
//	i2c_SendByte(devAddr|0x01);
//	i2c_WaitAck();
//	retV = i2c_ReadByte();		//读取字节数据
//	i2c_NAck();

//	i2c_Stop();
//  return retV;
//}



/***************************************************************************************************************************
*@funcName: uint8_t eeprom_24cxx_writeBytes(const uint8_t *buff, uint32_t len, uint8_t devAddr, uint32_t addr)
*@describe: 24cxx的多字节写入，必须<PAGE_SIZE(一般为32Byte) 同时注意总线空闲时间必须满足>tBuf
*@input:
    @buff: 需要写入的内容
    @len:  需要写入的字节数
    @devAddr: 设备地址
    @addr:    内存地址
*@return: 
    @status: 写入的状态  0:写入成功   other:error
****************************************************************************************************************************/
uint8_t eeprom_24cxx_writeBytes(const uint8_t *buff, uint32_t len, uint8_t devAddr, uint32_t addr)
{
  uint8_t status=0;
	uint16_t i;
	 
	i2c_Start();	
	i2c_SendByte(devAddr);				
	i2c_WaitAck();
	i2c_SendByte((uint8_t)((addr>>8)&0xff));
	i2c_WaitAck();
	i2c_SendByte((uint8_t)(addr&0xff));
	i2c_WaitAck(); 
  
	for(i=0;i<len;i++)							
	{
		i2c_SendByte(*(buff+i));
		status=i2c_WaitAck();              
	} 

	i2c_Stop();
  return status;
}

/***************************************************************************************************************************
*@funcName: uint8_t eeprom_24cxx_readBytes(uint8_t *const buff, uint32_t len, uint8_t devAddr, uint32_t addr)
*@describe: 读取eeprom 24cxx n Byte的字节
*@input:
    @buff: 存放读取的数据内容
    @len:  需要读取的字节数
    @devAddr: 设备地址
    @addr:    内存地址
*@return: 总是返回1 (保留)
****************************************************************************************************************************/
uint8_t eeprom_24cxx_readBytes(uint8_t *const buff, uint32_t len, uint8_t devAddr, uint32_t addr)
{
	uint16_t i; 
  
	i2c_Start();										
	i2c_SendByte(devAddr);						
  
	i2c_WaitAck();
	i2c_SendByte((uint8_t)((addr>>8)&0xff));
	i2c_WaitAck();        
	i2c_SendByte((uint8_t)((addr)&0xff));
	i2c_WaitAck();

	i2c_Start();
	i2c_SendByte(devAddr|0x01);
	i2c_WaitAck();
	for(i=0;i<len-1;i++) 				
	{
	 *(buff+i)=i2c_ReadByte();		
	 i2c_Ack();
	}
	*(buff+len-1)=i2c_ReadByte();
	i2c_NAck();

	i2c_Stop();
	return(1);
}

/*******************************************************************************************************/
//application interface
/***************************************************************************************************************************
*@funcName: 将参数写入外部内存eeprom 24cxx
*@describe: 写入参数
*@input:    
    @pPara: 需要写入的数据基地址
    @startAddr: 写入数据的相对地址
    @len:  写入的数据长度
*@return:   None
****************************************************************************************************************************/
void flash_write(const uint8_t *pPara, uint32_t startAddr, uint32_t len)
{
   uint8_t *p = (uint8_t *)pPara;     
   uint32_t temp=(startAddr+SYS_EEPROM_24CXX_START_ADDR);  
   uint16_t write_len=0;  
   volatile uint16_t t =FLASH_OP_TIM_DELAY; //过短导致写入一些数据读写错误
   
   write_len = MAX_24CXX_PAGE_BUFF_SIZE - temp % MAX_24CXX_PAGE_BUFF_SIZE;
   if (write_len != 0)
   {
      eeprom_24cxx_writeBytes((uint8_t *)p, write_len, DEVICE_24CXX_ADDR, temp);
      temp += write_len;
      p += write_len;
      len -= write_len; 
      while (t--);
      t = FLASH_OP_TIM_DELAY;      
   }
   
   while (len)              
   {
     if (len > MAX_24CXX_PAGE_BUFF_SIZE) {
         write_len = MAX_24CXX_PAGE_BUFF_SIZE;    
     }else {   
         write_len = len;
     }
     eeprom_24cxx_writeBytes((uint8_t *)p, write_len, DEVICE_24CXX_ADDR, temp); 
     while (t--);
     len -= write_len;   
     temp += write_len;    
     p += write_len;       
   }
}


/***************************************************************************************************************************
*@funcName: void flash_read(uint8_t *const pPara, uint32_t startAddr, uint32_t len) 
*@describe:  从flash读取参数
*@input:
      @pPara:  存放读取数据的缓存基地址
      @startAddr: 需要被读取的数据相对地址
      @len: 读取的数据长度
*@return:  None
****************************************************************************************************************************/
void flash_read(uint8_t *const pPara, uint32_t startAddr, uint32_t len) 
{
   uint8_t *p = (uint8_t *)pPara; 
   eeprom_24cxx_readBytes(p, len, DEVICE_24CXX_ADDR, SYS_EEPROM_24CXX_START_ADDR+startAddr);
}


/***************************************************************************************************************************
*@funcName: void flash_init(uint8_t *const prdBaseAddr, uint32_t readSize)
*@describe: flash的初始化
*@input:
@prdBaseAddr:数据存放基地址
@readSize: 需要读取的数据大小
*@return:
****************************************************************************************************************************/
void flash_init(uint8_t *const prdBaseAddr, uint32_t readSize)
{
  uint32_t value;
  volatile uint16_t t=FLASH_OP_TIM_DELAY;
  
  eeprom_24cxx_init();
  eeprom_24cxx_readBytes((uint8_t *)&value, sizeof(value), DEVICE_24CXX_ADDR, 0);  
  if (value != FLASH_INIT_FLAG) {
    value = FLASH_INIT_FLAG;
    flash_erase(DEVICE_24CXX_ADDR);   
    eeprom_24cxx_writeBytes((uint8_t *)&value, sizeof(value), DEVICE_24CXX_ADDR, 0);
  }
  while (t--);  
  eeprom_24cxx_readBytes(prdBaseAddr, readSize, DEVICE_24CXX_ADDR, SYS_EEPROM_24CXX_START_ADDR);
}

/***************************************************************************************************************************
*@funcName: void flash_erase(uint8_t devAddr)
*@describe: eeprom的全片擦除
*@input:
     @devAddr:设备的地址
*@return: None
****************************************************************************************************************************/
void flash_erase(uint8_t devAddr)
{
  uint8_t clear[MAX_24CXX_PAGE_BUFF_SIZE], i;
  uint16_t j, pageNum;
  uint32_t addr=0;
  volatile uint16_t t;

  for (i=0; i<MAX_24CXX_PAGE_BUFF_SIZE; i++) {
      clear[i] = 0XFF;
  }
  pageNum = (MAX_24CXX_SIZE/MAX_24CXX_PAGE_BUFF_SIZE);
  for (j=0; j<pageNum; j++)
  {
      eeprom_24cxx_writeBytes(clear, MAX_24CXX_PAGE_BUFF_SIZE, devAddr, addr);
      addr += MAX_24CXX_PAGE_BUFF_SIZE;
      t = FLASH_OP_TIM_DELAY;      
      while (t--);
  }
}
