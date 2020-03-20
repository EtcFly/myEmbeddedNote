/**********************************************************************************************************
 *@fileName: drv_can.c
 *@platform: MDK5.26.2.0 + STM32L432KB
 *@version:  v1.0.1
 *@describe:  board support packet of can driver  
 *@note: if in the mask filter mode, we must make the msk1 AND mask2 equal zero, otherwise  can't pass
         the mask themself
**********************************************************************************************************
 version          date          Author          modification                                  
----------------------------------------------------------------------------------------------------------
  v1.0.0         180330          wy             Create  file
  v1.0.1         190511          wy             portable the code to stm32f432kb and add the debug support
**********************************************************************************************************/
#include <stm32l4xx.h>
#include "drv_can.h"
#include <string.h>



 /*******************************************Config ************************************************/
#define   MAX_CAN_RX_BUFF_SIZE                        256u
#define   MAX_CAN_TX_BUFF_SIZE                        256u
/*if filter group band is 16bits and in mask mode, the filter id is the hight 4Byte, the mask is Low 4Byte  */
#define   FILTER_ID                                  (0x50u|(0Xf0<<16))
#define   MASK_ID                                    (0X14u|(0X0f<<16))
#define   FILTER_BIT_BAND                             0u  //0 16bit filter   1 32bit filter
#define   FILTER_MODE                                 0u  //1 list filter  0:mask filter
#define   CAN_BAUD_RATE                               CAN500kBaud
#define   SOC_SERIES_STM32L4


/*******************************************Private Define ************************************************/
//TX MAILBOX FLAG
#define CAN1_RCPQ0_FINISH                 0x00000001
#define CAN1_RCPQ1_FINISH                 0x00000100
#define CAN1_RCPQ2_FINISH                 0x00010000
#define CAN1_TXOK0                        0X00000002
#define CAN1_TXOK1                        0X00000200	
#define CAN1_TXOK2                        0X00020000	
#define CAN1_ALST0                        0X00000004
#define CAN1_ALST1                        0X00000400
#define CAN1_ALST2                        0X00040000	
#define CAN1_TERR0                        0X00000008
#define CAN1_TERR1                        0X00000800
#define CAN1_TERR2                        0X00080000
#define CAN1_ABRQ0                        0X00000080
#define CAN1_ABRQ1                        0X00008000
#define CAN1_ABRQ2                        0X00800000

#define CAN1_TX_FINISH		              (CAN1_RCPQ0_FINISH | CAN1_RCPQ1_FINISH | CAN1_RCPQ2_FINISH)
#define CAN1_TX_OK                        (CAN1_TXOK0 | CAN1_TXOK1 | CAN1_TXOK2)	
#define CAN1_ALST                         (CAN1_ALST0 | CAN1_ALST1 | CAN1_ALST2)
#define CAN1_TERR                         (CAN1_TERR0 | CAN1_TERR1 | CAN1_TERR2)
#define CAN1_ABRQ                         (CAN1_ABRQ0 | CAN1_ABRQ1 | CAN1_ABRQ2)

//RX FIFO
#define CAN_RX_FIFO_FINISH                0X0000001B
#define CAN_RX_FIFO_FULL                  0X00000008
#define CAN_RX_FIFO_OVLOAD                0X00000010
#define CAN_RX_FIFO_RECV                  0X00000003

#if FILTER_BIT_BAND
#define CAN_FILTER_ID                    ((FILTER_ID)<<5u) 
#define CAN_MASK_ID                      ((MASK_ID)<<5u)
#else
#define CAN_FILTER_ID                    ((FILTER_ID)<<5u) //if has id2 the expression is id1<<5|id2<<21
#define CAN_MASK_ID                      ((MASK_ID)<<5u)   //if has mask2 the expression is mask1<<5|mask2<<21
#endif

 /*******************************************Private enum Type ************************************************/
enum CAN_Baud
{
    CAN1MBaud = 0,
    CAN800kBaud,
    CAN500kBaud,
    CAN250kBaud,
    CAN125kBaud,
    CAN100kBaud,
    CAN50kBaud,
    CAN20kBaud,
    CAN10kBaud,
};

