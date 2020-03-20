
#include "stm32f10x.h"
#include "cs553x.h"


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//									CS553X驱动函数
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  	

#define		PIN_A_MISO		GPIO_Pin_12    
#define		PIN_A_MOSI		GPIO_Pin_1	
#define		PIN_A_SCLK		GPIO_Pin_2
#define		PIN_A_CS1			GPIO_Pin_4
#define		PIN_A_CS2			GPIO_Pin_3
#define		PIN_A_CS3			GPIO_Pin_0
#define		PIN_A_CS4			GPIO_Pin_12

#define		PORT_A_MISO		GPIOA
#define		PORT_A_MOSI		GPIOD	
#define		PORT_A_SCLK		GPIOD
#define		PORT_A_CS1		GPIOD
#define		PORT_A_CS2		GPIOD
#define		PORT_A_CS3		GPIOD
#define		PORT_A_CS4		GPIOC

#define		SET_SDI()		 		GPIO_SetBits(PORT_A_MOSI,PIN_A_MOSI)
#define		CLR_SDI()		 		GPIO_ResetBits(PORT_A_MOSI,PIN_A_MOSI)
#define		SET_SCK()		 		GPIO_SetBits(PORT_A_SCLK,PIN_A_SCLK)		
#define		CLR_SCK()		 		GPIO_ResetBits(PORT_A_SCLK,PIN_A_SCLK)	
#define		SET_ADCS_1()	 	GPIO_SetBits(PORT_A_CS1,PIN_A_CS1)
#define		CLR_ADCS_1()	 	GPIO_ResetBits(PORT_A_CS1,PIN_A_CS1)
#define		SET_ADCS_2()	 	GPIO_SetBits(PORT_A_CS2,PIN_A_CS2)			
#define		CLR_ADCS_2()	 	GPIO_ResetBits(PORT_A_CS2,PIN_A_CS2)
#define		SET_ADCS_3()	 	GPIO_SetBits(PORT_A_CS3,PIN_A_CS3)			
#define		CLR_ADCS_3()	 	GPIO_ResetBits(PORT_A_CS3,PIN_A_CS3)
#define		SET_ADCS_4()	 	GPIO_SetBits(PORT_A_CS4,PIN_A_CS4)			
#define		CLR_ADCS_4()	 	GPIO_ResetBits(PORT_A_CS4,PIN_A_CS4)

#define		STU_SDO			(GPIO_ReadInputDataBit(PORT_A_MISO,PIN_A_MISO))	
#define		ADC_CHN_NUM_MAX			4
#define		ADC_CS_1		0x01		
#define		ADC_CS_2		0x02
#define		ADC_CS_3		0x04
#define		ADC_CS_4		0x08


//配置寄存器
#define		CMD_WR_CFG		0x03		//写配置寄存器
#define		CMD_RD_CFG		0x0B		//读配置寄存器
#define		CMD_RD_ADC		0x0C		//按照SETUP1执行连续转换
#define		CMD_WR_CSR1		0x05		//写通道设置寄存器1
#define		CMD_WR_CSR2		0x15		//写通道设置寄存器2
#define		CMD_WR_CSR3		0x25		//写通道设置寄存器3
#define		CMD_WR_CSR4		0x35		//写通道设置寄存器4
#define		CMD_CONV_CTN	0xC0		//连续转换	
#define		CMD_CONV_SET1_CTN	0xC0		//连续转换	
#define		CMD_CONV_SET2_CTN	0xC8		//连续转换	

uint8_t cs553x_buf[ADC_CHN_NUM_MAX];
CS553X_ConfigTypeDef cs553x_config[ADC_CHN_NUM_MAX];
/**********************************************************************
functionName:void us_delay(uint8_t ud)
description:us级别的函数
**********************************************************************/
void us_delay(uint16_t ud)
{
	uint16_t i;
	while(ud--)
	{
		i=8;
		while(i--);	
	}
}


/**********************************************************************
functionName:void ms_delay(void)
description:ms级别的函数
**********************************************************************/
void ms_delay(uint8_t md)
{
	while(md--)
	{
		us_delay(800);
	}
}


  
 

