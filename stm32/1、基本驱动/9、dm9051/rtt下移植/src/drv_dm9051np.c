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
  v1.0.1         191213        EtcFly          add support to receive irq and the underlying network card 
											   sends data information statistics
**********************************************************************************************************/
#include <string.h>
#include <stdio.h>
#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <drivers/spi.h>
#include <drivers/pin.h>
#include "drv_spi.h"
#include <netif/ethernetif.h>
#include "stm32l4xx_hal.h"
#include "drv_dm9051np.h"


#define DBG_SECTION_NAME    "[drv.dm9051] "
#define DBG_LEVEL           DBG_LOG
#define DBG_COLOR
#include "rtdbg.h"



#ifdef DM9051_DEBUG
#define DM_TRACE(fmt, ...) rt_kprintf(fmt, ##__VA_ARGS__)
#else
#define DM_TRACE(...) 
#endif

#ifdef FIFO_OVERFLOW_CHECK
#define DM_CHECK(fmt, ...) DM_TRACE(fmt, ##__VA_ARGS__)
#else
#define DM_CHECK(...)
#endif

DM9051_Init_t init = {
	.mode = DM9051_AUTO,
	.imr = 
#ifdef DM9051_IRQ_EANBLE
		IMR_PAR | IMR_PRM,// | IMR_PTM,
#else
		IMR_OFF,
#endif
	.mac_addr = {0X00, 0X60, 0X6E, 0X0C, 0X34, 0X56},     /* OUI 00-60-6E Davicom Semiconductor, Inc. */
	.type = TYPE_DM9051A,
};
static ChipInfo_t info;

#ifdef DM9051_PACKET_STATUS_STAT
Packet_Info_t pkt_info;
#endif

#define DM9051_PHY              (0x01 << 6)    /* PHY address 0x01 */

/*
struct eth_cache
{	
	struct eth_cache *next;
	struct eth_cache *prev;
	
	uint8_t *buffer;
	uint16_t length;
	uint8_t  max_pkt; //max packet be support 
	uint8_t  free;    //remain size of packet support
}; */

#define dm9051_lock(dev)      rt_mutex_take(&((struct DM9051_eth*)dev)->lock, RT_WAITING_FOREVER);
#define dm9051_unlock(dev)    rt_mutex_release(&((struct DM9051_eth*)dev)->lock);

static void delay(uint32_t  mus)
{
	volatile uint32_t t = 0;
	for (t = 0; t < mus; t++)
	{
		volatile uint32_t j = 0;
		while (j++ < 2000);
	}
}

static uint8_t DM9051_write_reg(struct rt_spi_device *spi_dev, uint8_t cmd, uint8_t data)
{
	uint8_t buf[2];

	buf[0] = CMD_REG_WR(cmd);
	buf[1] = data;

    rt_spi_send(spi_dev, buf, 2);
	return 0;
}

static uint8_t DM9051_read_reg(struct rt_spi_device *spi_dev, uint8_t cmd)
{
	uint8_t reg, recv;
	
	reg = CMD_REG_RD(cmd);
    rt_spi_send_then_recv(spi_dev, &reg, 1, &recv, 1);

    return recv;
}

static void DM9051_write_mem(struct rt_spi_device *spi_device, const void *buf, uint32_t len)
{
	uint8_t cmd  = CMD_MEM_WR;
	
	rt_spi_send_then_send(spi_device, &cmd, 1, buf, len);
}

static void DM9051_read_mem(struct rt_spi_device *spi_dev, uint8_t *buf, uint32_t len)
{
	uint8_t cmd = CMD_MEM_RD;
	
    rt_spi_send_then_recv(spi_dev, &cmd, 1, buf, len);
}

static int8_t DM9051_phy_read(struct rt_spi_device *spi_dev, uint16_t reg, uint16_t *recv)
{
	uint8_t value = (reg & 0x3f) | DM9051_PHY;
	uint16_t timeout = 1000;

	DM9051_write_reg(spi_dev, DM9051_EPAR, value);
	DM9051_write_reg(spi_dev, DM9051_EPCR, 0xc);           /* Issue phyxcer read command                               */

	while((DM9051_read_reg(spi_dev, DM9051_EPCR) & 0x1) && timeout-- > 0) 
	{
		delay(1);
	}

	if (timeout == 0)
	{
		return -1;
	}

  	*recv = (DM9051_read_reg(spi_dev, DM9051_EPDRH) << 8) | DM9051_read_reg(spi_dev, DM9051_EPDRL);
	return 0;
}

static int8_t DM9051_phy_write(struct rt_spi_device *spi_dev, uint8_t reg, uint16_t data)
{
	uint8_t value = (reg & 0x3f) | DM9051_PHY;
	uint16_t timeout = 1000;

	DM9051_write_reg(spi_dev, DM9051_EPAR, value);
	/* Fill the written data into REG_0D & REG_0E                                                           */
	DM9051_write_reg(spi_dev, DM9051_EPDRL, (data & 0xff));
	DM9051_write_reg(spi_dev, DM9051_EPDRH, ((data >> 8) & 0xff));
	DM9051_write_reg(spi_dev, DM9051_EPCR, 0xa);           /* Issue phyxcer read command                               */

	while((DM9051_read_reg(spi_dev, DM9051_EPCR) & 0x1) && timeout-- > 0) 
	{
		delay(2);
	}

	if (timeout == 0)
	{
		return -1;
	}
	return 0;
}

