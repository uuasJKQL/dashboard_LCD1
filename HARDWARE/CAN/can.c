#include "can.h"
#include "usart.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 

CAN_InitTypeDef		CAN1_InitConf;
CAN_HandleTypeDef	CAN1_Handler;   //CAN1句柄
CAN_TxHeaderTypeDef	TxHeader;      //发送
CAN_RxHeaderTypeDef	RxHeader;      //接收
CAN_FilterTypeDef  sFilterConfig;
//////////////////////////////////////////////////////////////////////////////////	 

void CAN_Init()
{
	CAN1_Mode_Init(CAN_SJW_1TQ,CAN_BS2_8TQ,CAN_BS1_9TQ,8,CAN_MODE_NORMAL);
}

void CAN1_Mode_Init(u32 tsjw,u32 tbs2,u32 tbs1,u16 brp,u32 mode)
{
    CAN1_Handler.Instance=CAN1;
	CAN1_Handler.Init = CAN1_InitConf;
    CAN1_Handler.Init.Prescaler=brp;				//分频系数(Fdiv)为brp+1
    CAN1_Handler.Init.Mode=mode;					//模式设置 
    CAN1_Handler.Init.SyncJumpWidth=tsjw;			//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1TQ~CAN_SJW_4TQ
    CAN1_Handler.Init.TimeSeg1=tbs1;				//tbs1范围CAN_BS1_1TQ~CAN_BS1_16TQ
    CAN1_Handler.Init.TimeSeg2=tbs2;				//tbs2范围CAN_BS2_1TQ~CAN_BS2_8TQ
    CAN1_Handler.Init.TimeTriggeredMode=DISABLE;	//非时间触发通信模式 
    CAN1_Handler.Init.AutoBusOff=DISABLE;			//软件自动离线管理
    CAN1_Handler.Init.AutoWakeUp=DISABLE;			//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
    CAN1_Handler.Init.AutoRetransmission=DISABLE;	//禁止报文自动传送 
    CAN1_Handler.Init.ReceiveFifoLocked=DISABLE;	//报文不锁定,新的覆盖旧的 
    CAN1_Handler.Init.TransmitFifoPriority=ENABLE;	//优先级由报文先后顺序决定 
	
    HAL_CAN_Init(&CAN1_Handler);			        //初始化

	sFilterConfig.FilterBank = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = ((0x1631243F<<3)>>16)&0xffff;
	sFilterConfig.FilterIdLow = ((0x1631243F<<3)&0xffff)|CAN_ID_EXT;
	sFilterConfig.FilterMaskIdHigh = ((0xffffffff<<3)>>16)&0xffff;
	sFilterConfig.FilterMaskIdLow = ((0xffffffff<<3)&0xffff)|CAN_ID_EXT;
	
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14;

	// HAL_CAN_ConfigFilter(&CAN1_Handler, &sFilterConfig);
	
	// sFilterConfig.FilterBank = 1;
	// sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
	// sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	// sFilterConfig.FilterIdHigh = ((0x186240F3<<3)>>16)&0xffff;
	// sFilterConfig.FilterIdLow = ((0x186240F3<<3)&0xffff)|CAN_ID_EXT;
	// sFilterConfig.FilterMaskIdHigh = ((0x00000000<<3)>>16)&0xffff;
	// sFilterConfig.FilterMaskIdLow = ((0x00000000<<3)&0xffff)|CAN_ID_EXT;
	// sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	// sFilterConfig.FilterActivation = ENABLE;
	// sFilterConfig.SlaveStartFilterBank = 14; 

	HAL_CAN_ConfigFilter(&CAN1_Handler, &sFilterConfig);
	
//	sFilterConfig.FilterBank = 2;
//	sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
//	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
//	sFilterConfig.FilterIdHigh = 0x283<<5;
//	sFilterConfig.FilterIdLow =  0|CAN_ID_STD;
//	sFilterConfig.FilterMaskIdHigh = ((0x1FFF0002<<3)>>16)&0xffff;
//	sFilterConfig.FilterMaskIdLow = ((0x1FFF0002<<3)&0xffff)|CAN_ID_EXT;
//	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO1;
//	sFilterConfig.FilterActivation = ENABLE;
//	sFilterConfig.SlaveStartFilterBank = 14;

	HAL_CAN_ConfigFilter(&CAN1_Handler, &sFilterConfig);
	
	HAL_CAN_Start(&CAN1_Handler);			        //初始化
	
	HAL_CAN_ActivateNotification(&CAN1_Handler, CAN_IT_RX_FIFO0_MSG_PENDING);
	HAL_CAN_ActivateNotification(&CAN1_Handler, CAN_IT_RX_FIFO1_MSG_PENDING);
}

//CAN底层驱动，引脚配置，时钟配置，中断配置
//此函数会被HAL_CAN_Init()调用
//hcan:CAN句柄
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
	if(hcan->Instance==CAN1)
	{
		GPIO_InitTypeDef GPIO_Initure;
    
		__HAL_RCC_CAN1_CLK_ENABLE();                //使能CAN1时钟
		__HAL_RCC_GPIOA_CLK_ENABLE();			    //开启GPIOA时钟

		GPIO_Initure.Pin=GPIO_PIN_12;   			//PA12
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //推挽复用
		GPIO_Initure.Pull=GPIO_PULLUP;              //上拉
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;    //高速
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);         //初始化
		
		GPIO_Initure.Pin=GPIO_PIN_11;   			//PA11
		GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;     	//推挽复用
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);         //初始化
		
		HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
		HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
	}
}

void CAN1_RX0_IRQHandler()
{
	HAL_CAN_IRQHandler(&CAN1_Handler);
}

void CAN1_RX1_IRQHandler()
{
	HAL_CAN_IRQHandler(&CAN1_Handler);
}

void CAN1_Send_Message(u8* Vehicle_Data)
{
	u8 i=0;
	u32 TxMailbox;
	u8 Message[8];
    TxHeader.StdId=0xFFFF0001;        //标准标识符
    TxHeader.ExtId=0xFFFF0001;        //扩展标识符(29位)
    TxHeader.IDE=CAN_ID_EXT;    //使用扩展帧
    TxHeader.RTR=CAN_RTR_DATA;  //数据帧
    TxHeader.DLC=8;  
    TxHeader.TransmitGlobalTime = DISABLE;	
	for(i=0;i<=7;i++)
    {
		Message[i]=Vehicle_Data[i];
	}
	while(HAL_CAN_AddTxMessage(&CAN1_Handler, &TxHeader, Message, &TxMailbox)==1);
}