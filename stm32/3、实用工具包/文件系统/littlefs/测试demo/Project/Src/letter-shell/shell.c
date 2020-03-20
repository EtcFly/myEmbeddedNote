/**
 * @file shell.c
 * @author Letter (NevermindZZT@gmail.cn)
 * @brief letter shell
 * @version 2.0.0
 * @date 2018-12-29
 * 
 * @Copyright (c) 2018 Letter
 * @note: 
 * date:190411  wy  (add the clear command, Modify the way of default table installed) 
 */

#include "shell.h"
#include "string.h"

#if SHELL_AUTO_PRASE == 1
#include "shell_ext.h"
#endif

static SHELL_TypeDef *shellList[SHELL_MAX_NUMBER] = {NULL};     /**< shell列表 */

static void shellAdd(SHELL_TypeDef *shell);
static void shellDisplayItem(SHELL_TypeDef *shell, unsigned short index);
static void shellHelp(int argc, char *argv[]);
static void shellClear(int argc, char *argv[]);

#if SHELL_USING_CMD_EXPORT != 1
/**
 * @brief shell默认命令表
 * 
 * @note 当使用命令表方式定义命令的时候，此表才会生效
 * @note 添加命令时，可使用SHELL_CMD_ITEM宏，如SHELL_CMD_ITEM(help, shellHelp, command help)
 * @note 可不使用默认命令表，初始化完成之后，可调用shellSetCommandList接口设置命令表
 */
SHELL_CommandTypeDef shellDefaultCommandList[SHELL_PARAMETER_MAX_NUMBER] =
{
    SHELL_CMD_ITEM_EX(help,  shellHelp,  command help,  help [command] --show help info of command),
    SHELL_CMD_ITEM_EX(clear, shellClear, clear command, help [command] --show help info of command),
	0, 
};


/**
 * @brief shell添加命令列表
 * 
 * @param pshellCmd 命令对象基地址
 * @param cmdNum 添加的命令数
 * 
 * @note 此接口只能在使用默认shell列表时调用
 * @note 返回-1表示添加出错   0表示添加成功
 */
signed char shellInstallCommand(const char *cmdName, shellFunction func, const char *desc, const char *help)
{
   static unsigned char count = 2;
   
   if (count >= SHELL_PARAMETER_MAX_NUMBER)
   {
      return -1;
   }     
   shellDefaultCommandList[count].desc = desc;
   shellDefaultCommandList[count].function = func;
   shellDefaultCommandList[count].name = cmdName;
 
#if (SHELL_LONG_HELP == 1u) 
   shellDefaultCommandList[count].help = help;
#else
   help = (void *)help;
#endif

   count++;   
   return 0;
}

#endif


/**
 * @brief shell初始化
 * 
 * @param shell shell对象
 */
void shellInit(SHELL_TypeDef *shell, shellRead readFunc, shellWrite writeFunc)
{
    shell->read = readFunc;
    shell->write = writeFunc;
    shell->length = 0;
    shell->cursor = 0;
    shell->historyCount = 0;
    shell->historyFlag = 0;
    shell->historyOffset = 0;
    shell->status = SHELL_IN_NORMAL;
    shell->command = SHELL_DEFAULT_COMMAND;
    shell->isActive = 0;
    shellAdd(shell);
    shellDisplay(shell, shell->command);
    
#if SHELL_USING_CMD_EXPORT == 1
    #if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6000000)
        extern const unsigned int shellCommand$$Base;
        extern const unsigned int shellCommand$$Limit;

        shell->commandBase = (SHELL_CommandTypeDef *)(&shellCommand$$Base);
        shell->commandNumber = ((unsigned int)(&shellCommand$$Limit)
                                - (unsigned int)(&shellCommand$$Base))
                                / sizeof(SHELL_CommandTypeDef);
    #elif defined(__ICCARM__)
        shell->commandBase = (SHELL_CommandTypeDef *)(__section_begin("shellCommand"));
        shell->commandNumber = ((unsigned int)(__section_end("shellCommand"))
                                - (unsigned int)(__section_begin("shellCommand")))
                                / sizeof(SHELL_CommandTypeDef);
    #elif defined(__GNUC__)
        extern const unsigned int _shell_command_start;
        extern const unsigned int _shell_command_end;
        
        shell->commandBase = (SHELL_CommandTypeDef *)(&_shell_command_start);
        shell->commandNumber = ((unsigned int)(&_shell_command_end)
                                - (unsigned int)(&_shell_command_start))
                                / sizeof(SHELL_CommandTypeDef);
    #else
        #error not supported compiler, please use command table mode
    #endif
#else
    shell->commandBase = (SHELL_CommandTypeDef *)shellDefaultCommandList;
    shell->commandNumber = sizeof(shellDefaultCommandList) / sizeof(SHELL_CommandTypeDef);
#endif
    shellDisplay(shell, "\r\n\r\n");
    shellDisplay(shell, "+=========================================================+\r\n");
    shellDisplay(shell, "|                (C) COPYRIGHT 2018 Letter                |\r\n");
    shellDisplay(shell, "|                   Letter shell v"SHELL_VERSION"                   |\r\n");
    shellDisplay(shell, "|               Build: "__DATE__" "__TIME__"               |\r\n");
    shellDisplay(shell, "+=========================================================+\r\n");
}


