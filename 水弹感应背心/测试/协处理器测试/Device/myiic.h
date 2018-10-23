#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//IIC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
typedef unsigned int  u32;
typedef unsigned char  u8;
//IO��������
#define SDA_IN()  {GPIOE->MODER&=~(3<<(9*2));GPIOE->MODER|=0<<9*2;}	//PB9����ģʽ
#define SDA_OUT() {GPIOE->MODER&=~(3<<(9*2));GPIOE->MODER|=1<<9*2;} //PB9���ģʽ
//IO��������	 
#define IIC_SCL    PEout(8) //SCL
#define IIC_SDA    PEout(9) //SDA	 
#define READ_SDA   PEin(9)  //����SDA 

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

int I2C_WRITE_STRING(unsigned char DEV_ADDR,unsigned char *data,int len);
int I2C_WRITE_READ_STRING(unsigned char DEV_ADDR,unsigned char *Sdata,unsigned char *Rdata,int len);
#endif
















