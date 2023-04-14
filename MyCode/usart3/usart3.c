#include "usart3.h"	
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "stm32f10x.h"


void uart3_init(u32 bound)
{
	// GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  // GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); // ʹ��USART3��GPIOAʱ��
	// USART3_TX   PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // �����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// USART3_RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// Usart1 NVIC ����

	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4; // ��ռ���ȼ�4
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  // �����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);							  // ����ָ���Ĳ�����ʼ��VIC�Ĵ���

	// USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;										// һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// �ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								// ����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// �շ�ģʽ

	USART_Init(USART3, &USART_InitStructure);	   // ��ʼ������
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // �����ж�
	USART_Cmd(USART3, ENABLE);					   // ʹ�ܴ���
}


void USART3_IRQHandler(void) // ����3�жϷ������
{
	uint8_t usart3_data = 0;
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		usart3_data = USART_ReceiveData(USART1);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
			; // �ȴ����ڿ���
		USART_SendData(USART1, usart3_data);//RTK���ݲ�������ֱ��ת��
	}
}