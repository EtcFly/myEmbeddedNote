/**************************************************************************************************************
*@文 件 名: bsp_adc.c
*@开发环境: STM32F103ZE + MDK5.10.0.0
*@文件功能: 实现adc底层驱动
*@作    者: Wy
*@日    期: 180415
*@版    本: V1.0.0
*@修    改: None
* 声明: 默认adc注入组相关采集采用中断的方式,而规则组采用DMA的方式采集
**************************************************************************************************************/
#include "stm32f10x.h"
#include "bsp_adc.h"




/**************************************************************************************************************
                                   Public   Variable
**************************************************************************************************************/
ADC_RegularType adc_sampData; //规则组数据存储
ADC_InjectedType adc_samp_Injected; //注入组采样




/**************************************************************************************************************
                                   Private   Define
**************************************************************************************************************/
#if (ADC1_IN0_ENABLE || ADC2_IN0_ENABLE || ADC3_IN0_ENABLE)
#define  ADC123_IN0_ENABLE                                       1
#endif

#if (ADC1_IN1_ENABLE || ADC2_IN1_ENABLE || ADC3_IN1_ENABLE)
#define  ADC123_IN1_ENABLE                                       1
#endif

#if (ADC1_IN2_ENABLE || ADC2_IN2_ENABLE || ADC3_IN2_ENABLE)
#define  ADC123_IN2_ENABLE                                       1
#endif

#if (ADC1_IN3_ENABLE || ADC2_IN3_ENABLE || ADC3_IN3_ENABLE)
#define  ADC123_IN3_ENABLE                                       1
#endif

#if (ADC1_IN10_ENABLE || ADC2_IN10_ENABLE || ADC3_IN10_ENABLE)
#define  ADC123_IN10_ENABLE                                       1
#endif

#if (ADC1_IN11_ENABLE || ADC2_IN11_ENABLE || ADC3_IN11_ENABLE)
#define  ADC123_IN11_ENABLE                                       1
#endif

#if (ADC1_IN12_ENABLE || ADC2_IN12_ENABLE || ADC3_IN12_ENABLE)
#define  ADC123_IN12_ENABLE                                       1
#endif

#if (ADC1_IN13_ENABLE || ADC2_IN13_ENABLE || ADC3_IN13_ENABLE)
#define  ADC123_IN13_ENABLE                                       1
#endif

#if (ADC1_IN4_ENABLE || ADC2_IN4_ENABLE)
#define  ADC12_IN4_ENABLE                                       1
#endif

#if (ADC1_IN5_ENABLE || ADC2_IN5_ENABLE)
#define  ADC12_IN5_ENABLE                                       1
#endif

#if (ADC1_IN6_ENABLE || ADC2_IN6_ENABLE)
#define  ADC12_IN6_ENABLE                                       1
#endif

#if (ADC1_IN7_ENABLE || ADC2_IN7_ENABLE)
#define  ADC12_IN7_ENABLE                                       1
#endif

#if (ADC1_IN8_ENABLE || ADC2_IN8_ENABLE)
#define  ADC12_IN8_ENABLE                                       1
#endif

#if (ADC1_IN9_ENABLE || ADC2_IN9_ENABLE)
#define  ADC12_IN9_ENABLE                                       1
#endif

#if (ADC1_IN14_ENABLE || ADC2_IN14_ENABLE)
#define  ADC12_IN14_ENABLE                                       1
#endif

#if (ADC1_IN15_ENABLE || ADC2_IN15_ENABLE)
#define  ADC12_IN15_ENABLE                                       1
#endif

//ADC1使能的所有通道数
#define  ADC1_CHANNEL_NUM    (ADC1_IN0_ENABLE+ADC1_IN1_ENABLE+ADC1_IN2_ENABLE+ADC1_IN3_ENABLE+ADC1_IN4_ENABLE+ADC1_IN5_ENABLE \
														 +ADC1_IN6_ENABLE+ADC1_IN7_ENABLE+ADC1_IN8_ENABLE+ADC1_IN9_ENABLE+ADC1_IN10_ENABLE+ADC1_IN11_ENABLE \
													   +ADC1_IN12_ENABLE+ADC1_IN13_ENABLE+ADC1_IN14_ENABLE+ADC1_IN15_ENABLE)
