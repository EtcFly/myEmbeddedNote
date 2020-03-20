/**********************************************************************************************************
 *@fileName: drv_dm9051np.c
 *@platform: 
 *@version:  v1.0.0
 *@describe: Achieve DM9051NP Network Card Driver
 *@note:
**********************************************************************************************************
 version          date          Author          modification                                  
----------------------------------------------------------------------------------------------------------
  v1.0.0         1901125        EtcFly          Create  file
**********************************************************************************************************/
#include "string.h"
#include "stdio.h"
#include "stm32l4xx_hal.h"
#include "netif/etharp.h"
#include "netif/etharp.h"
#include "drv_dm9051np.h"
#include "dm9051_port.h"

DM9051_Init_t init = {
	.mode = DM9051_AUTO,
	.imr = IMR_OFF,//IMR_PAR | IMR_PRM ,//| IMR_PTM,
};
static ChipInfo_t info;

#ifdef DM9051_PACKET_STATUS_CHECK
Packet_Info_t pkt_info;
#endif


#define DM9051_PHY              (0x01 << 6)    /* PHY address 0x01 */


static uint8_t recv_flag;

static void set_recvFlag(void)
{
	recv_flag = 1;
}

int8_t get_recvFlag(void)
{
	uint8_t flag = recv_flag;
	recv_flag = 0;
	
	return flag;	
}


static uint8_t DM9051_write_reg(uint8_t cmd, uint8_t data)
{
	uint8_t retV;
	
	dm9051_port.chip_cs_active();
	
	dm9051_port.spi_rw(CMD_REG_WR(cmd), &retV);
	dm9051_port.spi_rw(data, &retV);
	
	dm9051_port.chip_cs_inactive();
	return retV;
}

static uint8_t DM9051_read_reg(uint8_t cmd)
{
	uint8_t retV;
	
	dm9051_port.chip_cs_active();
	
	dm9051_port.spi_rw(CMD_REG_RD(cmd), &retV);
	dm9051_port.spi_rw(0x00, &retV);

	dm9051_port.chip_cs_inactive();
	return retV;
}

static uint8_t DM9051_write_mem(const uint8_t *buff, uint16_t buff_len)
{
	uint8_t retV;
	
	dm9051_port.chip_cs_active();
	
	dm9051_port.spi_rw(CMD_MEM_WR, &retV);
	for (uint16_t i = 0; i < buff_len; i++)
	{
		dm9051_port.spi_rw(buff[i], &retV);	
	}
	
	dm9051_port.chip_cs_inactive();
	return 0;
}

static uint8_t DM9051_read_mem(uint8_t *buff, uint16_t recv_len)
{
	uint8_t retV;

	dm9051_port.chip_cs_active();
	
	dm9051_port.spi_rw(CMD_MEM_RD, &retV);
	for (uint16_t i = 0; i < recv_len; i++)
	{
		dm9051_port.spi_rw(0X00, &buff[i]);	
	}
	
	dm9051_port.chip_cs_inactive();
	return 0;
}

static int8_t DM9051_phy_read(uint8_t reg, uint16_t *recv)
{
	uint8_t value = (reg & 0x3f) | DM9051_PHY;
	uint16_t timeout = 1000;

	DM9051_write_reg(DM9051_EPAR, value);
	DM9051_write_reg(DM9051_EPCR, 0xc);           /* Issue phyxcer read command                               */

	while((DM9051_read_reg(DM9051_EPCR) & 0x1) && timeout-- > 0) 
	{
		dm9051_port.delay_ms(1);
	}

	if (timeout == 0)
	{
		return -1;
	}

  	*recv = (DM9051_read_reg(DM9051_EPDRH) << 8) | DM9051_read_reg(DM9051_EPDRL);
	return 0;
}

static int8_t DM9051_phy_write(uint8_t reg, uint16_t data)
{
	uint8_t value = (reg & 0x3f) | DM9051_PHY;
	uint16_t timeout = 1000;

	DM9051_write_reg(DM9051_EPAR, value);
	/* Fill the written data into REG_0D & REG_0E                                                           */
	DM9051_write_reg(DM9051_EPDRL, (data & 0xff));
	DM9051_write_reg(DM9051_EPDRH, ((data >> 8) & 0xff));
	DM9051_write_reg(DM9051_EPCR, 0xa);           /* Issue phyxcer read command                               */

	while((DM9051_read_reg(DM9051_EPCR) & 0x1) && timeout-- > 0) 
	{
		dm9051_port.delay_ms(2);
	}

	if (timeout == 0)
	{
		return -1;
	}
	return 0;
}

