
#ifndef __CS553X_H
#define	__CS553X_H
 
#include <stdint.h>



typedef enum
{
	TYPE_CS5532=0x32,
	TYPE_CS5530=0x30, 
	TYPE_NONE=0x00, 
}ADC_TYPE_TypeDef; 			//ADоƬ�ͺ�����

typedef enum
{
	GAIN_1X=0x01,
	GAIN_2X, 
	GAIN_4X,
	GAIN_8X,
	GAIN_16X,
	GAIN_32X,
	GAIN_64X, 
}ADC_GAIN_TypeDef; 			//ADת����������

typedef enum
{
	RATE_6P25=0x01,
	RATE_12P5, 
	RATE_25,
	RATE_50,
	RATE_100,
	RATE_200,
	RATE_400, 
}ADC_RATE_TypeDef; 			//ADC���Ƶ��

typedef enum
{
	POLAR_BIPOLAR=0x00,		//˫����
	POLAR_UNIPOLAR,  		//������
}ADC_POLAR_TypeDef;  		//ADת����������
 
typedef struct 
{
	ADC_TYPE_TypeDef type;				//оƬ�ͺţ����TYPE_NONE���򲻳�ʼ����ͨ��
	ADC_GAIN_TypeDef gain;				//�Ŵ���,	ֻ����CS5532�������CS550��оƬ���Ըò���
	ADC_RATE_TypeDef rate;				//�������	���ʹ̶�Ϊ50HZ����ʱ��������
	uint8_t vref;									//�ο���ѹֵ����λ��0.1V,25,����ο���ѹ2.5V
	ADC_POLAR_TypeDef polarity;			//��˫���Ի��ǵ����ԣ���ʱ����
}CS553X_ConfigTypeDef; 			
uint8_t cs553x_init(CS553X_ConfigTypeDef *p_cs553x_config,uint8_t ch_num);			//cs553x ��ʼ��������				
int32_t sample(uint8_t ch);				//���ز���ֵ,����ֵ���÷�Χ0-3 
 
// void us_delay(uint16_t ud);
// void ms_delay(uint8_t md); 

int32_t sample_ch0(ADC_RATE_TypeDef  rate);  /* ͨ��0������ͨ������������������20mv��Ӧ80���룬֧��6.25HZ��50HZ���� */
void fh_sample_rate_select(ADC_RATE_TypeDef  rate);  /* ����ͨ����������ѡ��֧��6.25HZ��50HZ���������л� */
void fh_rate_auto_select(void);  /* ����ִ�л���������״̬���Զ��л�����ͨ������Ƶ�ʡ���������ʱ����Ƶ��6.25HZ�� */

int32_t fh_sample(void);  /* ����ͨ����ֵ����������fh_sample_filter_flag ��־λ�����˲�����*/
void fh_sample_filter_auto_select(void);  /* ����ִ�л���������״̬���Զ������Ƿ����ֵ�����˲����� */


#endif



	

	

	












