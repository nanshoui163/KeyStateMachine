#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
#include "stdio.h"
/**************************************************************************************************** 
*                             长按、单击 定义
* 长按事件：按下时间大于 KEY_LONG_PRESS_TIME，释放后响应。（不支持连按，需要连按响应可自己配置）
* 单击事件：按下时间小于 KEY_LONG_PRESS_TIME 释放后响应。
****************************************************************************************************/
// 长按时长的宏定义
#define KEY_LONG_PRESS_TIME    50  // 20ms*50 = 1s
#define KEY_PRESSED_LEVEL      0   // 按键被按下时的电平
/**************************************************************************************************** 
*                             局部函数声明
****************************************************************************************************/
static KEY_PinLevel_TypeDef  KEY_ReadPin(void); //按键读取函数
static void KEY_GetAction(void);// 获取按键动作，按下或释放，保存到结构体

/**************************************************************************************************** 
*                            按键配置信息的全局结构体变量
****************************************************************************************************/
KEY_Configure_TypeDef KeyCfg={
		
		0,						// 按键长按时长计数
		KEY_Action_Release,		// 按键动作，按下或释放 
		KEY_Status_Idle,        // 按键状态
		KEY_Event_Null,         // 按键事件
		KEY_ReadPin             // 读引脚电平函数
};

/**************************************************************************************************** 
*                            函数定义
****************************************************************************************************/
//读取引脚的电平
static KEY_PinLevel_TypeDef  KEY_ReadPin(void) 
{
  return (KEY_PinLevel_TypeDef)GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);
}
static void KEY_GetAction(void) // 获取按键动作，按下或释放，保存到结构体
{
	if(KeyCfg.KEY_ReadPin_Fcn() == KEY_PRESSED_LEVEL)
	{
		KeyCfg.KEY_Action = KEY_Action_Press;
	}
	else
	{
		KeyCfg.KEY_Action =  KEY_Action_Release;
	}
 
}



//按键初始化函数，配置IO口
void KEY_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能PORTA,PORTB时钟

	//初始化 WK_UP-->GPIOA.0	  
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.0

}



void KEY_ReadStateMachine(void)
{
    KEY_GetAction();
	
	switch(KeyCfg.KEY_Status)
	{
		//状态：没有按键按下
		case KEY_Status_Idle:
			if(KeyCfg.KEY_Action == KEY_Action_Press)
			{
				KeyCfg.KEY_Status = KEY_Status_Debounce;
				KeyCfg.KEY_Event = KEY_Event_Null;
			}
			else
			{
				KeyCfg.KEY_Status = KEY_Status_Idle;
				KeyCfg.KEY_Event = KEY_Event_Null;
			}
			break;
			
		//状态：消抖
		case KEY_Status_Debounce:
			if(KeyCfg.KEY_Action == KEY_Action_Press)
			{
				KeyCfg.KEY_Status = KEY_Status_ConfirmPress;
				KeyCfg.KEY_Event = KEY_Event_Null;
			}
			else
			{
				KeyCfg.KEY_Status = KEY_Status_Idle;
				KeyCfg.KEY_Event = KEY_Event_Null;
			}
			break;	
			
		//状态：确认按下
		case KEY_Status_ConfirmPress:
			if( (KeyCfg.KEY_Action == KEY_Action_Press) && ( KeyCfg.KEY_Count >= KEY_LONG_PRESS_TIME))
			{
				printf("KEY_Status_ConfirmPressLong\r\n");
				KeyCfg.KEY_Count = 0;
				KeyCfg.KEY_Status = KEY_Status_ConfirmPressLong;
				KeyCfg.KEY_Event = KEY_Event_Null;
				
			}
			else if( (KeyCfg.KEY_Action == KEY_Action_Press) && (KeyCfg.KEY_Count < KEY_LONG_PRESS_TIME))
			{
				printf("继续按下 %d\r\n",KeyCfg.KEY_Count);
				KeyCfg.KEY_Count++;
				KeyCfg.KEY_Status = KEY_Status_ConfirmPress;
				KeyCfg.KEY_Event = KEY_Event_Null;
			}
			else
			{
				printf("突然gg，按短了，未达到长按时间，为短按 %d\r\n",KeyCfg.KEY_Count);
				KeyCfg.KEY_Count = 0;
				KeyCfg.KEY_Status = KEY_Status_Idle;
				KeyCfg.KEY_Event = KEY_Event_SingleClick;

			}
			break;	

			
		//状态：确认长按
		case KEY_Status_ConfirmPressLong:
			printf("KEY_Status_ConfirmPressLong\r\n");
			if(KeyCfg.KEY_Action == KEY_Action_Press) 
			{   // 一直等待其放开
				printf("一直按着 KEY_Status_ConfirmPressLong\r\n");
				KeyCfg.KEY_Status = KEY_Status_ConfirmPressLong;
				KeyCfg.KEY_Event = KEY_Event_Null;
				KeyCfg.KEY_Count = 0;
			}
			else
			{
				KeyCfg.KEY_Status = KEY_Status_Idle;
				KeyCfg.KEY_Event = KEY_Event_LongPress;
				KeyCfg.KEY_Count = 0;
			}
			break;	
		default:
			break;
	}

}