/**
 * @brief shell设置命令表
 * 
 * @param shell shell对象
 * @param base 命令表基址
 * @param size 命令数量
 * 
 * @note 此接口不可在shellInit之前调用
 * @note 不调用此接口，则使用默认命令表或命令导出形成的命令表(取决于命令定义方式)
 */
void shellSetCommandList(SHELL_TypeDef *shell, SHELL_CommandTypeDef *base, unsigned short size)
{
    shell->commandBase = base;
    shell->commandNumber = size;
}


/**
 * @brief 添加shell到shell列表
 * 
 * @param shell shell对象
 */
static void shellAdd(SHELL_TypeDef *shell)
{
    for (short i = 0; i < SHELL_MAX_NUMBER; i++)
    {
        if (shellList[i] == NULL)
        {
            shellList[i] = shell;
            return;
        }
    }
}


/**
 * @brief 获取当前活动shell
 * 
 * @return SHELL_TypeDef* 当前活动shell对象
 */
SHELL_TypeDef *shellGetCurrent(void)
{
    for (short i = 0; i < SHELL_MAX_NUMBER; i++)
    {
        if (shellList[i] != NULL && shellList[i]->isActive == 1)
        {
            return shellList[i];
        }
    }
    return NULL;
}


/**
 * @brief shell显示字符串
 * 
 * @param shell shell对象
 * @param string 字符串
 * @return unsigned short 字符串长度
 */
unsigned short shellDisplay(SHELL_TypeDef *shell, const char *string)
{
    unsigned short count = 0;
    if (shell->write == NULL)
    {
        return 0;
    }
    while(*string)
    {
        shell->write(*string++);
        count ++;
    }
    return count;
}


/**
 * @brief shell显示字符
 * 
 * @param shell shel对象
 * @param data 字符
 */
static void shellDisplayByte(SHELL_TypeDef *shell, char data)
{
    if (shell->write == NULL)
    {
        return;
    }
    shell->write(data);
}


#if SHELL_DISPLAY_RETURN == 1
/**
 * @brief shell显示函数调用返回值
 * 
 * @param shell shel对象
 * @param value 值
 */
static void shellDisplayReturn(SHELL_TypeDef *shell, int value)
{
    char str[11] = "0000000000";
    unsigned int v = value;
    char i = 10;
    char tmp;

    shellDisplay(shell, "Return: ");
    if (value < 0)
    {
        shellDisplay(shell, "-");
        v = -v;
    }
    while (v)
    {
        str[--i] = v % 10 + 48;
        v /= 10;
    }
    shellDisplay(shell, str + i - (value == 0));
    v = value;
    if (value < 0)
    {
        v = (unsigned int)value;
    }
    i = 8;
    str[8] = 0;
    while (v)
    {
        tmp = v & 0x0000000F;
        str[--i] = (tmp > 9) ? (tmp + 87) : (tmp + 48);
        v >>= 4;
    }
    shellDisplay(shell, ", 0x");
    shellDisplay(shell, str);
    shellDisplay(shell, "\r\n");
}
#endif /** SHELL_DISPLAY_RETURN == 1 */


/**
 * @brief shell字符串复制
 * 
 * @param dest 目标字符串
 * @param src 源字符串
 * @return unsigned short 字符串长度
 */
