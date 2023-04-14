#include "RTCM.h"
#include "string.h"
#include "stdio.h"
#include "sys.h"

RTKStateData rtk_data;

void Parse_RTK_Data(void)
{
	unsigned int msg_type;
	int i;
	unsigned char *data  = rtk_data.RTK_Buffer;
	char type[10];
	msg_type = (data[3]*16)+(data[4]/16);
	//printf("msg_type:%d\r\n",msg_type);
		switch(msg_type)
		{
			case 1003:strcpy(type , "base" ); break;
			case 1004:strcpy(type , "base" ); break;
			case 1005:strcpy(type , "base" ); break;
			case 1006:strcpy(type , "base" );break;
			case 1011:strcpy(type , "base" );break;
			case 1012:strcpy(type , "base" );break;
			
			case 1074:strcpy(type , "base" ); break;
			case 1075:strcpy(type , "base" ); break;
			case 1076:strcpy(type , "base" );break;
			case 1077:strcpy(type , "base" ); break;
			
			case 1084:strcpy(type , "base" ); break;
			case 1085:strcpy(type , "base" );break;
			case 1086:strcpy(type , "base" ); break;
			case 1087:strcpy(type , "base" ); break;
			
			case 1094:strcpy(type , "base" );break;
			case 1095:strcpy(type , "base" );break;
			case 1096:strcpy(type , "base" );break;
			case 1097:strcpy(type , "base" );break;
			
			case 1104:strcpy(type , "base" ); break;
			case 1105:strcpy(type , "base" );break;
			case 1106:strcpy(type , "base" );break;
			case 1107:strcpy(type , "base" ); break;
			
			case 1114:strcpy(type , "base" ); break;
			case 1115:strcpy(type , "base" ); break;
			case 1116:strcpy(type , "base" ); break;
			case 1117:strcpy(type , "base" ); break;
			
			case 1124:strcpy(type , "base" ); break;
			case 1125:strcpy(type , "base" ); break;
			case 1126:strcpy(type , "base" ); break;
			case 1127:strcpy(type , "base" ); break;
			
			case 1134:strcpy(type , "base" ); break;
			case 1135:strcpy(type , "base" ); break;
			case 1136:strcpy(type , "base" ); break;
			case 1137:strcpy(type , "base" ); break;
			
			case 1019:strcpy(type , "base" ); break;
			case 1020:strcpy(type , "base" ); break;
			case 1042:strcpy(type , "base" ); break;
			case 1044:strcpy(type , "base" ); break;
			
			case 1048:strcpy(type , "base" ); break;
			case 1033:strcpy(type , "base" ); break;
			case 1230:strcpy(type , "base"   ); break;
			
			
			case 1073:strcpy(type , "rover"); break;
			case 1083:strcpy(type , "rover" ); break;
			case 1093:strcpy(type , "rover" ); break;
			case 1103:strcpy(type , "rover" ); break;
			case 1113:strcpy(type , "rover" ); break;
			case 1123:strcpy(type , "rover" ); break;
		}
		printf("base:%d\r\n",strcmp(type,"base"));
		delay_us(50);
		printf("recover:%d\r\n",strcmp(type,"rover"));
		delay_us(50);
		if(strcmp(type,"base") ==0)
		{
			USART_SendData(USART2,0xAA);
			//use usart2 to send data
			/*for(i = 0; i<rtk_data.length;i++ )
			{
			USART_SendData(USART1,rtk_data.RTK_Buffer[i]);
			delay_us(50);
			}*/
			
		}
		else if(strcmp(type,"rover")==0 )
		{
				USART_SendData(USART1,0X55);
			//use usart3 to send data
			/*for(i = 0; i<rtk_data.length;i++ )
			{
				USART_SendData(USART2,data[i]);
				delay_us(50);
			}*/
			
		}
		
	}		
