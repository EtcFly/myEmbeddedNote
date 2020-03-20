#include "CRC.h"

/**********************************************************************************
//参数：pData[]为指向要计算的CRC数组      DataSize为计算的大小 
//返回：计算得到的CRC校验值  2Byte 
**********************************************************************************/
uint16_t CRC16_ModBus(uint8_t pData[], int DataSize)
{	
	uint8_t index_j;
	uint16_t CRC_Reg = 0XFFFF;
	do 
	{	 
		CRC_Reg ^= *pData++;
		for (index_j = 0; index_j < 8; index_j++)
		{
			
			if (CRC_Reg&0x0001)	CRC_Reg = (CRC_Reg>>1) ^ 0XA001;
			else CRC_Reg >>= 1;		
		}
	}while (--DataSize);
	return CRC_Reg;
}

/****************************************************************************** 
 * Name:    CRC-4/ITU           x4+x+1 
 * Poly:    0x03 
 * Init:    0x00 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0x00 
 * Note: 
 *****************************************************************************/  
uint8_t CRC4_ITU(uint8_t *data, uint8_t length)  
{  
    uint8_t i;  
    uint8_t crc = 0;                // Initial value  
    while(length--)  
    {  
        crc ^= *data++;                 // crc ^= *data; data++;  
        for (i = 0; i < 8; ++i)  
        {  
            if (crc & 1)  
                crc = (crc >> 1) ^ 0x0C;// 0x0C = (reverse 0x03)>>(8-4)  
            else  
                crc = (crc >> 1);  
        }  
    }  
    return crc;  
}  
  
/****************************************************************************** 
 * Name:    CRC-5/EPC           x5+x3+1 
 * Poly:    0x09 
 * Init:    0x09 
 * Refin:   False 
 * Refout:  False 
 * Xorout:  0x00 
 * Note: 
 *****************************************************************************/  
uint8_t CRC5_EPC(uint8_t *data, uint8_t length)  
{  
    uint8_t i;  
    uint8_t crc = 0x48;        // Initial value: 0x48 = 0x09<<(8-5)  
    while(length--)  
    {  
        crc ^= *data++;        // crc ^= *data; data++;  
        for ( i = 0; i < 8; i++ )  
        {  
            if ( crc & 0x80 )  
                crc = (crc << 1) ^ 0x48;        // 0x48 = 0x09<<(8-5)  
            else  
                crc <<= 1;  
        }  
    }  
    return crc >> 3;  
}  
  
/****************************************************************************** 
 * Name:    CRC-5/ITU           x5+x4+x2+1 
 * Poly:    0x15 
 * Init:    0x00 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0x00 
 * Note: 
 *****************************************************************************/  
uint8_t CRC5_ITU(uint8_t *data, uint8_t length)  
{  
    uint8_t i;  
    uint8_t crc = 0;                // Initial value  
    while(length--)  
    {  
        crc ^= *data++;                 // crc ^= *data; data++;  
        for (i = 0; i < 8; ++i)  
        {  
            if (crc & 1)  
                crc = (crc >> 1) ^ 0x15;// 0x15 = (reverse 0x15)>>(8-5)  
            else  
                crc = (crc >> 1);  
        }  
    }  
    return crc;  
}  
  
/****************************************************************************** 
 * Name:    CRC-5/USB           x5+x2+1 
 * Poly:    0x05 
 * Init:    0x1F 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0x1F 
 * Note: 
 *****************************************************************************/  
uint8_t CRC5_USB(uint8_t *data, uint8_t length)  
{  
    uint8_t i;  
    uint8_t crc = 0x1F;                // Initial value  
    while(length--)  
    {  
        crc ^= *data++;                 // crc ^= *data; data++;  
        for (i = 0; i < 8; ++i)  
        {  
            if (crc & 1)  
                crc = (crc >> 1) ^ 0x14;// 0x14 = (reverse 0x05)>>(8-5)  
            else  
                crc = (crc >> 1);  
        }  
    }  
    return crc ^ 0x1F;  
}  
  