static int8_t DM9051_eeprom_write(struct rt_spi_device *spi_dev, uint8_t word_addr, void *pdata, uint8_t len)
{
	if (pdata == NULL || len == 0) {
		return -1;
	}

	uint8_t value = (word_addr & 0x3f) | DM9051_PHY;
	uint8_t *p = (uint8_t *)pdata;

	DM9051_write_reg(spi_dev, DM9051_EPCR, 0x10); //reload eeprom	
	
	uint16_t timeout = 1000;
	for (uint8_t i = 0; i < len; i++)
	{
		DM9051_write_reg(spi_dev, DM9051_EPAR, value+i);
		DM9051_write_reg(spi_dev, DM9051_EPDRL, p[2*i]);
		DM9051_write_reg(spi_dev, DM9051_EPDRH, p[2*i+1]);	
		DM9051_write_reg(spi_dev, DM9051_EPCR, 0x12);           /* Issue phyxcer write command                               */
		
		while((DM9051_read_reg(spi_dev, DM9051_EPCR) & 0x2) && timeout-- > 0) {
			delay(2);
		}
		
		if (timeout == 0) {
			return -1;
		}
	}
	
	timeout = 1000;
	while (DM9051_read_reg(spi_dev, DM9051_EPCR) & 0x1 && timeout-- > 0) {
		delay(2);
	}
	
	if (timeout == 0) {
		return -1;
	}
	
	DM9051_write_reg(spi_dev, DM9051_EPCR, 0x00);
	delay(2); //error occurs when the write operation reads immediately
	return 0;
}

static int8_t DM9051_eeprom_read(struct rt_spi_device *spi_dev, uint8_t word_addr, void *pdata, uint8_t len)
{
	if (pdata == NULL || len == 0)
	{
		return -1;
	}

	uint8_t value = (word_addr & 0x3f) | DM9051_PHY;
	uint8_t *p = (uint8_t *)pdata;

	DM9051_write_reg(spi_dev, DM9051_EPCR, 0x00); //reload eeprom
	
	uint16_t timeout = 1000;
	for (uint8_t i = 0; i < len; i++)
	{			
		DM9051_write_reg(spi_dev, DM9051_EPAR, value+i);
		DM9051_write_reg(spi_dev, DM9051_EPCR, 0x04);           /* Issue phyxcer read command                               */
		while((DM9051_read_reg(spi_dev, DM9051_EPCR) & 0x4) && timeout-- > 0) {
			delay(2);
		}
		
		if (timeout == 0) {
			return -1;
		}
		p[2*i]   = DM9051_read_reg(spi_dev, DM9051_EPDRL);
		p[2*i+1] = DM9051_read_reg(spi_dev, DM9051_EPDRH);	
	}
	return 0;
}



static void DM9051_setMacAddr(struct rt_spi_device *spi_dev, uint8_t mac[6])
{
	DM9051_write_reg(spi_dev, DM9051_PHYADDR1, mac[0]);	
	DM9051_write_reg(spi_dev, DM9051_PHYADDR2, mac[1]);	
	DM9051_write_reg(spi_dev, DM9051_PHYADDR3, mac[2]);	
	DM9051_write_reg(spi_dev, DM9051_PHYADDR4, mac[3]);	
	DM9051_write_reg(spi_dev, DM9051_PHYADDR5, mac[4]);	
	DM9051_write_reg(spi_dev, DM9051_PHYADDR6, mac[5]);	
}

static int8_t DM9051_getMacAddr(struct rt_spi_device *spi_dev, uint8_t *mac)
{
	if (mac == NULL)
	{
		return -1;
	}

	mac[0] = DM9051_read_reg(spi_dev, DM9051_PHYADDR1);
	mac[1] = DM9051_read_reg(spi_dev, DM9051_PHYADDR2);
	mac[2] = DM9051_read_reg(spi_dev, DM9051_PHYADDR3);
	mac[3] = DM9051_read_reg(spi_dev, DM9051_PHYADDR4);
	mac[4] = DM9051_read_reg(spi_dev, DM9051_PHYADDR5);
	mac[5] = DM9051_read_reg(spi_dev, DM9051_PHYADDR6);
	
	return 0;
}

static void DM9051_setMultiCast_addr(struct rt_spi_device *spi_dev, uint8_t multiCast_addr[8])
{
	/* set multicast address      */
	DM9051_write_reg(spi_dev, DM9051_MULTICAST_ADDR1, multiCast_addr[0]);
	DM9051_write_reg(spi_dev, DM9051_MULTICAST_ADDR2, multiCast_addr[1]);
	DM9051_write_reg(spi_dev, DM9051_MULTICAST_ADDR3, multiCast_addr[2]);
	DM9051_write_reg(spi_dev, DM9051_MULTICAST_ADDR4, multiCast_addr[3]);
	DM9051_write_reg(spi_dev, DM9051_MULTICAST_ADDR5, multiCast_addr[4]);
	DM9051_write_reg(spi_dev, DM9051_MULTICAST_ADDR6, multiCast_addr[5]);
	DM9051_write_reg(spi_dev, DM9051_MULTICAST_ADDR7, multiCast_addr[6]);
	DM9051_write_reg(spi_dev, DM9051_MULTICAST_ADDR8, multiCast_addr[7]);
}

#if 0
static void DM9051_getMultiCast_addr(struct rt_spi_device *spi_dev, uint8_t multi_addr[8])
{
	multi_addr[0] = DM9051_read_reg(spi_dev, DM9051_MULTICAST_ADDR1);
	multi_addr[1] = DM9051_read_reg(spi_dev, DM9051_MULTICAST_ADDR2);
	multi_addr[2] = DM9051_read_reg(spi_dev, DM9051_MULTICAST_ADDR3);
	multi_addr[3] = DM9051_read_reg(spi_dev, DM9051_MULTICAST_ADDR4);
	multi_addr[4] = DM9051_read_reg(spi_dev, DM9051_MULTICAST_ADDR5);
	multi_addr[5] = DM9051_read_reg(spi_dev, DM9051_MULTICAST_ADDR6);
	multi_addr[6] = DM9051_read_reg(spi_dev, DM9051_MULTICAST_ADDR7);
	multi_addr[7] = DM9051_read_reg(spi_dev, DM9051_MULTICAST_ADDR8);
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
static void DM9051_set_phy_mode(struct rt_spi_device *spi_dev, enum DM9051_PHY_mode uMediaMode)
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
	DM9051_phy_write(spi_dev, DM9051_PHY_AUTO_NEG, phy_reg4);             /* Set PHY media mode                                               */
	DM9051_phy_write(spi_dev, DM9051_PHY_BMCR, phy_reg0);             /* Tmp                                                              */
	delay(10);
}
/**
 * @brief: set mode of general io port 
 * @parameter: 
 *     @spi_dev: spi device handle
 *     @pin_mask: the pin need to operation
 *     @mode_mask: the mode of mask
 *     @status_mask: the status of mask
 */ 

