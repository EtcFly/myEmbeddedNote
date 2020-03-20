/**************************************************************************************************************************
*@fileName: SW_Input.h
*@platform: MDK 5.10.0.0 + STM32F103C8T6
*@Version:  V1.0.0
*@describe: 开关输入输出驱动函数
*@modification
*==========================================================================================================================
* version    Author   Data                                            Details
*--------------------------------------------------------------------------------------------------------------------------
* V1.0.0      Wy     190218                                           Create File
===========================================================================================================================
**************************************************************************************************************************/
#include "stm32f10x.h"
#include "SW_Input.h"


#define  SW_INPUT_ADJUST                   0X0000ffffu   //switch input mask
#define  SW_OUTPUT_ADJUST                  0X0000ffffu   //switch output mask

uint16_t sw_inputReg=0X00;
uint16_t sw_outputReg=0Xffff;


void switch_inOutConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
//INPUT    
    GPIO_InitStruct.GPIO_Pin = PIN_KEY_UP;
    GPIO_Init(GPIO_KEY_UP, &GPIO_InitStruct);
  
    GPIO_InitStruct.GPIO_Pin = PIN_KEY_DW;
    GPIO_Init(GPIO_KEY_DW, &GPIO_InitStruct);
  
    GPIO_InitStruct.GPIO_Pin = PIN_KEY_RST;
    GPIO_Init(GPIO_KEY_RST, &GPIO_InitStruct);  

    GPIO_InitStruct.GPIO_Pin = PIN_KEY_QSTOP;
    GPIO_Init(GPIO_KEY_QSTOP, &GPIO_InitStruct);
  
    GPIO_InitStruct.GPIO_Pin = PIN_LIMIT_UP;
    GPIO_Init(GPIO_LIMIT_UP, &GPIO_InitStruct);
  
    GPIO_InitStruct.GPIO_Pin = PIN_LIMIT_DW;
    GPIO_Init(GPIO_LIMIT_DW, &GPIO_InitStruct); 

    GPIO_InitStruct.GPIO_Pin = PIN_INPUT_RST;
    GPIO_Init(GPIO_INPUT_RST, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = PIN_ALARM_IN;
    GPIO_Init(GPIO_ALARM_IN, &GPIO_InitStruct); 
    
    GPIO_InitStruct.GPIO_Pin = PIN_INPUT_RSV;
    GPIO_Init(GPIO_INPUT_RSV, &GPIO_InitStruct);     

//OUTPUT
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = PIN_CONV_DIR_P;
    GPIO_Init(GPIO_CONV_DIR_P, &GPIO_InitStruct);
    GPIO_SetBits(GPIO_CONV_DIR_P, PIN_CONV_DIR_P);

    GPIO_InitStruct.GPIO_Pin = PIN_CONV_DIR_N;
    GPIO_Init(GPIO_CONV_DIR_N, &GPIO_InitStruct);  
    GPIO_SetBits(GPIO_CONV_DIR_N, PIN_CONV_DIR_N);

    GPIO_InitStruct.GPIO_Pin = PIN_CONV_SPEED1;
    GPIO_Init(GPIO_CONV_SPEED1, &GPIO_InitStruct); 
    GPIO_SetBits(GPIO_CONV_SPEED1, PIN_CONV_SPEED1); //
    
    GPIO_InitStruct.GPIO_Pin = PIN_CONV_SPEED2;
    GPIO_Init(GPIO_CONV_SPEED2, &GPIO_InitStruct); 
    GPIO_SetBits(GPIO_CONV_SPEED2, PIN_CONV_SPEED2);

    GPIO_InitStruct.GPIO_Pin = PIN_POWER_LIGHT;
    GPIO_Init(GPIO_POWER_LIGHT, &GPIO_InitStruct); 
    GPIO_SetBits(GPIO_POWER_LIGHT, PIN_POWER_LIGHT);
    
    GPIO_InitStruct.GPIO_Pin = PIN_UP_LIGHT;
    GPIO_Init(GPIO_UP_LIGHT, &GPIO_InitStruct);    
    GPIO_SetBits(GPIO_UP_LIGHT, PIN_UP_LIGHT);
    
    GPIO_InitStruct.GPIO_Pin = PIN_DW_LIGHT;
    GPIO_Init(GPIO_DW_LIGHT, &GPIO_InitStruct); 
    GPIO_SetBits(GPIO_DW_LIGHT, PIN_DW_LIGHT);
    
//    GPIO_InitStruct.GPIO_Pin = PIN_CONV_DIR_P_ENABLE;
//    GPIO_Init(GPIO_CONV_DIR_P_ENABLE, &GPIO_InitStruct); 
//    GPIO_SetBits(GPIO_CONV_DIR_P_ENABLE, PIN_CONV_DIR_P_ENABLE);
//    
//    GPIO_InitStruct.GPIO_Pin = PIN_CONV_DIR_N_ENABLE;
//    GPIO_Init(GPIO_CONV_DIR_N_ENABLE, &GPIO_InitStruct);  
//    GPIO_SetBits(GPIO_CONV_DIR_N_ENABLE, PIN_CONV_DIR_N_ENABLE);
    
    GPIO_InitStruct.GPIO_Pin = PIN_RSV_OUT;
    GPIO_Init(GPIO_RSV_OUT, &GPIO_InitStruct); 
    GPIO_SetBits(GPIO_RSV_OUT, PIN_RSV_OUT);    
}
/***************************************************************************************************************************
*@funcName: void sw_inputTask(void)
*@describe: sw的输入任务
*@input:    None
*@return:   None
****************************************************************************************************************************/
void sw_inputTask(void)
{
     static uint32_t last_swInput;
     uint32_t input=0;
    //read input
    if (GPIO_ReadInputDataBit(GPIO_KEY_UP, PIN_KEY_UP) == SET)
    {
       input |= SW_KEY_UP;
    }
    if (GPIO_ReadInputDataBit(GPIO_KEY_DW, PIN_KEY_DW) == SET)
    {
       input |= SW_KEY_DW;
    }
    if (GPIO_ReadInputDataBit(GPIO_KEY_RST, PIN_KEY_RST) == SET)
    {
       input |= SW_KEY_RST;
    }
    if (GPIO_ReadInputDataBit(GPIO_KEY_QSTOP, PIN_KEY_QSTOP) == SET)
    {
       input |= SW_KEY_QSTOP;
    }
    if (GPIO_ReadInputDataBit(GPIO_LIMIT_UP, PIN_LIMIT_UP) == SET)
    {
       input |= SW_INPUT_LIMIT_UP;
    }
    if (GPIO_ReadInputDataBit(GPIO_LIMIT_DW, PIN_LIMIT_DW) == SET)
    {
       input |= SW_INPUT_LIMIT_DW;
    }   
    if (GPIO_ReadInputDataBit(GPIO_INPUT_RST, PIN_INPUT_RST) == SET)
    {
       input |= SW_INPUT_RST;
    }
    if (GPIO_ReadInputDataBit(GPIO_ALARM_IN, PIN_ALARM_IN) == SET)
    {
       input |= SW_INPUT_ALARM;
    }
    if (GPIO_ReadInputDataBit(GPIO_INPUT_RSV, PIN_INPUT_RSV) == SET)
    {
       input |= SW_KEY_RSV;
    }    
    
    input ^= SW_INPUT_ADJUST;  
    if (last_swInput != input) {
        last_swInput = input;
    } else {
        sw_inputReg = input;
    } 
}