static int8_t DM9051_eeprom_write(uint8_t word_addr, void *pdata, uint8_t len)
{
	if (pdata == NULL || len == 0)
	{
		return -1;
	}

	uint8_t value = (word_addr & 0x3f) | DM9051_PHY;
	uint8_t *p = (uint8_t *)pdata;

	DM9051_write_reg(DM9051_EPCR, 0x10); //reload eeprom	
	
	uint16_t timeout = 1000;
	for (uint8_t i = 0; i < len; i++)
	{
		DM9051_write_reg(DM9051_EPAR, value+i);
		DM9051_write_reg(DM9051_EPDRL, p[2*i]);
		DM9051_write_reg(DM9051_EPDRH, p[2*i+1]);	
		DM9051_write_reg(DM9051_EPCR, 0x12);           /* Issue phyxcer write command                               */
		
		while((DM9051_read_reg(DM9051_EPCR) & 0x2) && timeout-- > 0) 
		{
			dm9051_port.delay_ms(2);
		}
		
		if (timeout == 0)
		{
			return -1;
		}
	}
	
	timeout = 1000;
	while (DM9051_read_reg(DM9051_EPCR) & 0x1 && timeout-- > 0)
	{
		dm9051_port.delay_ms(2);
	}
	
	if (timeout == 0)
	{
		return -1;
	}
	
	DM9051_write_reg(DM9051_EPCR, 0x00);
	dm9051_port.delay_ms(2); //error occurs when the write operation reads immediately
	
	return 0;
}

static int8_t DM9051_eeprom_read(uint8_t word_addr, void *pdata, uint8_t len)
{
	if (pdata == NULL || len == 0)
	{
		return -1;
	}

	uint8_t value = (word_addr & 0x3f) | DM9051_PHY;
	uint8_t *p = (uint8_t *)pdata;

	DM9051_write_reg(DM9051_EPCR, 0x00); //reload eeprom
	
	uint16_t timeout = 1000;
	for (uint8_t i = 0; i < len; i++)
	{			
		DM9051_write_reg(DM9051_EPAR, value+i);
		DM9051_write_reg(DM9051_EPCR, 0x04);           /* Issue phyxcer read command                               */
		while((DM9051_read_reg(DM9051_EPCR) & 0x4) && timeout-- > 0) 
		{
			dm9051_port.delay_ms(2);
		}
		
		if (timeout == 0)
		{
			return -1;
		}

		p[2*i]   = DM9051_read_reg(DM9051_EPDRL);
		p[2*i+1] = DM9051_read_reg(DM9051_EPDRH);	
	}
	
	return 0;
}



static void DM9051_setMacAddr(uint8_t mac[6])
{
	DM9051_write_reg(DM9051_PHYADDR1, mac[0]);	
	DM9051_write_reg(DM9051_PHYADDR2, mac[1]);	
	DM9051_write_reg(DM9051_PHYADDR3, mac[2]);	
	DM9051_write_reg(DM9051_PHYADDR4, mac[3]);	
	DM9051_write_reg(DM9051_PHYADDR5, mac[4]);	
	DM9051_write_reg(DM9051_PHYADDR6, mac[5]);	
}

static int8_t DM9051_getMacAddr(uint8_t *mac)
{
	if (mac == NULL)
	{
		return -1;
	}

	mac[0] = DM9051_read_reg(DM9051_PHYADDR1);
	mac[1] = DM9051_read_reg(DM9051_PHYADDR2);
	mac[2] = DM9051_read_reg(DM9051_PHYADDR3);
	mac[3] = DM9051_read_reg(DM9051_PHYADDR4);
	mac[4] = DM9051_read_reg(DM9051_PHYADDR5);
	mac[5] = DM9051_read_reg(DM9051_PHYADDR6);
	
	return 0;
}

