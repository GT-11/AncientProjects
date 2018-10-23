#include "recorder.h" 
#include "audioplay.h"
#include "ff.h"
#include "malloc.h"
#include "text.h"
#include "usart.h"
#include "wm8978.h"
#include "i2s.h"
#include "led.h"
#include "lcd.h"
#include "delay.h"
//#include "key.h"
#include "exfuns.h"  
#include "text.h"
#include "string.h"  
#include "RecordingGyroAccelerometerData.h"
#include "adc.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//录音机 应用代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/6/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
extern void (*adc_rx_callback)(void);
extern u8 DMA_Rx_Flag;
extern u8 GAD_Rx_Flag;
extern char BordID[9];
extern unsigned __int64 Windows_time;
extern unsigned char Real_Time_Year;
extern unsigned char Real_Time_Month;
extern unsigned char Real_Time_Day;
extern unsigned char Real_Time_Hour;
extern unsigned char Real_Time_Minute;
extern unsigned char Real_Time_Second;
extern unsigned int Real_Time_Millise;


extern unsigned char Real_Time_Year_STOP,Real_Time_Month_STOP,Real_Time_Day_STOP,Real_Time_Hour_STOP,Real_Time_Minute_STOP,Real_Time_Second_STOP;
extern unsigned int Real_Time_Millise_STOP;
u8 *i2srecbuf1;
u8 *i2srecbuf2; 
u8 *secondcache;

u8 AM_Factor = 0;


u32 secondcache_write_loc = 0;   //存储位置（存储后）
u32 secondcache_read_loc = 0;  //读取位置（读取后）


FIL* f_rec=0;		//录音文件	
u32 wavsize;		//wav数据大小(字节数,不包括文件头!!)
u8 rec_sta=0;		//录音状态
					//[7]:0,没有开启录音;1,已经开启录音;
					//[6:1]:保留
					//[0]:0,正在录音;1,暂停录音;
u8 DMA_Rx_Flag=0;					
					
u32 PackegStart = 0;
u32 PackegEnd = 0;				
u8 ErrorRecData[2048];

u32 PackegLen = 0;
void wav_second_level_cache(void)
{
	int i;
	u16*p;
	if(rec_sta==0X80)//录音模式
	{  
		if(DMA2_Stream0->CR&(1<<19))
		{
			//res=f_write(f_rec,i2srecbuf1,I2S_RX_DMA_BUF_SIZE,(UINT*)&bw);//写入文件
			if(secondcache_write_loc == I2S_RX_DMA_BUF_SIZE * 90)
				secondcache_write_loc = 0;
			for(i=0;i<I2S_RX_DMA_BUF_SIZE;i++)
			{
			 secondcache[secondcache_write_loc + i] = i2srecbuf1[i];
			 if(i%2)
			 {
				 p = (u16*)&secondcache[secondcache_write_loc + i - 1];
				 *p -= 1900;
				 *p = *p << AM_Factor;
			 }
			}
			secondcache_write_loc+=I2S_RX_DMA_BUF_SIZE;
			DMA_Rx_Flag = 1;
		}else 
		{
			//res=f_write(f_rec,i2srecbuf2,I2S_RX_DMA_BUF_SIZE,(UINT*)&bw);//写入文件
			if(secondcache_write_loc == I2S_RX_DMA_BUF_SIZE * 90)
				secondcache_write_loc = 0;
			
			for(i=0;i<I2S_RX_DMA_BUF_SIZE;i++)
			{
			 secondcache[secondcache_write_loc + i] = i2srecbuf2[i];
				if(i%2)
				{
					p = (u16*)&secondcache[secondcache_write_loc + i - 1];
					*p -= 1900;
					*p = *p << AM_Factor;
				}
			}
			secondcache_write_loc+=I2S_RX_DMA_BUF_SIZE;
			DMA_Rx_Flag = 1;
		}
	} 
}
					
	//录音 I2S_DMA接收中断服务函数.在中断里面写入数据