/***************************************************************************************************************************
*@funcName: uint32_t get_swInput(void)
*@describe: 获取输入寄存器的值
*@input:    None
*@return:   
        @sw_inputReg:输入寄存器的值
****************************************************************************************************************************/
uint16_t get_swInput(void)
{
    return  sw_inputReg;
}

/***************************************************************************************************************************
*@funcName: void set_swOutput(uint32_t output)
*@describe: 设置开关输出
*@input:    None
*@return:   None
****************************************************************************************************************************/
void sw_outputTask(void)
{
   static uint32_t sw_outputShadow;
  if (sw_outputShadow != sw_outputReg)
   {
//      if (sw_outputReg&SW_CONV_DIR_P_ENABLE) {
//          GPIO_SetBits(GPIO_CONV_DIR_P_ENABLE, PIN_CONV_DIR_P_ENABLE); 
//      }else {
//          GPIO_ResetBits(GPIO_CONV_DIR_P_ENABLE, PIN_CONV_DIR_P_ENABLE); 
//      }        
     
//      if (sw_outputShadow&SW_CONV_DIR_N_ENABLE) {
//          GPIO_SetBits(GPIO_CONV_DIR_N_ENABLE, PIN_CONV_DIR_N_ENABLE); 
//      }else {
//          GPIO_ResetBits(GPIO_CONV_DIR_N_ENABLE, PIN_CONV_DIR_N_ENABLE); 
//      }
      
      if (sw_outputReg&SW_POWER_LIGHT) {
          GPIO_SetBits(GPIO_POWER_LIGHT, PIN_POWER_LIGHT); 
      }else {
          GPIO_ResetBits(GPIO_POWER_LIGHT, PIN_POWER_LIGHT); 
      }
      
      if (sw_outputReg&SW_UP_LIGHT) {
          GPIO_SetBits(GPIO_UP_LIGHT, PIN_UP_LIGHT); 
      }else {
          GPIO_ResetBits(GPIO_UP_LIGHT, PIN_UP_LIGHT); 
      }
      
      if (sw_outputReg&SW_DW_LIGHT) {
          GPIO_SetBits(GPIO_DW_LIGHT, PIN_DW_LIGHT); 
      }else {
          GPIO_ResetBits(GPIO_DW_LIGHT, PIN_DW_LIGHT); 
      }
      
      if (sw_outputReg&SW_CONV_DIR_P) {
          GPIO_SetBits(GPIO_CONV_DIR_P, PIN_CONV_DIR_P);        
      }else {
          GPIO_ResetBits(GPIO_CONV_DIR_P, PIN_CONV_DIR_P);    
      } 
      
      if (sw_outputReg&SW_CONV_DIR_N) {
          GPIO_SetBits(GPIO_CONV_DIR_N, PIN_CONV_DIR_N);        
      }else {
          GPIO_ResetBits(GPIO_CONV_DIR_N, PIN_CONV_DIR_N);    
      }

      if (sw_outputReg&SW_CONV_SPEED1) {
          GPIO_SetBits(GPIO_CONV_SPEED1, PIN_CONV_SPEED1);        
      }else {
          GPIO_ResetBits(GPIO_CONV_SPEED1, PIN_CONV_SPEED1);    
      }  

      if (sw_outputReg&SW_CONV_SPEED2) {
          GPIO_SetBits(GPIO_CONV_SPEED2, PIN_CONV_SPEED2);        
      }else {
          GPIO_ResetBits(GPIO_CONV_SPEED2, PIN_CONV_SPEED2);    
      }  

      if (sw_outputReg&SW_CONV_RSV1) {
          GPIO_SetBits(GPIO_RSV_OUT, PIN_RSV_OUT);        
      }else {
          GPIO_ResetBits(GPIO_RSV_OUT, PIN_RSV_OUT);    
      }  
      
      sw_outputShadow = sw_outputReg;
   }     
}