static void DM9051_setMultiCast_addr(uint8_t multiCast_addr[8])
{
	/* set multicast address      */
	DM9051_write_reg(DM9051_MULTICAST_ADDR1, multiCast_addr[0]);
	DM9051_write_reg(DM9051_MULTICAST_ADDR2, multiCast_addr[1]);
	DM9051_write_reg(DM9051_MULTICAST_ADDR3, multiCast_addr[2]);
	DM9051_write_reg(DM9051_MULTICAST_ADDR4, multiCast_addr[3]);
	DM9051_write_reg(DM9051_MULTICAST_ADDR5, multiCast_addr[4]);
	DM9051_write_reg(DM9051_MULTICAST_ADDR6, multiCast_addr[5]);
	DM9051_write_reg(DM9051_MULTICAST_ADDR7, multiCast_addr[6]);
	DM9051_write_reg(DM9051_MULTICAST_ADDR8, multiCast_addr[7]);
}

#if 0
static void DM9051_getMultiCast_addr(uint8_t multi_addr[8])
{
	multi_addr[0] = DM9051_read_reg(DM9051_MULTICAST_ADDR1);
	multi_addr[1] = DM9051_read_reg(DM9051_MULTICAST_ADDR2);
	multi_addr[2] = DM9051_read_reg(DM9051_MULTICAST_ADDR3);
	multi_addr[3] = DM9051_read_reg(DM9051_MULTICAST_ADDR4);
	multi_addr[4] = DM9051_read_reg(DM9051_MULTICAST_ADDR5);
	multi_addr[5] = DM9051_read_reg(DM9051_MULTICAST_ADDR6);
	multi_addr[6] = DM9051_read_reg(DM9051_MULTICAST_ADDR7);
	multi_addr[7] = DM9051_read_reg(DM9051_MULTICAST_ADDR8);
}
#endif

/*********************************************************************************************************//**
  * @brief  Set PHY mode.
  * @param  uMediaMode:
  *         @DM9051_AUTO Auto negotiation
  *         @DM9051_10MHD 10MHz, Half duplex
  *         @DM9051_10MFD 10MHz, Full duplex
  *         @DM9051_100MHD 100MHz, Half duplex
  *         @DM9051_100MFD 100MHz, Full duplex
  * @retval None
  ***********************************************************************************************************/
static void DM9051_set_phy_mode(enum DM9051_PHY_mode uMediaMode)
{
	uint16_t phy_reg4 = 0x01e1;
	uint16_t phy_reg0 = 0x1000; //enable auto negotiation

	switch (uMediaMode)
	{
	  case DM9051_10MHD:  { phy_reg4 = 0x021; phy_reg0 = 0x0000; break;  }
	  case DM9051_10MFD:  {	phy_reg4 = 0x041; phy_reg0 = 0x0100; break;  }
	  case DM9051_100MHD: {	phy_reg4 = 0x081; phy_reg0 = 0x2000; break;  }
	  case DM9051_100MFD: {	phy_reg4 = 0x101; phy_reg0 = 0x2100; break;  }
	  case DM9051_10M:    {	phy_reg4 = 0x061; phy_reg0 = 0x1200; break;  }
	  case DM9051_1M_HPNA:{ break; }
	  case DM9051_AUTO:   { break; }  
	  default : break;
	  
	}
	DM9051_phy_write(DM9051_PHY_AUTO_NEG, phy_reg4);             /* Set PHY media mode                                               */
	DM9051_phy_write(DM9051_PHY_BMCR, phy_reg0);             /* Tmp                                                              */
	dm9051_port.delay_ms(10);
}
/**
 * @brief: set mode of general io port 
 * @parameter: 
 * 		@pin: io label of pin, range 0-2 0XFF:set all pin
 * 		@mode: 
 * 			GPCR_GPIO_OUTPUT_MODE: output mode
 * 			GPCR_GPIO_INPUT_MODE: input  mode
 */ 
void DM9051_setIOMode(uint8_t pin, uint8_t mode)
{
	uint8_t value = DM9051_read_reg(DM9051_GPCR);

	switch (pin)
	{
		case 0x00: DM9051_write_reg(DM9051_GPR, value);
		case 0x01:
		case 0x02:
		case 0xff:
		default : break;
	}
}

// void DM9051_setOuputState(uint8_t pin, uint8_t status)
// {
	
// }

// uint8_t DM9051_getOutputState(uint8_t pin)
// {
// 	uint8_t state;
	
// }