//ADC2使能的所有通道数
#define  ADC2_CHANNEL_NUM    (ADC2_IN0_ENABLE+ADC2_IN1_ENABLE+ADC2_IN2_ENABLE+ADC2_IN3_ENABLE+ADC2_IN4_ENABLE+ADC2_IN5_ENABLE \
														 +ADC2_IN6_ENABLE+ADC2_IN7_ENABLE+ADC2_IN8_ENABLE+ADC2_IN9_ENABLE+ADC2_IN10_ENABLE+ADC2_IN11_ENABLE \
													   +ADC2_IN12_ENABLE+ADC2_IN13_ENABLE+ADC2_IN14_ENABLE+ADC2_IN15_ENABLE)
//ADC3使能的所有通道数
#define  ADC3_CHANNEL_NUM    (ADC3_IN0_ENABLE+ADC3_IN1_ENABLE+ADC3_IN2_ENABLE+ADC3_IN3_ENABLE+ADC3_IN4_ENABLE+ADC3_IN5_ENABLE \
														 +ADC3_IN6_ENABLE+ADC3_IN7_ENABLE+ADC3_IN8_ENABLE+ADC3_IN10_ENABLE+ADC3_IN11_ENABLE \
													   +ADC3_IN12_ENABLE+ADC3_IN13_ENABLE)
//ADC使能的所有通道数													 
#define  ADC_CHANNEL_NUM     (ADC1_CHANNEL_NUM+ADC2_CHANNEL_NUM+ADC3_CHANNEL_NUM)


//中断相关标志
#if ADC_WDOG_IT || ADC_REGULAR_IT || ADC_INJECTED_IT
	#define  ADC_ENABLE_IT                                         1
#else
	#define  ADC_ENABLE_IT                                         0
#endif //adc_it_enable


/**************************************************************************************************************
                                   Private Function
**************************************************************************************************************/
static void adc_Calibration(ADC_TypeDef* ADCx); //ADC校准
static void adc_DMAconfig(ADC_TypeDef* ADCx, uint32_t *DMA_BaseAddr, uint16_t DMA_BufferSize, uint8_t WordType);
static void adc_config(void);
static void bsp_adcGpioConfig(void);
static void bsp_adcClockConfig(void);
static void adc_nvicConfig(void);





/**************************************************************************************************************
*@函数名: void bsp_adcInit(void)
*@功  能: 实现adc的初始化(包括时钟、IO模式、ADC模式、DMA、以及中断的模式的配置)
*@输  入: 无
*@输  出: 无
*@返  回: 无
*@日  期: 180415
*@作  者: Wy
*@版  本: V1.0.0
*@修  改: None
**************************************************************************************************************/
//ADC引脚说明
//ADC123_IN0   PA0    ADC123_IN1    PA1   ADC123_IN2   PA2   ADC123_IN3   PA3 
//ADC3_IN4     PF6    ADC3_IN5      PF7   ADC3_IN6     PF8   ADC3_IN7     PF9  ADC3_IN8    PF10
//ADC12_IN4    PA4    ADC12_IN5     PA5   ADC12_IN6    PA6   ADC12_IN7    PA7  ADC12_IN8   PB0  
//ADC123_IN10  PC0    ADC123_IN11   PC1   ADC123_IN12  PC2   ADC123_IN13  PC3  ADC12_IN9 PB1 
//ADC12_IN14   PC4    ADC12_IN15    PC5 
void bsp_adcInit(void)
{
	bsp_adcClockConfig();
	bsp_adcGpioConfig();
	//adc配置
	adc_config();
#if ADC_ENABLE_IT
	adc_nvicConfig();
#endif
}

