
/* STM32					GPS
VCC		------>	VCC
GND		------>	GND
RX1		<------	TXD


STM32					USB-TTLģ��
GND		------>	GND
TX1		------>	RXD */

#include "stm32f10x.h"
#include "usart.h"
#include "usart2.h"
#include "usart3.h"

int main(void)
{	

	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart1_init(115200);	 	//PC
	uart2_init(115200);	 	//GPS
	uart3_init(115200);		//4G DTU

	while(1)
	{
		process_nmea_data();//����GPSģ������
	}
}

