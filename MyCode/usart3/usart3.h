#ifndef __USART3_H
#define __USART3_H
#include "stdio.h"	
#include "sys.h"
#include "string.h"


#define USART3_REC_LEN  			1100  	//�����������ֽ��� 1100
//#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern unsigned char  USART3_RX_BUF[USART3_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART3_RX_STA;         		//����״̬���	



#define false 0
#define true 1

//�������鳤��
#define RTK_Buf_Length 80


void uart3_init(u32 bound);



void CLR_RTKBuf(void);

void clrRTKStruct(void);
#endif


