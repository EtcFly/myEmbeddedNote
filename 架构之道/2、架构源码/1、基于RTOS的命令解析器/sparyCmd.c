/**********************************************************************************************************
 *@fileName: sparyCmd.c
 *@platform: stm32l432kbxx + mdk5.2.26.0
 *@version:  v1.0.0
 *@describe: Command manager and Parse thread
 *@note:
**********************************************************************************************************
 version          date          Author          modification
----------------------------------------------------------------------------------------------------------
  v1.0.0         190715         EtcFly         Create  file
  v1.0.1         190725         EtcFly         Modify command access to command queue access
**********************************************************************************************************/
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "ObjDict.h"
#include "canfestival.h"
#include "app_global.h"
#include "app_config.h"
#include "dev_op.h"
#include "sparyCmd.h"
#include "../midder/cmd_queue.h"
#include "para_stored.h"

static void spary_cmd_reboot(void *arg)
{
    struct cmd_rebootType *para = (struct cmd_rebootType *)arg;

    if (para->b_reboot) {
        HAL_NVIC_SystemReset();
    }
}

static void spary_cmd_swSpary(void *arg)
{
    struct cmd_swSparyType *para = (struct cmd_swSparyType *)arg;

    if (para->b_start) {
        set_motorStartReq();
    }
}

static void spary_cmd_setNodeId(void *arg)
{
    struct cmd_setNodeIdType *para = (struct cmd_setNodeIdType *)arg;
    struct rt_can_filter_config config;

    uint8_t nodeID = set_sysNodeID(para->nodeID);

    if (getNodeId(&RB_Slave_Data) != nodeID)
    {
        config.id = nodeID | (MASTER_ID<<16);
        config.mask = nodeID | (MASTER_ID<<16);
        config.mode = MASK_16B_MODE;
		config.bank = 0;

        if (set_canFilter(&config) != RT_EOK)
        {
            set_sysErrCode(SYS_ERR_FILTER_SET);
        }
    }
}

static void spary_cmd_setSparyMode(void *arg)
{
    struct cmd_setSparyModeType *para = (struct cmd_setSparyModeType *)(void *)arg;

    ctrl_paraType *ctrl = get_devCtrlPoint();
    ctrl->sparyMode = para->mode; 
}

static void spary_cmd_setSparyTime(void *arg)
{
    struct cmd_setSparyTimeType *para = (struct cmd_setSparyTimeType *)(void *)arg;

    ctrl_paraType *ctrl = get_devCtrlPoint();
    ctrl->sparyTime = para->sparyTime;
}

static void spary_cmd_setNozzleType(void *arg )
{
    struct cmd_setNozzleType *para = (struct cmd_setNozzleType *)(void *)arg;

    ctrl_paraType *ctrl = get_devCtrlPoint();    
    ctrl->nozzleType = (enum SparyType)para->type;
	rt_kprintf("nozzleType set :%d\r\n", ctrl->nozzleType);
}

static void spary_cmd_clearErrCode(void *arg)
{
    struct cmd_clearErrType *para = (struct cmd_clearErrType *)(void *)arg;

    (void)para;
    clear_sysErrCode();
}

static void spary_cmd_setRunStep(void *arg)
{
    struct cmd_setRunStepType *para = (struct cmd_setRunStepType *)(void *)arg;

    ctrl_paraType *ctrl = get_devCtrlPoint();  
    ctrl->motorRunStep = para->step;
}

static void spary_cmd_sysCodeUpdate(void *arg)
{
    struct cmd_setCodeUpdateType *para = (struct cmd_setCodeUpdateType *)(void *)arg;

    if (para->b_update)
    {
        //code update    
    }
}

static void spary_cmd_parameterStored(void *arg)
{
    struct cmd_setParaStored *para = (struct cmd_setParaStored *)(void *)arg;

    stored_para(para->wIndex, para->subIndex);
}

static void cmd_execute(void)
{
    cmd_unitType cmd;
    while (cmd_popQueue(&cmd) == RT_EOK)
    {
        switch (cmd.type)
        {
            case CMD_SYS_REBOOT:
            {
                spary_cmd_reboot(&cmd.para);
                break; 
            }

            case CMD_SYS_SWSPARY:
            {
                spary_cmd_swSpary(&cmd.para);
                break;
            }

            case CMD_SYS_SET_NODEID:
            {
                spary_cmd_setNodeId(&cmd.para);
                break;
            }

            case CMD_SYS_SET_SPARY_MODE:
            {
                spary_cmd_setSparyMode(&cmd.para);
                break;
            }

            case CMD_SYS_SET_SPARY_TIME:
            {
                spary_cmd_setSparyTime(&cmd.para);
                break;
            }

            case CMD_SYS_SET_NOZZLE_TYPE:
            {
                spary_cmd_setNozzleType(&cmd.para);
                break;
            }

            case CMD_SYS_CLEAR_ERRCODE:
            {
                spary_cmd_clearErrCode(&cmd.para);
                break;
            }

            case CMD_SYS_SET_RUN_STEP:
            {
                spary_cmd_setRunStep(&cmd.para);
                break;
            }

            case CMD_SYS_CODE_UPDATE:
            {
                spary_cmd_sysCodeUpdate(&cmd.para);
                break;
            }
            
            case CMD_SYS_PARA_STORED:
            {   
                spary_cmd_parameterStored(&cmd.para);
                break;
            }

            default : 
            {
                //invalid command 
                break;
            }
        }
		rt_kprintf("command:%d para:0X%08X\r\n", cmd.type, cmd.para);
    }
}

static void sparyCmdThread(void *p_arg)
{
    while (1)
    {   
        cmd_execute();
        rt_thread_mdelay(100);
    }
}

static int sparyCmdThread_init(void)
{
    rt_thread_t thread;
    thread = rt_thread_create("sparyThread", sparyCmdThread, (void *)0, 2048, PRIORITY_CANOPEN_TIM_TASK, TICK_CANOPEN_TIM_TASK);
    if (RT_NULL != thread)
    {
        if (rt_thread_startup(thread) == RT_EOK)
        {
            rt_kprintf("spary command Create \r\t\t\t\t......................... [ok]\r\n");
            return 0;
        }
    }
    rt_kprintf("spary command Create \r\t\t\t\t......................... [fail]\r\n");
    return 0;
}
INIT_APP_EXPORT(sparyCmdThread_init);
//.....

/**************************************** END OF FILE *************************************/