enum CAN_Mode
{
    NORMAL_MODE,
    SILENT_MODE,
    LOOPBACK_MODE,
    SILENT_LOOPBACK_MODE,
};

struct stm_baud_rate_tab
{
    enum CAN_Baud speed;
    uint32_t cfgValue;
};

struct stm32_can_rx
{
    CAN_RxHeaderTypeDef rxHead;
    uint8_t data[8];
};

struct stm32_can_tx
{
    CAN_TxHeaderTypeDef txHead;
    uint8_t data[8];
};

static const struct stm_baud_rate_tab can_baud_rate_tab[] = //the baud tabl
{
#if defined (SOC_SERIES_STM32F1)
    {CAN1MBaud  , (CAN_SJW_2TQ | CAN_BS1_2TQ  | CAN_BS2_1TQ | (9))},
    {CAN800kBaud, (CAN_SJW_2TQ | CAN_BS1_6TQ  | CAN_BS2_2TQ | (5))},
    {CAN500kBaud, (CAN_SJW_2TQ | CAN_BS1_4TQ  | CAN_BS2_1TQ | (12))},
    {CAN250kBaud, (CAN_SJW_2TQ | CAN_BS1_6TQ  | CAN_BS2_1TQ | (18))},
    {CAN125kBaud, (CAN_SJW_2TQ | CAN_BS1_6TQ  | CAN_BS2_1TQ | (36))},
    {CAN100kBaud, (CAN_SJW_2TQ | CAN_BS1_6TQ  | CAN_BS2_1TQ | (45))},
    {CAN50kBaud , (CAN_SJW_2TQ | CAN_BS1_6TQ  | CAN_BS2_1TQ | (90))},
    {CAN20kBaud , (CAN_SJW_2TQ | CAN_BS1_6TQ  | CAN_BS2_1TQ | (225))},
    {CAN10kBaud , (CAN_SJW_2TQ | CAN_BS1_6TQ  | CAN_BS2_1TQ | (450))}
#elif defined (SOC_SERIES_STM32F4)
    {CAN1MBaud  , (CAN_SJW_2TQ | CAN_BS1_9TQ  | CAN_BS2_4TQ | (3))},
    {CAN800kBaud, (CAN_SJW_2TQ | CAN_BS1_2TQ  | CAN_BS2_1TQ | (13))},
    {CAN500kBaud, (CAN_SJW_2TQ | CAN_BS1_16TQ | CAN_BS2_4TQ | (4))},
    {CAN250kBaud, (CAN_SJW_2TQ | CAN_BS1_6TQ  | CAN_BS2_1TQ | (21))},
    {CAN125kBaud, (CAN_SJW_2TQ | CAN_BS1_6TQ  | CAN_BS2_1TQ | (42))},
    {CAN100kBaud, (CAN_SJW_2TQ | CAN_BS1_7TQ  | CAN_BS2_2TQ | (42))},
    {CAN50kBaud , (CAN_SJW_2TQ | CAN_BS1_6TQ  | CAN_BS2_1TQ | (105))},
    {CAN20kBaud , (CAN_SJW_2TQ | CAN_BS1_11TQ | CAN_BS2_3TQ | (140))},
    {CAN10kBaud , (CAN_SJW_2TQ | CAN_BS1_6TQ  | CAN_BS2_1TQ | (525))}
#elif defined (SOC_SERIES_STM32L4)
    {CAN1MBaud  , (CAN_SJW_2TQ | CAN_BS1_2TQ  | CAN_BS2_1TQ | (20))},
    {CAN800kBaud, (CAN_SJW_2TQ | CAN_BS1_2TQ  | CAN_BS2_1TQ | (25))},
    {CAN500kBaud, (CAN_SJW_2TQ | CAN_BS1_3TQ  | CAN_BS2_1TQ | (32))},
    {CAN250kBaud, (CAN_SJW_2TQ | CAN_BS1_6TQ  | CAN_BS2_1TQ | (40))},
    {CAN125kBaud, (CAN_SJW_2TQ | CAN_BS1_13TQ | CAN_BS2_2TQ | (40))},
    {CAN100kBaud, (CAN_SJW_2TQ | CAN_BS1_6TQ  | CAN_BS2_1TQ | (100))},
    {CAN50kBaud , (CAN_SJW_2TQ | CAN_BS1_6TQ  | CAN_BS2_1TQ | (200))},
    {CAN20kBaud , (CAN_SJW_2TQ | CAN_BS1_13TQ | CAN_BS2_2TQ | (250))},
    {CAN10kBaud , (CAN_SJW_2TQ | CAN_BS1_7TQ  | CAN_BS2_2TQ | (801))}
#endif
};




