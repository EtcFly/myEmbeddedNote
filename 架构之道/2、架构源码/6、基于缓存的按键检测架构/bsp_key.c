
#include "bsp_key.h"
#include "string.h"

/*****************************************************************************
							典型码值设定
*****************************************************************************/
#define KEY_NO_DATA  	   NO_ANY_KEY_PRESS_STATUS  //无效按键
#define INVALID_KEY                    0
#define KEY_QUEUE_ISEMPTY            0XFF //缓冲区为空
#define COMBINATION_Base             0X80 //组合按键起点
#define MAX_KEY_SOPPURT              32 //理论最大支持32个


/*************************************************************************************************
											按键类型定义
**************************************************************************************************/
#if MAX_KEY <= 8
	typedef unsigned char KeyType;
#elif MAX_KEY <= 16
	typedef unsigned short int KeyType;
#else
	typedef unsigned int KeyType;
#endif

/*****************************************************************************
							多按键键值索引码配置区
*****************************************************************************/
//多按键键值索引码
#if MAX_KEY <= 8
	//8位组合按键键值索引编码
#define COMBINATION_KEY1_PRESS                 (NO_ANY_KEY_PRESS_STATUS&0XFC)
//#define COMBINATION_KEY2_PRESS                 (NO_ANY_KEY_PRESS_STATUS&0XFA)
#elif MAX_KEY <= 16
	//16位组合按键键值索引编码
#else
	//32位组合按键键值索引编码
#endif


//单一按键键值索引码
#if MAX_KEY <= 8
#define KEY1_PRESS                 (NO_ANY_KEY_PRESS_STATUS&0XFE)
#define KEY2_PRESS                 (NO_ANY_KEY_PRESS_STATUS&0XFD)
#define KEY3_PRESS                 (NO_ANY_KEY_PRESS_STATUS&0XFB)
#define KEY4_PRESS                 (NO_ANY_KEY_PRESS_STATUS&0XF7)
#define KEY5_PRESS                 (NO_ANY_KEY_PRESS_STATUS&0XEF)
#define KEY6_PRESS                 (NO_ANY_KEY_PRESS_STATUS&0XDF)
#define KEY7_PRESS                 (NO_ANY_KEY_PRESS_STATUS&0XBF)
#define KEY8_PRESS                 (NO_ANY_KEY_PRESS_STATUS&0X7F)

