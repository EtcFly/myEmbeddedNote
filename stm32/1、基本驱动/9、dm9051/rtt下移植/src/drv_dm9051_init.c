/**********************************************************************************************************
 *@fileName: drv_dm9051_init.c
 *@platform: 
 *@version:  v1.0.0
 *@describe: Network Card Initialize
 *@note:
**********************************************************************************************************
 version          date          Author          modification                                  
----------------------------------------------------------------------------------------------------------
  v1.0.0         191203        EtcFly          Create  file
  v1.0.1         191213        EtcFly          add support to receive irq
**********************************************************************************************************/
#include <stdint.h>
#include <stdlib.h>

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include "stm32l4xx_hal.h"
#include "drv_spi.h"
#include <netif/ethernetif.h>
#include "drv_dm9051np.h"



int DM9051_probe(const char *spi_dev_name, const char *device_name, int rst_pin, int int_pin);

void eth_thread_entry(void *arg)
{
	struct DM9051_eth *eth = ((struct DM9051_eth *)arg);

	while (1)
	{
#ifdef DM9051_IRQ_EANBLE
		rt_sem_take(eth->send_sem, 500); //RT_WAITING_FOREVER
		eth->clear_interrupt_flag(eth);
#else
		if (eth->check_eth_recv(eth) != 0)
		{	
#endif
		eth_device_ready(&eth->parent);

#ifndef DM9051_IRQ_EANBLE
		}
		rt_thread_mdelay(10);
#endif			
	}
}


int dm9051_auto_init(void)
{
	rt_thread_t thread;
	rt_device_t eth_dev;
	int int_pin = 10;
	
	__HAL_RCC_GPIOD_CLK_ENABLE();
	rt_hw_spi_device_attach("spi2", "spi_eth_dev", GPIOD, GPIO_PIN_12);

#ifdef DM9051_IRQ_PIN
	int_pin = DM9051_IRQ_PIN;
#endif	

    DM9051_probe("spi_eth_dev", DM9051_CARD_NAME, DM9051_RESET_PIN, int_pin);
	
	eth_dev = rt_device_find(DM9051_CARD_NAME);
	RT_ASSERT(eth_dev != RT_NULL);
	
	thread = rt_thread_create("eth_task", eth_thread_entry, (void *)eth_dev, 1024, DM9051_IRQ_TASK_PRIORITY, 10); 
	RT_ASSERT(thread != RT_NULL);
	rt_thread_startup(thread);
	
    return 0;
}
INIT_ENV_EXPORT(dm9051_auto_init);

#include <finsh.h>
MSH_CMD_EXPORT(dm9051_auto_init, dm9051_auto_init);