/**********************************************************************************************************
*                              				Private Typedef
**********************************************************************************************************/
typedef struct
{
    uint16_t   rx_rIndex;
    uint16_t   rx_wIndex;
    uint16_t   rx_dataNum;
    struct stm32_can_rx  rx_fifo[MAX_CAN_RX_BUFF_SIZE];

    uint16_t   tx_rIndex;
    uint16_t   tx_wIndex;
    uint16_t   tx_dataNum;
    struct stm32_can_tx  tx_fifo[MAX_CAN_TX_BUFF_SIZE];
} CAN_MangeTypeDef;

typedef struct
{
	//tx flag
	uint32_t txOK;
	uint32_t alst; //bus arbitration lost
	uint32_t txErr;
	uint32_t abrq;
	uint32_t tec;
	//rx status
	uint32_t rxOK;
	uint32_t rx_overflow;
	uint32_t rx_full;
	uint32_t rec;
	//sys status
	uint32_t bus_off;
	uint32_t err_passive;
	uint32_t bad_env; 
	//error code
	uint32_t last_errCode;
}CAN_StatusTypeDef;

typedef struct
{
	struct stm32_can_tx *s_pCurrTxData;
}CAN_TxCache_TypeDef;

typedef struct
{
	struct stm32_can_rx data;
}CAN_RXCache_TypeDef;


/**********************************************************************************************************
*                              				Private variables
**********************************************************************************************************/
CAN_MangeTypeDef     s_can_txRxMange; //发送接收管理单元
CAN_RXCache_TypeDef  s_currRxBuff[2]; //CAN rx Temporary cache
CAN_HandleTypeDef    can_handleStruct;
CAN_TxCache_TypeDef  s_currTxBuff = { .s_pCurrTxData = NULL, }; //CAN tx Temporary cache
#ifdef CAN_DEBUG_ENABLE
CAN_StatusTypeDef    s_can_StatusCtn; //can module status
#endif



/**
 * @brief write data to tx fifo 
 * @para: pData is point to the data we need to send 
 * @return: none
 * @note: none
 */
static int8_t CAN_WriteTxFifo(struct stm32_can_tx *pData)
{
	if (s_can_txRxMange.tx_dataNum == MAX_CAN_TX_BUFF_SIZE)
	{
		return -1; //tx buff is full
	}
	s_can_txRxMange.tx_wIndex %= MAX_CAN_TX_BUFF_SIZE;
	s_can_txRxMange.tx_fifo[s_can_txRxMange.tx_wIndex++] = *pData;
	
	__HAL_CAN_DISABLE_IT(&can_handleStruct, CAN_IT_TX_MAILBOX_EMPTY);
	s_can_txRxMange.tx_dataNum++;
	__HAL_CAN_ENABLE_IT(&can_handleStruct, CAN_IT_TX_MAILBOX_EMPTY);
	return 0;
}

/**
 * @brief write data to rx fifo 
 * @para: pData is point to the data we need to receive 
 * @return: none
 * @note: none
 */