/**************************************************************************************************************
*@函数名: void bsp_adcInjected_ConvCmd(ADC_TypeDef *pAdc)
*@功  能: 软件触发注入组通道转换
*@输  入: pAdc为需要触发的ADC
*@输  出: 无
*@返  回: 无
*@日  期: 180415
*@作  者: Wy
*@版  本: V1.0.0
*@修  改: None
**************************************************************************************************************/
void bsp_adcInjected_ConvCmd(ADC_TypeDef *pAdc)
{
	ADC_SoftwareStartInjectedConvCmd(pAdc, ENABLE); 
}



/**************************************************************************************************************
*@函数名: static void bsp_adcClockConfig(void)
*@功  能: adc相关时钟配置
*@输  入: 无
*@输  出: 无
*@返  回: 无
*@日  期: 180415
*@作  者: Wy
*@版  本: V1.0.0
*@修  改: None
**************************************************************************************************************/
static void bsp_adcClockConfig(void)
{
#if (ADC123_IN0_ENABLE || ADC123_IN1_ENABLE || ADC123_IN2_ENABLE || ADC123_IN3_ENABLE || ADC12_IN4_ENABLE \
 || ADC12_IN5_ENABLE || ADC12_IN6_ENABLE || ADC12_IN7_ENABLE)
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
#endif

#if (ADC12_IN8_ENABLE || ADC12_IN9_ENABLE)
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
#endif

#if (ADC123_IN10_ENABLE || ADC123_IN11_ENABLE || ADC123_IN12_ENABLE || ADC123_IN13_ENABLE \
	|| ADC12_IN14_ENABLE  || ADC12_IN15_ENABLE)
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);		
#endif

#if (ADC3_IN4_ENABLE || ADC3_IN5_ENABLE || ADC3_IN6_ENABLE || ADC3_IN7_ENABLE || ADC3_IN8_ENABLE)
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);		
#endif
}

/**************************************************************************************************************
*@函数名: static void bsp_adcGpioConfig(void)
*@功  能: adc相关的引脚模式配置
*@输  入: 无
*@输  出: 无
*@返  回: 无
*@日  期: 180415
*@作  者: Wy
*@版  本: V1.0.0
*@修  改: None
**************************************************************************************************************/
static void bsp_adcGpioConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	
#if ADC123_IN0_ENABLE
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif
	
#if ADC123_IN1_ENABLE
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif
	
#if ADC123_IN2_ENABLE
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif
	
#if ADC123_IN3_ENABLE
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif	

#if ADC12_IN4_ENABLE
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif	

#if ADC12_IN5_ENABLE
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif

#if ADC12_IN6_ENABLE
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif

#if ADC12_IN7_ENABLE
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif

#if ADC12_IN8_ENABLE
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif

#if ADC12_IN9_ENABLE
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif

#if ADC123_IN10_ENABLE
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif

#if ADC123_IN11_ENABLE
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif

#if ADC123_IN12_ENABLE
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif

#if ADC123_IN13_ENABLE
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif

#if ADC12_IN14_ENABLE
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif

#if ADC12_IN15_ENABLE
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif

#if ADC3_IN4_ENABLE
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
#endif

#if ADC3_IN5_ENABLE
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
#endif

#if ADC3_IN6_ENABLE
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
#endif

#if ADC3_IN7_ENABLE
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
#endif

#if ADC3_IN8_ENABLE
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
#endif
}

/**************************************************************************************************************
*@函数名: static void adc_nvicConfig(void)
*@功  能: adc相关中断的优先级配置
*@输  入: 无
*@输  出: 无
*@返  回: 无
*@日  期: 180415
*@作  者: Wy
*@版  本: V1.0.0
*@修  改: None
**************************************************************************************************************/
static void adc_nvicConfig(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//adc1_2优先级
	NVIC_InitStruct.NVIC_IRQChannel = ADC1_2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	//adc3优先级
	NVIC_InitStruct.NVIC_IRQChannel = ADC3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);	
}