int8_t DM9051_reset(void)
{
	DM9051_write_reg(DM9051_NCR, DM9051_REG_RESET);
	
	dm9051_port.delay_ms(1);
	
	int8_t flag = DM9051_read_reg(DM9051_NCR) & DM9051_REG_RESET;
	if (flag)
	{
		return -1;
	}	
	return 0;
}


uint32_t DM9051_closePhy(void)
{
  /* RESET devie                                                                                            */
  DM9051_phy_write(DM9051_PHY_BMCR, 0x8000); /* PHY RESET                                                   */
  DM9051_write_reg(DM9051_GPR, 0x01);    /* Power-Down PHY                                                  */
  DM9051_write_reg(DM9051_IMR, 0x80);    /* Disable all interrupt                                           */
  DM9051_write_reg(DM9051_RCR, 0x00);    /* Disable RX                                                      */

  return 0;
}

int8_t DM9051_isLink(void)
{
  uint16_t value;
  DM9051_phy_read(DM9051_PHY_STATUS, &value);
  return value&0x04;
}

#if 0
void Network_test(void)
{
	uint8_t value;
	uint8_t value1;
	
	value = DM9051_read_reg(DM9051_NSR);
	value1 = DM9051_read_reg(DM9051_NCR);
	if (value & 0x40)
	{
		printf("link is ok!\r\n");
		if (value & 0x80)
		{
			printf("10Mbps !\r\n");
		}else printf("100Mbps !\r\n");
		
		if (value1 & 0X08)
		{
			printf("Full-Duplex!\r\n");
		} else printf("Half-Duplex!\r\n");
	}
	else
		printf("link is fail!\r\n");
}
#endif //0 for test

