
#include "usart2.h"	
#include "string.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "stm32f10x.h"
#include <stdbool.h>

uint8_t usart2_buffer[NMEA_MAX_LEN]; // 定义缓存区
uint8_t buffer_index = 0; // 缓存区下标
bool start_collecting = false; // 是否开始接收NMEA数据的标志
bool is_nmea_complete = false; // 一条NMEA数据是否接收完毕的标志
bool gpgga_received = false;//接收到GGA数据标志
/* 这段代码是用来初始化STM32的USART2串口模块的，主要包括以下步骤：

1.使能USART2和GPIOD的时钟，这里使用的是APB2总线；
2.将USART2的TX和RX引脚映射到GPIOD上，使用了GPIO_PinRemapConfig()函数；
3.配置GPIO端口，分别初始化USART2的TX和RX引脚，其中USART2_TX使用复用推挽输出模式，USART2_RX使用浮空输入模式；
4.配置USART2中断，包括抢占优先级、子优先级和IRQ通道使能等；
5.初始化USART2串口，包括波特率、字长、停止位、奇偶校验位、硬件数据流控制和收发模式等；
6.开启接收中断；
7.使能USART2串口。 */

void uart2_init(u32 bound)
{
    // GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOD, ENABLE); // 使能USART2和GPIOD时钟
    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE); // 将USART2的TX和RX引脚映射到GPIOD上

    // USART2_TX   PD.5
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // USART2_RX	  PD.6
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // Usart2 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // 抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; // 子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // IRQ通道使能
    NVIC_Init(&NVIC_InitStructure); // 根据指定的参数初始化VIC寄存器

    // USART 初始化设置
    USART_InitStructure.USART_BaudRate = bound; // 波特率设置为传入的参数
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1; // 一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No; // 无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式
    USART_Init(USART2, &USART_InitStructure); // 初始化串口

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 开启接收中断

    USART_Cmd(USART2, ENABLE); // 使能串口
}


/* 这段代码是一个中断服务函数，用于处理USART2串口接收到数据的中断。

当USART2接收到数据时，会触发中断，此时CPU会跳转到该函数执行。

首先使用USART_GetITStatus函数判断是否接收到了数据，如果接收到了，则调用USART_ReceiveData函数获取接收到的数据。

接下来，通过start_collecting变量判断是否已经开始接收NMEA数据。

如果还没有开始接收，则判断接收到的数据是否为'$'，如果是，则说明开始接收数据，将start_collecting设置为true，并将'$'存入usart2_buffer缓存区中，同时将缓存区的索引buffer_index设置为1。

如果已经开始接收NMEA数据，则将数据存入usart2_buffer缓存区中，同时判断缓存区是否已满。

如果缓存区已满，则说明数据格式有误，需要清空缓存区重新接收。如果缓存区未满，则继续存储数据，同时判断是否接收到了一条完整的NMEA数据。

当接收到了换行符'\n'和回车符'\r'时，说明一条NMEA数据接收完毕，将is_nmea_complete标志设置为true，同时将start_collecting设置为false。

此外，还会检查接收到的数据是否包含"$GPGGA"，如果包含则将gpgga_received标志设置为true，否则设置为false。 */

void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // 接收到数据
	{
		uint8_t data = USART_ReceiveData(USART2);

		if (!start_collecting) // 如果还没有开始接收NMEA数据
		{
			if (data == '$') // 如果接收到了'$'，则开始接收数据
			{
				start_collecting = true;
				usart2_buffer[0] = data; // 将'$'存入缓存区
				buffer_index = 1;
			}
		}
		else // 如果已经开始接收NMEA数据
		{
			if (buffer_index >= NMEA_MAX_LEN) // 缓存区已满，说明数据格式有误，需要清空缓存区重新接收
			{
				start_collecting = false;
				buffer_index = 0;
			}
			else
			{
				usart2_buffer[buffer_index++] = data; // 将数据存入缓存区

				if (data == '\n' && usart2_buffer[buffer_index - 2] == '\r') // 如果接收到了换行符'\r\n'，说明一条NMEA数据接收完毕
				{
					is_nmea_complete = true;
					start_collecting = false;

					// 检查接收到的数据是否包含"$GPGGA"
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

/* 这段代码实现了处理接收到的NMEA数据的功能。

当is_nmea_complete为true时，表示已经接收到了完整的一条NMEA数据，此时可以对接收到的数据进行处理。

代码通过一个循环将usart2_buffer中的数据发送到USART2串口中，将接收到的NMEA数据回传给发送方。

同时，如果gpgga_received为true，则表示接收到了"$GPGGA"类型的NMEA数据，此时将数据发送到USART3串口中。

最后，代码清空usart2_buffer缓存区，将buffer_index设置为0，并将is_nmea_complete设置为false，准备接收下一条数据。 */

void process_nmea_data(void) // 处理接收到的NMEA数据
{
	uint8_t bylen = 0;

	if (is_nmea_complete) // 如果接收到了完整的一条NMEA数据
	{
		// 在这里处理接收到的NMEA数据
		for (uint8_t i = 0; i < buffer_index; i++)
		{
			while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
				; // 等待串口空闲
			USART_SendData(USART2, usart2_buffer[bylen++]);
		}

		bylen = 0;
	}

	if (gpgga_received)
	{
		// 在这里处理接收到的GGA数据
		for (uint8_t i = 0; i < buffer_index; i++)
		{
			while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
				; // 等待串口空闲
			USART_SendData(USART3, usart2_buffer[bylen++]);
		}
	}

	// 处理完毕后清空缓存区，准备接收下一条数据
	memset(usart2_buffer, 0, NMEA_MAX_LEN);
	buffer_index = 0;
	is_nmea_complete = false;
}