static void CAN_WriteRxFifo(struct stm32_can_rx *pData)
{
	s_can_txRxMange.rx_wIndex %= MAX_CAN_RX_BUFF_SIZE;
	s_can_txRxMange.rx_fifo[s_can_txRxMange.rx_wIndex++] = *pData;
	
	__HAL_CAN_DISABLE_IT(&can_handleStruct, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO0_FULL | CAN_IT_RX_FIFO0_OVERRUN | 
	                                        CAN_IT_RX_FIFO1_MSG_PENDING | CAN_IT_RX_FIFO1_FULL | CAN_IT_RX_FIFO1_OVERRUN);
	s_can_txRxMange.rx_dataNum++;
	__HAL_CAN_ENABLE_IT(&can_handleStruct, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO0_FULL | CAN_IT_RX_FIFO0_OVERRUN | 
	                                        CAN_IT_RX_FIFO1_MSG_PENDING | CAN_IT_RX_FIFO1_FULL | CAN_IT_RX_FIFO1_OVERRUN);
}


/**
 * @brief read the tx fifo of can
 * 
 * @para: pData is point to a type of struct stm32_can_tx point and return the transmit data address
 * @return: if tx buff is empty and then return -1(show no data need to be send)  
 * @note: none
 */
static int8_t CAN_ReadTxFifo(struct stm32_can_tx **pData)
{
	pData = pData;
	if (!s_can_txRxMange.tx_dataNum)
	{
		*pData = NULL;
		return -1; //tx buff is empty
	}
	
	__HAL_CAN_DISABLE_IT(&can_handleStruct, CAN_IT_TX_MAILBOX_EMPTY);
	s_can_txRxMange.tx_dataNum--;
	__HAL_CAN_ENABLE_IT(&can_handleStruct, CAN_IT_TX_MAILBOX_EMPTY);
	
	s_can_txRxMange.tx_rIndex %= MAX_CAN_TX_BUFF_SIZE;
	*pData = &s_can_txRxMange.tx_fifo[s_can_txRxMange.tx_rIndex++];
	return 0;
}


/**
 * @brief read the rx fifo of can
 * 
 * @para: pData is point to a type of struct stm32_can_rx point and return the receive data address
 * @return: if rx buff is empty and then return -1(show receive fail) if rx buff is overflow return 1 
 * @note: none
 */
static int8_t CAN_ReadRxFifo(struct stm32_can_rx **pData)
{
	pData = pData;

	if (!s_can_txRxMange.rx_dataNum)
	{
		*pData = NULL;
		return -1; //rx buff is empty
	}
	
	__HAL_CAN_DISABLE_IT(&can_handleStruct, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO0_FULL | CAN_IT_RX_FIFO0_OVERRUN | 
	                                        CAN_IT_RX_FIFO1_MSG_PENDING | CAN_IT_RX_FIFO1_FULL | CAN_IT_RX_FIFO1_OVERRUN);
	s_can_txRxMange.rx_dataNum--;
	__HAL_CAN_ENABLE_IT(&can_handleStruct, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO0_FULL | CAN_IT_RX_FIFO0_OVERRUN | 
	                                       CAN_IT_RX_FIFO1_MSG_PENDING | CAN_IT_RX_FIFO1_FULL | CAN_IT_RX_FIFO1_OVERRUN);
	
	s_can_txRxMange.rx_rIndex %= MAX_CAN_RX_BUFF_SIZE;
	*pData = &s_can_txRxMange.rx_fifo[s_can_txRxMange.rx_rIndex++];
	

	if (s_can_txRxMange.rx_dataNum+1 >= MAX_CAN_RX_BUFF_SIZE)
	{
		return 1; //rx buff is overflow
	}
	return 0; //normal receive
}

/**
 * @brief set the debug support of can
 * 
 * @para: none
 * @note: none
 */
static void can_set_debug_active(void)
{
    CAN1->MCR &= ~(0X01 << 16);
}


/**
 * @brief enable the IRQ
 * 
 * @para: none
 * @note: none
 */
static void can_setIRQEnable(void)
{
    HAL_NVIC_SetPriority(CAN1_TX_IRQn, 0, 1);
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 2);
    HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 0, 3);
	
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
	
