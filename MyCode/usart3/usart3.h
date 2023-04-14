#ifndef __USART3_H
#define __USART3_H
#include "stdio.h"	
#include "sys.h"
#include "string.h"


#define USART3_REC_LEN  			1100  	//定义最大接收字节数 1100
//#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
extern unsigned char  USART3_RX_BUF[USART3_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART3_RX_STA;         		//接收状态标记	



#define false 0
#define true 1

//定义数组长度
#define RTK_Buf_Length 80


void uart3_init(u32 bound);



void CLR_RTKBuf(void);

void clrRTKStruct(void);
#endif