void rec_adc_dma_rx_callback(void) 
{    
	u16 bw;
	u8 res;
	u32 writesize;
	u32 i = 0;
	u32 datasum = 0;
	int start;
	u32 ErrorS,ErrorE,start_len,start_sum;
	if(rec_sta==0X80)//录音模式
	{  
		
		start = StrEqual(USART_RX_BUF,"Fix Data Request\r\n",USART_REC_LEN,strlen("Fix Data Request\r\n"));
		if(start != -1)
		{
			start_len = USART_RX_BUF[start + 1] << 24;
			start_len |= USART_RX_BUF[start + 2] << 16;
			start_len |= USART_RX_BUF[start + 3] << 8;
			start_len |= USART_RX_BUF[start + 4];
			
			start_sum = USART_RX_BUF[start + 5] << 24;
			start_sum |= USART_RX_BUF[start + 6] << 16;
			start_sum |= USART_RX_BUF[start + 7] << 8;
			start_sum |= USART_RX_BUF[start + 8];
			
			datasum = 0;
			start_len &= 0x0000ffff;
			for(i = 0; i < (u8)start_len + 8; i++)
				datasum += USART_RX_BUF[start + 9 + i];
			for(i = 0; i < strlen("Fix Data Request\r\n"); i++)
				datasum += "Fix Data Request\r\n"[i];
			
			if(datasum == start_sum)
			{
				ErrorS = USART_RX_BUF[start + 17] << 24;
				ErrorS |= USART_RX_BUF[start + 18] << 16;
				ErrorS |= USART_RX_BUF[start + 19] << 8;
				ErrorS |= USART_RX_BUF[start + 20];
				
				ErrorE = USART_RX_BUF[start + 21] << 24;
				ErrorE |= USART_RX_BUF[start + 22] << 16;
				ErrorE |= USART_RX_BUF[start + 23] << 8;
				ErrorE |= USART_RX_BUF[start + 24];
				
				if(ErrorE > ErrorS && ErrorE < wavsize && ErrorS < wavsize && ErrorE > 0 && (ErrorS > 0 || ErrorS == 0))
				{
					f_lseek(f_rec,44 + ErrorS);
					if((ErrorE - ErrorS) > 2048)
					{
						f_read(f_rec,ErrorRecData,2048,&br);
						i = 2048;
						PackegEnd = ErrorS + 2048;
					}
					else
					{
						f_read(f_rec,ErrorRecData,(ErrorE - ErrorS),&br);
						i = (ErrorE - ErrorS);
						PackegEnd = ErrorE;
					}
					
					
					
					PackegStart = ErrorS;
					
					ErrorS = i;
					
					
					PackegEnd = PackegEnd - PackegStart + 12;
					datasum = 0;
					while(i--)
						datasum+=ErrorRecData[i];
					for(i = 0;i<8;i++)
						datasum+=BordID[i];
					for(i = 0;i<strlen("Recoder Repair Data\r\n");i++)
						datasum+="Recoder Repair Data\r\n"[i];
					datasum += ((u8*)&PackegStart)[3];
					datasum += ((u8*)&PackegStart)[2];
					datasum += ((u8*)&PackegStart)[1];
					datasum += ((u8*)&PackegStart)[0];
					datasum += Real_Time_Year_STOP + Real_Time_Month_STOP + Real_Time_Day_STOP + Real_Time_Hour_STOP + Real_Time_Minute_STOP + Real_Time_Second_STOP;
					datasum += ((u8*)&Real_Time_Millise_STOP)[1];
					datasum += ((u8*)&Real_Time_Millise_STOP)[0];
					
					
					printf("Recoder Repair Data\r\n");
					UARTSendData(&((u8*)&PackegEnd)[3],1);
					UARTSendData(&((u8*)&PackegEnd)[2],1);
					UARTSendData(&((u8*)&PackegEnd)[1],1);
					UARTSendData(&((u8*)&PackegEnd)[0],1);
					
					UARTSendData(&((u8*)&datasum)[3],1);
					UARTSendData(&((u8*)&datasum)[2],1);
					UARTSendData(&((u8*)&datasum)[1],1);
					UARTSendData(&((u8*)&datasum)[0],1);
					
					UARTSendData(BordID,8);
					
					UARTSendData(&((u8*)&PackegStart)[3],1);
					UARTSendData(&((u8*)&PackegStart)[2],1);
					UARTSendData(&((u8*)&PackegStart)[1],1);
					UARTSendData(&((u8*)&PackegStart)[0],1);
					
					

					
					UARTSendData(&Real_Time_Year_STOP,1);
					UARTSendData(&Real_Time_Month_STOP,1);
					UARTSendData(&Real_Time_Day_STOP,1);
					UARTSendData(&Real_Time_Hour_STOP,1);
					UARTSendData(&Real_Time_Minute_STOP,1);
					UARTSendData(&Real_Time_Second_STOP,1);
					UARTSendData(&((u8*)&Real_Time_Millise_STOP)[1],1);
					UARTSendData(&((u8*)&Real_Time_Millise_STOP)[0],1);
					//
//					for(i = 0;i<1024;i++)
//						ErrorRecData[i] = 11;
//					i = 1024;
//					ErrorS = i;
//					while(i--)
//						datasum+=ErrorRecData[i];
					//
					

					UARTSendData(ErrorRecData,ErrorS);

					
					datasum = 0;
					ClearBuffer(USART_RX_BUF,USART_REC_LEN);
					f_lseek(f_rec,44 + wavsize);
					return;
				}
			}
		}
		
		
		
		
		if(secondcache_read_loc < secondcache_write_loc)
		{
			writesize = secondcache_write_loc - secondcache_read_loc;
			res=f_write(f_rec,&secondcache[secondcache_read_loc],writesize,(UINT*)&bw);//写入文件
			
			i = writesize;
			datasum = 0;
			PackegStart = wavsize;
			PackegEnd = writesize + 12;
			while(i--)
				datasum+=secondcache[secondcache_read_loc + i];
			for(i = 0;i<8;i++)
				datasum+=BordID[i];
			for(i = 0;i<strlen("Recoder Source Data\r\n");i++)
				datasum+="Recoder Source Data\r\n"[i];
			datasum += ((u8*)&PackegStart)[3];
			datasum += ((u8*)&PackegStart)[2];
			datasum += ((u8*)&PackegStart)[1];
			datasum += ((u8*)&PackegStart)[0];
			datasum += Real_Time_Year_STOP + Real_Time_Month_STOP + Real_Time_Day_STOP + Real_Time_Hour_STOP + Real_Time_Minute_STOP + Real_Time_Second_STOP;
			datasum += ((u8*)&Real_Time_Millise_STOP)[1];
			datasum += ((u8*)&Real_Time_Millise_STOP)[0];
			
			
			printf("Recoder Source Data\r\n");
			UARTSendData(&((u8*)&PackegEnd)[3],1);
			UARTSendData(&((u8*)&PackegEnd)[2],1);
			UARTSendData(&((u8*)&PackegEnd)[1],1);
			UARTSendData(&((u8*)&PackegEnd)[0],1);
			
			UARTSendData(&((u8*)&datasum)[3],1);
			UARTSendData(&((u8*)&datasum)[2],1);
			UARTSendData(&((u8*)&datasum)[1],1);
			UARTSendData(&((u8*)&datasum)[0],1);
			
			UARTSendData(BordID,8);
			
			UARTSendData(&((u8*)&PackegStart)[3],1);
			UARTSendData(&((u8*)&PackegStart)[2],1);
			UARTSendData(&((u8*)&PackegStart)[1],1);
			UARTSendData(&((u8*)&PackegStart)[0],1);
			
			
					UARTSendData(&Real_Time_Year_STOP,1);
					UARTSendData(&Real_Time_Month_STOP,1);
					UARTSendData(&Real_Time_Day_STOP,1);
					UARTSendData(&Real_Time_Hour_STOP,1);
					UARTSendData(&Real_Time_Minute_STOP,1);
					UARTSendData(&Real_Time_Second_STOP,1);
					UARTSendData(&((u8*)&Real_Time_Millise_STOP)[1],1);
					UARTSendData(&((u8*)&Real_Time_Millise_STOP)[0],1);
			
			UARTSendData(&secondcache[secondcache_read_loc],writesize);

			

			if(!res)
			{
				secondcache_read_loc += writesize;
				wavsize+=writesize;
			}
			return;
		}
		
		
		
		
		if(secondcache_read_loc > secondcache_write_loc)
		{
			writesize = (90*I2S_RX_DMA_BUF_SIZE) - secondcache_read_loc;
			res=f_write(f_rec,&secondcache[secondcache_read_loc],writesize,(UINT*)&bw);//写入文件
			
			i = writesize;
			datasum = 0;
			while(i--)
				datasum+=secondcache[secondcache_read_loc + i];
			for(i = 0;i<8;i++)
				datasum+=BordID[i];
			for(i = 0;i<strlen("Recoder Source Data\r\n");i++)
				datasum+="Recoder Source Data\r\n"[i];
			
			PackegStart = wavsize;
			PackegEnd = writesize + 12;
			
			datasum += ((u8*)&PackegStart)[3];
			datasum += ((u8*)&PackegStart)[2];
			datasum += ((u8*)&PackegStart)[1];
			datasum += ((u8*)&PackegStart)[0];
			datasum += Real_Time_Year_STOP + Real_Time_Month_STOP + Real_Time_Day_STOP + Real_Time_Hour_STOP + Real_Time_Minute_STOP + Real_Time_Second_STOP;
			datasum += ((u8*)&Real_Time_Millise_STOP)[1];
			datasum += ((u8*)&Real_Time_Millise_STOP)[0];
			
			
			printf("Recoder Source Data\r\n");
			UARTSendData(&((u8*)&PackegEnd)[3],1);
			UARTSendData(&((u8*)&PackegEnd)[2],1);
			UARTSendData(&((u8*)&PackegEnd)[1],1);
			UARTSendData(&((u8*)&PackegEnd)[0],1);
			
			UARTSendData(&((u8*)&datasum)[3],1);
			UARTSendData(&((u8*)&datasum)[2],1);
			UARTSendData(&((u8*)&datasum)[1],1);
			UARTSendData(&((u8*)&datasum)[0],1);
			
			UARTSendData(BordID,8);
			
			UARTSendData(&((u8*)&PackegStart)[3],1);
			UARTSendData(&((u8*)&PackegStart)[2],1);
			UARTSendData(&((u8*)&PackegStart)[1],1);
			UARTSendData(&((u8*)&PackegStart)[0],1);
			
			
					UARTSendData(&Real_Time_Year_STOP,1);
					UARTSendData(&Real_Time_Month_STOP,1);
					UARTSendData(&Real_Time_Day_STOP,1);
					UARTSendData(&Real_Time_Hour_STOP,1);
					UARTSendData(&Real_Time_Minute_STOP,1);
					UARTSendData(&Real_Time_Second_STOP,1);
					UARTSendData(&((u8*)&Real_Time_Millise_STOP)[1],1);
					UARTSendData(&((u8*)&Real_Time_Millise_STOP)[0],1);
			
			UARTSendData(&secondcache[secondcache_read_loc],writesize);
			

			if(!res)
			{
				secondcache_read_loc = 0;
				wavsize+=writesize;
			}
			return;
		}

		
		
		
	} 

}				