/**************************************************************************************************************
*@函数名: static void adc_Calibration(ADC_TypeDef* ADCx)
*@功  能: 实现ADC的校准
*@输  入: ADCx需要校准的ADC
*@输  出: 无
*@返  回: 无
*@日  期: 180415
*@作  者: Wy
*@版  本: V1.0.0
*@修  改: None
**************************************************************************************************************/
static void adc_Calibration(ADC_TypeDef* ADCx)
{
	ADC_Cmd(ADCx, ENABLE);	
	
	//ADC3校准
	ADC_ResetCalibration(ADCx);
	while (ADC_GetResetCalibrationStatus(ADCx))
	{
		;
  } 
	ADC_StartCalibration(ADCx);
	while (ADC_GetCalibrationStatus(ADCx))
	{
		;
	}		
}

/**************************************************************************************************************
*@函数名: static void adc_DMAconfig(ADC_TypeDef* ADCx, uint32_t *DMA_BaseAddr, uint16_t DMA_BufferSize, uint8_t WordType)
*@功  能: 实现adc的DMA配置
*@输  入: ADCx需要配置的ADC, DMA_BaseAddr DMA传输的目的地址 DMA_BufferSize DMA循环的周期数目 
*         WordType 0表示Word传输  1表示HalfWord的传输
*@输  出: 无
*@返  回: 无
*@日  期: 180415
*@作  者: Wy
*@版  本: V1.0.0
*@修  改: None
**************************************************************************************************************/
static void adc_DMAconfig(ADC_TypeDef* ADCx, uint32_t *DMA_BaseAddr, uint16_t DMA_BufferSize, uint8_t WordType)
{
	DMA_InitTypeDef DMA_InitStruct;
	DMA_Channel_TypeDef* DMAy_Channelx;
	
	if (ADCx == ADC1) DMAy_Channelx = DMA1_Channel1;
	else if (ADCx == ADC3) DMAy_Channelx =  DMA2_Channel5;
	else return ; //无效DMA配置
	
	DMA_DeInit(DMAy_Channelx);
	DMA_InitStruct.DMA_BufferSize = DMA_BufferSize; //5表示5个通道的ADC
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStruct.DMA_MemoryBaseAddr = (u32)DMA_BaseAddr;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)&ADCx->DR;
	if (WordType == 0)
	{
		DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
		DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	}
	else
	{
		DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
		DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	
	}
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(DMAy_Channelx, &DMA_InitStruct);
	DMA_Cmd(DMAy_Channelx, ENABLE);
}

/**************************************************************************************************************
*@函数名: static void adc_config(void)
*@功  能: 实现adc的配置及校准
*@输  入: 无
*@输  出: 无
*@返  回: 无
*@日  期: 180415
*@作  者: Wy
*@版  本: V1.0.0
*@修  改: None
**************************************************************************************************************/
static void adc_config(void)
{
	ADC_InitTypeDef ADC_InitStruct;
/********************************************************************************************/
//adc相关DMA配置
	//DMA1_Channel1 ---> ADC1
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	adc_DMAconfig(ADC1, &adc_sampData.adc12_data[0], 5, 0);
	//DMA2_Channel5 ---> ADC2
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	adc_DMAconfig(ADC3, (uint32_t*)&adc_samp_Injected.data[0], 5, 1);
	
/*********************************************************************************************/	
//ADC1配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	ADC_DeInit(ADC1);
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStruct.ADC_NbrOfChannel = 5; 
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;
	ADC_Init(ADC1, &ADC_InitStruct);
	//ADC1规则组设置(max:16 min:0)
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 5, ADC_SampleTime_1Cycles5);
	//ADC1注入组配置  (Rank   max:4  min:0)