/****************************************************************************** 
 * Name:    CRC-6/ITU           x6+x+1 
 * Poly:    0x03 
 * Init:    0x00 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0x00 
 * Note: 
 *****************************************************************************/  
uint8_t CRC6_ITU(uint8_t *data, uint8_t length)  
{  
    uint8_t i;  
    uint8_t crc = 0;         // Initial value  
    while(length--)  
    {  
        crc ^= *data++;        // crc ^= *data; data++;  
        for (i = 0; i < 8; ++i)  
        {  
            if (crc & 1)  
                crc = (crc >> 1) ^ 0x30;// 0x30 = (reverse 0x03)>>(8-6)  
            else  
                crc = (crc >> 1);  
        }  
    }  
    return crc;  
}  
  
/****************************************************************************** 
 * Name:    CRC-7/MMC           x7+x3+1 
 * Poly:    0x09 
 * Init:    0x00 
 * Refin:   False 
 * Refout:  False 
 * Xorout:  0x00 
 * Use:     MultiMediaCard,SD,ect. 
 *****************************************************************************/  
1、简易版
 uint8_t CRC7_MMC(uint8_t *data, uint8_t length)  
{  
    uint8_t i;  
    uint8_t crc = 0;        // Initial value  
    while(length--)  
    {  
        crc ^= *data++;        // crc ^= *data; data++;  
        for ( i = 0; i < 8; i++ )  
        {  
            if ( crc & 0x80 )  
                crc = (crc << 1) ^ 0x12;        // 0x12 = 0x09<<(8-7)  
            else  
                crc <<= 1;  
        }  
    }  
    return crc >> 1;  
}  
2、高速版
static unsigned char auchCRCHi[] =
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40
} ;

static unsigned char auchCRCLo[] =
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
    0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
    0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
    0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
    0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
    0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
    0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
    0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
    0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
    0x40
};
unsigned char CRC7_SDCard(unsigned char *pData, int cnt)
{
    unsigned char i, j;
    unsigned char crc=0, dat;
    for (j=0; j<cnt; j++)
    {
        dat = pData[j];
        for (i=0;i<8;i++)
        {
            crc <<= 1;
            if ((dat & 0x80)^(crc & 0x80))
            {
                crc ^=0x09;
            }
            dat <<= 1;
        }
    }
    return(crc<<1);
}  
/****************************************************************************** 
 * Name:    CRC-8               x8+x2+x+1 
 * Poly:    0x07 
 * Init:    0x00 
 * Refin:   False 
 * Refout:  False 
 * Xorout:  0x00 
 * Note: 
 *****************************************************************************/  
uint8_t CRC8(uint8_t *data, uint8_t length)  
{  
    uint8_t i;  
    uint8_t crc = 0;        // Initial value  
    while(length--)  
    {  
        crc ^= *data++;        // crc ^= *data; data++;  
        for ( i = 0; i < 8; i++ )  
        {  
            if ( crc & 0x80 )  
                crc = (crc << 1) ^ 0x07;  
            else  
                crc <<= 1;  
        }  
    }  
    return crc;  
}  
  
/****************************************************************************** 
 * Name:    CRC-8/ITU           x8+x2+x+1 
 * Poly:    0x07 
 * Init:    0x00 
 * Refin:   False 
 * Refout:  False 
 * Xorout:  0x55 
 * Alias:   CRC-8/ATM 
 *****************************************************************************/  
uint8_t CRC8_ITU(uint8_t *data, uint8_t length)  
{  
    uint8_t i;  
    uint8_t crc = 0;        // Initial value  
    while(length--)  
    {  
        crc ^= *data++;        // crc ^= *data; data++;  
        for ( i = 0; i < 8; i++ )  
        {  
            if ( crc & 0x80 )  
                crc = (crc << 1) ^ 0x07;  
            else  
                crc <<= 1;  
        }  
    }  
    return crc ^ 0x55;  
}  
  
