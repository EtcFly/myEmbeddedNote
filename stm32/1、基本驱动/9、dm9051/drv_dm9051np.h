/**********************************************************************************************************
 *@fileName: drv_dm9051np.h
 *@platform: 
 *@version:  v1.0.0
 *@describe: Achieve DM9051NP Network Card Driver
 *@note:
**********************************************************************************************************
 version          date          Author          modification                                  
----------------------------------------------------------------------------------------------------------
  v1.0.0         1901125        EtcFly          Create  file
**********************************************************************************************************/
#ifndef _DRV_DM9015NP_H
#define _DRV_DM9015NP_H

#include "dm9051_port.h"


#define DM9000_ID           (0x90000A46)    /* DM9000 ID																										*/
#define DM9051_ID           (0x90510A46)    /* DM9051A ID                                                   */
#define DM9051_PKT_MAX      (1536)          /* Received packet max size                                     */
#define DM9051_PKT_RDY      (0x01)          /* Packet ready to receive                                      */

/********** register ***************/
#define DM9051_NCR             	(0x00) //Network control register
#define DM9051_NSR             	(0x01) //Network Status register
#define DM9051_TCR              (0x02) //Network control register
#define DM9051_TSRI             (0X03) //Tx Status register for packet Index I
#define DM9051_TSRII            (0X04) //Tx Status register II for packet index II
#define DM9051_RCR              (0X05) //RX Control register 
#define DM9051_RSR              (0X06) //RX Status register
#define DM9051_ROCR             (0X07) //Receive overflow counter register
#define DM9051_BPTR             (0x08) //Back Pressure threshold register
#define DM9051_FCTR             (0x09) //Flow control threshold register
#define DM9051_FCR              (0x0A) //RX/TX Flow control register
#define DM9051_EPCR             (0x0B) //EEPROM & PHY control register
#define DM9051_EPAR             (0x0C) //EEPROM & PHY Address register
#define DM9051_EPDRL            (0x0D) //EEPROM & PHY Data LOW register
#define DM9051_EPDRH            (0x0E) //EEPROM & PHY Data HIGH register
#define DM9051_WPCR             (0X0F) //Wake up control register

/* Physical address register */
#define DM9051_PHYADDR1         (0X10) 
#define DM9051_PHYADDR2         (0X11)
#define DM9051_PHYADDR3         (0X12)
#define DM9051_PHYADDR4         (0X13)
#define DM9051_PHYADDR5         (0X14)
#define DM9051_PHYADDR6         (0X15)

/* Multicast address hash table register */
#define DM9051_MULTICAST_ADDR1  (0X16)
#define DM9051_MULTICAST_ADDR2  (0X17)
#define DM9051_MULTICAST_ADDR3  (0X18)
#define DM9051_MULTICAST_ADDR4  (0X19)
#define DM9051_MULTICAST_ADDR5  (0X1A)
#define DM9051_MULTICAST_ADDR6  (0X1B)
#define DM9051_MULTICAST_ADDR7  (0X1C)
#define DM9051_MULTICAST_ADDR8  (0X1D)

#define DM9051_GPCR	       		(0x1E) //General Purpose control register
#define DM9051_GPR              (0x1F) //General Purpose register 
#define DM9051_TRPAL            (0x22) //TX Memory read pointer address register Low Byte
#define DM9051_TRPAH            (0x23) //TX Memory read pointer address register High Byte
#define DM9051_RWPAL            (0x24) //RX Memory write pointer address register Low Byte
#define DM9051_RWPAH            (0x25) //RX Memory write pointer address register High Byte

#define DM9051_VIDL             (0x28)
#define DM9051_VIDH             (0x29)
#define DM9051_PIDL             (0x2A)
#define DM9051_PIDH             (0x2B)

#define DM9051_CHIPR            (0x2C)
#define DM9051_TCR2             (0x2D) //Transmit control register
#define DM9051_OTCR             (0x2E)
#define DM9051_SMCR             (0x2F)

#define DM9051_ETCR         (0x30)    /* early transmit control/status register                             */
#define DM9051_CSCR         (0x31)    /* check sum control register                                         */
#define DM9051_RCSSR        (0x32)    /* receive check sum status register   */

#define DM9051_SPI          (0X38) //SPI Bus Control register
#define DM9051_INTR			(0x39) //INT Pin Control register
#define DM9051_PPCSR        (0X3D) //Pause Packet Control/Status register
#define DM9051_IECR         (0X3E) //IEEE 802.3az Enter Counter register
#define DM9051_ILCR         (0X3F) //IEEE 802.3az Leave Counter register
#define DM9051_SAECR        (0X4A) //SPI Byte Align Error Counter register

