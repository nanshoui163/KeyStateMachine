#include "led.h"
#include "delay.h"
#include "core_cm3.h"
#include "stdio.h"
//////////////////////////////////////////////////////////////////////////////////	 
//LED PA8
////////////////////////////////////////////////////////////////////////////////// 	   

// 亮度档位
u8 RGB_LED_LEVEL = 0;
//初始化PA8为输出口.并使能时钟		    
//LED IO初始化
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PA端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //LED0-->PA.8 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.8
 GPIO_SetBits(GPIOA,GPIO_Pin_8);						 //PA.8 输出高

}

void LED_Flashing(u8 mode)
{
    static u16 cnt = 0;
    u16 max = 0;
    u8 state = LED0_IN;
    if(mode >= 3) mode = 0;
    
    delay_ms(1);
    cnt++;
    switch(mode)
    {
        case 0:max = 200;if(cnt>=max) cnt =0;else LED0 = state?0:1;break;
        case 1:max = 150;if(cnt>=max) cnt =0; else LED0 = state?0:1;;break;
        case 2:max = 100;if(cnt>=max) cnt =0; else LED0 = state?0:1;;break;
    }
}


void RGB_LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 
	GPIO_InitStructure.GPIO_Pin = RGB_LED_GPIO_PIN;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_Init(RGB_LED_GPIOx, &GPIO_InitStructure);					
	GPIO_SetBits(RGB_LED_GPIOx,RGB_LED_GPIO_PIN);						 
}

/********************************************************/
//
/********************************************************/
void RGB_LED_Write0(void)
{
	RGB_LED_HIGH;
	__nop();__nop();__nop();__nop();__nop();__nop();
	RGB_LED_LOW;
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();
}

/********************************************************/
//
/********************************************************/

void RGB_LED_Write1(void)
{
	RGB_LED_HIGH;
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();
	RGB_LED_LOW;
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();
}

void RGB_LED_Reset(void)
{
	RGB_LED_LOW;
	delay_us(80);
}

void RGB_LED_Write_Byte(uint8_t byte)
{
	uint8_t i;

	for(i=0;i<8;i++)
		{
			if(byte&0x80)
				{
					RGB_LED_Write1();
			}
			else
				{
					RGB_LED_Write0();
			}
		byte <<= 1;
	}
}

void RGB_LED_Write_24Bits(uint8_t green,uint8_t red,uint8_t blue)
{
	RGB_LED_Write_Byte(green);
	RGB_LED_Write_Byte(red);
	RGB_LED_Write_Byte(blue);
}


//亮灯颜色设定，其他颜色以此类推
void RGB_LED_Red(void)
{
	 uint8_t i;
	//4个LED全彩灯
	for(i=0;i<8;i++)
		{
			RGB_LED_Write_24Bits(0, 0xff, 0);
	}
}

void RGB_LED_Green(void)
{
	uint8_t i;

	for(i=0;i<4;i++)
		{
			RGB_LED_Write_24Bits(0xff, 0, 0);
	}
}

void RGB_LED_Blue(void)
{
	uint8_t i;

	for(i=0;i<4;i++)
		{
			RGB_LED_Write_24Bits(0, 0, 0xff);
	}
}


void RGB_LED_WHITE(void)
{
    uint8_t i;
	//4个LED全彩灯
	for(i=0;i<8;i++)
		{
			RGB_LED_Write_24Bits(0xff, 0xff, 0xff);
	}
        
}

void RGB_LED_BRIGHT(uint8_t brightness)
{
   // __disable_irq();   // 关闭总中断

    uint8_t i;
	for(i=0;i<8;i++)
	{
//		RGB_LED_Write_24Bits(brightness, brightness, brightness);
        RGB_LED_Write_24Bits(brightness, 0, 0);
	}
    printf("brightness = %d\r\n",brightness);
    // __enable_irq();    // 开启总中断
}

// 设置亮度等级
void RGB_LED_SetLevel(uint8_t level)
{
    if(level > 5) level = 5;
    RGB_LED_BRIGHT(level*50);
    RGB_LED_LEVEL = level;

}


// 亮度增加
void RGB_LED_BrightnessIncrease(void)
{
    GPIO_ResetBits(GPIOA,GPIO_Pin_8);
    RGB_LED_LEVEL++;
    RGB_LED_SetLevel(RGB_LED_LEVEL);
    GPIO_SetBits(GPIOA,GPIO_Pin_8);
}

// 亮度减小
void RGB_LED_BrightnessDecrease(void)
{
    GPIO_ResetBits(GPIOA,GPIO_Pin_8);
    if(RGB_LED_LEVEL>0)RGB_LED_LEVEL--;
    RGB_LED_SetLevel(RGB_LED_LEVEL);
    GPIO_SetBits(GPIOA,GPIO_Pin_8);
}
