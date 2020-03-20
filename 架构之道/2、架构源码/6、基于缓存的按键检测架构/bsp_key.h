

/******************************************************************************************************
----------------------------------------bsp_key模块支持功能-------------------------------------------
1、支持长短按检测
2、支持连续按键操作
3、支持队列缓冲
4、支持最大3级组合按键  
5、支持四种可裁剪底层检测算法
6、模块可裁剪
*****************************************************************************************************/

#ifndef _BSP_KEY_H
#define _BSP_KEY_H

#include "stm32f10x.h"
#include "bitband.h"


#define BSP_KEY_VERSION              001u //当前版本号


/*****************************************************************************************************
								USER_KEY_DETECT_METHOD取值有以下几种
-1- 计数器计数法检测
	优点：可方便的实现长按与短按的识别，对抖动时间的调整方便，同时对多按键的扩展较为容易。
	缺点：按键未及时处理容易漏掉
-2- 状态机检测
	优点：抖动时间调整方便，对多按键的扩展较为容易，可实现长按短按的识别
	缺点：相对计数器检测方式更加复杂，没有那么直观，按键未处理同样容易漏掉
-3- 正点原子检测法
	优点：实现简单，多按键扩展容易
	缺点：按键抖动滤出不充分,键值判断不稳定，同样按键未处理容易漏掉，对长按短按的识别较困难
-4- 缓冲区检测 (内存代价太大 暂未实现)
	优点：按键不处理也不会漏掉，操作稳定
	缺点：内存RAM的消耗相对较大
*****************************************************************************************************/
#define USER_KEY_DETECT_METHOD       3 //按键检测方法
#define MAX_KEY                      2 //配置最大支持的按键数   理论最大32
#define MAX_KEY_NUM                  100 //消抖缓冲数

/*
无按键按下的状态  应该为MAX_KEY位的二进制数
例如：若使用16位按键，按键无效时为0状态，那么应该设置0000 0000 0000 0000
*/
#define NO_ANY_KEY_PRESS_STATUS       ((KeyType)3)      

//按键接口部分
#define BSP_KEY1                    PEin(2) 
#define BSP_KEY2					PEin(3)
//#define BSP_KEY3                    PEin(4)

#define KEY_SCAN_VALUE          (BSP_KEY1 | (BSP_KEY2 << 1))

//队列配置
#define KEY_QUEUE_ENBALE              0 //是否使用按键缓冲队列 0不使用  1使用
#define MAX_QUEUE_SIZE                64 //队列缓存的按键次数 缓冲次数不大于255

//长短按配置
#define SUPPORT_LONG_PRESS_ENABLE     0 //长短按支持  0不支持  1支持
#define LONG_KEY_PRESS                2
//连续按键操作支持
#define SUPPORT_KEY_CONTINUITY_ENABLE 0 //0不支持  1支持  

//组合按键配置
/*
---组合按键键值定义：
@def COMBINATION_KEYN 组合按键含义定义
@def 组合按键键值范围为128-191    普通按键是0-63  其他为无效按键
*/
#define COMBINATION_PRESS_ENABLE      1 //组合按键支持 0不支持  1支持	
#define MAX_COMBINATION_KEY           2//最大3级组合按键支持 理论最大支持只能32级 

	 
void bsp_keyGPIOConfig(void);

#if KEY_QUEUE_ENBALE
u8 keyReadFromQueue(void);
#else
void Key_scan(void);
#endif



#endif