/***************************************************************************************************************************
*@funcName: uint32_t get_swOutput(void)
*@describe: 获取输出寄存器的值
*@input:    None
*@return:   
      @sw_outputReg:输出寄存器的值
****************************************************************************************************************************/
uint16_t get_swOutput(void)
{
    return sw_outputReg;
}

/***************************************************************************************************************************
*@funcName: void set_swOutput(uint32_t output) 
*@describe: 设置开关输出
*@input:
      @output:输出内容
*@return: None
****************************************************************************************************************************/
void set_swOutput(uint32_t bits)
{
   uint32_t output1=sw_outputReg;
   uint32_t clearBit=~bits, maskBit=SW_OUTPUT_ADJUST&bits;

   output1 &= clearBit; 
   bits ^= maskBit;
   output1 |= bits;
   sw_outputReg = output1;
}

/***************************************************************************************************************************
*@funcName: void set_swOutputBit(uint32_t bit)
*@describe:  设置开关的bit输出
*@input:
        @bit: 输出bit内容
*@return:  None
****************************************************************************************************************************/
void set_swOutputBit(uint32_t bit) //  0010 0011 0001 
{
  set_swOutput(bit);
}

/***************************************************************************************************************************
*@funcName: void clear_swOutputBit(uint32_t bit)
*@describe: 清除特定bit位的输出
*@input:
         @bit: 特定的bit位
*@return:  None
****************************************************************************************************************************/
void clear_swOutputBit(uint32_t bit)
{
    uint32_t output=sw_outputReg;
    uint32_t clearBits=~bit, maskBit=SW_OUTPUT_ADJUST&bit;

    output &= clearBits;
    output |= maskBit;
    sw_outputReg = output;

}


