/**********************************************************************************************************
 *@fileName: dm9051_port.h
 *@platform: 
 *@version:  v1.0.0
 *@describe: DM9051NP Network Card Transplantation of interface
 *@note:
**********************************************************************************************************
 version          date          Author          modification                                  
----------------------------------------------------------------------------------------------------------
  v1.0.0         1901126        EtcFly          Create  file
**********************************************************************************************************/
#ifndef _DM9051_PORT_H
#define _DM9051_PORT_H

#define DM9051_DEBUG
#define DM9051_PACKET_STATUS_CHECK
#define FIFO_OVERFLOW_CHECK

typedef struct 
{
	void (*enter_critical)(void);
	void (*exit_critical)(void);
	void (*chip_cs_active)(void);
	void (*chip_cs_inactive)(void);
	int8_t (*spi_rw)(uint8_t sendData, uint8_t *recvData);
	void (*delay_ms)(uint32_t nMs);
#ifdef DM9051_DEBUG
	int (*info_printf)(const char *info, ...);
#endif
}DM_Port_t;




#ifdef __cplusplus
extern "C" {
#endif

extern DM_Port_t  dm9051_port;

#ifdef __cplusplus
}
#endif //END OF __cplusplus

#endif //END OF _DM9051_PORT_H
