void DataCheck()
{
	unsigned char delay_loop = 0;
	u32 i = 0;
	u32 datasum = 0;
	int start,end;
	u32 ErrorS,ErrorE,RecvComLoc3,start_len,start_sum;
	ClearBuffer(USART_RX_BUF,USART_REC_LEN);
	LED0 = 0;
	LED1 = 1;
	do
	{
		delay_loop++;
		
		
		if(delay_loop == 15)
		{
			delay_loop = 0;
			printf("Device is Idle\r\n");
			RecvComLoc3 = 1;
			UARTSendData(&((u8*)&RecvComLoc3)[3],1);
			UARTSendData(&((u8*)&RecvComLoc3)[2],1);
			UARTSendData(&((u8*)&RecvComLoc3)[1],1);
			UARTSendData(&((u8*)&RecvComLoc3)[0],1);
			RecvComLoc3 = 0;
			for(i = 0;i<strlen("Device is Idle\r\n");i++)
				RecvComLoc3+="Device is Idle\r\n"[i];
			for(i = 0;i<8;i++)
				RecvComLoc3+=BordID[i];
			RecvComLoc3+=5;
			UARTSendData(&((u8*)&RecvComLoc3)[3],1);
			UARTSendData(&((u8*)&RecvComLoc3)[2],1);
			UARTSendData(&((u8*)&RecvComLoc3)[1],1);
			UARTSendData(&((u8*)&RecvComLoc3)[0],1);
			UARTSendData(BordID,8);
			RecvComLoc3 = 5;
			UARTSendData(&((u8*)&RecvComLoc3)[0],1);
		}
		
		
		
		
		
		
		
		
		LED0 =! LED0; 
		LED1 =! LED1; 
		delay_ms(50);
		
		
		
		
		
		start = StrEqual(USART_RX_BUF,"Fix Data Request\r\n",USART_REC_LEN,strlen("Fix Data Request\r\n"));
		if(start != -1)
		{
			start_len = USART_RX_BUF[start + 1] << 24;
			start_len |= USART_RX_BUF[start + 2] << 16;
			start_len |= USART_RX_BUF[start + 3] << 8;
			start_len |= USART_RX_BUF[start + 4];
			
			start_sum = USART_RX_BUF[start + 5] << 24;
			start_sum |= USART_RX_BUF[start + 6] << 16;
			start_sum |= USART_RX_BUF[start + 7] << 8;
			start_sum |= USART_RX_BUF[start + 8];
			
			datasum = 0;
			start_len &= 0x0000ffff;
			for(i = 0; i < (u8)start_len + 8; i++)
				datasum += USART_RX_BUF[start + 9 + i];
			for(i = 0; i < strlen("Fix Data Request\r\n"); i++)
				datasum += "Fix Data Request\r\n"[i];
			
			if(datasum == start_sum)
			{
				delay_loop = 14;
				ErrorS = USART_RX_BUF[start + 17] << 24;
				ErrorS |= USART_RX_BUF[start + 18] << 16;
				ErrorS |= USART_RX_BUF[start + 19] << 8;
				ErrorS |= USART_RX_BUF[start + 20];
				
				ErrorE = USART_RX_BUF[start + 21] << 24;
				ErrorE |= USART_RX_BUF[start + 22] << 16;
				ErrorE |= USART_RX_BUF[start + 23] << 8;
				ErrorE |= USART_RX_BUF[start + 24];
				if(ErrorE > ErrorS && ErrorE < wavsize && ErrorS < wavsize && ErrorE > 0 && (ErrorS > 0 || ErrorS == 0))
				{
					f_lseek(f_rec,44 + ErrorS);
					if((ErrorE - ErrorS) > 2048)
					{
						f_read(f_rec,ErrorRecData,2048,&br);
						i = 2048;
						PackegEnd = ErrorS + 2048;
					}
					else
					{
						f_read(f_rec,ErrorRecData,(ErrorE - ErrorS),&br);
						i = (ErrorE - ErrorS);
						PackegEnd = ErrorE;
					}
					
					
					
					PackegStart = ErrorS;
					
					ErrorS = i;
					PackegEnd = ErrorS + 12;
					datasum = 0;
					while(i--)
						datasum+=ErrorRecData[i];
					
					
					
					for(i = 0;i<8;i++)
						datasum+=BordID[i];
					for(i = 0;i<strlen("Recoder Repair Data\r\n");i++)
						datasum+="Recoder Repair Data\r\n"[i];
					datasum += ((u8*)&PackegStart)[3];
					datasum += ((u8*)&PackegStart)[2];
					datasum += ((u8*)&PackegStart)[1];
					datasum += ((u8*)&PackegStart)[0];
					datasum += Real_Time_Year_STOP + Real_Time_Month_STOP + Real_Time_Day_STOP + Real_Time_Hour_STOP + Real_Time_Minute_STOP + Real_Time_Second_STOP;
					datasum += ((u8*)&Real_Time_Millise_STOP)[1];
					datasum += ((u8*)&Real_Time_Millise_STOP)[0];
					
					
					printf("Recoder Repair Data\r\n");
					UARTSendData(&((u8*)&PackegEnd)[3],1);
					UARTSendData(&((u8*)&PackegEnd)[2],1);
					UARTSendData(&((u8*)&PackegEnd)[1],1);
					UARTSendData(&((u8*)&PackegEnd)[0],1);
					
					UARTSendData(&((u8*)&datasum)[3],1);
					UARTSendData(&((u8*)&datasum)[2],1);
					UARTSendData(&((u8*)&datasum)[1],1);
					UARTSendData(&((u8*)&datasum)[0],1);
					
					UARTSendData(BordID,8);
					
					UARTSendData(&((u8*)&PackegStart)[3],1);
					UARTSendData(&((u8*)&PackegStart)[2],1);
					UARTSendData(&((u8*)&PackegStart)[1],1);
					UARTSendData(&((u8*)&PackegStart)[0],1);
					
					

					
					UARTSendData(&Real_Time_Year_STOP,1);
					UARTSendData(&Real_Time_Month_STOP,1);
					UARTSendData(&Real_Time_Day_STOP,1);
					UARTSendData(&Real_Time_Hour_STOP,1);
					UARTSendData(&Real_Time_Minute_STOP,1);
					UARTSendData(&Real_Time_Second_STOP,1);
					UARTSendData(&((u8*)&Real_Time_Millise_STOP)[1],1);
					UARTSendData(&((u8*)&Real_Time_Millise_STOP)[0],1);
					

					UARTSendData(ErrorRecData,ErrorS);
					
					datasum = 0;
					ClearBuffer(USART_RX_BUF,USART_REC_LEN);
					f_lseek(f_rec,44 + wavsize);
					//return;
				}
			}
		}
		
		
		
		

			
					//return;
				
			
		
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == 0)
			return;
	}while(StrEqual(USART_RX_BUF,"Device is Idle\r\n",USART_REC_LEN,strlen("Device is Idle\r\n")) == -1);
}


		  
void recoder_enter_rec_mode(u32 fs)
{
	Adc_Init(fs);
	ADCDMA_Config(DMA2_Stream0,DMA_Channel_0,(u32)&ADC1->DR,i2srecbuf1,i2srecbuf2,I2S_RX_DMA_BUF_SIZE/2);
	ADC_Rec_Start(); 	//开始ADC数据接收
}  