#ifdef CAN_ERROR_INTERRUPT_ENABLE	
    HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 1, 3);
    HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
#endif
}

/**
 * @brief it's achieve the low lever send function
 * 
 * @para: tx_fifo is the value we need to send
 * @return  -1:tx mailbox is full   0:send ok
 * @note: none
 */
static int8_t can_hw_send(CAN_TxCache_TypeDef *tx_fifo)
{
    uint32_t tmp;
	register struct stm32_can_tx *tx_head = (struct stm32_can_tx*)(void *)(tx_fifo->s_pCurrTxData);
	uint32_t mailbox;
	
	tmp = HAL_CAN_GetTxMailboxesFreeLevel(&can_handleStruct);
	if (0 == tmp)
	{
		return -1;
	}
	
	if (HAL_ERROR == HAL_CAN_AddTxMessage(&can_handleStruct, &tx_head->txHead, &tx_head->data[0], &mailbox))
	{
		return -1;
	}
	return mailbox;	
}

/**
 * @brief it's achieve the low lever read function
 * 
 * @para: rx_fifo is the buff to storage current receive 
 *        mailBox is decide which the information from 
 * @return  -1:tx mailbox is full   0:send ok
 * @note: none
 */
static void can_hw_read(CAN_RXCache_TypeDef *rx_fifo, uint8_t mailBox)
{
	uint8_t *pData = (uint8_t *)(void *)(&rx_fifo->data.data[0]);
	
	if (HAL_OK == HAL_CAN_GetRxMessage(&can_handleStruct, mailBox, &rx_fifo->data.rxHead, pData))
	{
		return ;
	}
}
/**
 * @brief config the gpio mode of can1
 * 
 * @para  none
 * @note: none
 */
static void CAN_GPIOConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; 
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
	
	/*  CAN_TX ----------> PA12   */
	/*  CAN_RX ----------> PA11   */	
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); //CAN_TX

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); //CAN_RX
}

/**
 * @brief the hardware init of can1
 * 
 * @para  none
 * @note: none
 */
