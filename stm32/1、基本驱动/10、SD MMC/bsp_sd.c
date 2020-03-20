/**************************************************************************************************************************
*@fileName: bsp_sd.h
*@platform: MDK 5.10.0.0 + STM32F103C8T6
*@Version:  V1.0.0
*@describe: None
*@modification
*==========================================================================================================================
* version    Author   Data                                            Details
*--------------------------------------------------------------------------------------------------------------------------
* V1.0.0      Wy     190323                                           Create File
===========================================================================================================================
**************************************************************************************************************************/
#include "stm32f10x.h"
#include "bsp_sd.h"
#include "bsp_spi_gpio.h"

/************************************************Config**********************************************************************/
#define  SD_SPI_ReadWriteByte        drv_spi_read_write_byte    //spi���ͺͽ��ܺ���






/************************************************Private Define**********************************************************************/
//SD������
#define  CMD0                            0u   //�����λ����
#define  CMD1                            1u
#define  CMD2                            2u   //��ȡ��Ƭ��CID
#define  CMD3                            3u   //֪ͨ�������µ�RCA
#define  CMD4                            4u   //����������������������Χ(������)
#define  CMD6                            6u   //�л�����(V1.1�Ժ�汾֧��)
#define  CMD7                            7u   //�󶨻��߽��SD��
#define  CMD8                            8u   //��֤SD���ӿڲ�������(V2.0ר��)
#define  CMD9                            9u   //��ȡ���ľ�������(�����鳤�ȡ��洢���������ݴ���ģʽ) 
#define  CMD10                           10u
#define  CMD12                           12u  //ֹͣ����
#define  CMD13                           13u  //��ѯ����״̬
#define  CMD16                           16u  //���ÿ�ĳ���
#define  CMD17                           17u  //�����ȡ
#define  CMD18                           18u  //����ȡ
#define  CMD23                           23u
#define  CMD24                           24u  //����д
#define  CMD25                           25u  //���д
#define  CMD27                           27u  //���CSR
#define  CMD30                           30u  //����д����
#define  CMD32                           32u  //�������ʼ
#define  CMD33                           33u  //���������
#define  CMD38                           38u  //��������(CMD32 --> CMD33 --> CMD38)
#define  CMD41                           41u  //ʶ��;ܾ���ƥ���VDD��ѹ��
#define  CMD42                           42u  //�����ͽ�������
#define  CMD55                           55u
#define  CMD56                           56u  //ͨ�ö�ģʽ����
#define  CMD57                           57u  //ͨ��дģʽ����  
#define  CMD58                           58u
#define  ACMD23                          23u  //Ԥ����(��CMD25ǰʹ�ÿ�����д���ٶ�)
#define  ACMD41                          41u
#define  ACMD51                          51u  //����SCR
#define  APP_CMD                         CMD55 //����Ӧ������
   

////////////////////////////////////////////////////////////////////////////////
//SD�������ֶ�
#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06	

//��������ظ�
#define MSD_DATA_OK                0x05
#define MSD_DATA_CRC_ERROR         0x0B
#define MSD_DATA_WRITE_ERROR       0x0D
#define MSD_DATA_OTHER_ERROR       0xFF

//SD����ظ�
#define MSD_RESPONSE_NO_ERROR      0x00
#define MSD_IN_IDLE_STATE          0x01
#define MSD_ERASE_RESET            0x02
#define MSD_ILLEGAL_COMMAND        0x04
#define MSD_COM_CRC_ERROR          0x08
#define MSD_ERASE_SEQUENCE_ERROR   0x10
#define MSD_ADDRESS_ERROR          0x20
#define MSD_PARAMETER_ERROR        0x40
#define MSD_RESPONSE_FAILURE       0xFF


/************************************************Private Variable**********************************************************************/
static uint8_t s_SD_Type; //sd������



/***************************************************************************************************************************
*@funcName: uint8_t CRC7_MMC(uint8_t *data, uint8_t length)
*@describe: ��ȡSD��������CRC7(��CRC7ΪԭCRC<<1)
*@input:
   @pdata:ָ��ԭ����
   @length:��Ҫ��������ݳ���
*@return:
   @crc: ԭ���ݵ�CRC7ֵ
****************************************************************************************************************************/
//static uint8_t CRC7_MMC(const uint8_t *data, uint8_t length)
//{
//    unsigned char i;
//    unsigned char crc = 0x0;        // Initial value
//    while(length--)
//    {
//        crc ^= *data++;        // crc ^= *data; data++;
//        for ( i = 0; i < 8; i++ )
//        {
//            if ( crc & 0x80 )
//                crc = (crc << 1) ^ 0x12;        // 0x12 = 0x09<<(8-7)
//            else
//                crc <<= 1;
//        }
//    }
//    return crc;
//}

