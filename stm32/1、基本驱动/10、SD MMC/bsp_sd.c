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
#define  SD_SPI_ReadWriteByte        drv_spi_read_write_byte    //spi发送和接受函数






/************************************************Private Define**********************************************************************/
//SD卡命令
#define  CMD0                            0u   //软件复位命令
#define  CMD1                            1u
#define  CMD2                            2u   //读取卡片的CID
#define  CMD3                            3u   //通知卡发布新的RCA
#define  CMD4                            4u   //配置所有驱动卡的驱动范围(电流等)
#define  CMD6                            6u   //切换命令(V1.1以后版本支持)
#define  CMD7                            7u   //绑定或者解绑SD卡
#define  CMD8                            8u   //验证SD卡接口操作条件(V2.0专用)
#define  CMD9                            9u   //获取卡的具体数据(包括块长度、存储容量、数据传输模式) 
#define  CMD10                           10u
#define  CMD12                           12u  //停止命令
#define  CMD13                           13u  //轮询卡的状态
#define  CMD16                           16u  //设置块的长度
#define  CMD17                           17u  //单块读取
#define  CMD18                           18u  //多块读取
#define  CMD23                           23u
#define  CMD24                           24u  //单块写
#define  CMD25                           25u  //多块写
#define  CMD27                           27u  //编程CSR
#define  CMD30                           30u  //发送写保护
#define  CMD32                           32u  //块擦除开始
#define  CMD33                           33u  //块擦除结束
#define  CMD38                           38u  //擦除命令(CMD32 --> CMD33 --> CMD38)
#define  CMD41                           41u  //识别和拒绝不匹配的VDD电压卡
#define  CMD42                           42u  //锁定和解锁命令
#define  CMD55                           55u
#define  CMD56                           56u  //通用读模式命令
#define  CMD57                           57u  //通用写模式命令  
#define  CMD58                           58u
#define  ACMD23                          23u  //预擦除(在CMD25前使用可提升写入速度)
#define  ACMD41                          41u
#define  ACMD51                          51u  //发送SCR
#define  APP_CMD                         CMD55 //特殊应用命令
   

////////////////////////////////////////////////////////////////////////////////
//SD卡类型字段
#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06	

//数据命令回复
#define MSD_DATA_OK                0x05
#define MSD_DATA_CRC_ERROR         0x0B
#define MSD_DATA_WRITE_ERROR       0x0D
#define MSD_DATA_OTHER_ERROR       0xFF

//SD命令回复
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
static uint8_t s_SD_Type; //sd卡类型



/***************************************************************************************************************************
*@funcName: uint8_t CRC7_MMC(uint8_t *data, uint8_t length)
*@describe: 获取SD卡的命令CRC7(该CRC7为原CRC<<1)
*@input:
   @pdata:指向原数据
   @length:需要计算的数据长度
*@return:
   @crc: 原数据的CRC7值
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
*@describe: sd卡释放spi总线
*@input:    None
*@return:   None
****************************************************************************************************************************/
static void SD_DisSelect(void)
{
	spi_set_nss_high();
 	SD_SPI_ReadWriteByte(0xff);//带待状态切换
}

/***************************************************************************************************************************
*@funcName: uint8_t SD_Select(void)
*@describe: sd卡征用spi总线
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
*@describe: 等待sd卡就绪
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
*@describe: 等待sd卡响应
*@input:
    @Response: 期望的回应值
*@return:  
       MSD_RESPONSE_NO_ERROR -获得期望的值(响应成功)   
       MSD_RESPONSE_FAILURE  -未获得(响应失败)
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
*@describe: 从sd读取一个数据包的内容
*@input:
@buf: 接受数据缓冲区
@len: 需要读取的数据长度(Byte)
*@return:  0 -读取成功    1 -读取失败
****************************************************************************************************************************/
static uint8_t SD_RecvData(uint8_t*buf,uint16_t len)
{			  	  
	if(SD_GetResponse(0xFE)) return 1;
  
  while(len--)
  {
     *buf=SD_SPI_ReadWriteByte(0xFF);
     buf++;
  } 
  //伪crc
  SD_SPI_ReadWriteByte(0xFF);
  SD_SPI_ReadWriteByte(0xFF);									  					    
  return 0;
}

