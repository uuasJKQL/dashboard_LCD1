#ifndef __CAN_H
#define __CAN_H
#include "sys.h"								  
//////////////////////////////////////////////////////////////////////////////////

void CAN_Init(void);
void CAN1_Mode_Init(u32 tsjw,u32 tbs2,u32 tbs1,u16 brp,u32 mode);//CAN1≥ı ºªØ
int CAN1_Config(void);
void CAN1_Send_Message(u8* Vehicle_Data);
#endif