static unsigned short shellStringCopy(char *dest, char* src)
{
    unsigned short count = 0;
    while (*(src + count))
    {
        *(dest + count) = *(src + count);
        count++;
    }
    *(dest + count) = 0;
    return count;
}


/**
 * @brief shell字符串比较
 * 
 * @param dest 目标字符串
 * @param src 源字符串
 * @return unsigned short 匹配长度
 */
static unsigned short shellStringCompare(char* dest, char *src)
{
    unsigned short match = 0;
    unsigned short i = 0;

    while (*(dest +i) && *(src + i))
    {
        if (*(dest + i) != *(src +i))
        {
            break;
        }
        match ++;
        i++;
    }
    return match;
}


/**
 * @brief shell删除
 * 
 * @param shell shell对象
 * @param length 删除的长度
 */
static void shellDelete(SHELL_TypeDef *shell, unsigned short length)
{
    while (length--)
    {
        shellDisplay(shell, "\b \b");
    }
}


/**
 * @brief shell清除输入
 * 
 * @param shell shell对象
 */
static void shellClearLine(SHELL_TypeDef *shell)
{
    for (short i = shell->length - shell->cursor; i > 0; i--)
    {
        shellDisplayByte(shell, ' ');
    }
    shellDelete(shell, shell->length);
}


/**
 * @brief shell历史记录添加
 * 
 * @param shell shell对象
 */
static void shellHistoryAdd(SHELL_TypeDef *shell)
{
    if (strcmp(shell->history[shell->historyFlag - 1], shell->buffer) == 0)
    {
        return;
    }
    if (shellStringCopy(shell->history[shell->historyFlag], shell->buffer) != 0)
    {
        shell->historyFlag++;
    }
    if (++shell->historyCount > SHELL_HISTORY_MAX_NUMBER)
    {
        shell->historyCount = SHELL_HISTORY_MAX_NUMBER;
    }
    if (shell->historyFlag >= SHELL_HISTORY_MAX_NUMBER)
    {
        shell->historyFlag = 0;
    }
    shell->historyOffset = 0;
}


/**
 * @brief shell历史记录查找
 * 
 * @param shell shell对象
 * @param dir 查找方向
 */
static void shellHistory(SHELL_TypeDef *shell, unsigned char dir)
{
    if (dir == 0)
    {
        if (shell->historyOffset--
            <= -((shell->historyCount > shell->historyFlag)
            ? shell->historyCount : shell->historyFlag))
        {
            shell->historyOffset = -((shell->historyCount > shell->historyFlag)
                                   ? shell->historyCount : shell->historyFlag);
        }
    }
    else if (dir == 1)
    {
        if (++shell->historyOffset > 0)
        {
            shell->historyOffset = 0;
            return;
        }
    }
    else
    {
        return;
    }
    shellClearLine(shell);
    if (shell->historyOffset == 0)
    {
        shell->cursor = shell->length = 0;
    }
    else
    {
        if ((shell->length = shellStringCopy(shell->buffer,
                shell->history[(shell->historyFlag + SHELL_HISTORY_MAX_NUMBER
                + shell->historyOffset) % SHELL_HISTORY_MAX_NUMBER])) == 0)
        {
            return;
        }
        shell->cursor = shell->length;
        shellDisplay(shell, shell->buffer);
    }
}


/**
 * @brief shell回车输入处理
 * 
 * @param shell shell对象
 */