int8_t DM9051_Init(uint8_t mac[6])
{
	uint8_t mac_addr[6];
	
	info.version.pvid.pid  =  DM9051_read_reg(DM9051_PIDL);
	info.version.pvid.vid  =  DM9051_read_reg(DM9051_VIDL);
	info.version.pvid.pid |= (DM9051_read_reg(DM9051_PIDH)<<8);
	info.version.pvid.vid |= (DM9051_read_reg(DM9051_VIDH)<<8);
	info.chip_version = DM9051_read_reg(DM9051_CHIPR);
	
	if (info.version.ver != DM9051_ID)
	{
		info.version.pvid.pid  =  DM9051_read_reg(DM9051_PIDL);
		info.version.pvid.vid  =  DM9051_read_reg(DM9051_VIDL);
		info.version.pvid.pid |= (DM9051_read_reg(DM9051_PIDH)<<8);
		info.version.pvid.vid |= (DM9051_read_reg(DM9051_VIDH)<<8);
		info.chip_version = DM9051_read_reg(DM9051_CHIPR);
	}

	memcpy(init.mac_addr, mac, 6);

#ifdef DM9051_DEBUG
	dm9051_port.info_printf("id:%08X version:%04X\r\n", info.version.ver, info.chip_version);
#endif
	
	while (DM9051_reset())
	{
#ifdef DM9051_DEBUG
		dm9051_port.info_printf("reset fail\r\n");
#endif
		DM9051_write_reg(DM9051_NCR, 0x00);
	}
	
	DM9051_write_reg(DM9051_NCR, 0x00);
	DM9051_write_reg(DM9051_GPCR, GPCR_GEP_CNTL); //set general output status/phy on
	DM9051_write_reg(DM9051_GPR, 0x00);
	
	dm9051_port.delay_ms(5); //at least 1ms time delay
	
//	DM9051_setIOMode(GPCR_GPIO_ALL_POS, GPCR_GPIO_INPUT_MODE);
	DM9051_set_phy_mode(init.mode);
		
	uint8_t multi_addr[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80};
	DM9051_setMultiCast_addr(multi_addr);
	
	if (mac != NULL)
	{
		DM9051_setMacAddr(mac);
		if (DM9051_eeprom_read(0, mac_addr, 3) >= 0) {
			if (memcmp(mac, mac_addr, 6) != 0) {
				if (DM9051_eeprom_write(0, mac, 3) < 0) {
		#ifdef DM9051_DEBUG
					dm9051_port.info_printf("eeprom write fail!\r\n");
		#endif
				}			
			}
		}
	#ifdef DM9051_DEBUG		
		else
			dm9051_port.info_printf("eeprom read fail!\r\n");
	#endif
		
	}

#ifdef DM9051_DEBUG
	DM9051_getMacAddr(mac_addr);
	dm9051_port.info_printf("macAddr:");
	for (uint8_t i = 0; i < 6; i++)
	{
		dm9051_port.info_printf("%02X ", mac_addr[i]);
	}
	dm9051_port.info_printf("\r\n**********************************************\r\n");
#endif
	/* Setup DM9051 Registers */
	DM9051_write_reg(DM9051_NCR, NCR_DEFAULT);
	DM9051_write_reg(DM9051_TCR, TCR_DEFAULT);
	DM9051_write_reg(DM9051_RCR, RCR_DEFAULT);
	DM9051_write_reg(DM9051_BPTR, BPTR_DEFAULT);
	DM9051_write_reg(DM9051_FCTR, 0x3A);
	DM9051_write_reg(DM9051_FCR,  FCR_DEFAULT);
	DM9051_write_reg(DM9051_SMCR, SMCR_DEFAULT);
	DM9051_write_reg(DM9051_TCR2, DM9051_TCR2_SET);
	DM9051_write_reg(DM9051_INTR, 0x00);
	DM9051_write_reg(DM9051_RSR, 0xff);
	
	/* Clear status */
	DM9051_write_reg(DM9051_NSR, NSR_CLR_STATUS);
	DM9051_write_reg(DM9051_ISR, ISR_CLR_STATUS);

	uint8_t i = 0;
	uint16_t value;
	if (init.mode == DM9051_AUTO) {
		do
		{
			if (DM9051_phy_read(DM9051_PHY_STATUS, &value) < 0) {
				return -1;
			}
			dm9051_port.delay_ms(2);
			if (i++ == 255)	{
//				dm9051_port.info_printf("could not establish link \r\n");
				return -1;
			}
		} while (!(value & 0x20));
	} 
	
#ifdef DM9051_DEBUG
  DM9051_phy_read(17, &value);
  int lnk = value >> 12;
  switch (lnk)
  {
    case 1: dm9051_port.info_printf("10M half duplex ");  info.mode = DM9051_10MHD;  break;
    case 2: dm9051_port.info_printf("10M full duplex ");  info.mode = DM9051_10MFD;  break;
    case 4: dm9051_port.info_printf("100M half duplex "); info.mode = DM9051_100MHD; break;
    case 8: dm9051_port.info_printf("100M full duplex "); info.mode = DM9051_100MFD; break;
    default:dm9051_port.info_printf("unknown: %d ", lnk); break;
  }
  
  dm9051_port.info_printf("mode \r\n");
#endif

  	DM9051_write_reg(DM9051_RCR, (RCR_DEFAULT | RCR_RXEN));  /* Enable RX */
	DM9051_write_reg(DM9051_IMR, init.imr);    /* Enable TX/RX interrupt mask                    */
	DM9051_write_reg(DM9051_ISR, ISR_CLR_STATUS);
 
	return 0;
}

static uint8_t DM9051_tx_requiet(void)
{
	DM9051_write_reg(DM9051_TCR, TCR_TXREQ);
    /* set send requir flag and Cleared after TX complete */
	return 0;
}