/***************************************************************************************************************************
*@funcName: uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc)
*@describe: 向sd卡发送一个命令
*@input:
    @cmd: 命令index
    @arg: 命令参数
    @crc: 命令的crc7
*@return: sd返回的响应
****************************************************************************************************************************/													  
static uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
  uint8_t r1;	
	uint8_t Retry=0; 

	SD_DisSelect();//取消上次片选
  if(SD_Select())  return 0XFF;//片选失效 
	//发送
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
  //等待响应或者超时退出
	Retry=0X1F;
	do
	{
		 r1=SD_SPI_ReadWriteByte(0xFF);
	}while((r1&0X80) && Retry--);	 
  return r1;
}	


/***************************************************************************************************************************
*@funcName: uint8_t SD_SendBlock(uint8_t*buf,uint8_t cmd)
*@describe: 向sd卡写入一个数据包的内容(512Byte)
*@input:
     @buf: 数据缓冲区
     @cmd: 指令代码
*@return:  0 -success    other -fail
****************************************************************************************************************************/
static uint8_t SD_SendBlock(uint8_t*buf,uint8_t cmd)
{	
	uint16_t t;		  	  
	if(SD_WaitReady())  return 1; //等待就绪超时
  
	SD_SPI_ReadWriteByte(cmd);
	if(cmd!=0XFD)
	{
		for(t=0; t<512; t++) 
    {
       SD_SPI_ReadWriteByte(buf[t]);
    }
	  SD_SPI_ReadWriteByte(0xFF); //伪crc
	  SD_SPI_ReadWriteByte(0xFF);
		t=SD_SPI_ReadWriteByte(0xFF);
    
		if((t&0x1F)!=0x05) 
       return 2;			//响应错误					  					    
	}						 									  					    
  return 0; //写入成功
}
	    																			  
/***************************************************************************************************************************
*@funcName: uint8_t SD_GetCID(uint8_t *cid_data)
*@describe: 获取sd卡的cid寄存器值
*@input:
     @cid_data: 存放获取的cid值 (>16byte)
*@return:   0 -no error   1 -error
****************************************************************************************************************************/												   
uint8_t SD_GetCID(uint8_t *cid_data)
{
  uint8_t r1;	   
  //发送CMD10命令 读取CID
  r1 = SD_SendCmd(CMD10,0,0x01);
  if(r1==0x00)
	{
		r1 = SD_RecvData(cid_data,16);//接受16Byte数据 
  }
	SD_DisSelect();

	if(r1)  return 1;
	else    return 0;
}																				  

/***************************************************************************************************************************
*@funcName: uint8_t SD_GetCSD(uint8_t *csd_data)
*@describe: 获取sd卡的csd寄存器值
*@input:
    @csd_data: 用于存放csd数据( >16Byte)
*@return:  0 -no error   1 -error
****************************************************************************************************************************/
uint8_t SD_GetCSD(uint8_t *csd_data)
{
  uint8_t r1;	 
  r1 = SD_SendCmd(CMD9,0,0x01);//发送CMD9,读取csd寄存器
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
*@describe: 获取sd卡的总扇区
*@input:   None
*@return:
@retV:  0 -容量获取失败   other:系统的容量(扇区数/512)
****************************************************************************************************************************/														  
uint32_t SD_GetSectorCount(void)
{
  uint8_t csd[16], n;
  uint32_t Capacity;  
	uint16_t csize;  					    
	//获取cds
  if(SD_GetCSD(csd)!=0) return 0;	    
  //如果为SDHC卡, 计算如下
  if((csd[0]&0xC0)==0x40)	 //V2.00卡
  {	
		 csize = csd[9] + ((uint16_t)csd[8] << 8) + 1;
		 Capacity = (uint32_t)csize << 10; //得到的扇区数		   
  }
  else//V1.XX的卡
  {	
		 n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		 csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 3) << 10) + 1;
		 Capacity= (uint32_t)csize << (n - 9);//得到的扇区数   
  }
  return Capacity;
}

