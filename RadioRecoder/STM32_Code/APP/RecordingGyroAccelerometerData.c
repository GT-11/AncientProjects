#include "RecordingGyroAccelerometerData.h"
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
#include <string.h>
#include "adc.h"

extern u32 wavsize;
extern u32 planesize;
FIL* f_gad=0;		//¼���ļ�	
u8 gad_sta=0;		//¼��״̬
u8 *gad_pname=0;
u8 gad_res;
u8 *data_string;
u8 *data_stringbuffer;
u32 gad_plane = 0;
u32 gad_times = 0;
 	DIR gaddir;	 					//Ŀ¼  

u32 secondcache_write_Loc = 0;   //�洢λ�ã��洢��
u32 secondcache_read_Loc = 0;  //��ȡλ�ã���ȡ��

u8 updata =0;

float gad_time;                        //ʱ���   1 * 32 = 32bit = 4Bit
float gad_pitch,gad_roll,gad_yaw; 		//ŷ����    32*3 = 96bit = 12Bit
short gad_aacx,gad_aacy,gad_aacz;		//���ٶȴ�����ԭʼ����  16 * 3 = 48bit = 6Bit
short gad_gyrox,gad_gyroy,gad_gyroz;	//������ԭʼ����  16 * 3 = 48bit = 6Bit          28Bit


float Timeparticle;
u8 GAD_Rx_Flag=0;	
void gad_second_level_cache(void)
{
	int i;
	if(gad_sta==0X80)//¼��ģʽ
	{	
		//if(updata)
		if(!(gad_times % gad_plane))
		{
			
			
			
					
					mpu_dmp_get_data(&gad_gyrox,&gad_gyroy,&gad_gyroz,&gad_aacx,&gad_aacy,&gad_aacz,&gad_pitch,&gad_roll,&gad_yaw);
					if(data_string)
					{
						data_string[0] = 0xAD;
						data_string[1] = 0xBC;
				
						data_string[2] = gad_gyrox >> 8;
						data_string[3] = gad_gyrox;
						data_string[4] = gad_gyroy >> 8;
						data_string[5] = gad_gyroy;
						data_string[6] = gad_gyroz >> 8;
						data_string[7] = gad_gyroz;
				
				
						data_string[8] = gad_aacx >> 8;
						data_string[9] = gad_aacx;
						data_string[10] = gad_aacy >> 8;
						data_string[11] = gad_aacy;
						data_string[12] = gad_aacz >> 8;
						data_string[13] = gad_aacz;		


						data_string[14] = ((unsigned int)(gad_pitch * 10000.0f)) >> 24;
						data_string[15] = ((unsigned int)(gad_pitch * 10000.0f)) >> 16;
						data_string[16] = ((unsigned int)(gad_pitch * 10000.0f)) >> 8;
						data_string[17] = ((unsigned int)(gad_pitch * 10000.0f));
						
						
						data_string[18] = ((unsigned int)(gad_roll * 10000.0f)) >> 24;
						data_string[19] = ((unsigned int)(gad_roll * 10000.0f)) >> 16;
						data_string[20] = ((unsigned int)(gad_roll * 10000.0f)) >> 8;
						data_string[21] = ((unsigned int)(gad_roll * 10000.0f));


						data_string[22] = ((unsigned int)(gad_yaw * 10000.0f)) >> 24;
						data_string[23] = ((unsigned int)(gad_yaw * 10000.0f)) >> 16;
						data_string[24] = ((unsigned int)(gad_yaw * 10000.0f)) >> 8;
						data_string[25] = ((unsigned int)(gad_yaw * 10000.0f));
				
				    gad_time = (gad_times * Timeparticle);
						data_string[26] = ((unsigned int)(gad_time * 10000.0f)) >> 24;
						data_string[27] = ((unsigned int)(gad_time * 10000.0f)) >> 16;
						data_string[28] = ((unsigned int)(gad_time * 10000.0f)) >> 8;
						data_string[29] = ((unsigned int)(gad_time * 10000.0f));
						updata = 1;
					}
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			updata = 0;
			if(secondcache_write_Loc == 200 * 30)
				secondcache_write_Loc = 0;
			for(i=0;i<30;i++)
				data_stringbuffer[secondcache_write_Loc + i] = data_string[i];
			secondcache_write_Loc += 30;
			GAD_Rx_Flag =1;
	  }
		
		gad_times++;
	}
}