int8_t DM9051_Send(uint8_t *buff, uint16_t len)
{
#ifdef FIFO_OVERFLOW_CHECK
	uint16_t calc_MWR;
#endif
	
	if (!DM9051_isLink())
	{
//		dm9051_port.info_printf("link is fail! \r\n");
		return -1;
	}
	
	//disable all interrupt
//	dm9051_port.info_printf("ISR1 Reg = 0x%02X.\r\n", DM9051_read_reg(DM9051_ISR));
	while(DM9051_read_reg(DM9051_TCR) & DM9051_TCR_SET) //wait other send finish
	{
		dm9051_port.delay_ms(5);
	}
	
#ifdef FIFO_OVERFLOW_CHECK
	calc_MWR = (DM9051_read_reg(DM9051_MDWAH) << 8) + DM9051_read_reg(DM9051_MDWAL);
	calc_MWR += len;
	if(calc_MWR > 0x0bff) calc_MWR -= 0x0c00;
#endif //FIFO_OVERFLOW_CHECK
	
	DM9051_write_reg(DM9051_TXPLL, len & 0xff);
    DM9051_write_reg(DM9051_TXPLH, (len >> 8) & 0xff);
    DM9051_write_mem((uint8_t*)buff, len);

#ifdef DM9051_DEBUG
//    dm9051_port.info_printf("Tx Read Pointer H = 0x%02X.\r\n", DM9051_read_reg(DM9051_TRPAH));
//	dm9051_port.info_printf("Tx Read Pointer L = 0x%02X.\r\n", DM9051_read_reg(DM9051_TRPAL));
//	dm9051_port.info_printf("DM9051_MWRH H = 0x%02X.\r\n", DM9051_read_reg(DM9051_MDWAH));
//	dm9051_port.info_printf("DM9051_MWRL L = 0x%02X.\r\n", DM9051_read_reg(DM9051_MDWAL));
#endif  //DM9051_DEBUG
	
	DM9051_tx_requiet();

#ifdef DM9051_DEBUG
//	dm9051_port.info_printf("******************** send late *******************************\r\n");
//	dm9051_port.info_printf("Tx control Reg = 0x%02X.\r\n", DM9051_read_reg(DM9051_TCR));
//	dm9051_port.info_printf("ISR Reg = 0x%02X.\r\n", DM9051_read_reg(DM9051_ISR));
//	dm9051_port.info_printf("NSR Reg = 0x%02X.\r\n", DM9051_read_reg(DM9051_NSR));
//    dm9051_port.info_printf("Tx Read Pointer H = 0x%02X.\r\n", DM9051_read_reg(DM9051_TRPAH));
//	dm9051_port.info_printf("Tx Read Pointer L = 0x%02X.\r\n", DM9051_read_reg(DM9051_TRPAL));
//	dm9051_port.info_printf("DM9051_MWRH H = 0x%02X.\r\n", DM9051_read_reg(DM9051_MDWAH));
//	dm9051_port.info_printf("DM9051_MWRL L = 0x%02X.\r\n", DM9051_read_reg(DM9051_MDWAL));	
#endif  //DM9051_DEBUG

#ifdef FIFO_OVERFLOW_CHECK
	if(calc_MWR != ((DM9051_read_reg(DM9051_MDWAH) << 8) + DM9051_read_reg(DM9051_MDWAL)))
	{
		dm9051_port.info_printf("DM9K MWR Error!! calc_MWR = 0x%X, SendLength = 0x%x\r\n", calc_MWR, len);
		dm9051_port.info_printf("MWRH = 0x%X, MWRL = 0x%X\r\n", DM9051_read_reg(DM9051_MDWAH), DM9051_read_reg(DM9051_MDWAL));

		DM9051_write_reg(DM9051_MDWAH, (calc_MWR >> 8) & 0xff);
		DM9051_write_reg(DM9051_MDWAL, calc_MWR & 0xff);
		
		DM9051_write_reg(DM9051_IMR, init.imr);
		return -1;
	}
#endif
	
	DM9051_write_reg(DM9051_IMR, init.imr);
	
#ifdef DM9051_DEBUG
	static uint32_t send_count = 0;
	dm9051_port.info_printf("data send success! [%d]\r\n", send_count++);
	dm9051_port.info_printf("RCR reg:%04X RSR:%04X NSR:%04X\r\n", DM9051_read_reg(DM9051_RCR), DM9051_read_reg(DM9051_RSR), DM9051_read_reg(DM9051_NSR));
#endif // DM9051_DEBUG
	DM9051_write_reg(DM9051_IMR, init.imr);	
	return 0;
}

uint8_t DM9051_checkRecv(void)
{
	uint8_t reg;
	if ((reg = DM9051_read_reg(DM9051_ISR)) & ISR_PRS)
	{
		DM9051_write_reg(DM9051_ISR, reg);
		return 1;
	}
	
	return 0;
}


struct pbuf * DM9051_Recv(struct netif *ifc)
{
	uint8_t rxbyte, reg;
	uint16_t rx_len;
#ifdef FIFO_OVERFLOW_CHECK
	uint16_t calc_MRR;	
#endif
	
	if (!DM9051_isLink() || ifc == NULL)
	{
		return 0;
	}
	
//	DM9051_write_reg(DM9051_ISR, reg | 0X80);
	
	
//	DM9051_write_reg(DM9051_IMR, init.imr);
#ifdef DM9051_DEBUG
	if (DM9051_read_reg(DM9051_NSR) & 0x02)
	{
		dm9051_port.info_printf("data is overflow!\r\n");
	#ifdef DM9051_PACKET_STATUS_CHECK
		pkt_info.rx_overflow_ctn++;
	#endif
	}
	