/************************************************Private function**********************************************************************/
static uint8_t SD_WaitReady(void);
static uint8_t SD_Select(void);
static uint8_t SD_WaitReady(void);
static uint8_t SD_RecvData(uint8_t*buf,uint16_t len);
static uint8_t SD_SendBlock(uint8_t*buf,uint8_t cmd);




/***************************************************************************************************************************
*@funcName: void SD_DisSelect(void)
*@describe: sd���ͷ�spi����
*@input:    None
*@return:   None
****************************************************************************************************************************/
static void SD_DisSelect(void)
{
	spi_set_nss_high();
 	SD_SPI_ReadWriteByte(0xff);//����״̬�л�
}

/***************************************************************************************************************************
*@funcName: uint8_t SD_Select(void)
*@describe: sd������spi����
*@input:   None
*@return:  0 -success  1 -timeout 
****************************************************************************************************************************/
static uint8_t SD_Select(void)
{
	spi_set_nss_low();
	if(SD_WaitReady()==0)
    return 0;
	SD_DisSelect();
	return 1;
}

/***************************************************************************************************************************
*@funcName: uint8_t SD_WaitReady(void)
*@describe: �ȴ�sd������
*@input:    None
*@return:   0 -success  1 -timeout
****************************************************************************************************************************/
static uint8_t SD_WaitReady(void)
{
	uint32_t t=0;
	do
	{
		if(SD_SPI_ReadWriteByte(0XFF)==0XFF)
      return 0;//OK
		t++;		  	
	}while(t<0XFFFFFF); 
	return 1;
}

/***************************************************************************************************************************
*@funcName: uint8_t SD_GetResponse(uint8_t Response)
*@describe: �ȴ�sd����Ӧ
*@input:
    @Response: �����Ļ�Ӧֵ
*@return:  
       MSD_RESPONSE_NO_ERROR -���������ֵ(��Ӧ�ɹ�)   
       MSD_RESPONSE_FAILURE  -δ���(��Ӧʧ��)
****************************************************************************************************************************/
static uint8_t SD_GetResponse(uint8_t Response)
{
	uint16_t Count=0xFFFF;  						  
	while ((SD_SPI_ReadWriteByte(0XFF)!=Response)&&Count)
  {
    Count--;
  }	  
	if (Count==0)
    return MSD_RESPONSE_FAILURE;  
	else 
    return MSD_RESPONSE_NO_ERROR;
}

/***************************************************************************************************************************
*@funcName: uint8_t SD_RecvData(uint8_t*buf,uint16_t len)
*@describe: ��sd��ȡһ�����ݰ�������
*@input:
@buf: �������ݻ�����
@len: ��Ҫ��ȡ�����ݳ���(Byte)
*@return:  0 -��ȡ�ɹ�    1 -��ȡʧ��
****************************************************************************************************************************/
static uint8_t SD_RecvData(uint8_t*buf,uint16_t len)
{			  	  
	if(SD_GetResponse(0xFE)) return 1;
  
  while(len--)
  {
     *buf=SD_SPI_ReadWriteByte(0xFF);
     buf++;
  } 
  //αcrc
  SD_SPI_ReadWriteByte(0xFF);
  SD_SPI_ReadWriteByte(0xFF);									  					    
  return 0;
}

/***************************************************************************************************************************
*@funcName: uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc)
*@describe: ��sd������һ������
*@input:
    @cmd: ����index
    @arg: �������
    @crc: �����crc7
*@return: sd���ص���Ӧ
****************************************************************************************************************************/													  
static uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
  uint8_t r1;	
	uint8_t Retry=0; 

	SD_DisSelect();//ȡ���ϴ�Ƭѡ
  if(SD_Select())  return 0XFF;//ƬѡʧЧ 
	//����
  SD_SPI_ReadWriteByte(cmd | 0x40);
  SD_SPI_ReadWriteByte(arg >> 24);
  SD_SPI_ReadWriteByte(arg >> 16);
  SD_SPI_ReadWriteByte(arg >> 8);
  SD_SPI_ReadWriteByte(arg);	  
  SD_SPI_ReadWriteByte(crc); 
  
	if(cmd==CMD12) 
  {
    SD_SPI_ReadWriteByte(0xff);//Skip a stuff byte when stop reading
  }
  //�ȴ���Ӧ���߳�ʱ�˳�
	Retry=0X1F;
	do
	{
		 r1=SD_SPI_ReadWriteByte(0xFF);
	}while((r1&0X80) && Retry--);	 
  return r1;
}	