static void DM9051_setIOMode(struct rt_spi_device *spi_dev, uint8_t pin_mask, uint8_t mode_mask, uint8_t status_mask)
{
	uint8_t mode_value = DM9051_read_reg(spi_dev, DM9051_GPCR);
	uint8_t status_value = DM9051_read_reg(spi_dev, DM9051_GPR);
	
	status_mask &= pin_mask;
	mode_mask &= pin_mask;
	
	status_mask |= status_value & 0x01;
	mode_mask |= mode_value & 0x01;
	
	DM9051_write_reg(spi_dev, DM9051_GPCR, mode_mask);
	DM9051_write_reg(spi_dev, DM9051_GPR, status_mask);
}

 void DM9051_setOuputState(struct rt_spi_device *spi_dev, uint8_t pin_mask, uint8_t status_mask)
 {
	uint8_t status_value = DM9051_read_reg(spi_dev, DM9051_GPR);
	 
    status_mask &= 	pin_mask;
	status_mask |= status_value & 0x01;
	DM9051_write_reg(spi_dev, DM9051_GPR, status_mask);
 }

 uint8_t DM9051_getOutputState(struct rt_spi_device *spi_dev, uint8_t pin_mask)
 {
 	uint8_t state;
	state = DM9051_read_reg(spi_dev, DM9051_GPR) & pin_mask;
	return state;
 }

static int8_t DM9051_reset(struct rt_spi_device *spi_dev)
{
	DM9051_write_reg(spi_dev, DM9051_NCR, DM9051_REG_RESET);
	delay(1);
	
	int8_t flag = DM9051_read_reg(spi_dev, DM9051_NCR) & DM9051_REG_RESET;
	if (flag)
	{
		return -1;
	}	
	return 0;
}


static uint32_t DM9051_closePhy(struct rt_spi_device *spi_dev)
{
  /* RESET devie                                                                                            */
  DM9051_phy_write(spi_dev, DM9051_PHY_BMCR, 0x8000); /* PHY RESET                                                   */
  DM9051_write_reg(spi_dev, DM9051_GPR, 0x01);    /* Power-Down PHY                                                  */
  DM9051_write_reg(spi_dev, DM9051_IMR, 0x80);    /* Disable all interrupt                                           */
  DM9051_write_reg(spi_dev, DM9051_RCR, 0x00);    /* Disable RX                                                      */

  return 0;
}

static int8_t DM9051_isLink(struct rt_spi_device *spi_dev)
{
  uint16_t value;
  DM9051_phy_read(spi_dev, DM9051_PHY_STATUS, &value);
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
		DM_TRACE("link is ok!\r\n");
		if (value & 0x80)
		{
			DM_TRACE("10Mbps !\r\n");
		}else DM_TRACE("100Mbps !\r\n");
		
		if (value1 & 0X08)
		{
			DM_TRACE("Full-Duplex!\r\n");
		} else DM_TRACE("Half-Duplex!\r\n");
	}
	else
		DM_TRACE("link is fail!\r\n");
}
#endif //0 for test

