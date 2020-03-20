/**********************************************************************************************************
 *@fileName: sparyCmd.h
 *@platform: stm32l432kbxx + mdk5.2.26.0
 *@version:  v1.0.0
 *@describe: independent source files about  spary communication command
 *@note:
**********************************************************************************************************
 version          date          Author          modification
----------------------------------------------------------------------------------------------------------
  v1.0.0         190715         EtcFly         Create  file
**********************************************************************************************************/
#ifndef _SPARY_CMD_H
#define _SPARY_CMD_H



#pragma pack(1)

/** 命令的格式 **/
struct cmd_rebootType
{
  uint8_t b_reboot;
  uint8_t rsv[3];
};

struct cmd_swSparyType
{
  uint8_t b_start;
  uint8_t rsv[3];
};

struct cmd_setSparyTimeType
{
  uint16_t sparyTime;
  uint8_t rsv[2];
};

struct cmd_setSparyModeType
{
  uint8_t mode;
  uint8_t rsv[3];
};

struct cmd_clearErrType
{
  uint8_t rsv[4];
};

struct cmd_setNozzleType
{
  uint8_t type;
  uint8_t rsv[3];
};


struct cmd_setRunStepType
{
  uint32_t step;
};

struct cmd_setNodeIdType
{
  uint8_t nodeID;
  uint8_t rsv[3];
};

struct cmd_setCodeUpdateType
{
  uint8_t b_update;
  uint8_t rsv[3];
};

struct cmd_setParaStored
{
  uint16_t wIndex;
  uint8_t  subIndex;
  uint8_t  rsv;
};

#pragma pack()






#ifdef _cplusplus
extern "C" {
#endif

//..................

#ifdef _cplusplus
}
#endif	//_cplusplus


#endif //_SPARY_CMD_H	
