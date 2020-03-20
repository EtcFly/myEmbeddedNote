
/**************************************************************************************************************************
*@fileName: SW_Input.h
*@platform: MDK 5.10.0.0 + STM32F103C8T6
*@Version:  V1.0.0
*@describe: None
*@modification
*==========================================================================================================================
* version    Author   Data                                            Details
*--------------------------------------------------------------------------------------------------------------------------
* V1.0.0      Wy     190220                                           Create File
===========================================================================================================================
**************************************************************************************************************************/

#ifndef _SW_INPUT_H
#define _SW_INPUT_H

/********************************config**************************************************/
//��������
#define   RCC_KEY_UP                         RCC_APB2Periph_GPIOB  
#define   PIN_KEY_UP                         GPIO_Pin_6
#define   GPIO_KEY_UP                        GPIOB
//�½�����
#define   RCC_KEY_DW                         RCC_APB2Periph_GPIOB
#define   PIN_KEY_DW                         GPIO_Pin_7
#define   GPIO_KEY_DW                        GPIOB
//�ֶ���λ����
#define   RCC_KEY_RST                        RCC_APB2Periph_GPIOB
#define   PIN_KEY_RST                        GPIO_Pin_5
#define   GPIO_KEY_RST                       GPIOB
//��ͣ����
#define   RCC_KEY_QSTOP                      RCC_APB2Periph_GPIOB
#define   PIN_KEY_QSTOP                      GPIO_Pin_8
#define   GPIO_KEY_QSTOP                     GPIOB
//����λ����
#define   RCC_LIMIT_UP                       RCC_APB2Periph_GPIOD
#define   PIN_LIMIT_UP                       GPIO_Pin_2
#define   GPIO_LIMIT_UP                      GPIOD
//����λ����
#define   RCC_LIMIT_DW                       RCC_APB2Periph_GPIOA
#define   PIN_LIMIT_DW                       GPIO_Pin_8
#define   GPIO_LIMIT_DW                      GPIOA
//�Ը�λ����
#define   RCC_INPUT_RST                      RCC_APB2Periph_GPIOC
#define   PIN_INPUT_RST                      GPIO_Pin_9
#define   GPIO_INPUT_RST                     GPIOC
//�����ź�����
#define   RCC_ALARM_IN                       RCC_APB2Periph_GPIOC
#define   PIN_ALARM_IN                       GPIO_Pin_4
#define   GPIO_ALARM_IN                      GPIOC
//��������
#define   RCC_INPUT_RSV                      RCC_APB2Periph_GPIOB
#define   PIN_INPUT_RSV                      GPIO_Pin_9
#define   GPIO_INPUT_RSV                     GPIOB

//�������
////��Ƶ��ת���
//#define   RCC_CONV_DIR_P                     RCC_APB2Periph_GPIOA
//#define   PIN_CONV_DIR_P                     GPIO_Pin_2
//#define   GPIO_CONV_DIR_P                    GPIOA
////��Ƶ��ת���
//#define   RCC_CONV_DIR_N                     RCC_APB2Periph_GPIOA
//#define   PIN_CONV_DIR_N                     GPIO_Pin_8
//#define   GPIO_CONV_DIR_N                    GPIOA
//����1�������
#define   RCC_CONV_SPEED1                    RCC_APB2Periph_GPIOC
#define   PIN_CONV_SPEED1                    GPIO_Pin_3
#define   GPIO_CONV_SPEED1                   GPIOC
//����2�������
#define   RCC_CONV_SPEED2                    RCC_APB2Periph_GPIOC
#define   PIN_CONV_SPEED2                    GPIO_Pin_2
#define   GPIO_CONV_SPEED2                   GPIOC
//��Դָʾ��
#define   RCC_POWER_LIGHT                    RCC_APB2Periph_GPIOB
#define   PIN_POWER_LIGHT                    GPIO_Pin_2
#define   GPIO_POWER_LIGHT                   GPIOB
//����ָʾ��
#define   RCC_UP_LIGHT                       RCC_APB2Periph_GPIOB
#define   PIN_UP_LIGHT                       GPIO_Pin_1
#define   GPIO_UP_LIGHT                      GPIOB
//�½�ָʾ��
#define   RCC_DW_LIGHT                       RCC_APB2Periph_GPIOB
#define   PIN_DW_LIGHT                       GPIO_Pin_0
#define   GPIO_DW_LIGHT                      GPIOB
//�����תʹ�����
#define   RCC_CONV_DIR_P                     RCC_APB2Periph_GPIOC
#define   PIN_CONV_DIR_P                     GPIO_Pin_5
#define   GPIO_CONV_DIR_P                    GPIOC
//�����תʹ�����
#define   RCC_CONV_DIR_N                     RCC_APB2Periph_GPIOC
#define   PIN_CONV_DIR_N                     GPIO_Pin_0
#define   GPIO_CONV_DIR_N                    GPIOC
//�������
#define   RCC_RSV_OUT                        RCC_APB2Periph_GPIOC
#define   PIN_RSV_OUT                        GPIO_Pin_1
#define   GPIO_RSV_OUT                       GPIOC