	#ifdef DM9051_PACKET_STATUS_CHECK
		pkt_info.total_rx_cnt++;
	#endif
//	dm9051_port.info_printf("================ rx mem data receive =================\r\n");
#endif	
	
	DM9051_write_reg(DM9051_RCR, RCR_DEFAULT);
	/* Check packet ready or not                                                                              */
	rxbyte = DM9051_read_reg(DM9051_MRCMDX);
	rxbyte = DM9051_read_reg(DM9051_MRCMDX);

	if ((rxbyte != 1) && (rxbyte != 0))
	{
#ifdef DM9051_DEBUG
		dm9051_port.info_printf("rxbyte = %02X.\r\n", rxbyte);
		dm9051_port.info_printf("DM9051 rx: rx error, stop device \r\n");
#endif
#ifdef DM9051_PACKET_STATUS_CHECK
		pkt_info.reset_ctn++;
#endif
	//	DM9051_Init(init.mac_addr);
		/* Reset RX FIFO pointer */
		DM9051_write_reg(DM9051_RCR, RCR_DEFAULT);	//RX disable
		DM9051_write_reg(DM9051_MPCR, 0x01);		//Reset RX FIFO pointer
		
		dm9051_port.delay_ms(2);
		
 		DM9051_write_reg(DM9051_RCR, (RCR_DEFAULT | RCR_RXEN));		//RX Enable
		return NULL;
	}
	
	if (rxbyte)
	{
		struct pbuf *p;
		uint8_t *buff;
		uint16_t rx_status;
		uint8_t ReceiveData[4];

#ifdef FIFO_OVERFLOW_CHECK			
		calc_MRR = (DM9051_read_reg(DM9051_MDRAH) << 8) + DM9051_read_reg(DM9051_MDRAL);	//Save RX SRAM pointer
#endif	//FifoPointCheck
			
		DM9051_read_reg(DM9051_MRCMDX);		//dummy read
		DM9051_read_mem(ReceiveData, 4);
		
		rx_status = ReceiveData[0] + (ReceiveData[1] << 8);
		rx_len    = ReceiveData[2] + (ReceiveData[3] << 8);
		
		p = pbuf_alloc(PBUF_RAW, rx_len, PBUF_POOL);
		buff = malloc(rx_len);
		
#ifdef DM9051_DEBUG
		dm9051_port.info_printf("DM9051 rx: status %04x len %d \r\n", rx_status, rx_len);
#endif
		
		DM9051_read_mem((uint8_t *)buff, rx_len);
		DM9051_write_reg(DM9051_RCR, (RCR_DEFAULT | RCR_RXEN));		//RX Enable

#ifdef FIFO_OVERFLOW_CHECK
		calc_MRR += (rx_len + 4);
		if(calc_MRR > 0x3fff) calc_MRR -= 0x3400;
	
		if(calc_MRR != ((DM9051_read_reg(DM9051_MDRAH) << 8) + DM9051_read_reg(DM9051_MDRAL)))
		{
#ifdef DM9051_DEBUG
			dm9051_port.delay_ms(2);
			dm9051_port.info_printf("DM9K MRR Error!!\r\n");
			dm9051_port.info_printf("Predicut RX Read pointer = 0x%X, Current pointer = 0x%X  RX_LEM = %d\r\n", \
				calc_MRR, ((DM9051_read_reg(DM9051_MDRAH) << 8) + DM9051_read_reg(DM9051_MDRAL)), rx_len);
#endif
			dm9051_port.delay_ms(2);
			DM9051_write_reg(DM9051_MDRAH, (calc_MRR >> 8) & 0xff);
			DM9051_write_reg(DM9051_MDRAL, calc_MRR & 0xff);
			dm9051_port.info_printf("RX Read pointer = 0x%X, Current pointer = 0x%X\r\n", \
				calc_MRR, ((DM9051_read_reg(DM9051_MDRAH) << 8) + DM9051_read_reg(DM9051_MDRAL)));	
			
			free(buff);
			free(p);
			return NULL;
		}
#endif  	
		
		uint32_t count = 0;
		for (struct pbuf *q = p; q != NULL; q = q->next)
		{
			memcpy(q->payload, &buff[count], q->len);
			count += q->len;
		}
		free(buff);
		//DM9051_write_reg(DM9051_IMR, init.imr);
		if ((rx_status & 0xbf00) || (rx_len < 0x40) || (rx_len > DM9051_PKT_MAX) )
		{ 
#ifdef DM9051_DEBUG
			dm9051_port.info_printf("rx error: status %04x, rx_len: %d \r\n", rx_status, rx_len);	
#endif
			if (rx_status & 0x100)
			{
#ifdef DM9051_DEBUG
				dm9051_port.info_printf("rx fifo error \r\n");
#endif
#ifdef DM9051_PACKET_STATUS_CHECK
				pkt_info.rx_err_fifoCtn++;
#endif
			}
		
			if (rx_status & 0x200)
			{
#ifdef DM9051_DEBUG
				dm9051_port.info_printf("rx crc error \r\n");
#endif
#ifdef DM9051_PACKET_STATUS_CHECK
				pkt_info.rx_err_crcCtn++;
#endif
			}
		
			if (rx_status & 0x8000)
			{
#ifdef DM9051_DEBUG
				dm9051_port.info_printf("rx length error \r\n");
#endif
#ifdef DM9051_PACKET_STATUS_CHECK
				pkt_info.rx_err_lenCtn++;
#endif
			}
			if (rx_len > DM9051_PKT_MAX)
			{
#ifdef DM9051_DEBUG
				dm9051_port.info_printf("rx length too big \r\n");
#endif
#ifdef DM9051_PACKET_STATUS_CHECK
				pkt_info.rx_err_lenCtn++;
#endif
			} 
		}
#ifdef DM9051_DEBUG	
	//	dm9051_port.info_printf("read point:  L->%X  H->%X\r\n", DM9051_read_reg(DM9051_RWPAL),DM9051_read_reg(DM9051_RWPAH));
#endif
		return p;
	} 
	else
	{
#ifdef DM9051_DEBUG
		dm9051_port.info_printf("DM9051 rx: No packets received. \r\n");      
#endif		
		/* restore receive interrupt                                                                            */
 		DM9051_write_reg(DM9051_RCR, (RCR_DEFAULT | RCR_RXEN));		//RX Enable
	}	
	return 0;
}

