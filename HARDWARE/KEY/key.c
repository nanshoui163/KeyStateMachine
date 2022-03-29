#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
#include "stdio.h"
/**************************************************************************************************** 
*                             ���������� ����
* �����¼�������ʱ����� KEY_LONG_PRESS_TIME���ͷź���Ӧ������֧����������Ҫ������Ӧ���Լ����ã�
* �����¼�������ʱ��С�� KEY_LONG_PRESS_TIME �ͷź���Ӧ��
****************************************************************************************************/
// ����ʱ���ĺ궨��
#define KEY_LONG_PRESS_TIME    50  // 20ms*50 = 1s
#define KEY_PRESSED_LEVEL      0   // ����������ʱ�ĵ�ƽ
/**************************************************************************************************** 
*                             �ֲ���������
****************************************************************************************************/
static KEY_PinLevel_TypeDef  KEY_ReadPin(void); //������ȡ����
static void KEY_GetAction(void);// ��ȡ�������������»��ͷţ����浽�ṹ��

/**************************************************************************************************** 
*                            ����������Ϣ��ȫ�ֽṹ�����
****************************************************************************************************/
KEY_Configure_TypeDef KeyCfg={
		
		0,						// ��������ʱ������
		KEY_Action_Release,		// �������������»��ͷ� 
		KEY_Status_Idle,        // ����״̬
		KEY_Event_Null,         // �����¼�
		KEY_ReadPin             // �����ŵ�ƽ����
};

/**************************************************************************************************** 
*                            ��������
****************************************************************************************************/
//��ȡ���ŵĵ�ƽ
static KEY_PinLevel_TypeDef  KEY_ReadPin(void) 
{
  return (KEY_PinLevel_TypeDef)GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);
}
static void KEY_GetAction(void) // ��ȡ�������������»��ͷţ����浽�ṹ��
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



//������ʼ������������IO��
void KEY_Init(void) //IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��PORTA,PORTBʱ��

	//��ʼ�� WK_UP-->GPIOA.0	  
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.0

}



void KEY_ReadStateMachine(void)
{
    KEY_GetAction();
	
	switch(KeyCfg.KEY_Status)
	{
		//״̬��û�а�������
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
			
		//״̬������
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
			
		//״̬��ȷ�ϰ���
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
				printf("�������� %d\r\n",KeyCfg.KEY_Count);
				KeyCfg.KEY_Count++;
				KeyCfg.KEY_Status = KEY_Status_ConfirmPress;
				KeyCfg.KEY_Event = KEY_Event_Null;
			}
			else
			{
				printf("ͻȻgg�������ˣ�δ�ﵽ����ʱ�䣬Ϊ�̰� %d\r\n",KeyCfg.KEY_Count);
				KeyCfg.KEY_Count = 0;
				KeyCfg.KEY_Status = KEY_Status_Idle;
				KeyCfg.KEY_Event = KEY_Event_SingleClick;

			}
			break;	

			
		//״̬��ȷ�ϳ���
		case KEY_Status_ConfirmPressLong:
			printf("KEY_Status_ConfirmPressLong\r\n");
			if(KeyCfg.KEY_Action == KEY_Action_Press) 
			{   // һֱ�ȴ���ſ�
				printf("һֱ���� KEY_Status_ConfirmPressLong\r\n");
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
