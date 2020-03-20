/**************************************************************************************************************
*@�� �� ��: bsp_adc.c
*@��������: STM32F103ZE + MDK5.10.0.0
*@�ļ�����: ʵ��adc�ײ�����
*@��    ��: Wy
*@��    ��: 180415
*@��    ��: V1.0.0
*@��    ��: None
* ����: Ĭ��adcע������زɼ������жϵķ�ʽ,�����������DMA�ķ�ʽ�ɼ�
**************************************************************************************************************/
#include "stm32f10x.h"
#include "bsp_adc.h"




/**************************************************************************************************************
                                   Public   Variable
**************************************************************************************************************/
ADC_RegularType adc_sampData; //���������ݴ洢
ADC_InjectedType adc_samp_Injected; //ע�������




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

//ADC1ʹ�ܵ�����ͨ����
#define  ADC1_CHANNEL_NUM    (ADC1_IN0_ENABLE+ADC1_IN1_ENABLE+ADC1_IN2_ENABLE+ADC1_IN3_ENABLE+ADC1_IN4_ENABLE+ADC1_IN5_ENABLE \
														 +ADC1_IN6_ENABLE+ADC1_IN7_ENABLE+ADC1_IN8_ENABLE+ADC1_IN9_ENABLE+ADC1_IN10_ENABLE+ADC1_IN11_ENABLE \
													   +ADC1_IN12_ENABLE+ADC1_IN13_ENABLE+ADC1_IN14_ENABLE+ADC1_IN15_ENABLE)
//ADC2ʹ�ܵ�����ͨ����
#define  ADC2_CHANNEL_NUM    (ADC2_IN0_ENABLE+ADC2_IN1_ENABLE+ADC2_IN2_ENABLE+ADC2_IN3_ENABLE+ADC2_IN4_ENABLE+ADC2_IN5_ENABLE \
														 +ADC2_IN6_ENABLE+ADC2_IN7_ENABLE+ADC2_IN8_ENABLE+ADC2_IN9_ENABLE+ADC2_IN10_ENABLE+ADC2_IN11_ENABLE \
													   +ADC2_IN12_ENABLE+ADC2_IN13_ENABLE+ADC2_IN14_ENABLE+ADC2_IN15_ENABLE)
//ADC3ʹ�ܵ�����ͨ����
#define  ADC3_CHANNEL_NUM    (ADC3_IN0_ENABLE+ADC3_IN1_ENABLE+ADC3_IN2_ENABLE+ADC3_IN3_ENABLE+ADC3_IN4_ENABLE+ADC3_IN5_ENABLE \
														 +ADC3_IN6_ENABLE+ADC3_IN7_ENABLE+ADC3_IN8_ENABLE+ADC3_IN10_ENABLE+ADC3_IN11_ENABLE \
													   +ADC3_IN12_ENABLE+ADC3_IN13_ENABLE)
//ADCʹ�ܵ�����ͨ����													 
#define  ADC_CHANNEL_NUM     (ADC1_CHANNEL_NUM+ADC2_CHANNEL_NUM+ADC3_CHANNEL_NUM)


//�ж���ر�־
#if ADC_WDOG_IT || ADC_REGULAR_IT || ADC_INJECTED_IT
	#define  ADC_ENABLE_IT                                         1
#else
	#define  ADC_ENABLE_IT                                         0
#endif //adc_it_enable


/**************************************************************************************************************
                                   Private Function
**************************************************************************************************************/
static void adc_Calibration(ADC_TypeDef* ADCx); //ADCУ׼
static void adc_DMAconfig(ADC_TypeDef* ADCx, uint32_t *DMA_BaseAddr, uint16_t DMA_BufferSize, uint8_t WordType);
static void adc_config(void);
static void bsp_adcGpioConfig(void);
static void bsp_adcClockConfig(void);
static void adc_nvicConfig(void);





/**************************************************************************************************************
*@������: void bsp_adcInit(void)
*@��  ��: ʵ��adc�ĳ�ʼ��(����ʱ�ӡ�IOģʽ��ADCģʽ��DMA���Լ��жϵ�ģʽ������)
*@��  ��: ��
*@��  ��: ��
*@��  ��: ��
*@��  ��: 180415
*@��  ��: Wy
*@��  ��: V1.0.0
*@��  ��: None
**************************************************************************************************************/
//ADC����˵��
//ADC123_IN0   PA0    ADC123_IN1    PA1   ADC123_IN2   PA2   ADC123_IN3   PA3 
//ADC3_IN4     PF6    ADC3_IN5      PF7   ADC3_IN6     PF8   ADC3_IN7     PF9  ADC3_IN8    PF10
//ADC12_IN4    PA4    ADC12_IN5     PA5   ADC12_IN6    PA6   ADC12_IN7    PA7  ADC12_IN8   PB0  
//ADC123_IN10  PC0    ADC123_IN11   PC1   ADC123_IN12  PC2   ADC123_IN13  PC3  ADC12_IN9 PB1 
//ADC12_IN14   PC4    ADC12_IN15    PC5 
void bsp_adcInit(void)
{
	bsp_adcClockConfig();
	bsp_adcGpioConfig();
	//adc����
	adc_config();
#if ADC_ENABLE_IT
	adc_nvicConfig();
#endif
}

