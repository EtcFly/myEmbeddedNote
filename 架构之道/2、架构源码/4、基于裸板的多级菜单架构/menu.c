
#include "menu.h"


/*
需求描述：
1、简化按键的菜单设计
2、实现多级菜单之间的切换 且菜单级数可确定
*/

typedef struct 
{
	uint8_t curret;
	uint8_t last;
	uint8_t next;
	uint8_t (*pCurrentFunc)(void *arg);
}Menu_BaseTypeDef;

typedef struct
{
	uint8_t menuIndex; //指向需要跳转的菜单
	uint8_t (*pFunc)(void *arg); //指向各菜单 
}Menu_MangeTypeDef; //菜单管理函数

Menu_BaseTypeDef s_mainMenu[]
{
	{0, 3, 1, function, }, //菜单1
	{1, 0, 2, function1,}, //菜单2
	{2, 1, 3, function2,}, //菜单3
	{3, 2, 0, function3,}, //菜单4
	//.....以下可以实现为上面各主菜单的子菜单
	{4, 4, 4, function, }, //菜单1-1
};

Menu_MangeTypeDef s_menuMangeStruct; //菜单管理


void main_menu_init(void)
{
	s_menuMangeStruct.menuIndex = 0;
	s_menuMangeStruct.pFunc = function;
}

void main_menu_scan(void)
{
	uint8_t cache; //临时存放数据
#if 0  //key1=next   key2=last
	if (key1)
	{
		cache = s_mainMenu[s_menuMangeStruct.menuIndex].next;
	}
	
	if (key2)
	{
		cache = s_mainMenu[s_menuMangeStruct.menuIndex].last;
	}
	
	s_menuMangeStruct.menuIndex = cache;
	s_menuMangeStruct.pFunc = s_mainMenu[s_menuMangeStruct.menuIndex].pCurrentFunc;
#endif

	s_menuMangeStruct.pFunc();
}