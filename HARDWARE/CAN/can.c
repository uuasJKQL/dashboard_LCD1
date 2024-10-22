#include "can.h"
#include "usart.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 

CAN_InitTypeDef		CAN1_InitConf;
CAN_HandleTypeDef	CAN1_Handler;   //CAN1���
CAN_TxHeaderTypeDef	TxHeader;      //����
CAN_RxHeaderTypeDef	RxHeader;      //����
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
    CAN1_Handler.Init.Prescaler=brp;				//��Ƶϵ��(Fdiv)Ϊbrp+1
    CAN1_Handler.Init.Mode=mode;					//ģʽ���� 
    CAN1_Handler.Init.SyncJumpWidth=tsjw;			//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1TQ~CAN_SJW_4TQ
    CAN1_Handler.Init.TimeSeg1=tbs1;				//tbs1��ΧCAN_BS1_1TQ~CAN_BS1_16TQ
    CAN1_Handler.Init.TimeSeg2=tbs2;				//tbs2��ΧCAN_BS2_1TQ~CAN_BS2_8TQ
    CAN1_Handler.Init.TimeTriggeredMode=DISABLE;	//��ʱ�䴥��ͨ��ģʽ 
    CAN1_Handler.Init.AutoBusOff=DISABLE;			//����Զ����߹���
    CAN1_Handler.Init.AutoWakeUp=DISABLE;			//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
    CAN1_Handler.Init.AutoRetransmission=DISABLE;	//��ֹ�����Զ����� 
    CAN1_Handler.Init.ReceiveFifoLocked=DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ� 
    CAN1_Handler.Init.TransmitFifoPriority=ENABLE;	//���ȼ��ɱ����Ⱥ�˳����� 
	
    HAL_CAN_Init(&CAN1_Handler);			        //��ʼ��

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
	
	HAL_CAN_Start(&CAN1_Handler);			        //��ʼ��
	
	HAL_CAN_ActivateNotification(&CAN1_Handler, CAN_IT_RX_FIFO0_MSG_PENDING);
	HAL_CAN_ActivateNotification(&CAN1_Handler, CAN_IT_RX_FIFO1_MSG_PENDING);
}

//CAN�ײ��������������ã�ʱ�����ã��ж�����
//�˺����ᱻHAL_CAN_Init()����
//hcan:CAN���
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
	if(hcan->Instance==CAN1)
	{
		GPIO_InitTypeDef GPIO_Initure;
    
		__HAL_RCC_CAN1_CLK_ENABLE();                //ʹ��CAN1ʱ��
		__HAL_RCC_GPIOA_CLK_ENABLE();			    //����GPIOAʱ��

		GPIO_Initure.Pin=GPIO_PIN_12;   			//PA12
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //���츴��
		GPIO_Initure.Pull=GPIO_PULLUP;              //����
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;    //����
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);         //��ʼ��
		
		GPIO_Initure.Pin=GPIO_PIN_11;   			//PA11
		GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;     	//���츴��
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);         //��ʼ��
		
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
    TxHeader.StdId=0xFFFF0001;        //��׼��ʶ��
    TxHeader.ExtId=0xFFFF0001;        //��չ��ʶ��(29λ)
    TxHeader.IDE=CAN_ID_EXT;    //ʹ����չ֡
    TxHeader.RTR=CAN_RTR_DATA;  //����֡
    TxHeader.DLC=8;  
    TxHeader.TransmitGlobalTime = DISABLE;	
	for(i=0;i<=7;i++)
    {
		Message[i]=Vehicle_Data[i];
	}
	while(HAL_CAN_AddTxMessage(&CAN1_Handler, &TxHeader, Message, &TxMailbox)==1);
}