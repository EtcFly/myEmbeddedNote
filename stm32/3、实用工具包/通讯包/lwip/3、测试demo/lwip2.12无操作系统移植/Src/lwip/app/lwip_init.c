
#include "lwip/init.h"
#include "lwip/netif.h"
#include "netif/etharp.h"
#include "lwip_init.h"

volatile struct netif *netif_ini;

err_t ethernetif_init(struct netif *netif);
void lwip_initAll(void)
{
	static struct netif eth_dm9051;
	ip_addr_t ipaddr, netmask, gw;
	
	lwip_init();

	IP4_ADDR(&gw, 192,168,13,1);
	IP4_ADDR(&ipaddr, 192,168,13,30);
	IP4_ADDR(&netmask, 255,255,255,0);
	
	eth_dm9051.hwaddr[0] = 0X12;
    eth_dm9051.hwaddr[1] = 0X13;
    eth_dm9051.hwaddr[2] = 0xf6;
    eth_dm9051.hwaddr[3] = 0x00;
    eth_dm9051.hwaddr[4] = 0x09;
    eth_dm9051.hwaddr[5] = 0x12;
	
	netif_ini = netif_add(&eth_dm9051, &ipaddr, &netmask, &gw, NULL, ethernetif_init, ethernet_input);
	if (netif_ini != NULL)
	{
		netif_set_default(&eth_dm9051);
		netif_set_up(&eth_dm9051);
	}	
}

