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
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//¼���� Ӧ�ô���	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/6/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
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


u32 secondcache_write_loc = 0;   //�洢λ�ã��洢��
u32 secondcache_read_loc = 0;  //��ȡλ�ã���ȡ��


FIL* f_rec=0;		//¼���ļ�	
u32 wavsize;		//wav���ݴ�С(�ֽ���,�������ļ�ͷ!!)
u8 rec_sta=0;		//¼��״̬
					//[7]:0,û�п���¼��;1,�Ѿ�����¼��;
					//[6:1]:����
					//[0]:0,����¼��;1,��ͣ¼��;
u8 DMA_Rx_Flag=0;					
					
u32 PackegStart = 0;
u32 PackegEnd = 0;				
u8 ErrorRecData[2048];

u32 PackegLen = 0;
void wav_second_level_cache(void)
{
	int i;
	u16*p;
	if(rec_sta==0X80)//¼��ģʽ
	{  
		if(DMA2_Stream0->CR&(1<<19))
		{
			//res=f_write(f_rec,i2srecbuf1,I2S_RX_DMA_BUF_SIZE,(UINT*)&bw);//д���ļ�
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
			//res=f_write(f_rec,i2srecbuf2,I2S_RX_DMA_BUF_SIZE,(UINT*)&bw);//д���ļ�
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
					
	//¼�� I2S_DMA�����жϷ�����.���ж�����д������
void rec_adc_dma_rx_callback(void) 
{    
	u16 bw;
	u8 res;
	u32 writesize;
	u32 i = 0;
	u32 datasum = 0;
	int start;
	u32 ErrorS,ErrorE,start_len,start_sum;
	if(rec_sta==0X80)//¼��ģʽ
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
			res=f_write(f_rec,&secondcache[secondcache_read_loc],writesize,(UINT*)&bw);//д���ļ�
			
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
			res=f_write(f_rec,&secondcache[secondcache_read_loc],writesize,(UINT*)&bw);//д���ļ�
			
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
	ADC_Rec_Start(); 	//��ʼADC���ݽ���
}  

//��ʼ��WAVͷ.
void recoder_wav_init(__WaveHeader* wavhead , u32 fs) //��ʼ��WAVͷ			   
{
	wavhead->riff.ChunkID=0X46464952;	//"RIFF"
	wavhead->riff.ChunkSize=0;			//��δȷ��,�����Ҫ����
	wavhead->riff.Format=0X45564157; 	//"WAVE"
	wavhead->fmt.ChunkID=0X20746D66; 	//"fmt "
	wavhead->fmt.ChunkSize=16; 			//��СΪ16���ֽ�
	wavhead->fmt.AudioFormat=0X01; 		//0X01,��ʾPCM;0X01,��ʾIMA ADPCM
 	wavhead->fmt.NumOfChannels=1;		//˫����
 	wavhead->fmt.SampleRate=fs;		//16Khz������ ��������
 	wavhead->fmt.ByteRate=wavhead->fmt.SampleRate*2;//�ֽ�����=������*ͨ����*(ADCλ��/8)
 	wavhead->fmt.BlockAlign=2;			//���С=ͨ����*(ADCλ��/8)
 	wavhead->fmt.BitsPerSample=16;		//16λPCM
   	wavhead->data.ChunkID=0X61746164;	//"data"
 	wavhead->data.ChunkSize=0;			//���ݴ�С,����Ҫ����  
} 						    

//ͨ��ʱ���ȡ�ļ���
//������SD������,��֧��FLASH DISK����
//��ϳ�:����"0:RECORDER/REC20120321210633.wav"���ļ���
void recoder_new_pathname(u8 *pname)
{	 
	u8 res;					 
	u16 index=0;
	while(index<0XFFFF)
	{
		sprintf((char*)pname,"0:RECORDER/REC%05d.wav",index);
		res=f_open(ftemp,(const TCHAR*)pname,FA_READ);//���Դ�����ļ�
		if(res==FR_NO_FILE)break;		//���ļ���������=����������Ҫ��.
		index++;
	}
} 
	u8 res;
	u8 rval=0;
	__WaveHeader *wavhead=0; 
 	DIR recdir;	 					//Ŀ¼  
 	u8 *pname=0;
	u8 timecnt=0;					//��ʱ�� 
//WAV¼�� 
u8 wav_recorder(u8 key,u32 fs)
{   
			switch(key)
			{		
				case 1://KEY2_PRES:	//STOP&SAVE
					if(rec_sta&0X80)//��¼��
					{
						rec_sta=0;	//�ر�¼��
						DataCheck();
						wavhead->riff.ChunkSize=wavsize+36;		//�����ļ��Ĵ�С-8;
				   		wavhead->data.ChunkSize=wavsize;		//���ݴ�С
						f_lseek(f_rec,0);						//ƫ�Ƶ��ļ�ͷ.
						//delay_ms(500);
						//printf("Begin Write Wave Head Data!");
						//delay_ms(500);
				  		f_write(f_rec,(const void*)wavhead,sizeof(__WaveHeader),&bw);//д��ͷ����
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

			    myfree(SRAMIN,secondcache);	//�ͷ��ڴ�
					myfree(SRAMIN,i2srecbuf1);	//�ͷ��ڴ�
					myfree(SRAMIN,i2srecbuf2);	//�ͷ��ڴ�  
					myfree(SRAMIN,f_rec);		//�ͷ��ڴ�
					myfree(SRAMIN,wavhead);		//�ͷ��ڴ�  
					myfree(SRAMIN,pname);		//�ͷ��ڴ�  
					return 0xff;

				case 2://KEY0_PRES:	//REC/PAUSE
					if(rec_sta&0X01)//ԭ������ͣ,����¼��
					{
						rec_sta&=0XFE;//ȡ����ͣ
						return 1;
					}else if(rec_sta&0X80)//�Ѿ���¼����,��ͣ
					{
						rec_sta|=0X01;	//��ͣ
						return 1;
					}else				//��û��ʼ¼�� 
					{

//						while(f_opendir(&recdir,"0:/RECORDER"))//��¼���ļ���
//						{	 			  
//							delay_ms(200);				  
//							f_mkdir("0:/RECORDER");				//������Ŀ¼   
//						}   
						wavsize=0;
						i2srecbuf1=mymalloc(SRAMIN,I2S_RX_DMA_BUF_SIZE);//I2S¼���ڴ�1����
						i2srecbuf2=mymalloc(SRAMIN,I2S_RX_DMA_BUF_SIZE);//I2S¼���ڴ�2����  
						secondcache=mymalloc(SRAMIN,I2S_RX_DMA_BUF_SIZE*90);//I2S¼���ڴ�2���� 
							f_rec=(FIL *)mymalloc(SRAMIN,sizeof(FIL));		//����FIL�ֽڵ��ڴ�����  
						wavhead=(__WaveHeader*)mymalloc(SRAMIN,sizeof(__WaveHeader));//����__WaveHeader�ֽڵ��ڴ����� 
						pname=mymalloc(SRAMIN,30);						//����30���ֽ��ڴ�,����"0:RECORDER/REC00001.wav" 
						if(!i2srecbuf1||!i2srecbuf2||!f_rec||!wavhead||!pname||!secondcache)
							return 0xff;
						recoder_enter_rec_mode(fs);	//����¼��ģʽ,��ʱ��������������ͷ�ɼ�������Ƶ   
						pname[0]=0;					//pnameû���κ��ļ��� 

						recoder_new_pathname(pname);			//�õ��µ�����
				 		recoder_wav_init(wavhead,fs);				//��ʼ��wav����	
	 					res=f_open(f_rec,(const TCHAR*)pname, FA_CREATE_ALWAYS | FA_WRITE | FA_READ); 
						if(res)			//�ļ�����ʧ��
						{
							rec_sta=0;	//�����ļ�ʧ��,����¼��
							//rval=0XFE;	//��ʾ�Ƿ����SD��
							return 0;
						}else 
						{
							res=f_write(f_rec,(const void*)wavhead,sizeof(__WaveHeader),&bw);//д��ͷ����

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
					rec_sta|=0X80;	//��ʼ¼��	 
				default:
					return rec_sta;
			} 

}