#define	DM9051_RPLCR        (0X52) //RX Packet Length Control register
#define DM9051_RBCR         (0X53) //RX Broadcast control register
#define DM9051_INT_COCR     (0X54) //INT Pin clock output  control register

#define DM9051_MPCR         (0X55) //Memory Pointer control register
#define DM9051_MLCR         (0X57) //More LED Control register
#define DM9051_MCR          (0X59) //Memory Control register
#define DM9051_TMSR         (0X5A) //Transmit Memory size register
#define DM9051_MBSR         (0X5D) //Memory BIST status register

#define DM9051_MRCMDX       (0X70) //Memory data pre-fetch read command without address incream register
#define DM9051_MRCMDX1      (0X71) //Memory read command without data pre-fetch and address increamemt register
#define DM9051_MRCMD        (0X72) //Memory data read command with address increamemt register
#define DM9051_RD_DLY       (0X73) //SPI Data read delay counter register

#define DM9051_MDRAL        (0X74) //Memory data read address register
#define DM9051_MDRAH        (0X75) 

#define DM9051_MWCMDX       (0X76) //Memory data write command without address increament register
#define DM9051_MWCMD        (0X78) //Memory data write command with address increament register

#define DM9051_MDWAL        (0X7A) //Memory data write address high byte
#define DM9051_MDWAH        (0X7B) 

#define DM9051_TXPLL        (0X7C) //TX Packet Length register
#define DM9051_TXPLH        (0X7D)

#define DM9051_ISR          (0X7E) //Interrupt status register
#define DM9051_IMR          (0X7F) //Interrupt Mask register

//PHY register
#define DM9051_PHY_BMCR      (0X00)
#define DM9051_PHY_STATUS    (0X01)
#define DM9051_PHYID1        (0X02)
#define DM9051_PHYID2        (0X03)
#define DM9051_PHY_AUTO_NEG  (0X04)
#define DM9051_PHY_ANLPAR    (0X05)
#define DM9051_PHY_ANER      (0X06)
#define DM9051_PHY_DSCR      (0X10)
#define DM9051_PHY_SPEC_CFG  (0X11)
#define DM9051_PHY_10T_CFG   (0X12)
#define DM9051_PHY_PWDOR     (0X13)
#define DM9051_PHY_SPEC_CFG1 (0X14)
#define DM9051_PHY_DSP_CTRL  (0X1B)
#define DM9051_PHY_PSCR      (0X1D)

/************ command *******************/
#define CMD_RD_OPCODE                   (0X00)
#define CMD_WR_OPCODE                   (0X80)

#define CMD_REG_WR(reg_addr)			(reg_addr | CMD_WR_OPCODE) //register write
#define CMD_REG_RD(reg_addr)			(reg_addr | CMD_RD_OPCODE) //register read
#define CMD_MEM_DUMMY_RD                (0X70 | CMD_RD_OPCODE) //memory dummy read 
#define CMD_MEM_DUMMY_RD_NO_PRE_FETCH   (0X71 | CMD_RD_OPCODE) //memory dummy read without pre-fetch
#define CMD_MEM_WR                      (0X78 | CMD_WR_OPCODE) //memory write 
#define CMD_MEM_RD                      (0X72 | CMD_RD_OPCODE) //memory read 
#define CMD_AUTO_TRANSMIT               (0X7A | CMD_WR_OPCODE) //Auto transmit 



#pragma pack(1)
/* format of ethernet parameter which stored in extern EEPROM */
typedef struct
{
	unsigned char mac[6];
	unsigned char auto_load_ctl[2];
	unsigned char vendor_id[2];
	unsigned char product_id[2];
	unsigned char pin_ctrl[2];
	unsigned char wk_up_ctrl[2];
	unsigned char ctrl3[2];
}EE_Fmt_t;

#pragma pack()

/**************** register define **************************/
#define DM9051_REG_RESET      0x01

#define GPCR_GPIO_OUTPUT_MODE 0X01
#define GPCR_GPIO_INPUT_MODE  0X00

#define GPCR_GPIO1_POS      (0x01 << 1)
#define GPCR_GPIO2_POS      (0x01 << 2)
#define GPCR_GPIO3_POS      (0x01 << 3)
#define GPCR_GPIO_ALL_POS   (0X07 << 1)


#define EPCR_OPER_EEPROM    (0X00 << 3)
#define EPCR_OPER_PHY       (0X01 << 3)

#define GPCR_GEP_CNTL       (1 << 0)

#define DM9051_REG_RESET      0x01
#define DM9051_IMR_OFF        0x80
#define DM9051_TCR2_SET       0x90	//one packet
#define DM9051_RCR_SET        0x31
#define DM9051_BPTR_SET       0x37
#define DM9051_FCTR_SET       0x38
#define DM9051_FCR_SET        0x28
#define DM9051_TCR_SET        0x01

