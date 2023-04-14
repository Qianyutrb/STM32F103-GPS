#ifndef __USART2_H
#define __USART2_H
#include "stm32f10x.h"
#define NMEA_MAX_LEN 0xff  // NMEA 0183数据最大长度为255

void uart2_init(u32 bound);
void process_nmea_data(void);
#endif