//初始化WAV头.
void recoder_wav_init(__WaveHeader* wavhead , u32 fs) //初始化WAV头			   
{
	wavhead->riff.ChunkID=0X46464952;	//"RIFF"
	wavhead->riff.ChunkSize=0;			//还未确定,最后需要计算
	wavhead->riff.Format=0X45564157; 	//"WAVE"
	wavhead->fmt.ChunkID=0X20746D66; 	//"fmt "
	wavhead->fmt.ChunkSize=16; 			//大小为16个字节
	wavhead->fmt.AudioFormat=0X01; 		//0X01,表示PCM;0X01,表示IMA ADPCM
 	wavhead->fmt.NumOfChannels=1;		//双声道
 	wavhead->fmt.SampleRate=fs;		//16Khz采样率 采样速率
 	wavhead->fmt.ByteRate=wavhead->fmt.SampleRate*2;//字节速率=采样率*通道数*(ADC位数/8)
 	wavhead->fmt.BlockAlign=2;			//块大小=通道数*(ADC位数/8)
 	wavhead->fmt.BitsPerSample=16;		//16位PCM
   	wavhead->data.ChunkID=0X61746164;	//"data"
 	wavhead->data.ChunkSize=0;			//数据大小,还需要计算  
} 						    

//通过时间获取文件名
//仅限在SD卡保存,不支持FLASH DISK保存
//组合成:形如"0:RECORDER/REC20120321210633.wav"的文件名
void recoder_new_pathname(u8 *pname)
{	 
	u8 res;					 
	u16 index=0;
	while(index<0XFFFF)
	{
		sprintf((char*)pname,"0:RECORDER/REC%05d.wav",index);
		res=f_open(ftemp,(const TCHAR*)pname,FA_READ);//尝试打开这个文件
		if(res==FR_NO_FILE)break;		//该文件名不存在=正是我们需要的.
		index++;
	}
} 
	u8 res;
	u8 rval=0;
	__WaveHeader *wavhead=0; 
 	DIR recdir;	 					//目录  
 	u8 *pname=0;
	u8 timecnt=0;					//计时器 
