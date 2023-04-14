
/* STM32					GPS
VCC		------>	VCC
GND		------>	GND
RX1		<------	TXD


STM32					USB-TTL模块
GND		------>	GND
TX1		------>	RXD */

#include "stm32f10x.h"
#include "usart.h"
#include "usart2.h"
#include "usart3.h"

int main(void)
{	

	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart1_init(115200);	 	//PC
	uart2_init(115200);	 	//GPS
	uart3_init(115200);		//4G DTU

	while(1)
	{
		process_nmea_data();//处理GPS模块数据
	}
}