void bsp_canInit(void)
{
    __HAL_RCC_CAN1_CLK_ENABLE();
    CAN_GPIOConfig();

    can_set_debug_active();
	can_handleStruct.Init.AutoBusOff = ENABLE;
	can_handleStruct.Init.AutoRetransmission = ENABLE; 
	can_handleStruct.Init.AutoWakeUp = DISABLE;
	can_handleStruct.Init.Mode = CAN_MODE_NORMAL;
	can_handleStruct.Init.Prescaler = can_baud_rate_tab[CAN_BAUD_RATE].cfgValue&(0x3FF);
	can_handleStruct.Init.ReceiveFifoLocked = DISABLE;
	can_handleStruct.Init.SyncJumpWidth = can_baud_rate_tab[CAN_BAUD_RATE].cfgValue&(0X03<<24);
	can_handleStruct.Init.TimeSeg1 = can_baud_rate_tab[CAN_BAUD_RATE].cfgValue&(0X0F<<16);
	can_handleStruct.Init.TimeSeg2 = can_baud_rate_tab[CAN_BAUD_RATE].cfgValue&(0X07<<20);
	can_handleStruct.Init.TimeTriggeredMode = DISABLE;
	can_handleStruct.Init.TransmitFifoPriority = DISABLE;
	can_handleStruct.Instance = CAN1;
	if (HAL_OK != HAL_CAN_Init(&can_handleStruct))
	{
		assert_param(0);
	}
	
	CAN_FilterTypeDef filterStruct;
	filterStruct.FilterActivation = CAN_FILTER_ENABLE;
	filterStruct.FilterBank = 0; //filter group 0
	filterStruct.FilterFIFOAssignment = CAN_FILTER_FIFO0;
#if FILTER_BIT_BAND
	filterStruct.FilterScale = CAN_FILTERSCALE_32BIT;
	filterStruct.FilterIdHigh = (CAN_FILTER_ID<<21);
	filterStruct.FilterIdLow =0x00;
	filterStruct.FilterMaskIdHigh = (CAN_MASK_ID<<21);
	filterStruct.FilterMaskIdLow = 0X00;
#else
	filterStruct.FilterScale = CAN_FILTERSCALE_16BIT;
	filterStruct.FilterIdHigh = (CAN_FILTER_ID>>16);
	filterStruct.FilterIdLow = (CAN_FILTER_ID&0X0FFFF);
	filterStruct.FilterMaskIdHigh = (CAN_MASK_ID>>16);
	filterStruct.FilterMaskIdLow = (CAN_MASK_ID&0X0FFFF);
#endif
	
#if FILTER_MODE
	filterStruct.FilterMode = CAN_FILTERMODE_IDLIST;
#else
	filterStruct.FilterMode = CAN_FILTERMODE_IDMASK;
#endif
	
	if (HAL_OK != HAL_CAN_ConfigFilter(&can_handleStruct, &filterStruct))
	{
		assert_param(0);
	}
	HAL_CAN_ActivateNotification(&can_handleStruct, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO0_FULL | CAN_IT_RX_FIFO0_OVERRUN |
	                                                CAN_IT_RX_FIFO1_MSG_PENDING | CAN_IT_RX_FIFO1_FULL | CAN_IT_RX_FIFO1_OVERRUN | 
													CAN_IT_TX_MAILBOX_EMPTY);
#ifdef CAN_ERROR_INTERRUPT_ENABLE		
	HAL_CAN_ActivateNotification(&can_handleStruct, CAN_IT_ERROR_WARNING | CAN_IT_BUSOFF | CAN_IT_ERROR_PASSIVE |
	                                                CAN_IT_LAST_ERROR_CODE | CAN_IT_ERROR );
#endif								
	can_setIRQEnable();
	if (HAL_OK != HAL_CAN_Start(&can_handleStruct))
	{
		HAL_CAN_Init(&can_handleStruct);
		HAL_CAN_ConfigFilter(&can_handleStruct, &filterStruct);
		HAL_CAN_Start(&can_handleStruct);
	}
}

/**
 * @brief provide the application interface to user, and achieve the application lever data send to low lever
 * 
 * @para  send is point to the application data
 * @return if the tx fifo is full and return -1  and if send success return 0
 * @note: none
 */
int8_t bsp_canSend(CAN_Dev_Typedef *send)
{
	struct stm32_can_tx tx;
	CAN_TxCache_TypeDef tx_tmp;
	int8_t retV;
	
	tx.txHead.StdId = send->dev_id;
	tx.txHead.ExtId = 0X00;
	tx.txHead.IDE = CAN_ID_STD;
	tx.txHead.RTR = CAN_RTR_DATA;
	tx.txHead.DLC = send->data_len;
	tx.txHead.TransmitGlobalTime = DISABLE;
	memcpy(tx.data, &send->data[0], send->data_len);
	
	retV = -1;
	if (-1 != CAN_WriteTxFifo(&tx))
	{

		if (-1 != CAN_ReadTxFifo(&tx_tmp.s_pCurrTxData))
		{
			__HAL_CAN_DISABLE_IT(&can_handleStruct, CAN_IT_TX_MAILBOX_EMPTY);			
			retV = can_hw_send(&tx_tmp);
			__HAL_CAN_DISABLE_IT(&can_handleStruct, CAN_IT_TX_MAILBOX_EMPTY);
		}
	}
	return retV;
}

/**
 * @brief provide the application interface to user, and achieve the low lever data receive 
 * 
 * @para  rx is point to the rx buff and storage the receive data
 * @return if the buff is  empty  return -1 and else return 0
 * @note: none
 */
int8_t bsp_canReceive(CAN_Dev_Typedef *pRx_buff)
{
	struct stm32_can_rx *rx;
	if (-1 == CAN_ReadRxFifo(&rx))
	{
		return -1; //the can buff is empty
	}
	
	pRx_buff->data_len = rx->rxHead.DLC;
	pRx_buff->dev_id = rx->rxHead.StdId; 
	memcpy(pRx_buff->data, rx->data, pRx_buff->data_len);
	return 0;
}

