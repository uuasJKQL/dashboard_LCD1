#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include"can.h"
#include"timer.h"
/************************************************
 ALIENTEK 精英STM32F103开发板 实验13
 LCD显示实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺： http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/
u32 num=0;
void Show_power(u8 percent);
void LCD_Show_interface(void);
void data_r(void);
void response_n(void);
int LCD_Speed=0;
int Speed=0;
u8 i=0;
u8 j=0;
u8 ID_0x1631043F_Timeout_Timer=0;
u8 ID_0x1631143F_Timeout_Timer=0;
u8 ID_0x1631243F_Timeout_Timer=0;

u16 alarm=0;//0
u16 error1=0;//1
u16 error2=0;
u16 error3=0;
u16 error4=0;

u8 Motor_MaxTem=0;//0
u8 Inverter_MaxTemp=0;//0
u8 Battery_Voltage=0;
u8 SOC=0;
u8 Cell_MaxTemp=0;
u16 Cell_MinVoltage=0;
 
 u8 ID_0x1631043F[8];
 u8 ID_0x1631143F[8];
 u8 ID_0x1631243F[8];
 u8 RX_Data[8];
extern CAN_HandleTypeDef   CAN1_Handler;     /* CANx句柄 */

extern CAN_RxHeaderTypeDef RxHeader;    /* 接收参数句柄 */
int main(void)
{
	
	u8 lcd_id[12];						//存放LCD ID字符串
	
    HAL_Init();                    	 	//初始化HAL库    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//设置时钟,72M
	delay_init(72);               		//初始化延时函数
	uart_init(115200);					//初始化串口
	LED_Init();							//初始化LED	
  
 	TIM3_Init();
    LCD_Init();           				//初始化LCD FSMC接口
	POINT_COLOR=RED;     				//画笔颜色：红色
	sprintf((char*)lcd_id,"LCD ID:%04X",lcddev.id);//将LCD ID打印到lcd_id数组。
  	CAN_Init();
    POINT_COLOR=RED;
    //LCD_ShowString(30,40,210,24,24,"SPEED");	
    LCD_Display_Dir(1); 
    LCD_Show_interface();
    //LCD_ShowFloat(30,40,3.14,210,24);

    while(1) 
	{		 


   Show_power(SOC);
  
   LCD_ShowFloat(26,50,Battery_Voltage,210,24);
       
         LCD_ShowFloat(350,420,Motor_MaxTem,210,24);
          
          LCD_ShowFloat(646,50,SOC,210,24);

     LCD_ShowFloat(26,180, Inverter_MaxTemp,210,24);
       
          LCD_ShowFloat(30,300, Cell_MaxTemp,210,24);
      
         // LCD_ShowFloat(600,180,Battery_Voltage,210,24);
      
          LCD_ShowFloat(600,300,Cell_MinVoltage,210,24);
LCD_Show_Speed();
LCD_Speed=Speed;
    } 
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim==(&TIM3_Handler))
    {
         data_r();
        response_n();
        
        
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{//HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);
	if(hcan->Instance==CAN1)
	{
		while(HAL_CAN_GetRxFifoFillLevel(&CAN1_Handler,CAN_RX_FIFO0)!=0)
		{
			HAL_CAN_GetRxMessage(&CAN1_Handler, CAN_RX_FIFO0, &RxHeader, RX_Data);
			switch(RxHeader.ExtId)
			{
//				case 0x1631043F:
//			    {//HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);
//					ID_0x1631043F_Timeout_Timer=0;
//					for(i=0;i<=7;i++)
//					{
//						ID_0x1631043F[i]=RX_Data[i];
//					}
//					break;
//				}
//                case 0x1631143F:
//			    {
//					ID_0x1631143F_Timeout_Timer=0;
//					for(i=0;i<=7;i++)
//					{
//						ID_0x1631143F[i]=RX_Data[i];
//					}
//					break;
//				}
                case 0x1631243F:
			    {
					ID_0x1631243F_Timeout_Timer=0;
					for(i=0;i<=7;i++)
					{
						ID_0x1631243F[i]=RX_Data[i];
					}
					break;
				}
			}
	    }
	}
}
//CAN1的接收邮箱组1回调函数
//void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
//{HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);
//	if(hcan->Instance==CAN1)
//	{
//		while(HAL_CAN_GetRxFifoFillLevel(&CAN1_Handler,CAN_RX_FIFO1)!=0)
//		{
//			HAL_CAN_GetRxMessage(&CAN1_Handler, CAN_RX_FIFO1, &RxHeader, RX_Data);
//			if(RxHeader.ExtId==0x1FFF0002)
//			

//				{
//                    //ID_0x1FFF0002_Timeout_Timer=0;
//					for(i=0;i<=7;i++)
//					{
//						//ID_0x1FFF0002[i]=RX_Data[i];
//					}
//				
//				}
//				if(RxHeader.StdId==0x283)
//				{
//                    ID_0x1631143F_Timeout_Timer=0;
//					for(i=0;i<=7;i++)
//					{
//						ID_0x1631043F[i]=RX_Data[i];
//					}
//					break;
//				}
//			}
//		}
//	}

void LCD_Show_interface()
{
    LCD_Showpic(0,280,223,120,image_power);
      LCD_ShowString(26,10,210,24,24,"Battery_Voltage"); 
    LCD_ShowString(350,370,210,24,24,"Motor_MaxTem");
    LCD_ShowString(646,10,210,24,24,"SOC");  
    LCD_ShowString(20,130,210,24,24," Inverter_MaxTemp"); 
    LCD_ShowString(26,250,210,24,24," Cell_MaxTemp");   
    //LCD_ShowString(600,130,210,24,24,"Battery_Voltage");   
    LCD_ShowString(600,250,210,24,24,"Cell_MinVoltage");

}
void Show_power(u8 percent)
{
      for(i=0;i<155-percent*1.6;i++)
 {   for(j=0;j<97;j++)
  {LCD_Fast_DrawPoint(470-i,j+12,0XFFFF);
  }
}
    for(i=0;i<percent*1.60;i++)
 {   for(j=0;j<97;j++)
    {LCD_Fast_DrawPoint(310+i,12+j,0X07E0);
    }
}
 if(percent==100)
 {for(i=0;i<18;i++)
 {   for(j=0;j<40;j++)
    {LCD_Fast_DrawPoint(470+i,40+j,0X07E0);
    }
}
 
 
 }
 else
{for(i=0;i<18;i++)
 {   for(j=0;j<40;j++)
    {LCD_Fast_DrawPoint(470+i,40+j,0XFFFF);
    }
}

}
}
void data_r(void)
{
   Inverter_MaxTemp=ID_0x1631043F[1];
   Motor_MaxTem=ID_0x1631043F[0];
    alarm=ID_0x1631043F[2]+ID_0x1631043F[3]*256;
    error1=ID_0x1631143F[0]+ID_0x1631143F[1]*256;
  error2=ID_0x1631143F[2]+ID_0x1631143F[3]*256;
   error3=ID_0x1631143F[4]+ID_0x1631143F[5]*256;
  error4=ID_0x1631143F[6]+ID_0x1631143F[7]*256;
    Battery_Voltage=ID_0x1631243F[0]+ID_0x1631243F[1]*256;
 SOC=ID_0x1631243F[2];
 Cell_MaxTemp=ID_0x1631243F[3];
 Cell_MinVoltage=ID_0x1631243F[4]+ID_0x1631243F[5]*256;
   Speed=ID_0x1631243F[6];
//    if(alarm!=0)
//    {  LCD_Showpic(350,590,100,93,gImage_alarm);}
  //  Speed=;
}
void response_n()
{
    ID_0x1631043F_Timeout_Timer++;
    //ID_0x1631143F_Timeout_Timer++;
    ID_0x1631243F_Timeout_Timer++;
if(ID_0x1631043F_Timeout_Timer>=100)
{// Motor_MaxTem=0;//0
 //Inverter_MaxTemp=0;//0
}
if(ID_0x1631243F_Timeout_Timer>=100)
{ //Battery_Voltage=0;
//SOC=0;
 //Cell_MaxTemp=0;
 //Cell_MinVoltage=0;
}
}
//if(ID_0x1631143F_Timeout_Timer>=100)
//{

//}