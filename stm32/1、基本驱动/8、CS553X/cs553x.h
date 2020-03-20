
#ifndef __CS553X_H
#define	__CS553X_H
 
#include <stdint.h>



typedef enum
{
	TYPE_CS5532=0x32,
	TYPE_CS5530=0x30, 
	TYPE_NONE=0x00, 
}ADC_TYPE_TypeDef; 			//AD芯片型号类型

typedef enum
{
	GAIN_1X=0x01,
	GAIN_2X, 
	GAIN_4X,
	GAIN_8X,
	GAIN_16X,
	GAIN_32X,
	GAIN_64X, 
}ADC_GAIN_TypeDef; 			//AD转换增益类型

typedef enum
{
	RATE_6P25=0x01,
	RATE_12P5, 
	RATE_25,
	RATE_50,
	RATE_100,
	RATE_200,
	RATE_400, 
}ADC_RATE_TypeDef; 			//ADC输出频率

typedef enum
{
	POLAR_BIPOLAR=0x00,		//双极性
	POLAR_UNIPOLAR,  		//单极性
}ADC_POLAR_TypeDef;  		//AD转换极性类型
 
typedef struct 
{
	ADC_TYPE_TypeDef type;				//芯片型号，如果TYPE_NONE，则不初始化该通道
	ADC_GAIN_TypeDef gain;				//放大倍数,	只限于CS5532，如果是CS550的芯片忽略该参数
	ADC_RATE_TypeDef rate;				//输出速率	速率固定为50HZ，暂时不用设置
	uint8_t vref;									//参考电压值，单位是0.1V,25,代表参考电压2.5V
	ADC_POLAR_TypeDef polarity;			//是双极性还是单极性，暂时不用
}CS553X_ConfigTypeDef; 			
uint8_t cs553x_init(CS553X_ConfigTypeDef *p_cs553x_config,uint8_t ch_num);			//cs553x 初始化函数，				
int32_t sample(uint8_t ch);				//返回采样值,参数值可用范围0-3 
 
// void us_delay(uint16_t ud);
// void ms_delay(uint8_t md); 

int32_t sample_ch0(ADC_RATE_TypeDef  rate);  /* 通道0（负荷通道）单独采样函数，20mv对应80万码，支持6.25HZ和50HZ采样 */
void fh_sample_rate_select(ADC_RATE_TypeDef  rate);  /* 负荷通道采样速率选择，支持6.25HZ和50HZ采样速率切换 */
void fh_rate_auto_select(void);  /* 根据执行机构的运行状态，自动切换负荷通道采样频率。横梁不动时，降频到6.25HZ。 */

int32_t fh_sample(void);  /* 负荷通道码值采样，根据fh_sample_filter_flag 标志位进行滤波处理*/
void fh_sample_filter_auto_select(void);  /* 根据执行机构的运行状态，自动决定是否对码值进行滤波处理。 */


#endif



	

	

	