static rt_err_t DM9051_init(rt_device_t dev)
{
	uint8_t mac_addr[6];
	struct DM9051_eth *eth = (struct DM9051_eth *)dev;
	struct rt_spi_device *spi_dev = eth->spi_device;
	
	dm9051_lock(eth);
	info.pid  =  DM9051_read_reg(spi_dev, DM9051_PIDL);
	info.vid  =  DM9051_read_reg(spi_dev, DM9051_VIDL);
	info.pid |= (DM9051_read_reg(spi_dev, DM9051_PIDH)<<8);
	info.vid |= (DM9051_read_reg(spi_dev, DM9051_VIDH)<<8);
	info.chip_version = DM9051_read_reg(spi_dev, DM9051_CHIPR);

	memcpy(init.mac_addr, eth->dev_addr, MAX_ADDR_LEN);

	DM_TRACE("pid:%04X vid:%04X version:%04X\r\n", info.pid, info.vid, info.chip_version);
	while (DM9051_reset(spi_dev))
	{
		DM_TRACE("reset fail\r\n");
		DM9051_write_reg(spi_dev, DM9051_NCR, 0x00);
	}
	
	DM9051_write_reg(spi_dev, DM9051_NCR, 0x00);
	DM9051_write_reg(spi_dev, DM9051_GPCR, GPCR_GEP_CNTL); //set general output status/phy on
	DM9051_write_reg(spi_dev, DM9051_GPR, 0x00);
	
	delay(500); //at least 1ms time delay
	
	DM9051_setIOMode(spi_dev, MASK_IO_PIN_ALL, MASK_MODE_OUTPUT_ALL, MASK_STATUS_PIN_ALL_HIGH);
	DM9051_set_phy_mode(spi_dev, init.mode);
		
	uint8_t multi_addr[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80};
	DM9051_setMultiCast_addr(spi_dev, multi_addr);
	
	if ((char *)eth->dev_addr != RT_NULL)
	{
		DM9051_setMacAddr(spi_dev, eth->dev_addr);
		if (DM9051_eeprom_read(spi_dev, 0, mac_addr, 3) >= 0) {
			if (memcmp(eth->dev_addr, mac_addr, 6) != 0) {
				if (DM9051_eeprom_write(spi_dev, 0, eth->dev_addr, 3) < 0) {
					DM_TRACE("eeprom write fail!\r\n");
				}			
			}
		}
		else {
			DM_TRACE("eeprom read fail!\r\n");
		}
	}

	DM9051_getMacAddr(spi_dev, mac_addr);
#ifdef DM9051_DEBUG
	DM_TRACE("macAddr:");
	for (uint8_t i = 0; i < 6; i++)
	{
		DM_TRACE("%02X ", mac_addr[i]);
	}
	DM_TRACE("\r\n**********************************************\r\n");
#endif
	/* Setup DM9051 Registers */
	DM9051_write_reg(spi_dev, DM9051_NCR, NCR_DEFAULT);
	DM9051_write_reg(spi_dev, DM9051_TCR, TCR_DEFAULT);
	DM9051_write_reg(spi_dev, DM9051_RCR, RCR_DEFAULT);
	DM9051_write_reg(spi_dev, DM9051_BPTR, BPTR_DEFAULT);
	DM9051_write_reg(spi_dev, DM9051_FCTR, 0x3A);
	DM9051_write_reg(spi_dev, DM9051_FCR,  FCR_DEFAULT);
	DM9051_write_reg(spi_dev, DM9051_SMCR, SMCR_DEFAULT);
	DM9051_write_reg(spi_dev, DM9051_TCR2, DM9051_TCR2_SET);
	DM9051_write_reg(spi_dev, DM9051_INTR, 0x00);
	DM9051_write_reg(spi_dev, DM9051_RSR, 0xff);
	
	/* Clear status */
	DM9051_write_reg(spi_dev, DM9051_NSR, NSR_CLR_STATUS);
	DM9051_write_reg(spi_dev, DM9051_ISR, ISR_CLR_STATUS);

	uint8_t i = 0;
	uint16_t value;
	if (init.mode == DM9051_AUTO) {
		do {
			if (DM9051_phy_read(spi_dev, DM9051_PHY_STATUS, &value) < 0) {
				return -1;
			}
			delay(20);
			if (i++ == 255)	{
				DM_TRACE("could not establish link \r\n");
				return RT_ERROR;
			}
		} while (!(value & 0x20));
	} 
	
	DM9051_phy_read(spi_dev, 17, &value);
	int lnk = value >> 12;
	switch (lnk)
	{
	case 1: DM_TRACE("10M half duplex ");  info.mode = DM9051_10MHD;  break;
	case 2: DM_TRACE("10M full duplex ");  info.mode = DM9051_10MFD;  break;
	case 4: DM_TRACE("100M half duplex "); info.mode = DM9051_100MHD; break;
	case 8: DM_TRACE("100M full duplex "); info.mode = DM9051_100MFD; break;
	default:DM_TRACE("unknown: %d ", lnk); break;
	}
	DM_TRACE("mode \r\n");

	DM9051_write_reg(spi_dev, DM9051_RCR, (RCR_DEFAULT | RCR_RXEN));  /* Enable RX */
	DM9051_write_reg(spi_dev, DM9051_IMR, init.imr);    /* Enable TX/RX interrupt mask                    */
	DM9051_write_reg(spi_dev, DM9051_ISR, ISR_CLR_STATUS);

#ifdef DM9051_IRQ_EANBLE
    rt_pin_irq_enable(eth->int_pin, PIN_IRQ_ENABLE);
	eth->send_sem = rt_sem_create("eth_sem", 0, RT_IPC_FLAG_FIFO);
	RT_ASSERT(eth->send_sem != RT_NULL);
#endif

	eth_device_linkchange(&eth->parent, RT_TRUE);
	dm9051_unlock(eth);
	
	return RT_EOK;
}