static void shellEnter(SHELL_TypeDef *shell)
{
    volatile unsigned char paramCount = 0;
    unsigned char quotes = 0;
    unsigned char record = 1;
    SHELL_CommandTypeDef *base;
    unsigned char runFlag = 0;
    int returnValue;

#if (SHELL_DISPLAY_RETURN == 0u)  
    returnValue = returnValue;
#endif  

    if (shell->length == 0)
    {
        shellDisplay(shell, shell->command);
        return;
    }
    
    *(shell->buffer + shell->length++) = 0;

    shellHistoryAdd(shell);

    for (unsigned short i = 0; i < shell->length; i++)
    {
        if ((quotes != 0 ||
            (*(shell->buffer + i) != ' ' &&
            *(shell->buffer + i) != '\t' &&
            *(shell->buffer + i) != ',')) &&
            *(shell->buffer + i) != 0)
        {
            if (*(shell->buffer + i) == '\"')
            {
                quotes = quotes ? 0 : 1;
            #if SHELL_AUTO_PRASE == 0
                *(shell->buffer + i) = 0;
                continue;
            #endif
            }
            if (record == 1)
            {
                shell->param[paramCount++] = shell->buffer + i;
                record = 0;
            }
            if (*(shell->buffer + i) == '\\' &&
                *(shell->buffer + i) != 0)
            {
                i++;
            }
        }
        else
        {
            *(shell->buffer + i) = 0;
            record = 1;
        }
    }
    shell->length = 0;
    shell->cursor = 0;
    if (paramCount == 0)
    {
        shellDisplay(shell, shell->command);
        return;
    }

    shellDisplay(shell, "\r\n");
    base = shell->commandBase;
    if (strcmp((const char *)shell->param[0], "help") == 0)
    {
        shell->isActive = 1;
        shellHelp(paramCount, shell->param);
        shell->isActive = 0;
        shellDisplay(shell, shell->command);
        return;
    }
    for (unsigned char i = 0; i < shell->commandNumber; i++)
    {
        if (strcmp((const char *)shell->param[0], (base + i)->name) == 0)
        {
            runFlag = 1;
            shell->isActive = 1;
        #if SHELL_AUTO_PRASE == 0
            returnValue = (base + i)->function(paramCount, shell->param);
        #else
            returnValue = shellExtRun((base + i)->function, paramCount, shell->param);
        #endif /** SHELL_AUTO_PRASE == 0 */
            shell->isActive = 0;
        #if SHELL_DISPLAY_RETURN == 1
            shellDisplayReturn(shell, returnValue);
        #endif /** SHELL_DISPLAY_RETURN == 1 */
        }
    }
    if (runFlag == 0)
    {
        shellDisplay(shell, "Command not found\r\n");
    }
    shellDisplay(shell, shell->command);
}


/**
 * @brief shell退格输入处理
 * 
 * @param shell shell对象
 */
static void shellBackspace(SHELL_TypeDef *shell)
{
    if (shell->length == 0)
    {
        return;
    }
    if (shell->cursor == shell->length)
    {
        shell->length--;
        shell->cursor--;
        shell->buffer[shell->length] = 0;
        shellDelete(shell, 1);
    }
    else if (shell->cursor > 0)
    {
        for (short i = 0; i < shell->length - shell->cursor; i++)
        {
            shell->buffer[shell->cursor + i - 1] = shell->buffer[shell->cursor + i];
        }
        shell->length--;
        shell->cursor--;
        shell->buffer[shell->length] = 0;
        shellDisplayByte(shell, '\b');
        for (short i = shell->cursor; i < shell->length; i++)
        {
            shellDisplayByte(shell, shell->buffer[i]);
        }
        shellDisplayByte(shell, ' ');
        for (short i = shell->length - shell->cursor + 1; i > 0; i--)
        {
            shellDisplayByte(shell, '\b');
        }
    }
}


/**
 * @brief shell Tab键输入处理
 * 
 * @param shell shell对象
 */
