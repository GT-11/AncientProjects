#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "bsp.h"
#include "GUI.h"

u8 Read_Net_Address(u8 *DATA,u8 ADDR)
{
	u8 delay;
	u8 buff[32];
	u8 Address[32]="******_Read_Address_0001_******";
	Address[23]=ADDR+0x30;
///////////////////////////////////////////////////////////////////////
			for(delay=0;delay<200;delay++){}
			NRF24L01_TX_Mode();
			for(delay=0;delay<200;delay++){}
			NRF24L01_TxPacket(Address);
			for(delay=0;delay<200;delay++){}
			NRF24L01_RX_Mode();
			for(delay=0;delay<200;delay++){}
			delay=0;
			while(1)
			{
				if(!NRF24L01_RxPacket(buff))
				{
					if(buff[0]==0x00&&buff[1]==ADDR)
					{
					//	DATA_N[0]=0;
						delay=(ADDR-1)*7;
						DATA[delay]=0x00;
						DATA[delay+1]=ADDR;
						DATA[delay+2]=buff[2];
						DATA[delay+3]=buff[3];
						DATA[delay+4]=buff[4];
						DATA[delay+5]=buff[5];
						DATA[delay+6]=buff[6];
						return 1;
					}
				}
				else if(delay>10)
				{
				//	DATA_N[0]++;
					delay=(ADDR-1)*7;
					DATA[delay]=0x00;
					DATA[delay+1]=ADDR;
					DATA[delay+2]=0x00;
					DATA[delay+3]=0x00;
					DATA[delay+4]=0x00;
					DATA[delay+5]=0x00;
					DATA[delay+6]=0x00;
					return 0;
				}
				if(delay<100)delay++;
				vTaskDelay( 10 );
			}
			/////////////////////////////////////////////////////////////////////////
}




u8 Write_Net_Address(u8 DATA1,u8 DATA2,u8 ADDR)
{
	u8 delay;
	u8 buff[32];
	u8 Address_DATA[32]="**Write_Address_0001_DATA_01**";
	Address_DATA[19]=ADDR+0x30;
	Address_DATA[26]=DATA1;
	Address_DATA[27]=DATA2;
///////////////////////////////////////////////////////////////////////
			for(delay=0;delay<200;delay++){}
			NRF24L01_TX_Mode();
			for(delay=0;delay<200;delay++){}
			NRF24L01_TxPacket(Address_DATA);
			for(delay=0;delay<200;delay++){}
			NRF24L01_RX_Mode();
			for(delay=0;delay<200;delay++){}
			delay=0;
			while(1)
			{
				if(!NRF24L01_RxPacket(buff))
				{
					if(buff[0]==0x00&&buff[1]==ADDR&&buff[2]==0xFA&&buff[3]==0xFC)
					{
					//	DATA_N[0]=0;
						
						return 1;
					}
				}
				else if(delay>10)
				{
				//	DATA_N[0]++;
					
					return 0;
				}
				if(delay<100)delay++;
				vTaskDelay( 10 );
			}
			/////////////////////////////////////////////////////////////////////////
}



