#ifndef __RTCM_H
#define __RTCM_H

#include "sys.h"




typedef struct 
{
	unsigned char RTK_Buffer[1000];	//receive data until receive state type
	char *state_type;
	char message_type;
	int length;
	char isUsefull;						//��λ��Ϣ�Ƿ���Ч
} RTKStateData;


extern RTKStateData rtk_data;

void Parse_RTK_Data(void);


#endif