//WAV录音 
u8 wav_recorder(u8 key,u32 fs)
{   
			switch(key)
			{		
				case 1://KEY2_PRES:	//STOP&SAVE
					if(rec_sta&0X80)//有录音
					{
						rec_sta=0;	//关闭录音
						DataCheck();
						wavhead->riff.ChunkSize=wavsize+36;		//整个文件的大小-8;
				   		wavhead->data.ChunkSize=wavsize;		//数据大小
						f_lseek(f_rec,0);						//偏移到文件头.
						//delay_ms(500);
						//printf("Begin Write Wave Head Data!");
						//delay_ms(500);
				  		f_write(f_rec,(const void*)wavhead,sizeof(__WaveHeader),&bw);//写入头数据
						//UARTSendData((u8*)wavhead,sizeof(__WaveHeader));
						//delay_ms(500);
						
						f_close(f_rec);
						//printf("Begin Close Wave File!");
						wavsize=0;
						secondcache_write_loc = 0;  
					  secondcache_read_loc = 0; 
						PackegStart = 0;
						PackegEnd = 0;
					}
					else
					{
						return 0;
					}
					rec_sta=0;

			    myfree(SRAMIN,secondcache);	//释放内存
					myfree(SRAMIN,i2srecbuf1);	//释放内存
					myfree(SRAMIN,i2srecbuf2);	//释放内存  
					myfree(SRAMIN,f_rec);		//释放内存
					myfree(SRAMIN,wavhead);		//释放内存  
					myfree(SRAMIN,pname);		//释放内存  
					return 0xff;

				case 2://KEY0_PRES:	//REC/PAUSE
					if(rec_sta&0X01)//原来是暂停,继续录音
					{
						rec_sta&=0XFE;//取消暂停
						return 1;
					}else if(rec_sta&0X80)//已经在录音了,暂停
					{
						rec_sta|=0X01;	//暂停
						return 1;
					}else				//还没开始录音 
					{

//						while(f_opendir(&recdir,"0:/RECORDER"))//打开录音文件夹
//						{	 			  
//							delay_ms(200);				  
//							f_mkdir("0:/RECORDER");				//创建该目录   
//						}   
						wavsize=0;
						i2srecbuf1=mymalloc(SRAMIN,I2S_RX_DMA_BUF_SIZE);//I2S录音内存1申请
						i2srecbuf2=mymalloc(SRAMIN,I2S_RX_DMA_BUF_SIZE);//I2S录音内存2申请  
						secondcache=mymalloc(SRAMIN,I2S_RX_DMA_BUF_SIZE*90);//I2S录音内存2申请 
							f_rec=(FIL *)mymalloc(SRAMIN,sizeof(FIL));		//开辟FIL字节的内存区域  
						wavhead=(__WaveHeader*)mymalloc(SRAMIN,sizeof(__WaveHeader));//开辟__WaveHeader字节的内存区域 
						pname=mymalloc(SRAMIN,30);						//申请30个字节内存,类似"0:RECORDER/REC00001.wav" 
						if(!i2srecbuf1||!i2srecbuf2||!f_rec||!wavhead||!pname||!secondcache)
							return 0xff;
						recoder_enter_rec_mode(fs);	//进入录音模式,此时耳机可以听到咪头采集到的音频   
						pname[0]=0;					//pname没有任何文件名 

						recoder_new_pathname(pname);			//得到新的名字
				 		recoder_wav_init(wavhead,fs);				//初始化wav数据	
	 					res=f_open(f_rec,(const TCHAR*)pname, FA_CREATE_ALWAYS | FA_WRITE | FA_READ); 
						if(res)			//文件创建失败
						{
							rec_sta=0;	//创建文件失败,不能录音
							//rval=0XFE;	//提示是否存在SD卡
							return 0;
						}else 
						{
							res=f_write(f_rec,(const void*)wavhead,sizeof(__WaveHeader),&bw);//写入头数据

							return 0xab;
						} 
 					}
				case 3:
					if(DMA_Rx_Flag)
					{
						DMA_Rx_Flag =0;
						
						rec_adc_dma_rx_callback();
					}
					return rec_sta;
				case 4:
					rec_sta|=0X80;	//开始录音	 
				default:
					return rec_sta;
			} 

}