void gad_rx_callback(void) 
{    
	u8 res;
	u32 writesize;
	if(gad_sta==0X80)//¼��ģʽ
	{  
		if(secondcache_read_Loc < secondcache_write_Loc)
		{
			writesize = secondcache_write_Loc - secondcache_read_Loc;
			res=f_write(f_gad,&data_stringbuffer[secondcache_read_Loc],writesize,(UINT*)&bw);//д���ļ�
			
			if(!res)
			{
				secondcache_read_Loc += writesize;
				//wavsize+=writesize;
			}
			return;
		}
		
		if(secondcache_read_Loc > secondcache_write_Loc)
		{
			writesize = (200*30) - secondcache_read_Loc;
			res=f_write(f_gad,&data_stringbuffer[secondcache_read_Loc],writesize,(UINT*)&bw);//д���ļ�
			if(!res)
			{
				secondcache_read_Loc = 0;
				//wavsize+=writesize;
			}
			return;
		}
	} 

} 
//ͨ��ʱ���ȡ�ļ���
//������SD������,��֧��FLASH DISK����
//��ϳ�:����"0:RECORDER/REC20120321210633.wav"���ļ���
void gad_new_pathname(u8 *gad_pname)
{	 
	u8 gad_res;					 
	u16 index=0;
	while(index<0XFFFF)
	{
		sprintf((char*)gad_pname,"0:RECORDER/GAD%05d.txt",index);
		gad_res=f_open(ftemp,(const TCHAR*)gad_pname,FA_READ);//���Դ�����ļ�
		if(gad_res==FR_NO_FILE)break;		//���ļ���������=����������Ҫ��.
		index++;
	}
} 

//WAV¼�� 
u8 gad_recorder(u8 key,u32 time,u32 fs)
{   
			switch(key)
			{		
				case 1://KEY2_Pgad_res:	//STOP&SAVE
					if(gad_sta&0X80)//��¼��
					{
						gad_sta=0;	//�ر�¼��
						
						f_close(f_gad);

					}
					else
					{
						return 0;
					}
					gad_sta=0;
					secondcache_write_Loc = 0;  
					secondcache_read_Loc = 0; 
					gad_plane = 0;
					gad_time = 0;
					gad_times = 0;
				 	//LED1=1;	 						//�ر�DS1
			    
					myfree(SRAMIN,data_string);	//�ͷ��ڴ�
					myfree(SRAMIN,f_gad);		//�ͷ��ڴ�
					myfree(SRAMIN,gad_pname);		//�ͷ��ڴ�  
					myfree(SRAMIN,data_stringbuffer);		//�ͷ��ڴ�  
					return 0xff;

				case 2://KEY0_Pgad_res:	//REC/PAUSE
					if(gad_sta&0X01)//ԭ������ͣ,����¼��
					{
						gad_sta&=0XFE;//ȡ����ͣ
						return 1;
					}else if(gad_sta&0X80)//�Ѿ���¼����,��ͣ
					{
						gad_sta|=0X01;	//��ͣ
						return 1;
					}else				//��û��ʼ¼�� 
					{
//						while(f_opendir(&gaddir,"0:/RECORDER"))//��¼���ļ���
//						{	 			  
//							delay_ms(200);				  
//							f_mkdir("0:/RECORDER");				//������Ŀ¼   
//						}   
						data_string=mymalloc(SRAMIN,30);//I2S¼���ڴ�1����
						data_stringbuffer=mymalloc(SRAMIN,30 * 200);//I2S¼���ڴ�1����
							f_gad=(FIL *)mymalloc(SRAMIN,sizeof(FIL));		//����FIL�ֽڵ��ڴ�����  
						gad_pname=mymalloc(SRAMIN,30);						//����30���ֽ��ڴ�,����"0:RECORDER/REC00001.wav" 
						if(!data_string||!f_gad||!gad_pname||!data_stringbuffer)
							return 0xff;

						gad_plane = time; 
						gad_times = time;

						Timeparticle = (512.0 / (double)fs);
						
						gad_pname[0]=0;					//gad_pnameû���κ��ļ��� 
						
						
						


						gad_new_pathname(gad_pname);			//�õ��µ�����
	 					gad_res=f_open(f_gad,(const TCHAR*)gad_pname, FA_CREATE_ALWAYS | FA_WRITE); 
						if(gad_res)			//�ļ�����ʧ��
						{
							gad_sta=0;	//�����ļ�ʧ��,����¼��

							return 0;
						}else 
						{

							return 0xab;
						} 
 					}
				case 3:
						if(GAD_Rx_Flag)
						{
							GAD_Rx_Flag =0;
							gad_rx_callback();
						}
						//MPU_Get_Accelerometer(&gad_aacx,&gad_aacy,&gad_aacz);	//�õ����ٶȴ���������
						//MPU_Get_Gyroscope(&gad_gyrox,&gad_gyroy,&gad_gyroz);	//�õ�����������


						//sprintf((char*)data_string,"Time:%10.3f,AX:%05d,AY:%05d,AZ:%05d,GX:%05d,GY:%05d,GZ:%05d\r\n",(gad_times * 0.128), gad_aacx, gad_aacy, gad_aacz, gad_gyrox, gad_gyroy, gad_gyroz);
						//sprintf((char*)data_string,"Data Time:%10.3f , Pitch:%8.2f , Roll:%8.2f , Yaw:%8.2f , AacX:%05d , AacY:%05d , AacZ:%05d , GyroX:%05d , GyroY:%05d , GyroZ:%05d\r\n",(gad_times * 0.032), gad_pitch, gad_roll, gad_yaw, gad_aacx, gad_aacy, gad_aacz, gad_gyrox, gad_gyroy, gad_gyroz);
					
					return gad_sta;
					
				case 4:
					gad_sta|=0X80;	//��ʼ¼��	 
				default:
					return gad_sta;
			} 

}