void DM9051_isr_handler(void)
{
  uint16_t int_status, status;
  uint8_t count = 3;
//  DM9051_write_reg(DM9051_RCR, (RCR_DEFAULT));
//  DM9051_write_reg(DM9051_IMR, DM9051_IMR_OFF);		//Clear interrupt flag
//  /* Got DM9051 interrupt status  */
  status = int_status = DM9051_read_reg(DM9051_ISR);    
  DM9051_write_reg(DM9051_ISR, (uint8_t)(status));    /* Clear ISR status    */ 
                                       
#ifdef DM9051_DEBUG
 // dm9051_port.info_printf("clear DM9051 isr: int status %04x \r\n", DM9051_read_reg(DM9051_ISR)); 
#endif
	
  /* receive overflow                                                                                       */
  if (int_status & ISR_ROS)
  {
#ifdef DM9051_DEBUG
  //  dm9051_port.info_printf("overflow \r\n");
#endif
  }

  if (int_status & ISR_ROOS)
  {
#ifdef DM9051_DEBUG
  //  dm9051_port.info_printf("overflow counter overflow \r\n");
#endif
  }
  /* Received the coming packet                                                                             */
  if (int_status & ISR_PRS)
  {  
    /* disable receive interrupt                                                                            */
	  set_recvFlag(); //data recvive 
  }

  /* Transmit Interrupt check                                                                               */
  if (int_status & ISR_PTS)
  {
    /* transmit done                                                                                        */
    int tx_status = DM9051_read_reg(DM9051_NSR);    /* Got TX status                                        */

    if (tx_status & (NSR_TX2END | NSR_TX1END))
    {  	
                                                                         
    }
  }
//  DM9051_write_reg(DM9051_RCR, (RCR_DEFAULT | RCR_RXEN));
  DM9051_write_reg(DM9051_IMR, init.imr);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin)
	{
		case GPIO_PIN_5:
		case GPIO_PIN_6: 
		case GPIO_PIN_7: break;
		case GPIO_PIN_8: DM9051_isr_handler();break;
		case GPIO_PIN_9: break;
		default : break;
	}
}


