/***************************************************************************************************************************
*@funcName: uint8_t SD_SendBlock(uint8_t*buf,uint8_t cmd)
*@describe: ��sd��д��һ�����ݰ�������(512Byte)
*@input:
     @buf: ���ݻ�����
     @cmd: ָ�����
*@return:  0 -success    other -fail
****************************************************************************************************************************/
static uint8_t SD_SendBlock(uint8_t*buf,uint8_t cmd)
{	
	uint16_t t;		  	  
	if(SD_WaitReady())  return 1; //�ȴ�������ʱ
  
	SD_SPI_ReadWriteByte(cmd);
	if(cmd!=0XFD)
	{
		for(t=0; t<512; t++) 
    {
       SD_SPI_ReadWriteByte(buf[t]);
    }
	  SD_SPI_ReadWriteByte(0xFF); //αcrc
	  SD_SPI_ReadWriteByte(0xFF);
		t=SD_SPI_ReadWriteByte(0xFF);
    
		if((t&0x1F)!=0x05) 
       return 2;			//��Ӧ����					  					    
	}						 									  					    
  return 0; //д��ɹ�
}
	    																			  
/***************************************************************************************************************************
*@funcName: uint8_t SD_GetCID(uint8_t *cid_data)
*@describe: ��ȡsd����cid�Ĵ���ֵ
*@input:
     @cid_data: ��Ż�ȡ��cidֵ (>16byte)
*@return:   0 -no error   1 -error
****************************************************************************************************************************/												   
uint8_t SD_GetCID(uint8_t *cid_data)
{
  uint8_t r1;	   
  //����CMD10���� ��ȡCID
  r1 = SD_SendCmd(CMD10,0,0x01);
  if(r1==0x00)
	{
		r1 = SD_RecvData(cid_data,16);//����16Byte���� 
  }
	SD_DisSelect();

	if(r1)  return 1;
	else    return 0;
}																				  

/***************************************************************************************************************************
*@funcName: uint8_t SD_GetCSD(uint8_t *csd_data)
*@describe: ��ȡsd����csd�Ĵ���ֵ
*@input:
    @csd_data: ���ڴ��csd����( >16Byte)
*@return:  0 -no error   1 -error
****************************************************************************************************************************/
uint8_t SD_GetCSD(uint8_t *csd_data)
{
  uint8_t r1;	 
  r1 = SD_SendCmd(CMD9,0,0x01);//����CMD9,��ȡcsd�Ĵ���
  if(r1 == 0)
	{
   	r1=SD_RecvData(csd_data, 16);
  }
 	SD_DisSelect();
  
	if(r1) return 1;
	else   return 0;
}  

/***************************************************************************************************************************
*@funcName: uint32_t SD_GetSectorCount(void)
*@describe: ��ȡsd����������
*@input:   None
*@return:
@retV:  0 -������ȡʧ��   other:ϵͳ������(������/512)
****************************************************************************************************************************/														  
uint32_t SD_GetSectorCount(void)
{
  uint8_t csd[16], n;
  uint32_t Capacity;  
	uint16_t csize;  					    
	//��ȡcds
  if(SD_GetCSD(csd)!=0) return 0;	    
  //���ΪSDHC��, ��������
  if((csd[0]&0xC0)==0x40)	 //V2.00��
  {	
		 csize = csd[9] + ((uint16_t)csd[8] << 8) + 1;
		 Capacity = (uint32_t)csize << 10; //�õ���������		   
  }
  else//V1.XX�Ŀ�
  {	
		 n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		 csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 3) << 10) + 1;
		 Capacity= (uint32_t)csize << (n - 9);//�õ���������   
  }
  return Capacity;
}

