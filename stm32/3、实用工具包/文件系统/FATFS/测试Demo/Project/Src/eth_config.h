
#ifndef _ETH_CONFIG_H
#define _ETH_CONFIG_H







#ifndef DATA_BUF_SIZE
	#define DATA_BUF_SIZE			2048
#endif

//#define USING_SOFT_SPI  


typedef struct
{
	uint8_t  ipv4[4];
	uint16_t port;
}IP_DstType;


#ifdef _cplusplus
extern "C" {
#endif




void eth_init(void);
int32_t eth_tcpServel(uint8_t sn, uint8_t* buf, uint16_t port, 
					void *(*callback)(void *arg, uint16_t size, uint16_t *sendbuff_size, uint32_t *total_size));
int32_t eth_tcpClient(uint8_t sn, uint8_t* buf, uint8_t* destip, uint16_t destport);
int32_t eth_udpServel(uint8_t sn, uint8_t* buf, uint16_t port, 
					void *(*callback)(void *buff, uint16_t rxSize, 
						 uint16_t *sentSize, uint32_t *totalSize));
int32_t eth_udpClient(uint8_t sn, uint8_t* buf, uint32_t len, uint8_t *addr, uint16_t port);

#ifdef _cplusplus
}
#endif

#endif