#elif MAX_KEY <= 16
#define KEY1_PRESS                 (NO_ANY_KEY_PRESS_STATUS&0XFFFE)
#define KEY2_PRESS                 (NO_ANY_KEY_PRESS_STATUS&0XFFFD)
#define KEY3_PRESS                 (NO_ANY_KEY_PRESS_STATUS&0XFFFB)
#define KEY4_PRESS                 (NO_ANY_KEY_PRESS_STATUS&0XFFF7)
#define KEY5_PRESS                 (NO_ANY_KEY_PRESS_STATUS&0XFFEF)
#define KEY6_PRESS                 (NO_ANY_KEY_PRESS_STATUS&0XFFDF)
#define KEY7_PRESS                 (NO_ANY_KEY_PRESS_STATUS&0XFFBF)
#define KEY8_PRESS                 (NO_ANY_KEY_PRESS_STATUS&0XFF7F)
#define KEY9_PRESS                 (NO_ANY_KEY_PRESS_STATUS&0XFEFF)
#define KEY10_PRESS                (NO_ANY_KEY_PRESS_STATUS&0XFDFF)
#define KEY11_PRESS                (NO_ANY_KEY_PRESS_STATUS&0XFBFF)
#define KEY12_PRESS                (NO_ANY_KEY_PRESS_STATUS&0XF7FF)
#define KEY13_PRESS                (NO_ANY_KEY_PRESS_STATUS&0XEFFF)
#define KEY14_PRESS                (NO_ANY_KEY_PRESS_STATUS&0XDFFF)
#define KEY15_PRESS                (NO_ANY_KEY_PRESS_STATUS&0XBFFF)
#define KEY16_PRESS                (NO_ANY_KEY_PRESS_STATUS&0X7FFF)
#else 
#define KEY1_PRESS                 (NO_ANY_KEY_PRESS_STATUS&FFFFFFFE)
#define KEY2_PRESS                 (NO_ANY_KEY_PRESS_STATUS&FFFFFFFD)
#define KEY3_PRESS                 (NO_ANY_KEY_PRESS_STATUS&FFFFFFFB)
#define KEY4_PRESS                 (NO_ANY_KEY_PRESS_STATUS&FFFFFFF7)
#define KEY5_PRESS                 (NO_ANY_KEY_PRESS_STATUS&FFFFFFEF)
#define KEY6_PRESS                 (NO_ANY_KEY_PRESS_STATUS&FFFFFFDF)
#define KEY7_PRESS                 (NO_ANY_KEY_PRESS_STATUS&FFFFFFBF)
#define KEY8_PRESS                 (NO_ANY_KEY_PRESS_STATUS&FFFFFF7F)
#define KEY9_PRESS                 (NO_ANY_KEY_PRESS_STATUS&FFFFFEFF)
#define KEY10_PRESS                (NO_ANY_KEY_PRESS_STATUS&FFFFFDFF)
#define KEY11_PRESS                (NO_ANY_KEY_PRESS_STATUS&FFFFFBFF)
#define KEY12_PRESS                (NO_ANY_KEY_PRESS_STATUS&FFFFF7FF)
#define KEY13_PRESS                (NO_ANY_KEY_PRESS_STATUS&FFFFEFFF)
#define KEY14_PRESS                (NO_ANY_KEY_PRESS_STATUS&FFFFDFFF)
#define KEY15_PRESS                (NO_ANY_KEY_PRESS_STATUS&FFFFBFFF)
#define KEY16_PRESS                (NO_ANY_KEY_PRESS_STATUS&FFFF7FFF)	
#define KEY17_PRESS                (NO_ANY_KEY_PRESS_STATUS&FFFEFFFF)
#define KEY18_PRESS                (NO_ANY_KEY_PRESS_STATUS&FFFDFFFF)
#define KEY19_PRESS                (NO_ANY_KEY_PRESS_STATUS&FFFBFFFF)
#define KEY20_PRESS                (NO_ANY_KEY_PRESS_STATUS&FFF7FFFF)
#define KEY21_PRESS                (NO_ANY_KEY_PRESS_STATUS&FFEFFFFF)
#define KEY22_PRESS                (NO_ANY_KEY_PRESS_STATUS&FFDFFFFF)
#define KEY23_PRESS                (NO_ANY_KEY_PRESS_STATUS&FFBFFFFF)
#define KEY24_PRESS                (NO_ANY_KEY_PRESS_STATUS&FF7FFFFF)
#define KEY25_PRESS                (NO_ANY_KEY_PRESS_STATUS&FEFFFFFF)
#define KEY26_PRESS                (NO_ANY_KEY_PRESS_STATUS&FDFFFFFF)
#define KEY27_PRESS                (NO_ANY_KEY_PRESS_STATUS&FBFFFFFF)
#define KEY28_PRESS                (NO_ANY_KEY_PRESS_STATUS&F7FFFFFF)
#define KEY29_PRESS                (NO_ANY_KEY_PRESS_STATUS&EFFFFFFF)
#define KEY30_PRESS                (NO_ANY_KEY_PRESS_STATUS&DFFFFFFF)
#define KEY31_PRESS                (NO_ANY_KEY_PRESS_STATUS&BFFFFFFF)
#define KEY32_PRESS                (NO_ANY_KEY_PRESS_STATUS&7FFFFFFF)
#endif



#define KEY_JUDGE(KeyIndex, KeyNum)     case (KeyIndex): ReadNowValue = KeyNum; break;