static rt_err_t DM9051_Send(rt_device_t dev, struct pbuf *p)
{
#ifdef FIFO_OVERFLOW_CHECK
	uint16_t calc_MWR;
	uint16_t len = p->tot_len;
#endif

    struct DM9051_eth *eth;
    struct rt_spi_device *spi_dev;
	uint8_t *pBuf = RT_NULL;
	
	RT_ASSERT(dev != RT_NULL && p != NULL);
	
	eth = (struct DM9051_eth *)dev;
	spi_dev = (struct rt_spi_device *)eth->spi_device;
	
	RT_ASSERT(spi_dev != RT_NULL);
	
	dm9051_lock(eth);
	if (!DM9051_isLink(spi_dev))
	{
		DM_TRACE("link is fail! \r\n");
		dm9051_unlock(eth);
		return RT_ERROR;
	} 
	
	//disable all interrupt
	DM_TRACE("ISR1 Reg = 0x%02X.\r\n", DM9051_read_reg(spi_dev, DM9051_ISR));
//	DM9051_write_reg(spi_dev, DM9051_IMR, init.imr);//clear send interrupt
//	DM9051_write_reg(spi_dev, DM9051_IMR, init.imr & (~IMR_PTM));//clear send interrupt
	
	uint8_t retry_count = 20;
	while(DM9051_read_reg(spi_dev, DM9051_TCR) & DM9051_TCR_SET) //wait other send finish
	{
		rt_thread_mdelay(2);
		if (retry_count -- == 0)
		{
			DM_TRACE("DM9051 wait send timeout !\r\n");
			goto exit;
		}
	}
	
#ifdef FIFO_OVERFLOW_CHECK
	calc_MWR = (DM9051_read_reg(spi_dev, DM9051_MDWAH) << 8) + DM9051_read_reg(spi_dev, DM9051_MDWAL);
	calc_MWR += len;
	if(calc_MWR > 0x0bff) calc_MWR -= 0x0c00;
#endif //FIFO_OVERFLOW_CHECK
	

	if (p->tot_len != p->len)
	{
		pBuf = rt_malloc(p->tot_len);
		if (pBuf == RT_NULL) {
			goto exit;
		}
		pbuf_copy_partial(p, pBuf, p->tot_len, 0);
	}
	
	if (pBuf != RT_NULL) {
		DM9051_write_mem(spi_dev, (uint8_t*)pBuf, p->tot_len);
	} else 	{
		DM9051_write_mem(spi_dev, (uint8_t*)p->payload, p->tot_len);
	}
	
	DM9051_write_reg(spi_dev, DM9051_TXPLL, p->tot_len & 0xff);
    DM9051_write_reg(spi_dev, DM9051_TXPLH, (p->tot_len >> 8) & 0xff);

    DM_TRACE("Tx Read Pointer H = 0x%02X.\r\n", DM9051_read_reg(spi_dev, DM9051_TRPAH));
	DM_TRACE("Tx Read Pointer L = 0x%02X.\r\n", DM9051_read_reg(spi_dev, DM9051_TRPAL));
	DM_TRACE("DM9051_MWRH H = 0x%02X.\r\n", DM9051_read_reg(spi_dev, DM9051_MDWAH));
	DM_TRACE("DM9051_MWRL L = 0x%02X.\r\n", DM9051_read_reg(spi_dev, DM9051_MDWAL));
	
    DM9051_write_reg(spi_dev, DM9051_TCR, TCR_TXREQ);    /* set send requir flag and Cleared after TX complete */
	retry_count = 10;
	while (DM9051_read_reg(spi_dev, DM9051_TCR) & TCR_TXREQ)
	{
		rt_thread_mdelay(5);
		if (retry_count-- == 0)
		{
			goto exit;
		}
	}

	DM_TRACE("******************** send late *******************************\r\n");
	DM_TRACE("Tx control Reg = 0x%02X.\r\n", DM9051_read_reg(spi_dev, DM9051_TCR));
	DM_TRACE("ISR Reg = 0x%02X.\r\n", DM9051_read_reg(spi_dev, DM9051_ISR));
	DM_TRACE("NSR Reg = 0x%02X.\r\n", DM9051_read_reg(spi_dev, DM9051_NSR));
    DM_TRACE("Tx Read Pointer H = 0x%02X.\r\n", DM9051_read_reg(spi_dev, DM9051_TRPAH));
	DM_TRACE("Tx Read Pointer L = 0x%02X.\r\n", DM9051_read_reg(spi_dev, DM9051_TRPAL));
	DM_TRACE("DM9051_MWRH H = 0x%02X.\r\n", DM9051_read_reg(spi_dev, DM9051_MDWAH));
	DM_TRACE("DM9051_MWRL L = 0x%02X.\r\n", DM9051_read_reg(spi_dev, DM9051_MDWAL));	

#ifdef FIFO_OVERFLOW_CHECK
	if(calc_MWR != ((DM9051_read_reg(spi_dev, DM9051_MDWAH) << 8) + DM9051_read_reg(spi_dev, DM9051_MDWAL)))
	{
		DM_CHECK("DM9K MWR Error!! calc_MWR = 0x%X, SendLength = 0x%x\r\n", calc_MWR, len);
		DM_CHECK("MWRH = 0x%X, MWRL = 0x%X\r\n", DM9051_read_reg(spi_dev, DM9051_MDWAH), DM9051_read_reg(spi_dev, DM9051_MDWAL));

		DM9051_write_reg(spi_dev, DM9051_MDWAH, (calc_MWR >> 8) & 0xff);
		DM9051_write_reg(spi_dev, DM9051_MDWAL, calc_MWR & 0xff);
		
		DM9051_write_reg(spi_dev, DM9051_IMR, init.imr);
	
	#ifdef DM9051_PACKET_STATUS_STAT
		pkt_info.tx_err_ctn++;
	#endif
		dm9051_unlock(eth);
		return RT_ERROR;
	}
#endif
	
#ifdef DM9051_PACKET_STATUS_STAT
		pkt_info.total_tx_cnt++;
		pkt_info.tx_ok_ctn++;
#endif
	DM9051_write_reg(spi_dev, DM9051_IMR, init.imr);

	dm9051_unlock(eth);
	return RT_EOK;

exit:
	if (pBuf != RT_NULL)
	{
		rt_free(pBuf);
	}
#ifdef DM9051_PACKET_STATUS_STAT
	pkt_info.tx_err_ctn++;
#endif
	dm9051_unlock(eth);
	return RT_ERROR;
}

static struct pbuf * DM9051_Recv(rt_device_t dev)
{
	uint8_t rxbyte;
	uint16_t rx_len;
	struct pbuf *p;
	
#ifdef FIFO_OVERFLOW_CHECK
	uint16_t calc_MRR;	
#endif
	
    struct DM9051_eth *eth;
    struct rt_spi_device *spi_dev;
	
	RT_ASSERT(dev != RT_NULL && p != NULL);
	
	eth = (struct DM9051_eth *)dev;
	spi_dev = (struct rt_spi_device *)eth->spi_device;
	
	RT_ASSERT(spi_dev != RT_NULL);
	
	dm9051_lock(eth);
	
	if (!DM9051_isLink(spi_dev))
	{
		dm9051_unlock(eth);
		return RT_NULL;
	} 

#ifdef DM9051_DEBUG
	if (DM9051_read_reg(spi_dev, DM9051_NSR) & 0x02)
	{
		DM_TRACE("data is overflow!\r\n");
	#ifdef DM9051_PACKET_STATUS_STAT
		pkt_info.rx_overflow_ctn++;
	#endif
	}
	
	#ifdef DM9051_PACKET_STATUS_STAT
		pkt_info.total_rx_cnt++;
	#endif
#endif	
	
	/* Check packet ready or not                                                                              */
	rxbyte = DM9051_read_reg(spi_dev, DM9051_MRCMDX);
	rxbyte = DM9051_read_reg(spi_dev, DM9051_MRCMDX);

