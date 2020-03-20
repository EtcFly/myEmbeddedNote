#ifndef _NTP_H
#define _NTP_H






/* for ntpclient */ 
typedef signed char s_char;
typedef unsigned long long tstamp;
typedef unsigned int tdist;

typedef struct _DateTime
{
  uint16_t year[2];
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
} Date;

typedef struct _CONFIG_MSG
{
  uint8_t op[4];//header: FIND;SETT;FACT...
  uint8_t mac[6];
  uint8_t sw_ver[2];
  uint8_t lip[4];
  uint8_t sub[4];
  uint8_t gw[4];
  uint8_t dns[4];	
  uint8_t dhcp;
  uint8_t debug;

  uint16_t fw_len;
  uint8_t state;
  
  Date date;
  
}CONFIG_MSG;


typedef struct _ntpformat
{ 
	uint8_t  dstaddr[4];        /* destination (local) address */
	char    version;        /* version number */
	char    leap;           /* leap indicator */
	char    mode;           /* mode */
	char    stratum;        /* stratum */
	char    poll;           /* poll interval */
	s_char  precision;      /* precision */
	tdist   rootdelay;      /* root delay */
	tdist   rootdisp;       /* root dispersion */
	char    refid;          /* reference ID */
	tstamp  reftime;        /* reference time */
	tstamp  org;            /* origin timestamp */
	tstamp  rec;            /* receive timestamp */
	tstamp  xmt;            /* transmit timestamp */
} ntpformat;

typedef struct _datetime
{
  uint16_t yy;
  uint8_t  mo;
  uint8_t  dd;
  uint8_t  hh;
  uint8_t  mm;
  uint8_t  ss;
} datetime;

#define ntp_port                123                     //ntp server port number
#define SECS_PERDAY     	    86400UL             	// seconds in a day = 60*60*24
#define UTC_ADJ_HRS         	9              	        // SEOUL : GMT+9 
#define EPOCH                   1900                    // NTP start year
#define SOCK_NTP                5
#define TX_RX_MAX_BUF_SIZE      2048



#ifdef _cplusplus
extern "C" {
#endif

void ntpclient_init(void);
void do_ntp_client(void);


#ifdef _cplusplus
}
#endif

#endif