/***************************************************************************************************************************
*@funcName: uint8_t SD_Initialize(void)
*@describe: ��ʼ��sd��
*@input:    None
*@return:
@retV: 0 -sd�����ͻ�ȡʧ��  0xaa:��������   other:sd���ķ���ֵ
****************************************************************************************************************************/
uint8_t SD_Initialize(void)
{
  uint8_t r1;      // ���sd�ķ���ֵ
  uint16_t i, retry;  // ��ʱ��Ԫ
  uint8_t buf[4];  

	drv_spi_init();		//��ʼ��spi����
//  SD_SPI_SpeedLow(); //���ó�ʼ���ٶ�( ������100KHz<speed<400KHz֮��) 
 	
  for(i=0; i<10; i++) {
     SD_SPI_ReadWriteByte(0XFF);//��������74������
  }

	retry = 20;
	do
	{
		r1=SD_SendCmd(CMD0,0,0x95);//����IDLE״̬
	}while ((r1!=0X01) && retry--);

 	s_SD_Type = 0;//sd��������
	if(r1==0X01)
	{
		 if(SD_SendCmd(CMD8,0x1AA,0x87)==1)//SD V2.0
		 {
			 for(i=0; i<4; i++) {
          buf[i]=SD_SPI_ReadWriteByte(0XFF);	//Get trailing return value of R7 resp
       }
			 if(buf[2]==0X01&&buf[3]==0XAA)//���Ƿ�֧��2.7~3.6V
			 {
			   retry=0XFFFE;
			 	 do
				 {
				 	  SD_SendCmd(CMD55,0,0X01);	
					  r1 = SD_SendCmd(CMD41,0x40000000,0X01);
				}while (r1&&retry--);

				if(retry&&SD_SendCmd(CMD58,0,0X01)==0)//����V2.0�汾��SD��
				{
					for(i=0;i<4;i++) {
             buf[i] = SD_SPI_ReadWriteByte(0XFF);//�õ�OCR
          }
					if(buf[0]&0x40)  s_SD_Type = SD_TYPE_V2HC;    //���CCS
					else  s_SD_Type = SD_TYPE_V2;   
				}
			}
		}
    else//SD V1.x/ MMC V3
		{
			 SD_SendCmd(CMD55,0,0X01);		
			 r1=SD_SendCmd(CMD41,0,0X01);
			 if(r1<=1) {		
			   s_SD_Type=SD_TYPE_V1;
			 	 retry=0XFFFE;
				 do //�ȴ��˳�IDLE״̬
				 {
					  SD_SendCmd(CMD55,0,0X01);	
					  r1=SD_SendCmd(CMD41,0,0X01);
				 }while(r1&&retry--);
			 }
       else
			 {
			 	 s_SD_Type=SD_TYPE_MMC;//MMC V3
				 retry=0XFFFE;
				 do //�ȴ��˳�IDLE
				 {											    
					 r1=SD_SendCmd(CMD1,0,0X01);
				 }while(r1&&retry--);  
			 }
			if(retry==0||SD_SendCmd(CMD16,512,0X01)!=0)
          s_SD_Type = SD_TYPE_ERR;
		}
	}
	SD_DisSelect();
//	SD_SPI_SpeedHigh(); //�������SD���ݴ���״̬(һ��Ϊ25MHz)

	if(s_SD_Type) return 0;
	else if(r1) return r1; 	   
	return 0xaa;//��������
}

/***************************************************************************************************************************
*@funcName: uint8_t SD_ReadDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
*@describe: ��ȡsd��������
*@input:
   @buf:    �������ݵĻ�����
   @sector: ��Ҫ��ȡ������
   @cnt:    ��Ҫ��ȡ��������Ŀ
*@return:  0: read ok      other:error 
****************************************************************************************************************************/
uint8_t SD_ReadDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
{
	uint8_t r1;

	if(s_SD_Type != SD_TYPE_V2HC) 
    sector <<= 9;//������ַת��Ϊ�ֽڵ�ַ
	
  if(cnt==1)
	{
		r1 = SD_SendCmd(CMD17,sector,0X01);//������
		if(r1==0) //ָ��ͳɹ�
		{
			 r1=SD_RecvData(buf,512);//����512Byte  
		}
	}
  else
	{
		r1=SD_SendCmd(CMD18,sector,0X01);//����������
		do
		{
			 r1=SD_RecvData(buf,512);//����512Byte 
			 buf += 512;  
		}while(--cnt && r1==0); 	
		SD_SendCmd(CMD12,0,0X01);	//���ܽ�������
	}   
	SD_DisSelect();
	return r1;
}

/***************************************************************************************************************************
*@funcName: uint8_t SD_WriteDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
*@describe: ��sd��д������
*@input:
   @buf:    ��Ҫд�������
   @sector: ��Ҫд��������Ż���ַ
   @cnt:    ��Ҫд�����������
*@return:  0: write ok  other:error
****************************************************************************************************************************/
uint8_t SD_WriteDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
{
	uint8_t r1;

	if(s_SD_Type!=SD_TYPE_V2HC)
     sector *= 512;//ת��ΪByte��ַ
  
	if(cnt==1)
	{
		r1=SD_SendCmd(CMD24,sector,0X01); //д����
		if(r1==0) //������ܳɹ�
		{
			r1=SD_SendBlock(buf,0xFE); //д��512Byte����  
		}
	}else
	{
		if(s_SD_Type!=SD_TYPE_MMC)
		{
			SD_SendCmd(CMD55,0,0X01);	
			SD_SendCmd(CMD23,cnt,0X01);//����ָ��	
		}
 		r1=SD_SendCmd(CMD25,sector,0X01);//����д����
		if(r1==0)
		{
			do
			{
				r1=SD_SendBlock(buf,0xFC);//д��512Byte
				buf+=512;  
			}while(--cnt && r1==0);
			r1=SD_SendBlock(0,0xFD);
		}
	}   
	SD_DisSelect();
	return r1;
}	   










