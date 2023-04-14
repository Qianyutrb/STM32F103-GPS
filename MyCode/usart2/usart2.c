
#include "usart2.h"	
#include "string.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "stm32f10x.h"
#include <stdbool.h>

uint8_t usart2_buffer[NMEA_MAX_LEN]; // ���建����
uint8_t buffer_index = 0; // �������±�
bool start_collecting = false; // �Ƿ�ʼ����NMEA���ݵı�־
bool is_nmea_complete = false; // һ��NMEA�����Ƿ������ϵı�־
bool gpgga_received = false;//���յ�GGA���ݱ�־
/* ��δ�����������ʼ��STM32��USART2����ģ��ģ���Ҫ�������²��裺

1.ʹ��USART2��GPIOD��ʱ�ӣ�����ʹ�õ���APB2���ߣ�
2.��USART2��TX��RX����ӳ�䵽GPIOD�ϣ�ʹ����GPIO_PinRemapConfig()������
3.����GPIO�˿ڣ��ֱ��ʼ��USART2��TX��RX���ţ�����USART2_TXʹ�ø����������ģʽ��USART2_RXʹ�ø�������ģʽ��
4.����USART2�жϣ�������ռ���ȼ��������ȼ���IRQͨ��ʹ�ܵȣ�
5.��ʼ��USART2���ڣ����������ʡ��ֳ���ֹͣλ����żУ��λ��Ӳ�����������ƺ��շ�ģʽ�ȣ�
6.���������жϣ�
7.ʹ��USART2���ڡ� */

void uart2_init(u32 bound)
{
    // GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOD, ENABLE); // ʹ��USART2��GPIODʱ��
    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE); // ��USART2��TX��RX����ӳ�䵽GPIOD��

    // USART2_TX   PD.5
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // �����������
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // USART2_RX	  PD.6
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ��������
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // Usart2 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // ��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; // �����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure); // ����ָ���Ĳ�����ʼ��VIC�Ĵ���

    // USART ��ʼ������
    USART_InitStructure.USART_BaudRate = bound; // ����������Ϊ����Ĳ���
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // �ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1; // һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No; // ����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // �շ�ģʽ
    USART_Init(USART2, &USART_InitStructure); // ��ʼ������

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // ���������ж�

    USART_Cmd(USART2, ENABLE); // ʹ�ܴ���
}


/* ��δ�����һ���жϷ����������ڴ���USART2���ڽ��յ����ݵ��жϡ�

��USART2���յ�����ʱ���ᴥ���жϣ���ʱCPU����ת���ú���ִ�С�

����ʹ��USART_GetITStatus�����ж��Ƿ���յ������ݣ�������յ��ˣ������USART_ReceiveData������ȡ���յ������ݡ�

��������ͨ��start_collecting�����ж��Ƿ��Ѿ���ʼ����NMEA���ݡ�

�����û�п�ʼ���գ����жϽ��յ��������Ƿ�Ϊ'$'������ǣ���˵����ʼ�������ݣ���start_collecting����Ϊtrue������'$'����usart2_buffer�������У�ͬʱ��������������buffer_index����Ϊ1��

����Ѿ���ʼ����NMEA���ݣ������ݴ���usart2_buffer�������У�ͬʱ�жϻ������Ƿ�������

�����������������˵�����ݸ�ʽ������Ҫ��ջ��������½��ա����������δ����������洢���ݣ�ͬʱ�ж��Ƿ���յ���һ��������NMEA���ݡ�

�����յ��˻��з�'\n'�ͻس���'\r'ʱ��˵��һ��NMEA���ݽ�����ϣ���is_nmea_complete��־����Ϊtrue��ͬʱ��start_collecting����Ϊfalse��

���⣬��������յ��������Ƿ����"$GPGGA"�����������gpgga_received��־����Ϊtrue����������Ϊfalse�� */

void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // ���յ�����
	{
		uint8_t data = USART_ReceiveData(USART2);

		if (!start_collecting) // �����û�п�ʼ����NMEA����
		{
			if (data == '$') // ������յ���'$'����ʼ��������
			{
				start_collecting = true;
				usart2_buffer[0] = data; // ��'$'���뻺����
				buffer_index = 1;
			}
		}
		else // ����Ѿ���ʼ����NMEA����
		{
			if (buffer_index >= NMEA_MAX_LEN) // ������������˵�����ݸ�ʽ������Ҫ��ջ��������½���
			{
				start_collecting = false;
				buffer_index = 0;
			}
			else
			{
				usart2_buffer[buffer_index++] = data; // �����ݴ��뻺����

				if (data == '\n' && usart2_buffer[buffer_index - 2] == '\r') // ������յ��˻��з�'\r\n'��˵��һ��NMEA���ݽ������
				{
					is_nmea_complete = true;
					start_collecting = false;

					// �����յ��������Ƿ����"$GPGGA"
					if (strstr((const char *)usart2_buffer, "$GPGGA") != NULL)
					{
						gpgga_received = true;
					}
					else
					{
						gpgga_received = false;
					}
				}
			}
		}
	}
}

/* ��δ���ʵ���˴�����յ���NMEA���ݵĹ��ܡ�

��is_nmea_completeΪtrueʱ����ʾ�Ѿ����յ���������һ��NMEA���ݣ���ʱ���ԶԽ��յ������ݽ��д���

����ͨ��һ��ѭ����usart2_buffer�е����ݷ��͵�USART2�����У������յ���NMEA���ݻش������ͷ���

ͬʱ�����gpgga_receivedΪtrue�����ʾ���յ���"$GPGGA"���͵�NMEA���ݣ���ʱ�����ݷ��͵�USART3�����С�

��󣬴������usart2_buffer����������buffer_index����Ϊ0������is_nmea_complete����Ϊfalse��׼��������һ�����ݡ� */

void process_nmea_data(void) // ������յ���NMEA����
{
	uint8_t bylen = 0;

	if (is_nmea_complete) // ������յ���������һ��NMEA����
	{
		// �����ﴦ����յ���NMEA����
		for (uint8_t i = 0; i < buffer_index; i++)
		{
			while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
				; // �ȴ����ڿ���
			USART_SendData(USART2, usart2_buffer[bylen++]);
		}

		bylen = 0;
	}

	if (gpgga_received)
	{
		// �����ﴦ����յ���GGA����
		for (uint8_t i = 0; i < buffer_index; i++)
		{
			while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
				; // �ȴ����ڿ���
			USART_SendData(USART3, usart2_buffer[bylen++]);
		}
	}

	// ������Ϻ���ջ�������׼��������һ������
	memset(usart2_buffer, 0, NMEA_MAX_LEN);
	buffer_index = 0;
	is_nmea_complete = false;
}