/**********************************************************************
functionName:void cs553x_io_init(void)
description:和cs553x有关的IO初始化
**********************************************************************/
void cs553x_io_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = PIN_A_MOSI;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(PORT_A_MOSI, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PIN_A_SCLK;
	GPIO_Init(PORT_A_SCLK, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PIN_A_CS1;
	GPIO_Init(PORT_A_CS1, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PIN_A_CS2;
	GPIO_Init(PORT_A_CS2, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PIN_A_CS3;
	GPIO_Init(PORT_A_CS3, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PIN_A_CS4;
	GPIO_Init(PORT_A_CS4, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PIN_A_MISO;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(PORT_A_MISO, &GPIO_InitStructure);


	SET_ADCS_1();		
	SET_ADCS_2();	
	SET_ADCS_3();  
	SET_ADCS_4();  
	CLR_SDI();
	CLR_SCK();
}	 

/**********************************************************************
functionName:void cs553x_wr_byte(uint8_t dat)
description:写字节
**********************************************************************/
void cs553x_wr_byte(uint8_t dat)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		if(dat&0x80)
		SET_SDI();
		else
		CLR_SDI(); 
		us_delay(1); 
		SET_SCK();
		us_delay(1); 		
		CLR_SCK();
		dat<<=1;
		us_delay(1); 
	}
	SET_SDI();
}


/**********************************************************************
functionName:uint8_t cs553x_rd_byte(void)
description:读字节
**********************************************************************/
uint8_t cs553x_rd_byte(void)
{
	uint8_t i;
	uint8_t temp=0;
	for(i=0;i<8;i++)
	{
		temp<<=1;
		SET_SCK();
		us_delay(1);   
		if(STU_SDO)
		temp|=1;
		CLR_SCK();
		us_delay(1);  
	}
	return temp;
}

/**********************************************************************
functionName:void cs553x_wr_reg(uint8_t cmd,uint32_t dat)
description:写cs553x寄存器
**********************************************************************/
void cs553x_wr_reg(uint8_t cmd,uint32_t dat)
{
	uint32_t temp[4];
	uint32_t temp8;
	
	temp[0]=dat>>24;
	temp[1]=dat>>16;
	temp[2]=dat>>8;
	temp[3]=dat;
	cs553x_wr_byte(cmd);
	
	temp8=temp[0];
	cs553x_wr_byte(temp8);
	temp8=temp[1];
	cs553x_wr_byte(temp8);
	temp8=temp[2];
	cs553x_wr_byte(temp8);
	temp8=temp[3];
	cs553x_wr_byte(temp8);	 
}


/**********************************************************************
functionName:void cs553x_rd_reg(uint8_t cmd)
description:读cs553x寄存器
**********************************************************************/
void cs553x_rd_reg(uint8_t cmd)
{
	cs553x_wr_byte(cmd);
	cs553x_buf[3]=cs553x_rd_byte();		//高八位
	cs553x_buf[2]=cs553x_rd_byte();
	cs553x_buf[1]=cs553x_rd_byte();
	cs553x_buf[0]=cs553x_rd_byte();
}


/**********************************************************************
functionName:void cs553x_soft_rst(void)
description:软件复位
**********************************************************************/
void cs553x_soft_rst(void)
{
	uint8_t i;
	for(i=0;i<15;i++)
	{
		cs553x_wr_byte(0xFF);
	}
	cs553x_wr_byte(0xFE);
}
  
  /**********************************************************************
functionName:void cs553x_cs_cfg(uint8_t cfg)
description:配置CS553X选通引脚
**********************************************************************/
void cs553x_cs_cfg(uint8_t cfg)
{
	if(cfg&ADC_CS_1)
	CLR_ADCS_1();	
	else
	SET_ADCS_1();		
	if(cfg&ADC_CS_2)
	CLR_ADCS_2();	
	else
	SET_ADCS_2();
	if(cfg&ADC_CS_3)
	CLR_ADCS_3();	
	else
	SET_ADCS_3();
	if(cfg&ADC_CS_4)
	CLR_ADCS_4();	
	else
	SET_ADCS_4();
} 


/**********************************************************************
functionName:void cs553x_init(void)
description:cs553x初始化函数
#define		ADC_CS_1		0x01		
#define		ADC_CS_2		0x02
#define		ADC_CS_3		0x04
#define		ADC_CS_4		0x08
返回0表示正确
0x30 返回通道0初始化出错
0x31 返回通道1初始化出错...
**********************************************************************/
uint8_t cs553x_init(CS553X_ConfigTypeDef *p_cs553x_config,uint8_t ch_num)
{
	uint8_t rv=0x00;		//返回值 0 表示成功返回	
	uint16_t j;
	uint8_t i,k;
	cs553x_io_init();
	ms_delay(10);				//等待时钟稳定	

	for(i=0;i<ch_num;i++)
	cs553x_config[i]=p_cs553x_config[i];


	cs553x_cs_cfg(0xFF);
	us_delay(1); 
	cs553x_soft_rst();								//软件复位
	cs553x_wr_reg(CMD_WR_CFG,0x20000000);			//系统复位，RS=1
	ms_delay(50);									 
	cs553x_wr_reg(CMD_WR_CFG,0x00000000);			//RS=0
	cs553x_cs_cfg(0x00);
	
	//配置寄存器读取
	for(i=0;i<ch_num;i++)
	{ 
		if((cs553x_config[i].type&0xF0)==TYPE_CS5530)			//表示芯片存在
		{
			j=0;
			cs553x_cs_cfg((0x01<<i));
			cs553x_rd_reg(CMD_RD_CFG);							//读取配置寄存器
			while(cs553x_buf[3]&0x10)							//等待RV返回0
			{
				j++;
				if(j>=1000)
				{	
					rv=0x30+i;
					break;
				}	
				cs553x_rd_reg(CMD_RD_CFG);		
			}
			cs553x_cs_cfg(0x00);	
		}
	}
	for(i=0;i<ch_num;i++)
	{
		cs553x_cs_cfg((0x01<<i));
		if(cs553x_config[i].type==TYPE_CS5532)
		{
			us_delay(1);
			cs553x_wr_byte(0x45);
			for(k=0;k<4;k++)
			{
				if(cs553x_config[i].gain==GAIN_32X)
				cs553x_wr_byte(0x28);		//G=32
				else if(cs553x_config[i].gain==GAIN_64X)	
				cs553x_wr_byte(0x30);		//G=64
				cs553x_wr_byte(0x80);
				   /* SETUP2 */
				cs553x_wr_byte(0x2A);   /* G=32 RATE=6.25 */
				cs553x_wr_byte(0x00); 
			}
			us_delay(1);
			cs553x_wr_byte(0x81);		//自偏移校准
			us_delay(10);
			while(STU_SDO);	 
		}	
		else if(cs553x_config[i].type==TYPE_CS5530)
		{
			if(cs553x_config[i].vref>25)
			cs553x_wr_reg(CMD_WR_CFG,0x00000800);			//设置配置寄存器
			else
			cs553x_wr_reg(CMD_WR_CFG,0x02000800);					//设置配置寄存器
		} 	
		cs553x_cs_cfg(0x00);	
	}
	cs553x_cs_cfg(0xFF);
	us_delay(10);
	cs553x_wr_byte(CMD_CONV_CTN); 							//执行连续转换
	cs553x_cs_cfg(0x00);
	return rv; 
}

 CS553X_ConfigTypeDef cs5532_cfg[4]; 
/**********************************************************************
functionName:int32_t sample(uint8_t ch)
description:读取ADC数值
#define		ADC_CS_1		0x01		
#define		ADC_CS_2		0x02
#define		ADC_CS_3		0x04
#define		ADC_CS_4		0x08
**********************************************************************/
int32_t sample(uint8_t ch)
{
	int32_t *result;
		uint32_t l=0;
		static uint8_t c=0;
		static int32_t adcf[4];
	if(ch==0)
	{
		cs553x_cs_cfg(ADC_CS_1);
	}
	else if(ch==1)
	{
		cs553x_cs_cfg(ADC_CS_2);
	}
	else if(ch==2)
	{
		cs553x_cs_cfg(ADC_CS_3);
	}
	else if(ch==3)
	{
		cs553x_cs_cfg(ADC_CS_4);
	} 
	us_delay(1);
	while(STU_SDO)//AD卡死这里的时候，进行初始化步骤。 YWC
			{ 
					us_delay(2);
					l++;
					if(l>100000L)
					{  
						c=3;
						cs553x_init(cs5532_cfg,4); //初始化
						return adcf[ch];
					}	
	     } 
	cs553x_wr_byte(0x00); 
	cs553x_buf[3]=cs553x_rd_byte();		//高八位
	cs553x_buf[2]=cs553x_rd_byte();
	cs553x_buf[1]=cs553x_rd_byte();
	cs553x_buf[0]=cs553x_rd_byte();
	result=(int32_t *)(void *)cs553x_buf;
	
	if((cs553x_config[ch].type==TYPE_CS5532)&&(cs553x_config[ch].gain==GAIN_32X))
	*result>>=12;
	else if((cs553x_config[ch].type==TYPE_CS5530)&&(cs553x_config[ch].vref<=25))
	*result>>=12;	
	else
	*result>>=13;
	if(c)
		{
			c--;
			return adcf[ch];
		}
	adcf[ch]=*result;
  return(*result);									 
} 