static void shellTab(SHELL_TypeDef *shell)
{
    unsigned short maxMatch = SHELL_COMMAND_MAX_LENGTH;
    unsigned short lastMatchIndex = 0;
    unsigned short matchNum = 0;
    unsigned short length;
    SHELL_CommandTypeDef *base = shell->commandBase;

    if (shell->length != 0)
    {
        shell->buffer[shell->length] = 0;
        for (short i = 0; i < shell->commandNumber; i++)
        {
            if (shellStringCompare(shell->buffer, 
                (char *)(base + i)->name)
                == shell->length)
            {
                if (matchNum != 0)
                {
                    if (matchNum == 1)
                    {
                        shellDisplay(shell, "\r\n");
                    }
                    shellDisplayItem(shell, lastMatchIndex);
                    length = shellStringCompare((char *)(base + lastMatchIndex)->name,
                                                (char *)(base +i)->name);
                    maxMatch = (maxMatch > length) ? length : maxMatch;
                }
                lastMatchIndex = i;
                matchNum ++;
            }
        }

        if (matchNum == 0)
        {
            return;
        }
        if (matchNum == 1)
        {
            shellClearLine(shell);
        }
        if (matchNum != 0)
        {
            shell->length = shellStringCopy(shell->buffer,
                                            (char *)(base + lastMatchIndex)->name);
        }
        if (matchNum > 1)
        {
            shellDisplayItem(shell, lastMatchIndex);
            shellDisplay(shell, shell->command);
            shell->length = maxMatch;
        }
        shell->buffer[shell->length] = 0;
        shell->cursor = shell->length;
        shellDisplay(shell, shell->buffer);
    }
    else
    {
        shell->isActive = 1;
        shellHelp(1, (void *)0);
        shell->isActive = 0;
        shellDisplay(shell, shell->command);
    }

#if SHELL_LONG_HELP == 1
    static int time = 0;
    
    if (SHELL_GET_TICK())
    {
        if (matchNum == 1 && SHELL_GET_TICK() - time < SHELL_DOUBLE_CLICK_TIME)
        {
            shellClearLine(shell);
            for (short i = shell->length; i >= 0; i--)
            {
                shell->buffer[i + 5] = shell->buffer[i];
            }
            shellStringCopy(shell->buffer, "help");
            shell->buffer[4] = ' ';
            shell->length += 5;
            shell->cursor = shell->length;
            shellDisplay(shell, shell->buffer);
        }
        time = SHELL_GET_TICK();
    }
#endif /** SHELL_LONG_HELP == 1 */
}


/**
 * @brief shell正常按键处理
 * 
 * @param shell shell对象
 * @param data 输入的数据
 */
static void shellNormal(SHELL_TypeDef *shell, char data)
{
    
    if (shell->length < SHELL_COMMAND_MAX_LENGTH - 1)
    {
        if (shell->length == shell->cursor)
        {
            shell->buffer[shell->length++] = data;
            shell->cursor++;
            shellDisplayByte(shell, data);
        }
        else
        {
            for (short i = shell->length - shell->cursor; i > 0; i--)
            {
                shell->buffer[shell->cursor + i] = shell->buffer[shell->cursor + i - 1];
            }
            shell->buffer[shell->cursor++] = data;
            shell->buffer[++shell->length] = 0;
            for (short i = shell->cursor - 1; i < shell->length; i++)
            {
                shellDisplayByte(shell, shell->buffer[i]);
            }
            for (short i = shell->length - shell->cursor; i > 0; i--)
            {
                shellDisplayByte(shell, '\b');
            }
        }
    }
    else
    {
        shellDisplay(shell, "\r\nWarnig: Command is too long\r\n");
        shellDisplay(shell, shell->command);
        shellDisplay(shell, shell->buffer);
        shell->cursor = shell->length;
    }
}


/**
 * @brief shell ansi控制系列处理
 * 
 * @param shell shell对象
 * @param data 输入的数据
 */
void shellAnsi(SHELL_TypeDef *shell, char data)
{
    switch ((unsigned char)(shell->status))
    {
    case SHELL_ANSI_CSI:
        switch (data)
        {
        case 0x41:                                              /** 方向上键 */
            shellHistory(shell, 0);
            break;  
        
        case 0x42:                                              /** 方向下键 */
            shellHistory(shell, 1);
            break;

        case 0x43:                                              /** 方向右键 */
            if (shell->cursor < shell->length)
            {
                shellDisplayByte(shell, shell->buffer[shell->cursor]);
                shell->cursor++;
            }
            break;

        case 0x44:                                              /** 方向左键 */
            if (shell->cursor > 0)
            {
                shellDisplayByte(shell, '\b');
                shell->cursor--;
            }
            break;

        default:
            break;
        }
        shell->status = SHELL_IN_NORMAL;
        break;

    case SHELL_ANSI_ESC:
        if (data == 0x5B)
        {
            shell->status = SHELL_ANSI_CSI;
        }
        else
        {
            shell->status = SHELL_IN_NORMAL;
        }
        break;

    default:
        break;
    }
}


/**
 * @brief shell处理
 * 
 * @param shell shell对象
 * @param data 输入数据
 */
void shellHandler(SHELL_TypeDef *shell, char data)
{
    if (shell->status == SHELL_IN_NORMAL)
    {
        switch (data)
        {
        case '\r':  
        case '\n':
            shellEnter(shell);
            break;

        case '\b':
        case 0x7F:
            shellBackspace(shell);
            break;

        case '\t':
            shellTab(shell);
            break;

        case 0x1B:
            shell->status = SHELL_ANSI_ESC;
            break;

        default:
            shellNormal(shell, data);
            break;
        }
    }
    else
    {
        shellAnsi(shell, data);
    }
    
}