#if COMBINATION_PRESS_ENABLE
#define KEY_JUDGE_COMBINATION(KeyIndex, KeyNum)    \
	case (KeyIndex): \
	ReadNowValue = (KeyNum|COMBINATION_Base); break;
#endif

#define KEY_BEGIN() 		switch (KeyNowValue){
#define KEY_END() 				default :  ReadNowValue = 0; }

/*******************************************************************************************************
												函数声明
******************************************************************************************************/
void Key_scan(void);


/*******************************************************************************************************
											缓冲队列的实现
********************************************************************************************************/
#if KEY_QUEUE_ENBALE

typedef struct
{
	u8 keyQueue[MAX_QUEUE_SIZE];
	u8 readInKey; //按键读取指针
	u8 writeInKey; //写按键指针
//	u8 readSpeedUpUnit;
//	u8 writeSpeedUpUint;
}KeyQueueStruct;


KeyQueueStruct bsp_keyQueue;

void keyQueueInit(void)
{
	memset((KeyQueueStruct *)&bsp_keyQueue, 0, sizeof(bsp_keyQueue));	
}

void keyWriteToQueue(u8 keyValue)
{
	if (bsp_keyQueue.writeInKey > (MAX_QUEUE_SIZE-1))
		bsp_keyQueue.writeInKey = 0;
	bsp_keyQueue.keyQueue[bsp_keyQueue.writeInKey++] = keyValue;
}

u8 keyReadFromQueue(void)
{
	Key_scan();
	if (bsp_keyQueue.readInKey == bsp_keyQueue.writeInKey) 
		return KEY_QUEUE_ISEMPTY; //队列为空
	if (bsp_keyQueue.readInKey > (MAX_QUEUE_SIZE-1))
		bsp_keyQueue.readInKey = 0;
	return bsp_keyQueue.keyQueue[bsp_keyQueue.readInKey++];
}

#endif


#if !KEY_QUEUE_ENBALE
u8 KeyValue_G = 0;
#endif

#if USER_KEY_DETECT_METHOD == 1


void Key_scan(void)
{
	KeyType KeyNowValue;
	static u8 Key_Num = 0;
	static u8 ReadNowValue = 0;
	static KeyType LastKey = NO_ANY_KEY_PRESS_STATUS;
#if SUPPORT_LONG_PRESS_ENABLE
	static u8 longKeyPressCount;
#endif	
	
	KeyNowValue = KEY_SCAN_VALUE; //获取当前按键值(需要人为配置)
	if (KeyNowValue == KEY_NO_DATA)
	{		
#if !KEY_QUEUE_ENBALE
		KeyValue_G = INVALID_KEY; //无按键按下
#endif
	if (!ReadNowValue) return ;
		
#if SUPPORT_LONG_PRESS_ENABLE
	if (longKeyPressCount & 0x40)
		ReadNowValue |= 0x40;	
	if (ReadNowValue & 0x80) //去除组合按键按下不同时的干扰
		ReadNowValue &= 0xBF;
	#if !KEY_QUEUE_ENBALE 
		KeyValue_G = ReadNowValue;
	#else
		keyWriteToQueue(ReadNowValue);
	#endif		
	longKeyPressCount = 0;
#endif	
		LastKey = NO_ANY_KEY_PRESS_STATUS;
		ReadNowValue = 0;
		return ;
	}
	if (KeyNowValue == LastKey)
	{
		Key_Num++;
		if (Key_Num > MAX_KEY_NUM)
		{
			Key_Num = 0;
		#if SUPPORT_LONG_PRESS_ENABLE
			longKeyPressCount++;
			if (longKeyPressCount > LONG_KEY_PRESS)
			{
				longKeyPressCount |= 0X40; //标志长按
				longKeyPressCount &= 0X40; //清除计数
			}
		#elif !SUPPORT_KEY_CONTINUITY_ENABLE
			if (ReadNowValue > 0) return ; //按键未释放
		#endif
			KeyNowValue &= 0XBF; //去除组合按键不同时的编码干扰
			KEY_BEGIN()
			//需要人为配置的区域
			KEY_JUDGE(KEY1_PRESS, 1);
			KEY_JUDGE(KEY2_PRESS, 2);
			KEY_JUDGE(KEY3_PRESS, 3);				
	#if COMBINATION_PRESS_ENABLE	
			KEY_JUDGE_COMBINATION(COMBINATION_KEY1_PRESS, 1);
//			KEY_JUDGE_COMBINATION(COMBINATION_KEY2_PRESS, 2);
	#endif		
			//配置结束
			KEY_END()	
#if !SUPPORT_LONG_PRESS_ENABLE
		#if !KEY_QUEUE_ENBALE
			KeyValue_G = ReadNowValue;
		#else
			keyWriteToQueue(ReadNowValue);
		#endif
#endif
		}
		return ;
	}
	Key_Num = 0;
	LastKey = KeyNowValue;
}
#endif


