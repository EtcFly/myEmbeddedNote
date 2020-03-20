/**=============================================================================
* @ModuleName : SERVO_V1.1_PCB
* @FileName   : drv_ad5663r.c
* @ARMVersion : 1.0
* @Author     : Garfield LV
* @Description: AD5663R driver functions
* @Notes      :
* @History    :
* -----------------------------------------------------------------------------
*	Version 1.1 - 2017.11.29 By Garfield Lv
*	[Bug Fix]
*	<Description> xxx.
*	<Modify> xxx;
* -----------------------------------------------------------------------------
*	Version 1.0 - 2017.05.22 By Garfield Lv
*	File created.
*==============================================================================*/

/* Includes ------------------------------------------------------------------*/
#include "drv_ad5663r.h"
#include "delay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************
* @FUNCTION:
*  DRV_AD5663R_SendData
* @DESCRIPTION:
*  AD5663 send data,falling edge lanch the data,use the I/O simulate the SPI
* @PARAMETERS:
*  u32	sendData
* @RETURN:
*  N/A
* @NOTES:
*  N/A
* @HISTORY:
*  Version 1.0 - 2017.11.29 by garfield, Create
******************************************************************************/
void	DRV_AD5663R_SendData(u32 sendData)
{	
	u8 i = 0;
    
	//pull down sync,enable chip
	DA_SERVO_SYNC_Rst();
	
	delay_us(1);
	for (i = 0; i < 24; i++)
	{
		DA_SERVO_SCK_Set();
		
		if((sendData&0x800000)==0x800000)
		{
			DA_SERVO_MOSI_Set();
		}
		else
		{
			DA_SERVO_MOSI_Rst();
		}
		sendData = sendData<<1;
		
		DA_SERVO_SCK_Rst();

		delay_us(1);
	}
	
	//pull up sync,disable chip
	DA_SERVO_SYNC_Set();

}

/******************************************************************************
* @FUNCTION:
*  DRV_AD5663R_Set
* @DESCRIPTION:
*  AD5663 config output,select the chip,and send cmd+address+data.
* @PARAMETERS:
*  id    : DA_SERVO_CHIP or DA_PH_OFFSET_CHIP;
*  com   : AD5663 CMD,ref the datasheet;
*  addr  : AD5663 address, ref the datasheet;
*  dadata: output value,0-65536;
* @RETURN:
*  N/A
* @NOTES:
*  N/A
* @HISTORY:
*  Version 1.0 - 2017.11.29 by garfield, Create
******************************************************************************/
void	DRV_AD5663R_Set(u8 com, u8 addr, u16 dadata)
{
	u32	senddata = 0;

	senddata = AD5663R_ComCombine(com,addr);
	senddata = senddata << 16;
	senddata = senddata | (dadata);

	//Send command
	DRV_AD5663R_SendData(senddata);
}

/******************************************************************************
* @FUNCTION:
*  Drv_AD5563R_Data
* @DESCRIPTION:
*  AD5663 Set out data.
* @PARAMETERS:
*  data of output
* @RETURN:
*  N/A
* @NOTES:
*  N/A
* @HISTORY:
*  Version 1.0 - 2017.11.29 by garfield, Create
******************************************************************************/
void Drv_AD5563R_Data(uint16_t dacData)
{
	dacData = (dacData<<4);/* 这里是ad5623r,12位的所以右移4位 */
	DRV_AD5663R_Set(AD5663R_Cmd_0, AD5663R_ADDR_7, dacData);
}

/******************************************************************************
* @FUNCTION:
*  DRV_AD5663R_Initial
* @DESCRIPTION:
*  AD5663 Initia All CHIP.
* @PARAMETERS:
*  N/A
* @RETURN:
*  N/A
* @NOTES:
*  N/A
* @HISTORY:
*  Version 1.0 - 2017.11.29 by garfield, Create
******************************************************************************/
void	DRV_AD5663R_Initial(void)
{
	Drv_AD5563R_PinConfig();
    //io initial
	DA_SERVO_SYNC_Set();
	DA_SERVO_LDAC_Set();

//	//enable the internal ref
//	DRV_AD5663R_Set(AD5663R_COM_REF,AD5663R_ADDR_7,AD5663R_DATA_INREFON);//需要使用内部电压的时候

    //enalbe software ldac
//    DRV_AD5663R_Set(AD5663R_COM_LDAC,AD5663R_ADDR_7,AD5663R_DATA_LDACON);//需要使用异步更新时
}

/******************************************************************************
* @FUNCTION:
*  DRV_AD5663R_Reset
* @DESCRIPTION:
*  AD5663 reset DAC register and Input register.
* @PARAMETERS:
*  N/A
* @RETURN:
*  N/A
* @NOTES:
*  N/A
* @HISTORY:
*  Version 1.0 - 2017.11.29 by garfield, Create
******************************************************************************/
void	DRV_AD5663R_Reset(void)
{
	DRV_AD5663R_Set(AD5663R_COM_RESET,AD5663R_ADDR_7,0);//复位DAC/input register
}
/******************************************************************************
* @FUNCTION:
*  Drv_AD5563R_PinConfig
* @DESCRIPTION:
*  configuration the AD5663R GPIO,for simulate SPI.
* @PARAMETERS:
*  N/A
* @RETURN:
*  N/A
* @NOTES:
*  N/A
* @HISTORY:
*  Version 1.0 - 2017.11.29 by garfield, Create
******************************************************************************/
void 	Drv_AD5563R_PinConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );

	/* DA_SERVO_CHIP */
	//configure LDAC OUTPUT PP
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Pin = PIN_DA_SERVO_LDAC;
	GPIO_Init(GPIO_DA_SERVO_LDAC, &GPIO_InitStruct);

    //configure SYNC OUTPUT PP
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Pin = PIN_DA_SERVO_SYNC;
	GPIO_Init(GPIO_DA_SERVO_SYNC, &GPIO_InitStruct);
    
	//configure SCK	OUTPUT PP
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Pin = PIN_DA_SERVO_SCK;
	GPIO_Init(GPIO_DA_SERVO_SCK, &GPIO_InitStruct);

	//configure MOSI OUTPUT PP
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Pin = PIN_DA_SERVO_MOSI;
	GPIO_Init(GPIO_DA_SERVO_MOSI, &GPIO_InitStruct);
}

/************************ (C) COPYRIGHT xxx *****END OF FILE****/