#if SHELL_USING_TASK == 1
/**
 * @brief shell 任务
 * 
 * @param param shell对象
 * 
 * @note 使用操作系统时，定义的shell read函数必须是阻塞式的
 * @note 不使用操作系统时，可以通过不断查询的方式使用shell，修改宏SHELL_TASK_WHILE
 *       为0，然后在主循环中不断调用此函数
 */
void shellTask(void *param)
{
    SHELL_TypeDef *shell = (SHELL_TypeDef *)param;
    char data;
    if (shell->read == NULL)
    {
        shellDisplay(shell, "error: shell.read must be defined\r\n");
        while (1) ;
    }
#if SHELL_TASK_WHILE == 1
    while (1)
    {
#endif
        if (shell->read(&data) == 0)
        {
            shellHandler(shell, data);
        }
#if SHELL_TASK_WHILE == 1
    }
#endif
}
#endif


/**
 * @brief shell显示一条命令信息
 * 
 * @param shell shell对象
 * @param index 要显示的命令索引
 */
static void shellDisplayItem(SHELL_TypeDef *shell, unsigned short index)
{
    unsigned short spaceLength;
    SHELL_CommandTypeDef *base = shell->commandBase;
    
    spaceLength = 22 - shellDisplay(shell, (base + index)->name);
    spaceLength = (spaceLength > 0) ? spaceLength : 4;
    do {
        shellDisplay(shell, " ");
    } while (--spaceLength);
    shellDisplay(shell, "--");
    shellDisplay(shell, (base + index)->desc);
    shellDisplay(shell, "\r\n");

}


/**
 * @brief shell帮助
 * 
 * @param argc 参数个数
 * @param argv 参数
 */
static void shellHelp(int argc, char *argv[])
{
    SHELL_TypeDef *shell = shellGetCurrent();
    if (!shell)
    {
        return;
    }
#if SHELL_LONG_HELP == 1
    if (argc == 1)
    {
#endif /** SHELL_LONG_HELP == 1 */
        shellDisplay(shell, "\r\nCOMMAND LIST:\r\n\r\n");       
        for(unsigned short i = 0; i < shell->commandNumber; i++)
        {
            shellDisplayItem(shell, i);
#if (SHELL_USING_CMD_EXPORT != 1u) 
            if (shellDefaultCommandList[i].function == (shellFunction)0)
            {
                return ;
            }
#endif          
        }
#if SHELL_LONG_HELP == 1
    }
    else if (argc == 2) {
        SHELL_CommandTypeDef *base = shell->commandBase;
        for (unsigned char i = 0; i < shell->commandNumber; i++)
        {
#if (SHELL_USING_CMD_EXPORT != 1u) 
            if (shellDefaultCommandList[i].function == (shellFunction)0)
            {
                return ;
            }
#endif     
            if (strcmp((const char *)argv[1], (base + i)->name) == 0)
            {
                
                shellDisplay(shell, "command help --");
                shellDisplay(shell, (base + i)->name);
                shellDisplay(shell, ":\r\n");
                shellDisplay(shell, (base + i)->desc);
                shellDisplay(shell, "\r\n");
                if ((base + i)->help)
                {
                    shellDisplay(shell, (base + i)->help);
                    shellDisplay(shell, "\r\n");
                }
                return;
            }
        }
        shellDisplay(shell, "command not found\r\n");
    }
#endif /** SHELL_LONG_HELP == 1 */
}
SHELL_EXPORT_CMD_EX(help, shellHelp, command help, help [command] --show help info of command);


/**
 * @brief shell清屏
 * 
 * @param argc 参数个数
 * @param argv 参数
 */
static void shellClear(int argc, char *argv[])
{
    SHELL_TypeDef *shell = shellGetCurrent();
    if (!shell)
    {
        return;
    }
    const char ch[2] = { '\x0c', 0x00};
    shellDisplay((SHELL_TypeDef *)shell, ch);   
}
SHELL_EXPORT_CMD_EX(clear, shellClear, clear command, help [command] --show help info of command);