#if USER_KEY_DETECT_METHOD == 2
	
void Key_scan(void)
{
	KeyType KeyNowValue;
	static u8 keyIndex = 0;
	static u8 Key_Num = 0;
	static KeyType LastKey; 
	static u8 ReadNowValue = 0;
#if SUPPORT_LONG_PRESS_ENABLE
	static u8 longKeyPressCount;
#endif	
	KeyNowValue = KEY_SCAN_VALUE; 
	switch (keyIndex)
	{
		case 0: //检测态
			   if (KeyNowValue == KEY_NO_DATA)  
			   { 
			#if !KEY_QUEUE_ENBALE
					KeyValue_G = INVALID_KEY; //无按键按下
			#endif
					if (!ReadNowValue) return ;
			#if SUPPORT_LONG_PRESS_ENABLE
					if (longKeyPressCount & 0x40)	ReadNowValue |= 0x40;	
					if (ReadNowValue & 0X80) ReadNowValue &= 0XBF;
			#if !KEY_QUEUE_ENBALE 
					KeyValue_G = ReadNowValue;
			#else
					keyWriteToQueue(ReadNowValue);
			#endif		
					longKeyPressCount = 0;
			#endif	
					ReadNowValue = 0; return ;
			   }
			   LastKey = KeyNowValue;
			   Key_Num = 0;
			   keyIndex++;		
			   break;
		case 1: //消抖态
				if (KeyNowValue != LastKey) { keyIndex = 0; return ;}
				if (Key_Num > MAX_KEY_NUM) keyIndex++;
				else Key_Num++;
				break;
		case 2: //确认键值以及释放等待态
			{
				keyIndex = 0;	
			#if SUPPORT_LONG_PRESS_ENABLE
				longKeyPressCount++;
				if (longKeyPressCount > LONG_KEY_PRESS)
				{
					longKeyPressCount |= 0X40; //标志长按
					longKeyPressCount &= 0X40; //清除计数
				}
			#elif !SUPPORT_KEY_CONTINUITY_ENABLE
				if (ReadNowValue > 0) return ; //按键未释放
			#endif
				ReadNowValue &= 0XBF;
				KEY_BEGIN()
				//需要人为配置的区域
				KEY_JUDGE(KEY1_PRESS, 1);
				KEY_JUDGE(KEY2_PRESS, 2);
				KEY_JUDGE(KEY3_PRESS, 3);	
			#if COMBINATION_PRESS_ENABLE	
				KEY_JUDGE_COMBINATION(COMBINATION_KEY1_PRESS, 1);
		//		KEY_JUDGE_COMBINATION(COMBINATION_KEY2_PRESS, 2);
			#endif					
				//配置结束
				KEY_END()	
#if !SUPPORT_LONG_PRESS_ENABLE
		#if !KEY_QUEUE_ENBALE
				KeyValue_G = ReadNowValue;
		#else
				keyWriteToQueue(ReadNowValue);
		#endif
#endif	
			}
			break;
	}
}	