/****************************************************************************** 
 * Name:    CRC-8/ROHC          x8+x2+x+1 
 * Poly:    0x07 
 * Init:    0xFF 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0x00 
 * Note: 
 *****************************************************************************/  
uint8_t CRC8_ROHC(uint8_t *data, uint8_t length)  
{  
    uint8_t i;  
    uint8_t crc = 0xFF;         // Initial value  
    while(length--)  
    {  
        crc ^= *data++;            // crc ^= *data; data++;  
        for (i = 0; i < 8; ++i)  
        {  
            if (crc & 1)  
                crc = (crc >> 1) ^ 0xE0;        // 0xE0 = reverse 0x07  
            else  
                crc = (crc >> 1);  
        }  
    }  
    return crc;  
}  
  
/****************************************************************************** 
 * Name:    CRC-8/MAXIM         x8+x5+x4+1 
 * Poly:    0x31 
 * Init:    0x00 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0x00 
 * Alias:   DOW-CRC,CRC-8/IBUTTON 
 * Use:     Maxim(Dallas)'s some devices,e.g. DS18B20 
 *****************************************************************************/  
uint8_t CRC8_MAXIM(uint8_t *data, uint8_t length)  
{  
    uint8_t i;  
    uint8_t crc = 0;         // Initial value  
    while(length--)  
    {  
        crc ^= *data++;        // crc ^= *data; data++;  
        for (i = 0; i < 8; i++)  
        {  
            if (crc & 1)  
                crc = (crc >> 1) ^ 0x8C;        // 0x8C = reverse 0x31  
            else  
                crc >>= 1;  
        }  
    }  
    return crc;  
}  
  
/****************************************************************************** 
 * Name:    CRC-16/IBM          x16+x15+x2+1 
 * Poly:    0x8005 
 * Init:    0x0000 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0x0000 
 * Alias:   CRC-16,CRC-16/ARC,CRC-16/LHA 
 *****************************************************************************/  
uint16_t CRC16_IBM(uint8_t *data, uint8_t length)  
{  
    uint8_t i;  
    uint16_t crc = 0;        // Initial value  
    while(length--)  
    {  
        crc ^= *data++;        // crc ^= *data; data++;  
        for (i = 0; i < 8; ++i)  
        {  
            if (crc & 1)  
                crc = (crc >> 1) ^ 0xA001;        // 0xA001 = reverse 0x8005  
            else  
                crc = (crc >> 1);  
        }  
    }  
    return crc;  
}  
  
/****************************************************************************** 
 * Name:    CRC-16/MAXIM        x16+x15+x2+1 
 * Poly:    0x8005 
 * Init:    0x0000 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0xFFFF 
 * Note: 
 *****************************************************************************/  
uint16_t CRC16_MAXIM(uint8_t *data, uint8_t length)  
{  
    uint8_t i;  
    uint16_t crc = 0;        // Initial value  
    while(length--)  
    {  
        crc ^= *data++;        // crc ^= *data; data++;  
        for (i = 0; i < 8; ++i)  
        {  
            if (crc & 1)  
                crc = (crc >> 1) ^ 0xA001;        // 0xA001 = reverse 0x8005  
            else  
                crc = (crc >> 1);  
        }  
    }  
    return ~crc;    // crc^0xffff  
}  
  
/****************************************************************************** 
 * Name:    CRC-16/USB          x16+x15+x2+1 
 * Poly:    0x8005 
 * Init:    0xFFFF 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0xFFFF 
 * Note: 
 *****************************************************************************/  
uint16_t CRC16_USB(uint8_t *data, uint8_t length)  
{  
    uint8_t i;  
    uint16_t crc = 0xffff;        // Initial value  
    while(length--)  
    {  
        crc ^= *data++;            // crc ^= *data; data++;  
        for (i = 0; i < 8; ++i)  
        {  
            if (crc & 1)  
                crc = (crc >> 1) ^ 0xA001;        // 0xA001 = reverse 0x8005  
            else  
                crc = (crc >> 1);  
        }  
    }  
    return ~crc;    // crc^0xffff  
}  
  
