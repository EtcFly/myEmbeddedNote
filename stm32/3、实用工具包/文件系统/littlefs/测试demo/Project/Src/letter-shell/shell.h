/**
 * @file shell.h
 * @author Letter (NevermindZZT@gmail.cn)
 * @brief letter shell
 * @version 2.0.0
 * @date 2018-12-29
 * 
 * @Copyright (c) 2018 Letter
 * 
 */

#ifndef     __SHELL_H__
#define     __SHELL_H__

#define     SHELL_VERSION               "2.0.2"                 /**< 版本号 */

#define     SHELL_USING_TASK            1                       /**< 是否使用默认shell任务 */
#define     SHELL_USING_CMD_EXPORT      1                       /**< 是否使用命令导出方式 */
#define     SHELL_DISPLAY_RETURN        0                       /**< 是否显示命令调用函数返回值 */
#define     SHELL_TASK_WHILE            0                       /**< 是否使用默认shell任务while循环 */
#define     SHELL_AUTO_PRASE            0                       /**< 是否使用shell参数自动解析 */
#define     SHELL_LONG_HELP             1                       /**< 是否使用shell长帮助 */
#define     SHELL_COMMAND_MAX_LENGTH    50                      /**< shell命令最大长度 */
#define     SHELL_PARAMETER_MAX_NUMBER  30                      /**< shell命令参数最大数量 */
#define     SHELL_HISTORY_MAX_NUMBER    5                       /**< 历史命令记录数量 */
#define     SHELL_DOUBLE_CLICK_TIME     200                     /**< 双击间隔(ms) */
#define     SHELL_MAX_NUMBER            1                       /**< 管理的最大shell数量 */

extern unsigned int HAL_GetTick(void);
#define     SHELL_GET_TICK()            (HAL_GetTick()*10)                       /**< 获取系统时间(ms) */

#define     SHELL_DEFAULT_COMMAND       "\r\n[EtcFly@shell ~]#"          /**< shell默认提示符 */

#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6000000)
    #define SECTION(x)                  __attribute__((section(x)))
	#define SHELL_USED                  __attribute__((used))
	#define SHELL_UNUSED                  __attribute__((unused))
#elif defined(__ICCARM__)
    #define SECTION(x)                  @ x
#elif defined(__GNUC__)
    #define SECTION(x)                  __attribute__((section(x)))
#else
    #define SECTION(x)
#endif

/**
 * @brief shell命令导出
 * 
 * @attention 命令导出方式目前仅支持 keil 所带的编译器，使用时需要在 keil 的
 *            Option for Target 中 Linker 选项卡的 Misc controls 中添加 --keep shellCommand*
 */
#if SHELL_USING_CMD_EXPORT == 1
#if SHELL_LONG_HELP == 1
#define     SHELL_EXPORT_CMD(cmd, func, desc)                               \
            SHELL_USED const char shellCmd##cmd[] = #cmd;                              \
            SHELL_USED const char shellDesc##cmd[] = #desc;                            \
            SHELL_USED const SHELL_CommandTypeDef                                      \
            SHELL_USED shellCommand##cmd SECTION("shellCommand") =                     \
            {                                                               \
                shellCmd##cmd,                                              \
                (int (*)())func,                                            \
                shellDesc##cmd,                                             \
                (void *)0                                                   \
            }
#define     SHELL_EXPORT_CMD_EX(cmd, func, desc, help)                      \
            SHELL_USED const char shellCmd##cmd[] = #cmd;                              \
            SHELL_USED const char shellDesc##cmd[] = #desc;                            \
            SHELL_USED const char shellHelp##cmd[] = #help;                            \
            SHELL_USED const SHELL_CommandTypeDef                                      \
            SHELL_USED shellCommand##cmd SECTION("shellCommand") =                     \
            {                                                               \
                shellCmd##cmd,                                              \
                (int (*)())func,                                            \
                shellDesc##cmd,                                             \
                shellHelp##cmd                                              \
            }
#else /** SHELL_LONG_HELP == 1 */
#define     SHELL_EXPORT_CMD(cmd, func, desc)                               \
            SHELL_USED const char shellCmd##cmd[] = #cmd;                              \
            SHELL_USED const char shellDesc##cmd[] = #desc;                            \
            SHELL_USED const SHELL_CommandTypeDef                                      \
            SHELL_USED shellCommand##cmd SECTION("shellCommand") =                     \
            {                                                               \
                #cmd,                                                       \
                (int (*)())func,                                            \
                #desc                                                       \
            }
#define     SHELL_EXPORT_CMD_EX(cmd, func, desc, help)                      \
            SHELL_USED const char shellCmd##cmd[] = #cmd;                              \
            SHELL_USED const char shellDesc##cmd[] = #desc;                            \
            SHELL_USED const SHELL_CommandTypeDef                                      \
            SHELL_USED shellCommand##cmd SECTION("shellCommand") =                     \
            {                                                               \
                #cmd,                                                       \
                (int (*)())func,                                            \
                #desc,                                                      \
            }