#endif



#if USER_KEY_DETECT_METHOD == 3
void Key_scan(void)
{
	static u8 keyPressFlag; //0表示未按下 1表示按下
	KeyType KeyNowValue;	
	static u8 ReadNowValue = 0;
	static KeyType LastKey; 
	static u8 Key_Num = 0;	
#if SUPPORT_LONG_PRESS_ENABLE
	static u8 longKeyPressCount;
#endif	

#if SUPPORT_KEY_CONTINUITY_ENABLE
	keyPressFlag = 0;
#endif	
	
	KeyNowValue = KEY_SCAN_VALUE; 
	
	if (!keyPressFlag && KeyNowValue != KEY_NO_DATA)
	{
		keyPressFlag = 1;
		LastKey = KeyNowValue;
#if SUPPORT_LONG_PRESS_ENABLE
	    longKeyPressCount = 0;    //开始计数
#endif	 
	}
	else if (KeyNowValue == KEY_NO_DATA)
	{
		keyPressFlag = 0;
		if (!ReadNowValue) return ;
		
#if SUPPORT_LONG_PRESS_ENABLE
		if (longKeyPressCount & 0x40) ReadNowValue |= 0x40;	
		if (ReadNowValue & 0x80) ReadNowValue &= 0xBF;			
	#if !KEY_QUEUE_ENBALE 
		KeyValue_G = ReadNowValue;
	#else
		keyWriteToQueue(ReadNowValue);
	#endif		
		longKeyPressCount = 0;
#endif	
		ReadNowValue = 0;
	}
	

	if (keyPressFlag && LastKey == KeyNowValue)
	{
		Key_Num++;
		if (Key_Num > MAX_KEY_NUM)
		{
			Key_Num = 0;
			KeyNowValue &= 0XBF;
			KEY_BEGIN()
			//需要人为配置的区域
			KEY_JUDGE(KEY1_PRESS, 1);
			KEY_JUDGE(KEY2_PRESS, 2);
			KEY_JUDGE(KEY3_PRESS, 3);	
		#if COMBINATION_PRESS_ENABLE	
			KEY_JUDGE_COMBINATION(COMBINATION_KEY1_PRESS, 1);
	//		KEY_JUDGE_COMBINATION(COMBINATION_KEY2_PRESS, 2);
		#endif					
			//配置结束
			KEY_END()
			
	#if !SUPPORT_LONG_PRESS_ENABLE
		#if !KEY_QUEUE_ENBALE
			KeyValue_G = ReadNowValue;
		#else
			keyWriteToQueue(ReadNowValue);
		#endif
	#endif	

	#if SUPPORT_LONG_PRESS_ENABLE
			longKeyPressCount++;
			if (longKeyPressCount > LONG_KEY_PRESS)
			{
				longKeyPressCount |= 0X40; //标志长按
				longKeyPressCount &= 0X40; //清除计数
			}
	#endif
		}
	}
	else 
	{
		Key_Num = 0;
		keyPressFlag = 0;
	}


}

#endif

/*******************************************************************************************************
											按键底层驱动
********************************************************************************************************/
void bsp_keyGPIOConfig(void)
{
/*
	user  bsp_key
*/
#if KEY_QUEUE_ENBALE
	keyQueueInit();
#endif
}

/****************************************************************************
								错误提示
*******************************************************************************/
#if SUPPORT_LONG_PRESS_ENABLE && SUPPORT_KEY_CONTINUITY_ENABLE
	#error "key::don't allow long press and continue press set together"
#endif

#if MAX_KEY < MAX_COMBINATION_KEY
	#error "key::the most key is less than combination key"
#endif

#if MAX_KEY > MAX_KEY_SOPPURT
	#error "The key is to much";
#endif