/****************************************************************************** 
 * Name:    CRC-16/MODBUS       x16+x15+x2+1 
 * Poly:    0x8005 
 * Init:    0xFFFF 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0x0000 
 * Note: 
 *****************************************************************************/  
uint16_t CRC16_modbus(uint8_t *data, uint8_t length)  
{  
    uint8_t i;  
    uint16_t crc = 0xffff;        // Initial value  
    while(length--)  
    {  
        crc ^= *data++;            // crc ^= *data; data++;  
        for (i = 0; i < 8; ++i)  
        {  
            if (crc & 1)  
                crc = (crc >> 1) ^ 0xA001;        // 0xA001 = reverse 0x8005  
            else  
                crc = (crc >> 1);  
        }  
    }  
    return crc;  
}  
  
/****************************************************************************** 
 * Name:    CRC-16/CCITT        x16+x12+x5+1 
 * Poly:    0x1021 
 * Init:    0x0000 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0x0000 
 * Alias:   CRC-CCITT,CRC-16/CCITT-TRUE,CRC-16/KERMIT 
 *****************************************************************************/  
uint16_t CRC16_CCITT(uint8_t *data, uint8_t length)  
{  
    uint8_t i;  
    uint16_t crc = 0;        // Initial value  
    while(length--)  
    {  
        crc ^= *data++;        // crc ^= *data; data++;  
        for (i = 0; i < 8; ++i)  
        {  
            if (crc & 1)  
                crc = (crc >> 1) ^ 0x8408;        // 0x8408 = reverse 0x1021  
            else  
                crc = (crc >> 1);  
        }  
    }  
    return crc;  
}  
  
/****************************************************************************** 
 * Name:    CRC-16/CCITT-FALSE   x16+x12+x5+1 
 * Poly:    0x1021 
 * Init:    0xFFFF 
 * Refin:   False 
 * Refout:  False 
 * Xorout:  0x0000 
 * Note: 
 *****************************************************************************/  
uint16_t CRC16_CCITT_FALSE(uint8_t *data, uint8_t length)  
{  
    uint8_t i;  
    uint16_t crc = 0xffff;        //Initial value  
    while(length--)  
    {  
        crc ^= (uint16_t)(*data++) << 8; // crc ^= (uint6_t)(*data)<<8; data++;  
        for (i = 0; i < 8; ++i)  
        {  
            if ( crc & 0x8000 )  
                crc = (crc << 1) ^ 0x1021;  
            else  
                crc <<= 1;  
        }  
    }  
    return crc;  
}  
  
/****************************************************************************** 
 * Name:    CRC-16/X25          x16+x12+x5+1 
 * Poly:    0x1021 
 * Init:    0xFFFF 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0XFFFF 
 * Note: 
 *****************************************************************************/  
uint16_t CRC16_X25(uint8_t *data, uint8_t length)  
{  
    uint8_t i;  
    uint16_t crc = 0xffff;        // Initial value  
    while(length--)  
    {  
        crc ^= *data++;            // crc ^= *data; data++;  
        for (i = 0; i < 8; ++i)  
        {  
            if (crc & 1)  
                crc = (crc >> 1) ^ 0x8408;        // 0x8408 = reverse 0x1021  
            else  
                crc = (crc >> 1);  
        }  
    }  
    return ~crc;                // crc^Xorout  
}  
  
/****************************************************************************** 
 * Name:    CRC-16/XMODEM       x16+x12+x5+1 
 * Poly:    0x1021 
 * Init:    0x0000 
 * Refin:   False 
 * Refout:  False 
 * Xorout:  0x0000 
 * Alias:   CRC-16/ZMODEM,CRC-16/ACORN 
 *****************************************************************************/ 