/**
 * @brief the handler of tx 
 * 
 * @para none
 * @note: none
 */
void CAN1_TX_IRQHandler(void)
{
	volatile register uint32_t irqTmp, tmp;
	
	irqTmp = CAN1->TSR;	
	switch (irqTmp & CAN1_TX_FINISH)
	{
	case CAN1_RCPQ0_FINISH:
	{
#ifdef CAN_DEBUG_ENABLE
		if (irqTmp&CAN1_TXOK0) s_can_StatusCtn.txOK++;	
		if (irqTmp & (CAN1_ALST0 | CAN1_TERR0 | CAN1_ABRQ0))
		{
			if (irqTmp&CAN1_ALST0) s_can_StatusCtn.alst++;
			if (irqTmp&CAN1_TERR0) s_can_StatusCtn.txErr++;
			if (irqTmp&CAN1_ABRQ0) s_can_StatusCtn.abrq++;
		}
#endif
		tmp = CAN1_RCPQ0_FINISH;
		break;
	}
	case CAN1_RCPQ1_FINISH:
	{
	#ifdef CAN_DEBUG_ENABLE
		if (irqTmp&CAN1_TXOK1) s_can_StatusCtn.txOK++;	
		if (irqTmp & (CAN1_ALST1 | CAN1_TERR1 | CAN1_ABRQ1))
		{
			if (irqTmp&CAN1_ALST1) s_can_StatusCtn.alst++;
			if (irqTmp&CAN1_TERR1) s_can_StatusCtn.txErr++;
			if (irqTmp&CAN1_ABRQ1) s_can_StatusCtn.abrq++;
		}
	#endif
		tmp = CAN1_RCPQ1_FINISH;
		break;
	}
	case CAN1_RCPQ2_FINISH:
	{
	#ifdef CAN_DEBUG_ENABLE
		if (irqTmp&CAN1_TXOK2) s_can_StatusCtn.txOK++;	
		if (irqTmp & (CAN1_ALST2 | CAN1_TERR2 | CAN1_ABRQ2))
		{
			if (irqTmp&CAN1_ALST2) s_can_StatusCtn.alst++;
			if (irqTmp&CAN1_TERR2) s_can_StatusCtn.txErr++;
			if (irqTmp&CAN1_ABRQ2) s_can_StatusCtn.abrq++;
		}
	#endif
		tmp = CAN1_RCPQ2_FINISH;
		break;
	}
	default :
		//多个邮箱同时Finish
		tmp = CAN1_TX_FINISH & irqTmp;
		break;
	}
#ifdef CAN_DEBUG_ENABLE
	if (irqTmp & ((CAN1_ALST0|CAN1_ALST1|CAN1_ALST2) | 
				  (CAN1_TERR0|CAN1_TERR1|CAN1_TERR2) | 
				  (CAN1_ABRQ0|CAN1_ABRQ1|CAN1_ABRQ2)))
	{
		s_can_StatusCtn.tec = (CAN1->ESR>>16)&0XFF;
	}
#endif	
	
	if (-1 != CAN_ReadTxFifo(&s_currTxBuff.s_pCurrTxData))
	{
		can_hw_send(&s_currTxBuff);
	}
	CAN1->TSR |= tmp;
}


/**
 * @brief the handler of rx0 
 * 
 * @para none
 * @note: none
 */