	if ((rxbyte != 1) && (rxbyte != 0))
	{
		DM_TRACE("rxbyte = %02X.\r\n", rxbyte);
		DM_TRACE("DM9051 rx: rx error, stop device \r\n");
#ifdef DM9051_PACKET_STATUS_CHECK
		pkt_info.reset_ctn++;
#endif
	//	DM9051_Init(init.mac_addr);
		/* Reset RX FIFO pointer */
		DM9051_write_reg(spi_dev, DM9051_RCR, RCR_DEFAULT);	//RX disable
		DM9051_write_reg(spi_dev, DM9051_MPCR, 0x01);		//Reset RX FIFO pointer
		
		rt_thread_mdelay(2);
		
 		DM9051_write_reg(spi_dev, DM9051_RCR, (RCR_DEFAULT | RCR_RXEN));		//RX Enable
		DM9051_write_reg(spi_dev, DM9051_IMR, init.imr); 

		dm9051_unlock(eth);	
		
		return RT_NULL;
	}
	
	if (rxbyte)
	{
		uint16_t rx_status;
		uint8_t ReceiveData[4];

#ifdef FIFO_OVERFLOW_CHECK			
		calc_MRR = (DM9051_read_reg(spi_dev, DM9051_MDRAH) << 8) + DM9051_read_reg(spi_dev, DM9051_MDRAL);	//Save RX SRAM pointer
#endif	//FifoPointCheck
						
		DM9051_read_reg(spi_dev, DM9051_MRCMDX);		//dummy read
		DM9051_read_mem(spi_dev, ReceiveData, 4);
		
		rx_status = ReceiveData[0] + (ReceiveData[1] << 8);
		rx_len    = ReceiveData[2] + (ReceiveData[3] << 8);

		DM_TRACE("DM9051 rx: status %04x len %d \r\n", rx_status, rx_len);
		if ((p = pbuf_alloc(PBUF_LINK, rx_len, PBUF_RAM)) == RT_NULL)
		{
			dm9051_unlock(eth);
			return RT_NULL;
		}
		
		DM9051_read_mem(spi_dev, p->payload, rx_len);
		
#ifdef DM9051_PACKET_STATUS_STAT
		pkt_info.total_rx_cnt++;
#endif
		
#ifdef FIFO_OVERFLOW_CHECK
		calc_MRR += (rx_len + 4);
		if(calc_MRR > 0x3fff) calc_MRR -= 0x3400;
	
		if(calc_MRR != ((DM9051_read_reg(spi_dev, DM9051_MDRAH) << 8) + DM9051_read_reg(spi_dev, DM9051_MDRAL)))
		{
			rt_thread_mdelay(2);
			DM_TRACE("DM9K MRR Error!!\r\n");
			DM_TRACE("Predicut RX Read pointer = 0x%X, Current pointer = 0x%X\r\n", \
				calc_MRR, ((DM9051_read_reg(spi_dev, DM9051_MDRAH) << 8) + DM9051_read_reg(spi_dev, DM9051_MDRAL)));
			
			rt_thread_mdelay(2);
			DM9051_write_reg(spi_dev, DM9051_MDRAH, (calc_MRR >> 8) & 0xff);
			DM9051_write_reg(spi_dev, DM9051_MDRAL, calc_MRR & 0xff);
		
		#ifdef DM9051_PACKET_STATUS_STAT
			pkt_info.rx_err_lenCtn++;
		#endif
			dm9051_unlock(eth);
			return RT_NULL;
		}
#endif  	

		if ((rx_status & 0xbf00) || (rx_len < 0x40) || (rx_len > DM9051_PKT_MAX) )
		{ 
			DM_TRACE("rx error: status %04x, rx_len: %d \r\n", rx_status, rx_len);	
			if (rx_status & 0x100)
			{
				DM_TRACE("rx fifo error \r\n");
#ifdef DM9051_PACKET_STATUS_STAT
				pkt_info.rx_err_fifoCtn++;
#endif
			}
		
			if (rx_status & 0x200)
			{
				DM_TRACE("rx crc error \r\n");
#ifdef DM9051_PACKET_STATUS_STAT
				pkt_info.rx_err_crcCtn++;
#endif
			}
		
			if (rx_status & 0x8000)
			{
				DM_TRACE("rx length error \r\n");
#ifdef DM9051_PACKET_STATUS_STAT
				pkt_info.rx_err_lenCtn++;
#endif
			}
			if (rx_len > DM9051_PKT_MAX)
			{
				DM_TRACE("rx length too big \r\n");
#ifdef DM9051_PACKET_STATUS_STAT
				pkt_info.rx_err_lenCtn++;
#endif
			} 
		}
	
#ifdef DM9051_PACKET_STATUS_STAT
		pkt_info.rx_ok_ctn++;
#endif		
		DM_TRACE("read point:  L->%X  H->%X\r\n", DM9051_read_reg(spi_dev, DM9051_RWPAL),DM9051_read_reg(spi_dev, DM9051_RWPAH));
		dm9051_unlock(eth);
		return p;
	} 
	
	dm9051_unlock(eth);
	return RT_NULL;
}