//  ADC_AutoInjectedConvCmd(ADC1, ENABLE);	
//	ADC_InjectedDiscModeCmd(ADC1, DISABLE); //注入组间断模式使能
//  ADC_InjectedSequencerLengthConfig(ADC1, 4); //注入组长度为4个
//	
//  ADC_InjectedChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5);
//  ADC_InjectedChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_1Cycles5);
//  ADC_InjectedChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_1Cycles5);
//  ADC_InjectedChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_1Cycles5);
//	
//  ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_None);	
//  ADC_ExternalTrigInjectedConvCmd(ADC1, DISABLE);
//  ADC_SoftwareStartInjectedConvCmd(ADC1, ENABLE);
//  ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
	//ADC1的DMA配置及校准
	ADC_DMACmd(ADC1, ENABLE);
  adc_Calibration(ADC1);
	
/*********************************************************************************************/	
//ADC2配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
	ADC_DeInit(ADC2);
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStruct.ADC_Mode = ADC_Mode_RegSimult;
	ADC_InitStruct.ADC_NbrOfChannel = 5; //转换的规则组数目(不同的规则组需要配置)
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;
	ADC_Init(ADC2, &ADC_InitStruct);	
	//ADC2规则组设置
	ADC_RegularChannelConfig(ADC2, ADC_Channel_4, 1, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_3, 2, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_2, 3, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_1, 4, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_0, 5, ADC_SampleTime_1Cycles5);
	//ADC2注入组配置  (Rank   max:4  min:0)
//  ADC_AutoInjectedConvCmd(ADC2, ENABLE);	
//	ADC_InjectedDiscModeCmd(ADC2, DISABLE); //注入组间断模式使能
//  ADC_InjectedSequencerLengthConfig(ADC2, 4); //注入组长度为4个
//	
//  ADC_InjectedChannelConfig(ADC2, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5);
//  ADC_InjectedChannelConfig(ADC2, ADC_Channel_1, 2, ADC_SampleTime_1Cycles5);
//  ADC_InjectedChannelConfig(ADC2, ADC_Channel_2, 3, ADC_SampleTime_1Cycles5);
//  ADC_InjectedChannelConfig(ADC2, ADC_Channel_3, 4, ADC_SampleTime_1Cycles5);
//	
//  ADC_ExternalTrigInjectedConvConfig(ADC2, ADC_ExternalTrigInjecConv_None);	
//  ADC_ExternalTrigInjectedConvCmd(ADC2, DISABLE);
//  ADC_SoftwareStartInjectedConvCmd(ADC2, ENABLE);	
//  ADC_ITConfig(ADC2, ADC_IT_JEOC, ENABLE);
	//ADC2的校准(ADC2无DMA传输功能)
	adc_Calibration(ADC2);
	
/*********************************************************************************************/	
//ADC3配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
	ADC_DeInit(ADC3);
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStruct.ADC_NbrOfChannel = 5; 
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;
	ADC_Init(ADC3, &ADC_InitStruct);
	//ADC3规则组设置
	ADC_RegularChannelConfig(ADC3, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_1, 2, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_2, 3, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_3, 4, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_4, 5, ADC_SampleTime_1Cycles5);
	//ADC3注入组配置  (Rank   max:4  min:0)
//  ADC_AutoInjectedConvCmd(ADC3, ENABLE);	
//	ADC_InjectedDiscModeCmd(ADC3, DISABLE); //注入组间断模式使能
//  ADC_InjectedSequencerLengthConfig(ADC3, 4); //注入组长度为4个
//	
//  ADC_InjectedChannelConfig(ADC3, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5);
//  ADC_InjectedChannelConfig(ADC3, ADC_Channel_1, 2, ADC_SampleTime_1Cycles5);
//  ADC_InjectedChannelConfig(ADC3, ADC_Channel_2, 3, ADC_SampleTime_1Cycles5);
//  ADC_InjectedChannelConfig(ADC3, ADC_Channel_3, 4, ADC_SampleTime_1Cycles5);
//	
//  ADC_ExternalTrigInjectedConvConfig(ADC3, ADC_ExternalTrigInjecConv_None);	
//  ADC_ExternalTrigInjectedConvCmd(ADC3, DISABLE);
//  ADC_SoftwareStartInjectedConvCmd(ADC3, ENABLE);	
//  ADC_ITConfig(ADC3, ADC_IT_JEOC, ENABLE);	
	//ADC3的DMA配置及校准	