#endif /** SHELL_LONG_HELP == 1 */
#else
#define     SHELL_EXPORT_CMD(cmd, func, desc)
#define     SHELL_EXPORT_CMD_EX(cmd, func, desc, help)
#endif /** SHELL_USING_CMD_EXPORT == 1 */

/**
 * @brief shell命令条目
 * 
 * @note 用于shell命令通过命令表的方式定义
 */
#if SHELL_USING_CMD_EXPORT == 0
#if SHELL_LONG_HELP == 1
#define     SHELL_CMD_ITEM(cmd, func, desc)                                 \
            {                                                               \
                #cmd,                                                       \
                (int (*)())func,                                            \
                #desc,                                                      \
                (void *)0                                                   \
            }
#define     SHELL_CMD_ITEM_EX(cmd, func, desc, help)                        \
            {                                                               \
                #cmd,                                                       \
                (int (*)())func,                                            \
                #desc,                                                      \
                #help                                                       \
            }   
#else /** SHELL_LONG_HELP == 1 */
#define     SHELL_CMD_ITEM(cmd, func, desc)                                 \
            {                                                               \
                #cmd,                                                       \
                (int (*)())func,                                            \
                #desc                                                       \
            }
#define     SHELL_CMD_ITEM_EX(cmd, func, desc, help)                        \
            {                                                               \
                #cmd,                                                       \
                (int (*)())func,                                            \
                #desc,                                                      \
            }  
#endif /** SHELL_LONG_HELP == 1 */
#endif /** SHELL_USING_CMD_EXPORT == 0 */

/**
 * @brief shell读取数据函数原型
 * 
 * @param char shell读取的字符
 * 
 * @return char 0 读取数据成功
 * @return char -1 读取数据失败
 */
typedef signed char (*shellRead)(char *);

/**
 * @brief shell写数据函数原型
 * 
 * @param const char 需写的字符
 */
typedef void (*shellWrite)(const char);

/**
 * @brief shell指令执行函数原型
 * 
 */
typedef int (*shellFunction)();


/**
 * @brief shell输入状态
 * 
 */
typedef enum
{
    SHELL_IN_NORMAL = 0,
    SHELL_ANSI_ESC,
    SHELL_ANSI_CSI,
}SHELL_InputMode;


/**
 * @brief shell 命令定义
 * 
 */
typedef struct
{
    const char *name;                                           /**< shell命令名称 */
    shellFunction function;                                     /**< shell命令函数 */
    const char *desc;                                           /**< shell命令描述 */
#if SHELL_LONG_HELP == 1
    const char *help;                                           /**< shell长帮助信息 */
#endif
}SHELL_CommandTypeDef;


/**
 * @brief shell对象定义
 * 
 */
typedef struct
{
    char *command;                                              /**< shell命令提示符 */
    char buffer[SHELL_COMMAND_MAX_LENGTH];                      /**< shell命令缓冲 */
    unsigned short length;                                      /**< shell命令长度 */
    unsigned short cursor;                                      /**< shell光标位置 */
    char *param[SHELL_PARAMETER_MAX_NUMBER];                    /**< shell参数 */
    char history[SHELL_HISTORY_MAX_NUMBER][SHELL_COMMAND_MAX_LENGTH];  /**< 历史记录 */
    unsigned short historyCount;                                /**< 历史记录数量 */
    short historyFlag;                                          /**< 当前记录位置 */
    short historyOffset;                                        /**< 历史记录偏移 */
    SHELL_CommandTypeDef *commandBase;                          /**< 命令表基址 */
    unsigned short commandNumber;                               /**< 命令数量 */
    SHELL_InputMode status;                                     /**< 输入状态 */
    unsigned char isActive;                                     /**< 是否是当前活动shell */
    shellRead read;                                             /**< shell读字符 */
    shellWrite write;                                           /**< shell写字符 */
}SHELL_TypeDef;

void shellInit(SHELL_TypeDef *shell, shellRead readFunc, shellWrite writeFunc);
void shellSetCommandList(SHELL_TypeDef *shell, SHELL_CommandTypeDef *base, unsigned short size);
SHELL_TypeDef *shellGetCurrent(void);
unsigned short shellDisplay(SHELL_TypeDef *shell, const char *string);
void shellHandler(SHELL_TypeDef *shell, char data);

#if (SHELL_USING_TASK == 1u)
void shellTask(void *param);
#endif

#if (SHELL_USING_CMD_EXPORT != 1u)
signed char shellInstallCommand(const char *cmdName, shellFunction func, const char *desc, const char *help);
#endif

#endif