/***************************************************************************************************************************
*@funcName: uint8_t SD_Initialize(void)
*@describe: 初始化sd卡
*@input:    None
*@return:
@retV: 0 -sd卡类型获取失败  0xaa:其他错误   other:sd卡的返回值
****************************************************************************************************************************/
uint8_t SD_Initialize(void)
{
  uint8_t r1;      // 存放sd的返回值
  uint16_t i, retry;  // 超时单元
  uint8_t buf[4];  

	drv_spi_init();		//初始化spi驱动
//  SD_SPI_SpeedLow(); //设置初始化速度( 必须在100KHz<speed<400KHz之间) 
 	
  for(i=0; i<10; i++) {
     SD_SPI_ReadWriteByte(0XFF);//发送至少74个脉冲
  }

	retry = 20;
	do
	{
		r1=SD_SendCmd(CMD0,0,0x95);//进入IDLE状态
	}while ((r1!=0X01) && retry--);

 	s_SD_Type = 0;//sd卡的类型
	if(r1==0X01)
	{
		 if(SD_SendCmd(CMD8,0x1AA,0x87)==1)//SD V2.0
		 {
			 for(i=0; i<4; i++) {
          buf[i]=SD_SPI_ReadWriteByte(0XFF);	//Get trailing return value of R7 resp
       }
			 if(buf[2]==0X01&&buf[3]==0XAA)//卡是否支持2.7~3.6V
			 {
			   retry=0XFFFE;
			 	 do
				 {
				 	  SD_SendCmd(CMD55,0,0X01);	
					  r1 = SD_SendCmd(CMD41,0x40000000,0X01);
				}while (r1&&retry--);

				if(retry&&SD_SendCmd(CMD58,0,0X01)==0)//鉴别V2.0版本的SD卡
				{
					for(i=0;i<4;i++) {
             buf[i] = SD_SPI_ReadWriteByte(0XFF);//得到OCR
          }
					if(buf[0]&0x40)  s_SD_Type = SD_TYPE_V2HC;    //检查CCS
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
				 do //等待退出IDLE状态
				 {
					  SD_SendCmd(CMD55,0,0X01);	
					  r1=SD_SendCmd(CMD41,0,0X01);
				 }while(r1&&retry--);
			 }
       else
			 {
			 	 s_SD_Type=SD_TYPE_MMC;//MMC V3
				 retry=0XFFFE;
				 do //等待退出IDLE
				 {											    
					 r1=SD_SendCmd(CMD1,0,0X01);
				 }while(r1&&retry--);  
			 }
			if(retry==0||SD_SendCmd(CMD16,512,0X01)!=0)
          s_SD_Type = SD_TYPE_ERR;
		}
	}
	SD_DisSelect();
//	SD_SPI_SpeedHigh(); //进入高速SD数据传输状态(一般为25MHz)

	if(s_SD_Type) return 0;
	else if(r1) return r1; 	   
	return 0xaa;//其他错误
}

/***************************************************************************************************************************
*@funcName: uint8_t SD_ReadDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
*@describe: 读取sd磁盘数据
*@input:
   @buf:    接受数据的缓冲区
   @sector: 需要读取的扇区
   @cnt:    需要读取的扇区数目
*@return:  0: read ok      other:error 
****************************************************************************************************************************/
uint8_t SD_ReadDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
{
	uint8_t r1;

	if(s_SD_Type != SD_TYPE_V2HC) 
    sector <<= 9;//扇区地址转换为字节地址
	
  if(cnt==1)
	{
		r1 = SD_SendCmd(CMD17,sector,0X01);//读命令
		if(r1==0) //指令发送成功
		{
			 r1=SD_RecvData(buf,512);//接受512Byte  
		}
	}
  else
	{
		r1=SD_SendCmd(CMD18,sector,0X01);//连续读命令
		do
		{
			 r1=SD_RecvData(buf,512);//接受512Byte 
			 buf += 512;  
		}while(--cnt && r1==0); 	
		SD_SendCmd(CMD12,0,0X01);	//接受结束命令
	}   
	SD_DisSelect();
	return r1;
}

/***************************************************************************************************************************
*@funcName: uint8_t SD_WriteDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
*@describe: 向sd卡写入数据
*@input:
   @buf:    需要写入的数据
   @sector: 需要写入的扇区号基地址
   @cnt:    需要写入的扇区数量
*@return:  0: write ok  other:error
****************************************************************************************************************************/
uint8_t SD_WriteDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
{
	uint8_t r1;

	if(s_SD_Type!=SD_TYPE_V2HC)
     sector *= 512;//转化为Byte地址
  
	if(cnt==1)
	{
		r1=SD_SendCmd(CMD24,sector,0X01); //写命令
		if(r1==0) //命令接受成功
		{
			r1=SD_SendBlock(buf,0xFE); //写入512Byte数据  
		}
	}else
	{
		if(s_SD_Type!=SD_TYPE_MMC)
		{
			SD_SendCmd(CMD55,0,0X01);	
			SD_SendCmd(CMD23,cnt,0X01);//发送指令	
		}
 		r1=SD_SendCmd(CMD25,sector,0X01);//连续写命令
		if(r1==0)
		{
			do
			{
				r1=SD_SendBlock(buf,0xFC);//写入512Byte
				buf+=512;  
			}while(--cnt && r1==0);
			r1=SD_SendBlock(0,0xFD);
		}
	}   
	SD_DisSelect();
	return r1;
}	   










