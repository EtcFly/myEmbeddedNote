/**************************************************************************************************************************
*@fileName: 24cxx.c
*@platform: MDK 5.10.0.0 + STM32F103C8T6
*@Version:  V1.0.0
*@describe: eeprom 24cxx�������
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
*@describe: eeprom�Լ�
*@input:    None
*@return:   0:��ʾ����  1:��ʾ������
****************************************************************************************************************************/
static uint8_t bsp_InitEEP(void)
{
	if (i2c_CheckDevice(EEP_DEVICE_ADDR) == 0) 	{
		return 0;
	}else {	  /* ʧ�ܺ���I2C����ֹͣ�ź� */
		i2c_Stop();
		return 1;
	} 
} 


/***************************************************************************************************************************
*@funcName: void eeprom_24cxx_init(void)
*@describe: 24cxx��Ӳ����ʼ��
*@input:    None
*@return:   None
****************************************************************************************************************************/
static void eeprom_24cxx_init(void)
{
  while (bsp_InitEEP());
}


///***************************************************************************************************************************
//*@funcName: void eeprom_24cxx_writeByte(uint8_t devAddr, uint8_t byte, uint32_t addr)
//*@describe: д��1Byte���ݵ�EEPROM
//*@input:
//     @devAddr: �豸��ַ
//     @byte: д����ֽ�����
//     @addr: д��ĵ�ַ
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
//*@describe: ��ȡEEPROMָ��λ�õ�1Byte����
//*@input:
//     @devAddr: �豸��ַ
//     @addr: д�����ݵĵ�ַ
//*@return: 
//     @retV: ��ȡ��1Byte����
//****************************************************************************************************************************/
//static uint8_t eeprom_24cxx_readByte(uint8_t devAddr, uint32_t addr)
//{
//   uint8_t retV;

//	i2c_Start();										//����I2C

//	i2c_SendByte(devAddr);						//����������ַ
//	i2c_WaitAck();
//	i2c_SendByte((uint8_t)((addr>>8)&0xff));
//	i2c_WaitAck();        
//	i2c_SendByte((uint8_t)((addr)&0xff));
//	i2c_WaitAck();

//	i2c_Start();
//	i2c_SendByte(devAddr|0x01);
//	i2c_WaitAck();
//	retV = i2c_ReadByte();		//��ȡ�ֽ�����
//	i2c_NAck();

//	i2c_Stop();
//  return retV;
//}



/***************************************************************************************************************************
*@funcName: uint8_t eeprom_24cxx_writeBytes(const uint8_t *buff, uint32_t len, uint8_t devAddr, uint32_t addr)
*@describe: 24cxx�Ķ��ֽ�д�룬����<PAGE_SIZE(һ��Ϊ32Byte) ͬʱע�����߿���ʱ���������>tBuf
*@input:
    @buff: ��Ҫд�������
    @len:  ��Ҫд����ֽ���
    @devAddr: �豸��ַ
    @addr:    �ڴ��ַ
*@return: 
    @status: д���״̬  0:д��ɹ�   other:error
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
*@describe: ��ȡeeprom 24cxx n Byte���ֽ�
*@input:
    @buff: ��Ŷ�ȡ����������
    @len:  ��Ҫ��ȡ���ֽ���
    @devAddr: �豸��ַ
    @addr:    �ڴ��ַ
*@return: ���Ƿ���1 (����)
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
*@funcName: ������д���ⲿ�ڴ�eeprom 24cxx
*@describe: д�����
*@input:    
    @pPara: ��Ҫд������ݻ���ַ
    @startAddr: д�����ݵ���Ե�ַ
    @len:  д������ݳ���
*@return:   None
****************************************************************************************************************************/
void flash_write(const uint8_t *pPara, uint32_t startAddr, uint32_t len)
{
   uint8_t *p = (uint8_t *)pPara;     
   uint32_t temp=(startAddr+SYS_EEPROM_24CXX_START_ADDR);  
   uint16_t write_len=0;  
   volatile uint16_t t =FLASH_OP_TIM_DELAY; //���̵���д��һЩ���ݶ�д����
   
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
*@describe:  ��flash��ȡ����
*@input:
      @pPara:  ��Ŷ�ȡ���ݵĻ������ַ
      @startAddr: ��Ҫ����ȡ��������Ե�ַ
      @len: ��ȡ�����ݳ���
*@return:  None
****************************************************************************************************************************/
void flash_read(uint8_t *const pPara, uint32_t startAddr, uint32_t len) 
{
   uint8_t *p = (uint8_t *)pPara; 
   eeprom_24cxx_readBytes(p, len, DEVICE_24CXX_ADDR, SYS_EEPROM_24CXX_START_ADDR+startAddr);
}


/***************************************************************************************************************************
*@funcName: void flash_init(uint8_t *const prdBaseAddr, uint32_t readSize)
*@describe: flash�ĳ�ʼ��
*@input:
@prdBaseAddr:���ݴ�Ż���ַ
@readSize: ��Ҫ��ȡ�����ݴ�С
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
*@describe: eeprom��ȫƬ����
*@input:
     @devAddr:�豸�ĵ�ַ
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
