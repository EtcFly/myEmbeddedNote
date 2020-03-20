
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
//上升按键
#define   RCC_KEY_UP                         RCC_APB2Periph_GPIOB  
#define   PIN_KEY_UP                         GPIO_Pin_6
#define   GPIO_KEY_UP                        GPIOB
//下降按键
#define   RCC_KEY_DW                         RCC_APB2Periph_GPIOB
#define   PIN_KEY_DW                         GPIO_Pin_7
#define   GPIO_KEY_DW                        GPIOB
//手动复位按键
#define   RCC_KEY_RST                        RCC_APB2Periph_GPIOB
#define   PIN_KEY_RST                        GPIO_Pin_5
#define   GPIO_KEY_RST                       GPIOB
//急停按键
#define   RCC_KEY_QSTOP                      RCC_APB2Periph_GPIOB
#define   PIN_KEY_QSTOP                      GPIO_Pin_8
#define   GPIO_KEY_QSTOP                     GPIOB
//上限位输入
#define   RCC_LIMIT_UP                       RCC_APB2Periph_GPIOD
#define   PIN_LIMIT_UP                       GPIO_Pin_2
#define   GPIO_LIMIT_UP                      GPIOD
//下限位输入
#define   RCC_LIMIT_DW                       RCC_APB2Periph_GPIOA
#define   PIN_LIMIT_DW                       GPIO_Pin_8
#define   GPIO_LIMIT_DW                      GPIOA
//自复位输入
#define   RCC_INPUT_RST                      RCC_APB2Periph_GPIOC
#define   PIN_INPUT_RST                      GPIO_Pin_9
#define   GPIO_INPUT_RST                     GPIOC
//报警信号输入
#define   RCC_ALARM_IN                       RCC_APB2Periph_GPIOC
#define   PIN_ALARM_IN                       GPIO_Pin_4
#define   GPIO_ALARM_IN                      GPIOC
//保留输入
#define   RCC_INPUT_RSV                      RCC_APB2Periph_GPIOB
#define   PIN_INPUT_RSV                      GPIO_Pin_9
#define   GPIO_INPUT_RSV                     GPIOB

//输出配置
////变频正转输出
//#define   RCC_CONV_DIR_P                     RCC_APB2Periph_GPIOA
//#define   PIN_CONV_DIR_P                     GPIO_Pin_2
//#define   GPIO_CONV_DIR_P                    GPIOA
////变频反转输出
//#define   RCC_CONV_DIR_N                     RCC_APB2Periph_GPIOA
//#define   PIN_CONV_DIR_N                     GPIO_Pin_8
//#define   GPIO_CONV_DIR_N                    GPIOA
//段速1控制输出
#define   RCC_CONV_SPEED1                    RCC_APB2Periph_GPIOC
#define   PIN_CONV_SPEED1                    GPIO_Pin_3
#define   GPIO_CONV_SPEED1                   GPIOC
//段速2控制输出
#define   RCC_CONV_SPEED2                    RCC_APB2Periph_GPIOC
#define   PIN_CONV_SPEED2                    GPIO_Pin_2
#define   GPIO_CONV_SPEED2                   GPIOC
//电源指示灯
#define   RCC_POWER_LIGHT                    RCC_APB2Periph_GPIOB
#define   PIN_POWER_LIGHT                    GPIO_Pin_2
#define   GPIO_POWER_LIGHT                   GPIOB
//上升指示灯
#define   RCC_UP_LIGHT                       RCC_APB2Periph_GPIOB
#define   PIN_UP_LIGHT                       GPIO_Pin_1
#define   GPIO_UP_LIGHT                      GPIOB
//下降指示灯
#define   RCC_DW_LIGHT                       RCC_APB2Periph_GPIOB
#define   PIN_DW_LIGHT                       GPIO_Pin_0
#define   GPIO_DW_LIGHT                      GPIOB
//电机正转使能输出
#define   RCC_CONV_DIR_P                     RCC_APB2Periph_GPIOC
#define   PIN_CONV_DIR_P                     GPIO_Pin_5
#define   GPIO_CONV_DIR_P                    GPIOC
//电机反转使能输出
#define   RCC_CONV_DIR_N                     RCC_APB2Periph_GPIOC
#define   PIN_CONV_DIR_N                     GPIO_Pin_0
#define   GPIO_CONV_DIR_N                    GPIOC
//备用输出
#define   RCC_RSV_OUT                        RCC_APB2Periph_GPIOC
#define   PIN_RSV_OUT                        GPIO_Pin_1
#define   GPIO_RSV_OUT                       GPIOC



/************************end config****************************************************/
//key input define
#define   SW_KEY_UP                      0X0008u      //上升按键
#define   SW_KEY_DW                      0X0004u      //下降按键
#define   SW_KEY_RST                     0X0010u      //复位按键
#define   SW_KEY_QSTOP                   0X0002u      //急停按键
#define   SW_KEY_RSV                     0X0001u      //保留按键
//limit and reset input
#define   SW_INPUT_LIMIT_UP              0X0080u      //上限位输入
#define   SW_INPUT_LIMIT_DW              0X0040u      //下限位输入
#define   SW_INPUT_RST                   0X0020u      //自复位输入
#define   SW_INPUT_ALARM                 0X0800u      //报警信号输入

//control switch output
#define   SW_CONV_DIR_P                  0X0010u      //变频器正向输入信号
#define   SW_CONV_DIR_N                  0x0008u      //变频器反向输入信号
#define   SW_CONV_SPEED1                 0x0004u      //变频器1段速输入信号
#define   SW_CONV_SPEED2                 0x0002u      //变频器2段速输入信号
#define   SW_POWER_LIGHT                 0X0080u       //电源指示灯
#define   SW_UP_LIGHT                    0X0040u       //上升指示灯
#define   SW_DW_LIGHT                    0X0020u       //下降指示灯
#define   SW_CONV_DIR_P_ENABLE           0X0200u       //变频器正向输出使能
#define   SW_CONV_DIR_N_ENABLE           0X0100u       //变频器反向输出使能
#define   SW_CONV_RSV1                   0x0001u      //变频器输入保留1

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
