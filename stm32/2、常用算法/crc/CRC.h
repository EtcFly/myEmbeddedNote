#ifndef _CRC_H
#define _CRC_H


typedef  unsigned int  uint32_t;
typedef  unsigned short int uint16_t;
typedef  unsigned char  uint8_t;

uint16_t CRC16_ModBus(uint8_t pData[], int DataSize);
uint8_t  CRC4_ITU(uint8_t *data, uint8_t length);  
uint8_t  CRC5_EPC(uint8_t *data, uint8_t length);  
uint8_t  CRC5_ITU(uint8_t *data, uint8_t length);  
uint8_t  CRC5_USB(uint8_t *data, uint8_t length);  
uint8_t  CRC6_ITU(uint8_t *data, uint8_t length);  
uint8_t  CRC7_MMC(uint8_t *data, uint8_t length);  
uint8_t  CRC8(uint8_t *data, uint8_t length);  
uint8_t  CRC8_ITU(uint8_t *data, uint8_t length);  
uint8_t  CRC8_ROHC(uint8_t *data, uint8_t length);  
uint8_t  CRC8_MAXIM(uint8_t *data, uint8_t length);//DS18B20  
uint16_t CRC16_IBM(uint8_t *data, uint8_t length);  
uint16_t CRC16_MAXIM(uint8_t *data, uint8_t length);  
uint16_t CRC16_USB(uint8_t *data, uint8_t length);  
uint16_t CRC16_modbus(uint8_t *data, uint8_t length);  
uint16_t CRC16_CCITT(uint8_t *data, uint8_t length);  
uint16_t CRC16_CCITT_FALSE(uint8_t *data, uint8_t length);  
uint16_t CRC16_X25(uint8_t *data, uint8_t length);  
uint16_t CRC16_XMODEM(uint8_t *data, uint8_t length);  
uint16_t CRC16_DNP(uint8_t *data, uint8_t length);  
uint32_t CRC32(uint8_t *data, uint8_t length);  
uint32_t CRC32_MPEG_2(uint8_t *data, uint8_t length);  
uint16_t CRC_Signal(uint16_t Data); 





#endif


