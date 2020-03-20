
#include "stm32f1xx.h"
#include "bsp_sim7600CE.h"
#include "bsp_uart.h"



#define GPIO_SIM7600CE_PWR        GPIOC        
#define GPIO_SIM7600CE_RESET      GPIOC
#define GPIO_SIM7600CE_STATUS     GPIOA

#define PIN_SIM7600CE_PWR         GPIO_PIN_8
#define PIN_SIM7600CE_RESET       GPIO_PIN_9
#define PIN_SIM7600CE_STATUS      GPIO_PIN_15

#define SIM7600_COM               COM1

typedef enum
{
	SOCK_CREATE,
	SOCK_DEL,
	SOCK_CONNECT,
	SOCK_CLOSE,	
}eStatus;

typedef struct ATCmd
{
	eStatus Do;
	char *cmd;
}ATCmd;

ATCmd sim7600ce_Cmd[] = {
	{ SOCK_CREATE, "AT+NETOPEN\r\n"  },  //建立socket连接
	{ SOCK_DEL, "AT+NETCLOSE\r\n" }, //关闭socket连接
	{ SOCK_CONNECT, "AT+CIPOPEN?\r\n" }, //多socket建立模式
	{ SOCK_CLOSE, "AT+CIPOPEN=9,\"TCP\",\"114.55.56.64\",1883\r\n"},
	{ 0, "AT+CIPOPEN=1,\"TCP\",\"117.131.85.139\",5253\r\n"},
	{ 0, "AT+CDNSGIP=\"www.boda-its.com\"\r\n"}
};


static void sim7600_pwrUp(void)
{
	HAL_GPIO_WritePin(GPIO_SIM7600CE_PWR, PIN_SIM7600CE_PWR, GPIO_PIN_SET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(GPIO_SIM7600CE_PWR, PIN_SIM7600CE_PWR, GPIO_PIN_RESET);
}

static void sim7600_pwrDw(void)
{
	HAL_GPIO_WritePin(GPIO_SIM7600CE_PWR, PIN_SIM7600CE_PWR, GPIO_PIN_RESET);
	HAL_Delay(3000);
	HAL_GPIO_WritePin(GPIO_SIM7600CE_PWR, PIN_SIM7600CE_PWR, GPIO_PIN_SET);
}

static void sim7600_reset(void)
{
	HAL_GPIO_WritePin(GPIO_SIM7600CE_RESET, PIN_SIM7600CE_RESET, GPIO_PIN_SET);
	HAL_Delay(200);
	HAL_GPIO_WritePin(GPIO_SIM7600CE_RESET, PIN_SIM7600CE_RESET, GPIO_PIN_RESET);
}


void bsp_sim7600_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	
	GPIO_InitStruct.Pin = PIN_SIM7600CE_PWR;	
	HAL_GPIO_Init(GPIO_SIM7600CE_PWR, &GPIO_InitStruct);	
	
	GPIO_InitStruct.Pin = PIN_SIM7600CE_RESET;
	HAL_GPIO_Init(GPIO_SIM7600CE_RESET, &GPIO_InitStruct);
	
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;	
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pin = PIN_SIM7600CE_STATUS;	
	HAL_GPIO_Init(GPIO_SIM7600CE_STATUS, &GPIO_InitStruct);

	bsp_InitCOM();
	
	sim7600_pwrUp();
	sim7600_reset();	
}

int8_t sim7600ce_isPwrUp(void)
{
	if (HAL_GPIO_ReadPin(GPIO_SIM7600CE_STATUS, PIN_SIM7600CE_STATUS) == 1)
	{
		return 0;
	}
	
	return -1;
}

int8_t sim7600ce_send(uint8_t *s, uint8_t len)
{
	if (ComSend(SIM7600_COM, s, len) == 0)
	{
		return 0;
	}
	
	return -1;
}

int8_t sim7600ce_recv(uint8_t *recv, uint8_t buffLen)
{
	uint8_t *p = recv;
	uint8_t count = 0;
	
	if (0 == recv || buffLen == 0) {
		return 0;
	}
	
	while (ComGet(SIM7600_COM, p) == 1)
	{
		p++;  count ++;
		if (count > buffLen) {
			break;
		}
	}
	return count;
}

#include "libc_string.h"
int8_t sim7600ce_run(uint8_t num)
{
	if (num >= (sizeof(sim7600ce_Cmd)/sizeof(ATCmd)))
	{
		return -1;
	}
	return sim7600ce_send((uint8_t *)sim7600ce_Cmd[num].cmd, libc_strlen(sim7600ce_Cmd[num].cmd));;
}