#ifdef DM9051_IRQ_EANBLE
static int8_t DM9051_clearFlag(void *arg)
{	
  struct DM9051_eth *eth = ((struct DM9051_eth *)arg);
  struct rt_spi_device *spi_dev = (struct rt_spi_device *)eth->spi_device;
  uint16_t int_status;
	
  DM9051_write_reg(spi_dev, DM9051_RCR, (RCR_DEFAULT));
  DM9051_write_reg(spi_dev, DM9051_IMR, DM9051_IMR_OFF);		//Clear interrupt flag
	
  /* Got DM9051 interrupt status  */
  int_status = DM9051_read_reg(spi_dev, DM9051_ISR);    
  DM9051_write_reg(spi_dev, DM9051_ISR, (uint8_t)(int_status));    /* Clear ISR status    */ 
                                       
  DM_TRACE("clear DM9051 isr: int status %04x \r\n", DM9051_read_reg(spi_dev, DM9051_ISR)); 
  /* receive overflow                                                                                       */
  if (int_status & ISR_ROS)
  {
    DM_TRACE("overflow \r\n");
  }

  if (int_status & ISR_ROOS)
  {
    DM_TRACE("overflow counter overflow \r\n");
  }
  /* Received the coming packet                                                                             */
  if (int_status & ISR_PRS)
  {  
    /* disable receive interrupt                                                                            */
  }

  /* Transmit Interrupt check                                                                               */
  if (int_status & ISR_PTS)
  {
    /* transmit done                                                                                        */
//    int tx_status = DM9051_read_reg(spi_dev, DM9051_NSR);    /* Got TX status                                        */

//    if (tx_status & (NSR_TX2END | NSR_TX1END))
//    {  	
//                                                                         
//    }
  }
  DM9051_write_reg(spi_dev, DM9051_RCR, (RCR_DEFAULT | RCR_RXEN));
  DM9051_write_reg(spi_dev, DM9051_IMR, init.imr);
  rt_pin_irq_enable(eth->int_pin, PIN_IRQ_ENABLE);
  
  return 0;
}

void DM9051_isr_handler(void *args)
{
  struct DM9051_eth *eth = (struct DM9051_eth *)args;

  rt_pin_irq_enable(eth->int_pin, PIN_IRQ_DISABLE);
  rt_sem_release(eth->send_sem);
}
#endif

/* control the interface */
static rt_err_t DM9051_control(rt_device_t dev, int cmd, void *args)
{
    struct DM9051_eth *eth;
    eth = (struct DM9051_eth *)dev;

    switch (cmd)
    {
    case NIOCTL_GADDR:
        /* get mac address */
        if (args)
            rt_memcpy(args, eth->dev_addr, 6);
        else
            return -RT_ERROR;
        break;

    default:
        break;
    }

    return RT_EOK;
}

/* Open the ethernet interface */
static rt_err_t DM9051_open(rt_device_t dev, uint16_t oflag)
{
    LOG_D("[%s L%d]", __FUNCTION__, __LINE__);
    return RT_EOK;
}

/* Close the interface */
static rt_err_t DM9051_close(rt_device_t dev)
{
    LOG_D("[%s L%d]", __FUNCTION__, __LINE__);
    return RT_EOK;
}

/* Read */
static rt_size_t DM9051_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    LOG_D("[%s L%d]", __FUNCTION__, __LINE__);
    rt_set_errno(-RT_ENOSYS);
    return RT_EOK;
}

/* Write */
static rt_size_t DM9051_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    LOG_D("[%s L%d]", __FUNCTION__, __LINE__);
    rt_set_errno(-RT_ENOSYS);
    return 0;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops dm9051_ops =
{
    DM9051_init,
    DM9051_open,
    DM9051_close,
    DM9051_read,
    DM9051_write,
    DM9051_control
};
#endif /* RT_USING_DEVICE_OPS */

static struct DM9051_eth *dm9051_monitor;

#ifndef DM9051_IRQ_EANBLE
int8_t dem9051_check_recv(void *arg)
{
	int8_t linkStatus;
	static int8_t last_linkStatus;
	struct DM9051_eth *eth = (struct DM9051_eth *)arg;
	struct rt_spi_device *spi_dev = eth->spi_device;
	uint8_t reg;
	
	linkStatus = DM9051_isLink(spi_dev);
	if (linkStatus != last_linkStatus)
	{
		if (linkStatus) eth_device_linkchange(&eth->parent, RT_TRUE);
		else eth_device_linkchange(&eth->parent, RT_FALSE);
		
#ifdef DM9051_PACKET_STATUS_STAT
		pkt_info.link_change_ctn++;
#endif
	}
	last_linkStatus = linkStatus;
	
	if ((reg = DM9051_read_reg(spi_dev, DM9051_ISR)) & (ISR_PRS))
	{
		DM9051_write_reg(spi_dev, DM9051_ISR, reg);
		return 1;
	}
	return 0;
}
#endif

int DM9051_probe(const char *spi_dev_name, const char *device_name, int rst_pin, int int_pin)
{
    struct DM9051_eth *eth;
    uint32_t device_id;

    struct rt_spi_device *spi_device;

    spi_device = (struct rt_spi_device *)rt_device_find(spi_dev_name);
    if (spi_device == RT_NULL)
    {
        LOG_E("[%s:%d] spi device %s not found!.", __FUNCTION__, __LINE__, spi_dev_name);
        return -RT_ENOSYS;
    }

    /* config spi */
    {
        struct rt_spi_configuration cfg;
        cfg.data_width = 8;
        cfg.mode = RT_SPI_MODE_3 | RT_SPI_MSB; /* SPI Compatible: Mode 0 and Mode 3 */
        cfg.max_hz = 40 * 1000 * 1000;               /* 50M */
        rt_spi_configure(spi_device, &cfg);
    }
	
	
    device_id  = DM9051_read_reg(spi_device, DM9051_VIDL);
    device_id |= DM9051_read_reg(spi_device, DM9051_VIDH) << 8;
    device_id |= DM9051_read_reg(spi_device, DM9051_PIDL) << 16;
    device_id |= DM9051_read_reg(spi_device, DM9051_PIDH) << 24;
    LOG_I("[%s L%d] device_id: %08X", __FUNCTION__, __LINE__, device_id);

    if((device_id & 0xffff0000) != (DM9051_ID & 0xffff0000))
    {
        return -1;
    }

    device_id  = DM9051_read_reg(spi_device, DM9051_CHIPR);
    LOG_I("[%s L%d] CHIP Revision: %02X", __FUNCTION__, __LINE__, device_id);

    eth = rt_calloc(1, sizeof(struct DM9051_eth));
    if(!eth)
    {
        return -1;
    }

#ifdef DM9051_IRQ_EANBLE
	eth->clear_interrupt_flag = DM9051_clearFlag;
#else
	eth->check_eth_recv = dem9051_check_recv;
#endif
	
    eth->spi_device = spi_device;
    eth->rst_pin = rst_pin;
	eth->int_pin = int_pin;

	memcmp(&eth->dev_addr[0], &init.mac_addr[0], sizeof(init.mac_addr));

#ifdef DM9051_IRQ_EANBLE
    rt_pin_mode(eth->int_pin, PIN_MODE_INPUT_PULLDOWN);
    device_id = rt_pin_attach_irq(eth->int_pin, PIN_IRQ_MODE_RISING, DM9051_isr_handler, eth); /* default: push-pull, high active. PIN_IRQ_MODE_HIGH_LEVEL or PIN_IRQ_MODE_RISING */
    LOG_D("[%s L%d] rt_pin_attach_irq #%d res:%d \n", __FUNCTION__, __LINE__, eth->int_pin, device_id);
#endif
	
    /* init rt-thread device struct */
    eth->parent.parent.type = RT_Device_Class_NetIf;
#ifdef RT_USING_DEVICE_OPS
    eth->parent.parent.ops = &dm9051_ops;
#else
    eth->parent.parent.init    = DM9051_init;
    eth->parent.parent.open    = DM9051_open;
    eth->parent.parent.close   = DM9051_close;
    eth->parent.parent.read    = DM9051_read;
    eth->parent.parent.write   = DM9051_write;
    eth->parent.parent.control = DM9051_control;
#endif /* RT_USING_DEVICE_OPS */

    /* init rt-thread ethernet device struct */
    eth->parent.eth_rx  = DM9051_Recv;
    eth->parent.eth_tx  = DM9051_Send;

    rt_mutex_init(&eth->lock, "dm9051", RT_IPC_FLAG_FIFO);

    dm9051_monitor = eth;
    eth_device_init(&eth->parent, (void *)device_name);
    return 0;
}