/************************end config****************************************************/
//key input define
#define   SW_KEY_UP                      0X0008u      //��������
#define   SW_KEY_DW                      0X0004u      //�½�����
#define   SW_KEY_RST                     0X0010u      //��λ����
#define   SW_KEY_QSTOP                   0X0002u      //��ͣ����
#define   SW_KEY_RSV                     0X0001u      //��������
//limit and reset input
#define   SW_INPUT_LIMIT_UP              0X0080u      //����λ����
#define   SW_INPUT_LIMIT_DW              0X0040u      //����λ����
#define   SW_INPUT_RST                   0X0020u      //�Ը�λ����
#define   SW_INPUT_ALARM                 0X0800u      //�����ź�����

//control switch output
#define   SW_CONV_DIR_P                  0X0010u      //��Ƶ�����������ź�
#define   SW_CONV_DIR_N                  0x0008u      //��Ƶ�����������ź�
#define   SW_CONV_SPEED1                 0x0004u      //��Ƶ��1���������ź�
#define   SW_CONV_SPEED2                 0x0002u      //��Ƶ��2���������ź�
#define   SW_POWER_LIGHT                 0X0080u       //��Դָʾ��
#define   SW_UP_LIGHT                    0X0040u       //����ָʾ��
#define   SW_DW_LIGHT                    0X0020u       //�½�ָʾ��
#define   SW_CONV_DIR_P_ENABLE           0X0200u       //��Ƶ���������ʹ��
#define   SW_CONV_DIR_N_ENABLE           0X0100u       //��Ƶ���������ʹ��
#define   SW_CONV_RSV1                   0x0001u      //��Ƶ�����뱣��1

//switch input define
#define   SW_INPUT_1                     SW_KEY_UP
#define   SW_INPUT_2                     SW_KEY_DW
#define   SW_INPUT_3                     SW_KEY_RST
#define   SW_INPUT_4                     SW_KEY_QSTOP
#define   SW_INPUT_5                     SW_INPUT_LIMIT_UP
#define   SW_INPUT_6                     SW_INPUT_LIMIT_DW
#define   SW_INPUT_7                     SW_INPUT_RST
#define   SW_INPUT_8                     SW_KEY_RSV

//switch output define
#define   SW_OUTPUT_1                 SW_CONV_DIR_P
#define   SW_OUTPUT_2                 SW_CONV_DIR_N
#define   SW_OUTPUT_3                 SW_CONV_SPEED1
#define   SW_OUTPUT_4                 SW_CONV_SPEED2
#define   SW_OUTPUT_5                 SW_CONV_RSV1
#define   SW_OUTPUT_6                 SW_CONV_RSV2


void switch_inOutConfig(void);
void set_swOutputBit(uint32_t bit);  
void clear_swOutputBit(uint32_t bit);
void sw_inputTask(void);
void sw_outputTask(void);
uint16_t get_swOutput(void);
uint16_t get_swInput(void);

#endif