//	ADC_DMACmd(ADC3, ENABLE);
//	adc_Calibration(ADC3);


/*********************************************************************************************/	
//WDOG看门狗配置
//	ADC_AnalogWatchdogSingleChannelConfig(ADC1, ADC_Channel_1);
//	ADC_AnalogWatchdogThresholdsConfig(ADC1, 0XF00, 0); //看门狗阈值配置
//	ADC_AnalogWatchdogCmd(ADC1, ADC_AnalogWatchdog_SingleRegEnable); //看门狗关联到多少个ADC通道


/*********************************************************************************************/		
//ADC开始转换开始使能
//开启ADC的转换 ,由于ADC2是依靠ADC1进行双DMA转换的,所以 ADC2必须先ADC1转换
	ADC_SoftwareStartConvCmd(ADC2, ENABLE);	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	ADC_SoftwareStartConvCmd(ADC3, ENABLE);	
}

#if ADC_ENABLE_IT

/**************************************************************************************************************
*@函数名: void ADC3_IRQHandler(void)
*@功  能: ADC3的中断处理函数
*@输  入: 无
*@输  出: 无
*@返  回: 无
*@日  期: 180415
*@作  者: Wy
*@版  本: V1.0.0
*@修  改: None
**************************************************************************************************************/
void ADC3_IRQHandler(void)
{
	if (ADC_GetITStatus(ADC3, ADC_IT_EOC) == SET)
	{
		//...........
		ADC_ClearITPendingBit(ADC3, ADC_IT_EOC);
	}
#if ADC_WDOG_IT	
	if (ADC_GetITStatus(ADC3, ADC_IT_AWD) == SET)
	{
		//...........
		ADC_ClearITPendingBit(ADC3, ADC_IT_AWD);	
	}
#endif
#if ADC_WDOG_JEOC	
	if (ADC_GetITStatus(ADC3, ADC_IT_JEOC) == SET)
	{
		//...........
		ADC_ClearITPendingBit(ADC3, ADC_IT_JEOC);		
	}
#endif
}

/**************************************************************************************************************
*@函数名: void ADC1_2_IRQHandler(void)
*@功  能: ADC1和2的中断处理函数
*@输  入: 无
*@输  出: 无
*@返  回: 无
*@日  期: 180415
*@作  者: Wy
*@版  本: V1.0.0
*@修  改: None
**************************************************************************************************************/
void ADC1_2_IRQHandler(void)
{
	ADC_TypeDef *pAdc;
	
	#define  ADC_IT_BITS    0X07	
	if (ADC1->SR & ADC_IT_BITS)	       	pAdc = ADC1;
	else if (ADC2->SR & ADC_IT_BITS)    pAdc = ADC2;
	else	return ;
	#undef ADC_IT_BITS
	
	if (ADC_GetITStatus(pAdc, ADC_IT_EOC) == SET)
	{
		//...........
		ADC_ClearITPendingBit(pAdc, ADC_IT_EOC);
	}
#if ADC_WDOG_AWD
	if (ADC_GetITStatus(pAdc, ADC_IT_AWD) == SET)
	{
		//...........
		ADC_ClearITPendingBit(pAdc, ADC_IT_AWD);	
	}
#endif
	
#if ADC_WDOG_JEOC		
	if (ADC_GetITStatus(pAdc, ADC_IT_JEOC) == SET)
	{
		//...........
		ADC_ClearITPendingBit(pAdc, ADC_IT_JEOC);		
	}
#endif
}
#endif