/**************************************************************************************************************
*@������: void bsp_adcInjected_ConvCmd(ADC_TypeDef *pAdc)
*@��  ��: �������ע����ͨ��ת��
*@��  ��: pAdcΪ��Ҫ������ADC
*@��  ��: ��
*@��  ��: ��
*@��  ��: 180415
*@��  ��: Wy
*@��  ��: V1.0.0
*@��  ��: None
**************************************************************************************************************/
void bsp_adcInjected_ConvCmd(ADC_TypeDef *pAdc)
{
	ADC_SoftwareStartInjectedConvCmd(pAdc, ENABLE); 
}



/**************************************************************************************************************
*@������: static void bsp_adcClockConfig(void)
*@��  ��: adc���ʱ������
*@��  ��: ��
*@��  ��: ��
*@��  ��: ��
*@��  ��: 180415
*@��  ��: Wy
*@��  ��: V1.0.0
*@��  ��: None
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
*@������: static void bsp_adcGpioConfig(void)
*@��  ��: adc��ص�����ģʽ����
*@��  ��: ��
*@��  ��: ��
*@��  ��: ��
*@��  ��: 180415
*@��  ��: Wy
*@��  ��: V1.0.0
*@��  ��: None
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
*@������: static void adc_nvicConfig(void)
*@��  ��: adc����жϵ����ȼ�����
*@��  ��: ��
*@��  ��: ��
*@��  ��: ��
*@��  ��: 180415
*@��  ��: Wy
*@��  ��: V1.0.0
*@��  ��: None
**************************************************************************************************************/
static void adc_nvicConfig(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//adc1_2���ȼ�
	NVIC_InitStruct.NVIC_IRQChannel = ADC1_2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	//adc3���ȼ�
	NVIC_InitStruct.NVIC_IRQChannel = ADC3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);	
}

