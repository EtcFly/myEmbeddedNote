/**************************************************************************************************************
*@�� �� ��: bsp_adc.h
*@��������: STM32F103ZE + MDK5.10.0.0
*@�ļ�����: ʵ��adc�ײ�����
*@��    ��: Wy
*@��    ��: 180415
*@��    ��: V1.0.0
*@��    ��: None
**************************************************************************************************************/
#ifndef _BSP_ADC_H
#define _BSP_ADC_H

/**************************************************************************************************************
                                   Public   TypeDef
**************************************************************************************************************/
typedef struct
{
	uint32_t adc12_data[16];
	uint16_t adc3_data[16]; //REGULAR
}ADC_RegularType;

typedef struct
{
	uint16_t data[4]; //Injected
}ADC_InjectedType;


/**************************************************************************************************************
                                   Public   Variable
**************************************************************************************************************/
extern ADC_RegularType adc_sampData;
extern ADC_InjectedType adc_samp_Injected;




/**************************************************************************************************************
                                   Public   Define
**************************************************************************************************************/
//adc����
//ADC1
#define  ADC1_IN0_ENABLE                                     1 //1��ʾ��ͨ������
#define  ADC1_IN1_ENABLE                                     0
#define  ADC1_IN2_ENABLE                                     0
#define  ADC1_IN3_ENABLE                                     0
#define  ADC1_IN4_ENABLE                                     0
#define  ADC1_IN5_ENABLE                                     0
#define  ADC1_IN6_ENABLE                                     0
#define  ADC1_IN7_ENABLE                                     0
#define  ADC1_IN8_ENABLE                                     0
#define  ADC1_IN9_ENABLE                                     0
#define  ADC1_IN10_ENABLE                                    0
#define  ADC1_IN11_ENABLE                                    0
#define  ADC1_IN12_ENABLE                                    0
#define  ADC1_IN13_ENABLE                                    0
#define  ADC1_IN14_ENABLE                                    0
#define  ADC1_IN15_ENABLE                                    0
//ADC2
#define  ADC2_IN0_ENABLE                                     0 //1��ʾ��ͨ������
#define  ADC2_IN1_ENABLE                                     0
#define  ADC2_IN2_ENABLE                                     0
#define  ADC2_IN3_ENABLE                                     0
#define  ADC2_IN4_ENABLE                                     0
#define  ADC2_IN5_ENABLE                                     0
#define  ADC2_IN6_ENABLE                                     0
#define  ADC2_IN7_ENABLE                                     0
#define  ADC2_IN8_ENABLE                                     0
#define  ADC2_IN9_ENABLE                                     0
#define  ADC2_IN10_ENABLE                                    0
#define  ADC2_IN11_ENABLE                                    0
#define  ADC2_IN12_ENABLE                                    0
#define  ADC2_IN13_ENABLE                                    0
#define  ADC2_IN14_ENABLE                                    0
#define  ADC2_IN15_ENABLE                                    0
//ADC3
#define  ADC3_IN0_ENABLE                                     0 //1��ʾ��ͨ������
#define  ADC3_IN1_ENABLE                                     0
#define  ADC3_IN2_ENABLE                                     0
#define  ADC3_IN3_ENABLE                                     0
#define  ADC3_IN4_ENABLE                                     0
#define  ADC3_IN5_ENABLE                                     0
#define  ADC3_IN6_ENABLE                                     0
#define  ADC3_IN7_ENABLE                                     0
#define  ADC3_IN8_ENABLE                                     0
//#define  ADC2_IN9_ENABLE                                   0 //adc3û��IN9
#define  ADC3_IN10_ENABLE                                    0
#define  ADC3_IN11_ENABLE                                    0
#define  ADC3_IN12_ENABLE                                    0
#define  ADC3_IN13_ENABLE                                    0

//ʹ���ж�
#define  ADC_WDOG_IT                                         1
#define  ADC_INJECTED_IT                                     1



/**************************************************************************************************************
                                   ExPort Function
**************************************************************************************************************/
void bsp_adcInit(void);
void bsp_adcInjected_ConvCmd(ADC_TypeDef *pAdc);


#endif