void CAN1_RX0_IRQHandler(void)
{	
    switch (CAN1->RF0R & CAN_RX_FIFO_FINISH)
    {
    case CAN_RX_FIFO_FULL: //给出错误代码
    {
#ifdef CAN_DEBUG_ENABLE
		s_can_StatusCtn.rx_full++;
		s_can_StatusCtn.rec = (CAN1->ESR>>24)&0xff;
#endif
		break;
    }
    case CAN_RX_FIFO_OVLOAD: //给出错误代码
    {
#ifdef CAN_DEBUG_ENABLE
		s_can_StatusCtn.rec = (CAN1->ESR>>24)&0xff;
		s_can_StatusCtn.rx_overflow++;
#endif
        break;
    }
    case 0X00000003: 
    case 0X00000002:
    case 0X00000001:
    {
#ifdef CAN_DEBUG_ENABLE
		s_can_StatusCtn.rxOK++;
#endif
		break;
    }
    default:
        break;
    }
	can_hw_read(&s_currRxBuff[0], 0);
	
	__HAL_CAN_DISABLE_IT(&can_handleStruct, CAN_IT_RX_FIFO1_MSG_PENDING|CAN_IT_RX_FIFO1_FULL|CAN_IT_RX_FIFO1_OVERRUN);//DISABLE RX1
	CAN_WriteRxFifo(&s_currRxBuff[0].data);
	__HAL_CAN_ENABLE_IT(&can_handleStruct, CAN_IT_RX_FIFO1_MSG_PENDING|CAN_IT_RX_FIFO1_FULL|CAN_IT_RX_FIFO1_OVERRUN); //ENABLE RX1
}

/**
 * @brief the handler of rx1 
 * 
 * @para none
 * @note: none
 */
void CAN1_RX1_IRQHandler(void)
{
    switch (CAN1->RF1R & CAN_RX_FIFO_FINISH)
    {
    case CAN_RX_FIFO_FULL: //给出错误代码
    {
#ifdef CAN_DEBUG_ENABLE
		s_can_StatusCtn.rx_full++;
#else
		s_can_StatusCtn.rec = (CAN1->ESR>>24)&0xff;
#endif
		
		break;
    }
    case CAN_RX_FIFO_OVLOAD: //给出错误代码
    {
#ifdef CAN_DEBUG_ENABLE
		s_can_StatusCtn.rx_overflow++;
#else
		s_can_StatusCtn.rec = (CAN1->ESR>>24)&0xff;	
#endif
        break;
    }
    case 0X00000003: 
    case 0X00000002:
    case 0X00000001:
    {
#ifdef CAN_DEBUG_ENABLE
		s_can_StatusCtn.rxOK++;
#endif
		break;
    }
    default:
        break;
    }
	can_hw_read(&s_currRxBuff[1], 1);
	
	__HAL_CAN_DISABLE_IT(&can_handleStruct, CAN_IT_RX_FIFO0_MSG_PENDING|CAN_IT_RX_FIFO0_FULL|CAN_IT_RX_FIFO0_OVERRUN);//DISABLE RX0
	CAN_WriteRxFifo(&s_currRxBuff[1].data);
	__HAL_CAN_ENABLE_IT(&can_handleStruct, CAN_IT_RX_FIFO0_MSG_PENDING|CAN_IT_RX_FIFO0_FULL|CAN_IT_RX_FIFO0_OVERRUN);
}

/**
 * @brief the handler of error  
 * 
 * @para none
 * @note: none
 */
#ifdef CAN_ERROR_INTERRUPT_ENABLE
void CAN1_SCE_IRQHandler(void) //STATUS CHANGE IRQ
{
	register uint32_t tmp;
	tmp = CAN1->ESR;
	
#ifdef CAN_DEBUG_ENABLE	
	if (tmp & 0x07)
	{
		if (tmp & 0X01)	s_can_StatusCtn.bad_env++;
		if (tmp & 0X02)	s_can_StatusCtn.err_passive++;
		if (tmp & 0X04)	s_can_StatusCtn.bus_off++;
	}
	s_can_StatusCtn.last_errCode = (tmp>>4)&0X07;
	s_can_StatusCtn.rec = (tmp>>24) & 0x0ff;
	s_can_StatusCtn.tec = (tmp>>16) & 0x0ff;
#else
	UNUSED(tmp);
#endif
	__HAL_CAN_CLEAR_FLAG(&can_handleStruct, CAN_FLAG_ERRI);
	__HAL_CAN_CLEAR_FLAG(&can_handleStruct, CAN_FLAG_WKU);
}
#endif

/*******************************************End of File************************************************/