#define NCR_EXT_PHY         (1 << 7)
#define NCR_WAKEEN          (1 << 6)
#define NCR_FCOL            (1 << 4)
#define NCR_FDX             (1 << 3)
#define NCR_LBK             (3 << 1)
#define NCR_RST             (1 << 0)
#define NCR_DEFAULT		    0x0						// Disable Wakeup

#define NSR_SPEED           (1 << 7)
#define NSR_LINKST          (1 << 6)
#define NSR_WAKEST          (1 << 5)
#define NSR_TX2END          (1 << 3)
#define NSR_TX1END          (1 << 2)
#define NSR_RXOV            (1 << 1)
#define NSR_CLR_STATUS		(NSR_WAKEST | NSR_TX2END | NSR_TX1END)

#define TCR_TJDIS           (1 << 6)
#define TCR_EXCECM          (1 << 5)
#define TCR_PAD_DIS2        (1 << 4)
#define TCR_CRC_DIS2        (1 << 3)
#define TCR_PAD_DIS1        (1 << 2)
#define TCR_CRC_DIS1        (1 << 1)
#define TCR_TXREQ           (1 << 0)		//Start TX
#define TCR_DEFAULT		    0x0

#define RCR_WTDIS           (1 << 6)
#define RCR_DIS_LONG        (1 << 5)
#define RCR_DIS_CRC         (1 << 4)
#define RCR_ALL             (1 << 3)
#define RCR_RUNT            (1 << 2)
#define RCR_PRMSC           (1 << 1)
#define RCR_RXEN            (1 << 0)
#define RCR_DEFAULT		(RCR_DIS_LONG | RCR_DIS_CRC)

#define IMR_PAR             (1 << 7)
#define IMR_LNKCHGI         (1 << 5)
#define IMR_UDRUN			(1 << 4)
#define IMR_ROOM            (1 << 3)
#define IMR_ROM             (1 << 2)
#define IMR_PTM             (1 << 1)
#define IMR_PRM             (1 << 0)
#define IMR_FULL 			(IMR_PAR | IMR_LNKCHGI | IMR_UDRUN | IMR_ROOM | IMR_ROM | IMR_PTM | IMR_PRM)
#define IMR_OFF				IMR_PAR
#define IMR_DEFAULT			(IMR_PAR | IMR_PRM | IMR_PTM) 

#define ISR_LINK            (1 << 5)
#define ISR_ROOS            (1 << 3)
#define ISR_ROS             (1 << 2)
#define ISR_PTS             (1 << 1)
#define ISR_PRS             (1 << 0)
#define ISR_CLR_STATUS      (0x80 | 0x3F)

#define BPTR_DEFAULT	    0x3f
#define FCTR_DEAFULT	    0x38
#define FCR_DEFAULT		    0xFF
#define SMCR_DEFAULT	    0x0
////////////////////////////////////////////////////////////////////////////////////////
//application data struct 

/* Private typedef -----------------------------------------------------------------------------------------*/
enum DM9051_PHY_mode
{
  DM9051_10MHD   = 0,
  DM9051_100MHD  = 1,
  DM9051_10MFD   = 4,
  DM9051_100MFD  = 5,
  DM9051_10M     = 6,
  DM9051_AUTO    = 8,
  DM9051_1M_HPNA = 0x10
};

enum DM9051_TYPE
{
  TYPE_DM9051E,
  TYPE_DM9051A,
  TYPE_DM9051B,
  TYPE_DM9051
};

typedef struct
{
	union 
	{
		uint32_t ver;
		struct pvid
		{
			uint16_t pid;
			uint16_t vid;	
		}pvid;
	}version;
	
	uint8_t  chip_version;
#if 0
	enum DM9051_TYPE type;
#endif
	enum DM9051_PHY_mode mode;
}ChipInfo_t;

typedef struct
{
	enum DM9051_TYPE type;
	enum DM9051_PHY_mode mode;
	uint8_t imr;
	uint8_t mac_addr[6];
}DM9051_Init_t;


#ifdef DM9051_PACKET_STATUS_CHECK
typedef struct
{
	uint32_t link_change_ctn;
	uint32_t rx_overflow_ctn;
	uint32_t total_tx_cnt;
	uint32_t total_rx_cnt;
	uint32_t tx_ok_ctn;
	uint32_t rx_err_crcCtn;
	uint32_t rx_err_lenCtn;
	uint32_t rx_err_fifoCtn;
	uint32_t reset_ctn;
}Packet_Info_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

int8_t get_recvFlag(void);
int8_t DM9051_Init(uint8_t mac[6]);
int8_t DM9051_Send(uint8_t *buff, uint16_t len);
int8_t DM9051_Recv(uint8_t *buff, uint16_t buff_len, uint16_t *recv_len);


#ifdef __cplusplus
}
#endif




#endif