static int dm9051_dump(void)
{
    uint32_t value;
    uint32_t pos = 0;
    struct rt_spi_device *spi_device = dm9051_monitor->spi_device;

    pos=0;
    value = DM9051_read_reg(spi_device, pos);
    rt_kprintf("#%02X 0x%02X NCR\n", pos, value);

    pos=1;
    value = DM9051_read_reg(spi_device, pos);
    rt_kprintf("#%02X 0x%02X NSR\n", pos, value);

    pos=2;
    value = DM9051_read_reg(spi_device, pos);
    rt_kprintf("#%02X 0x%02X TCR\n", pos, value);

    pos=3;
    value = DM9051_read_reg(spi_device, pos);
    rt_kprintf("#%02X 0x%02X TSR1\n", pos, value);

    pos=4;
    value = DM9051_read_reg(spi_device, pos);
    rt_kprintf("#%02X 0x%02X TSR2\n", pos, value);

    pos=5;
    value = DM9051_read_reg(spi_device, pos);
    rt_kprintf("#%02X 0x%02X RCR\n", pos, value);

    pos=6;
    value = DM9051_read_reg(spi_device, pos);
    rt_kprintf("#%02X 0x%02X RSR\n", pos, value);

    pos=7;
    value = DM9051_read_reg(spi_device, pos);
    rt_kprintf("#%02X 0x%02X ROCR, recv overflow count\n", pos, value);

    pos=0x39;
    value = DM9051_read_reg(spi_device, pos);
    rt_kprintf("#%02X 0x%02X INTCR\n", pos, value);

    pos=0x3E;
    value = DM9051_read_reg(spi_device, pos);
    rt_kprintf("#%02X 0x%02X EEE_IN, 802.3az enter count\n", pos, value);

    pos=0x3F;
    value = DM9051_read_reg(spi_device, pos);
    rt_kprintf("#%02X 0x%02X EEE_OUT, 802.3az leave count\n", pos, value);

    pos=0x59;
    value = DM9051_read_reg(spi_device, pos);
    rt_kprintf("#%02X 0x%02X memory control. ", pos, value);
    rt_kprintf("%s \n", 
    (value&0x01)?"By 5AH":"3K for TX, 13K for RX." );

    if(value&0x01)
    {
        pos = 0x5A;
        value = DM9051_read_reg(spi_device, pos);
        rt_kprintf("#%02X 0x%02X TRAM_SIZE.\n", pos, value);
    }

    pos=0x7E;
    value = DM9051_read_reg(spi_device, pos);
    rt_kprintf("#%02X 0x%02X ISR, interrupt status\n", pos, value);

    pos=0x7F;
    value = DM9051_read_reg(spi_device, pos);
    rt_kprintf("#%02X 0x%02X IMR, interrupt mask\n", pos, value);

    return 0;
}
MSH_CMD_EXPORT(dm9051_dump, dump dm9051 register);

#ifdef DM9051_PACKET_STATUS_STAT
static int dm9051_netstat(void)
{
	rt_kprintf("\r-----------------------------------------------------------------------------\r\n");
	rt_kprintf(" Ifce        RXok      RXerrLen      RXerrCrc      RXerrFifo      Rxoverflow\r\n");
	rt_kprintf("\r%-*.s %10d    %10d    %10d     %10d     %10d \r\n", 6, "dm9051", pkt_info.rx_ok_ctn, 
			pkt_info.rx_err_lenCtn, pkt_info.rx_err_crcCtn, pkt_info.rx_err_fifoCtn, pkt_info.rx_overflow_ctn);
	rt_kprintf("\r-----------------------------------------------------------------------------\r\n");
	rt_kprintf("\r Ifce        TXok       Txtotal      Reset      LINKchange\r\n");
	rt_kprintf("\r%-*.s %10d   %10d  %10d      %10d \r\n", 6, "dm9051", pkt_info.tx_ok_ctn, pkt_info.total_tx_cnt, \
									pkt_info.reset_ctn, pkt_info.link_change_ctn);
	
	return 0;
}
MSH_CMD_EXPORT(dm9051_netstat, status of network card);
#endif