/**************************************************************************************************************
*@������: static void adc_Calibration(ADC_TypeDef* ADCx)
*@��  ��: ʵ��ADC��У׼
*@��  ��: ADCx��ҪУ׼��ADC
*@��  ��: ��
*@��  ��: ��
*@��  ��: 180415
*@��  ��: Wy
*@��  ��: V1.0.0
*@��  ��: None
**************************************************************************************************************/
static void adc_Calibration(ADC_TypeDef* ADCx)
{
	ADC_Cmd(ADCx, ENABLE);	
	
	//ADC3У׼
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
*@������: static void adc_DMAconfig(ADC_TypeDef* ADCx, uint32_t *DMA_BaseAddr, uint16_t DMA_BufferSize, uint8_t WordType)
*@��  ��: ʵ��adc��DMA����
*@��  ��: ADCx��Ҫ���õ�ADC, DMA_BaseAddr DMA�����Ŀ�ĵ�ַ DMA_BufferSize DMAѭ����������Ŀ 
*         WordType 0��ʾWord����  1��ʾHalfWord�Ĵ���
*@��  ��: ��
*@��  ��: ��
*@��  ��: 180415
*@��  ��: Wy
*@��  ��: V1.0.0
*@��  ��: None
**************************************************************************************************************/
static void adc_DMAconfig(ADC_TypeDef* ADCx, uint32_t *DMA_BaseAddr, uint16_t DMA_BufferSize, uint8_t WordType)
{
	DMA_InitTypeDef DMA_InitStruct;
	DMA_Channel_TypeDef* DMAy_Channelx;
	
	if (ADCx == ADC1) DMAy_Channelx = DMA1_Channel1;
	else if (ADCx == ADC3) DMAy_Channelx =  DMA2_Channel5;
	else return ; //��ЧDMA����
	
	DMA_DeInit(DMAy_Channelx);
	DMA_InitStruct.DMA_BufferSize = DMA_BufferSize; //5��ʾ5��ͨ����ADC
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
*@������: static void adc_config(void)
*@��  ��: ʵ��adc�����ü�У׼
*@��  ��: ��
*@��  ��: ��
*@��  ��: ��
*@��  ��: 180415
*@��  ��: Wy
*@��  ��: V1.0.0
*@��  ��: None
**************************************************************************************************************/
static void adc_config(void)
{
	ADC_InitTypeDef ADC_InitStruct;
/********************************************************************************************/
//adc���DMA����
	//DMA1_Channel1 ---> ADC1
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	adc_DMAconfig(ADC1, &adc_sampData.adc12_data[0], 5, 0);
	//DMA2_Channel5 ---> ADC2
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	adc_DMAconfig(ADC3, (uint32_t*)&adc_samp_Injected.data[0], 5, 1);
	
/*********************************************************************************************/	
//ADC1����
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	ADC_DeInit(ADC1);
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStruct.ADC_NbrOfChannel = 5; 
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;
	ADC_Init(ADC1, &ADC_InitStruct);
	//ADC1����������(max:16 min:0)
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 5, ADC_SampleTime_1Cycles5);
	//ADC1ע��������  (Rank   max:4  min:0)
//  ADC_AutoInjectedConvCmd(ADC1, ENABLE);	
//	ADC_InjectedDiscModeCmd(ADC1, DISABLE); //ע������ģʽʹ��
//  ADC_InjectedSequencerLengthConfig(ADC1, 4); //ע���鳤��Ϊ4��
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
	//ADC1��DMA���ü�У׼
	ADC_DMACmd(ADC1, ENABLE);
  adc_Calibration(ADC1);
	
/*********************************************************************************************/	
//ADC2����
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
	ADC_DeInit(ADC2);
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStruct.ADC_Mode = ADC_Mode_RegSimult;
	ADC_InitStruct.ADC_NbrOfChannel = 5; //ת���Ĺ�������Ŀ(��ͬ�Ĺ�������Ҫ����)
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;
	ADC_Init(ADC2, &ADC_InitStruct);	
	//ADC2����������
	ADC_RegularChannelConfig(ADC2, ADC_Channel_4, 1, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_3, 2, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_2, 3, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_1, 4, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_0, 5, ADC_SampleTime_1Cycles5);
	//ADC2ע��������  (Rank   max:4  min:0)
//  ADC_AutoInjectedConvCmd(ADC2, ENABLE);	
//	ADC_InjectedDiscModeCmd(ADC2, DISABLE); //ע������ģʽʹ��
//  ADC_InjectedSequencerLengthConfig(ADC2, 4); //ע���鳤��Ϊ4��
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
	//ADC2��У׼(ADC2��DMA���书��)
	adc_Calibration(ADC2);
	
/*********************************************************************************************/	
//ADC3����
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
	ADC_DeInit(ADC3);
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStruct.ADC_NbrOfChannel = 5; 
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;
	ADC_Init(ADC3, &ADC_InitStruct);
	//ADC3����������
	ADC_RegularChannelConfig(ADC3, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_1, 2, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_2, 3, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_3, 4, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_4, 5, ADC_SampleTime_1Cycles5);
	//ADC3ע��������  (Rank   max:4  min:0)
//  ADC_AutoInjectedConvCmd(ADC3, ENABLE);	
//	ADC_InjectedDiscModeCmd(ADC3, DISABLE); //ע������ģʽʹ��
//  ADC_InjectedSequencerLengthConfig(ADC3, 4); //ע���鳤��Ϊ4��
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
	//ADC3��DMA���ü�У׼	
//	ADC_DMACmd(ADC3, ENABLE);
//	adc_Calibration(ADC3);


/*********************************************************************************************/	
//WDOG���Ź�����
//	ADC_AnalogWatchdogSingleChannelConfig(ADC1, ADC_Channel_1);
//	ADC_AnalogWatchdogThresholdsConfig(ADC1, 0XF00, 0); //���Ź���ֵ����
//	ADC_AnalogWatchdogCmd(ADC1, ADC_AnalogWatchdog_SingleRegEnable); //���Ź����������ٸ�ADCͨ��


/*********************************************************************************************/		
//ADC��ʼת����ʼʹ��
//����ADC��ת�� ,����ADC2������ADC1����˫DMAת����,���� ADC2������ADC1ת��
	ADC_SoftwareStartConvCmd(ADC2, ENABLE);	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	ADC_SoftwareStartConvCmd(ADC3, ENABLE);	
}

#if ADC_ENABLE_IT

/**************************************************************************************************************
*@������: void ADC3_IRQHandler(void)
*@��  ��: ADC3���жϴ�����
*@��  ��: ��
*@��  ��: ��
*@��  ��: ��
*@��  ��: 180415
*@��  ��: Wy
*@��  ��: V1.0.0
*@��  ��: None
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
*@������: void ADC1_2_IRQHandler(void)
*@��  ��: ADC1��2���жϴ�����
*@��  ��: ��
*@��  ��: ��
*@��  ��: ��
*@��  ��: 180415
*@��  ��: Wy
*@��  ��: V1.0.0
*@��  ��: None
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

