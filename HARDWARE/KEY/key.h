#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"


// 按键状态
typedef enum _KEY_StatusList_TypeDef 
{
	KEY_Status_Idle 	= 0, // 空闲
	KEY_Status_Debounce ,    // 消抖
	KEY_Status_ConfirmPress	,    // 确认按下	
	KEY_Status_ConfirmPressLong,    // 确认长按
}KEY_StatusList_TypeDef;

// 按键事件
typedef enum _KEY_EventList_TypeDef 
{
	KEY_Event_Null 		   = 0x00, // 空闲
	KEY_Event_SingleClick  = 0x01, // 单击
	KEY_Event_LongPress    = 0x02 // 长按
}KEY_EventList_TypeDef;

// 按键动作，
typedef enum
{ 
	KEY_Action_Press = 0,
	KEY_Action_Release
}KEY_Action_TypeDef;

// 按键引脚的电平
typedef enum
{ 
	KKEY_PinLevel_Low = 0,
	KEY_PinLevel_High
}KEY_PinLevel_TypeDef;


typedef struct _KEY_Configure_TypeDef 
{
	uint16_t                        KEY_Count;                 // 按键长按时长计数
	KEY_Action_TypeDef             KEY_Action;                // 按键动作，按下或释放
	KEY_StatusList_TypeDef         KEY_Status;                // 按键状态
	KEY_EventList_TypeDef          KEY_Event;                 // 按键事件
	KEY_PinLevel_TypeDef           (*KEY_ReadPin_Fcn)(void);  // 读引脚电平函数
}KEY_Configure_TypeDef;


//按键初始化函数
void KEY_Init(void); //IO初始化
void KEY_ReadStateMachine(void);// 读取按键状态机


			    
#endif