1、简易版： 
uint16_t CRC16_XMODEM(uint8_t *data, uint8_t length)  
{  
    uint8_t i;  
    uint16_t crc = 0;            // Initial value  
    while(length--)  
    {  
        crc ^= (uint16_t)(*data++) << 8; // crc ^= (uint16_t)(*data)<<8; data++;  
        for (i = 0; i < 8; ++i)  
        {  
            if ( crc & 0x8000 )  
                crc = (crc << 1) ^ 0x1021;  
            else  
                crc <<= 1;  
        }  
    }  
    return crc;  
}  
2、高速版
uint16 const CRCTable[256]=
{
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
	0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
	0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
	0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
	0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
	0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
	0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
	0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
	0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
	0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
	0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
	0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
	0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
	0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
	0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
	0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
	0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
	0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
	0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
	0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
	0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
	0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
	0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
	0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
	0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
	0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
	0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
	0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
	0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
	0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
	0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
	0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
}; 

uint16_t CRC16_XMODEM(uint8_t *data, uint8_t length)
{
	uint16_t i;
	uint16_t result = 0;
	for (i = 0; i < len; i++)
	result = (result << 8) ^ (uint16)CRCTable[(result >> 8) ^ *data++];
	return result;
} 
/****************************************************************************** 
 * Name:    CRC-16/DNP          x16+x13+x12+x11+x10+x8+x6+x5+x2+1 
 * Poly:    0x3D65 
 * Init:    0x0000 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0xFFFF 
 * Use:     M-Bus,ect. 
 *****************************************************************************/  
uint16_t CRC16_DNP(uint8_t *data, uint8_t length)  
{  
    uint8_t i;  
    uint16_t crc = 0;            // Initial value  
    while(length--)  
    {  
        crc ^= *data++;            // crc ^= *data; data++;  
        for (i = 0; i < 8; ++i)  
        {  
            if (crc & 1)  
                crc = (crc >> 1) ^ 0xA6BC;        // 0xA6BC = reverse 0x3D65  
            else  
                crc = (crc >> 1);  
        }  
    }  
    return ~crc;                // crc^Xorout  
}  
  
/****************************************************************************** 
 * Name:    CRC-32  x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1 
 * Poly:    0x4C11DB7 
 * Init:    0xFFFFFFF 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0xFFFFFFF 
 * Alias:   CRC_32/ADCCP 
 * Use:     WinRAR,ect. 
 *****************************************************************************/  
uint32_t CRC32(uint8_t *data, uint8_t length)  
{  
    uint8_t i;  
    uint32_t crc = 0xffffffff;        // Initial value  
    while(length--)  
    {  
        crc ^= *data++;                // crc ^= *data; data++;  
        for (i = 0; i < 8; ++i)  
        {  
            if (crc & 1)  
                crc = (crc >> 1) ^ 0xEDB88320;// 0xEDB88320= reverse 0x04C11DB7  
            else  
                crc = (crc >> 1);  
        }  
    }  
    return ~crc;  
}  
  
/****************************************************************************** 
 * Name:    CRC-32/MPEG-2  x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1 
 * Poly:    0x4C11DB7 
 * Init:    0xFFFFFFF 
 * Refin:   False 
 * Refout:  False 
 * Xorout:  0x0000000 
 * Note: 
 *****************************************************************************/  
uint32_t CRC32_MPEG_2(uint8_t *data, uint8_t length)  
{  
    uint8_t i;  
    uint32_t crc = 0xffffffff;  // Initial value  
    while(length--)  
    {  
        crc ^= (uint32_t)(*data++) << 24;// crc ^=(uint32_t)(*data)<<24; data++;  
        for (i = 0; i < 8; ++i)  
        {  
            if ( crc & 0x80000000 )  
                crc = (crc << 1) ^ 0x04C11DB7;  
            else  
                crc <<= 1;  
        }  
    }  
    return crc;  
} 

//计算单个Data的CRC计算方法 
uint16_t CRC_Signal(uint16_t Data)
{
	uint8_t index = 0;
	do
	{
		if (Data&0x0001) Data = (Data>>1) ^ 0XA001;
		else Data >>= 1;
	}while (++index < 8);
	return 	Data;
} 