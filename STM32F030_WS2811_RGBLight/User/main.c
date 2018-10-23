#include "stm32f0xx.h"
#include "stm32f0xx_spi.h"
#include "delay.h"
#define light 64
char a2,cc=0;
unsigned char cr=0 ,cg=0,cb=0;
char flaga=0,flagb=0,flagc=0;
unsigned char data[light][6];
void SPI_Flash_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;	
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);   

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_0);    //SCK
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_0);    //MISO
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_0);    //MOSI


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);

	
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx; 
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;		
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;	
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;	
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	
	SPI_InitStructure.SPI_CRCPolynomial = 7;	
	SPI_Init(SPI1, &SPI_InitStructure);  
	SPI_Cmd(SPI1, ENABLE); //??SPI??
}
void ReadWriteByte(uint16_t TxData)
{
    while((SPI1->SR&1<<1)==0);
    SPI1->DR=TxData; 
}

void low()
{
	ReadWriteByte(0xe000);
}

void high()
{
	ReadWriteByte(0xfff0);
  cc=0;
}
void send(unsigned char dat)
{
	for(a2 =0;a2<8;a2++)
	{
		if(dat&0x80) high();
		else low();
		dat= dat<<1;
	}
}
void res()
{
	int a1;
	for(a1=0;a1<500;a1++)ReadWriteByte(0x0000);
}
void rgb(unsigned char R,unsigned char G,unsigned char B)
{
	send(R);
	send(G);
	send(B);	
}
void change()
{
		char num;
		for(num=0;num<light;num++)
		{
			if(data[num][3]==0)data[num][0]+=1;
			else data[num][0]-=2;
			if(data[num][4]==0)data[num][1]+=2;
			else data[num][1]-=3;
			if(data[num][5]==0)data[num][2]+=3;
			else data[num][2]-=1;
			if(data[num][0]>250)data[num][3]=1;
			if(data[num][1]>250)data[num][4]=1;
			if(data[num][2]>250)data[num][5]=1;
			if(data[num][0]<4)data[num][3]=0;
			if(data[num][1]<4)data[num][4]=0;
			if(data[num][2]<4)data[num][5]=0;
		}
}
void play()
{
	char num;
	change();
	for(num=0;num<light;num++)
	{
		rgb(data[num][0],data[num][1],data[num][2]);
	}
		
}
void init()
{
	char num;
	for(num=0;num<light;num++)
	{
		data[num][0]=200/light*num;
		data[num][1]=200/light*num;
		data[num][2]=200/light*num;
		data[num][3]=0;
		data[num][4]=0;
		data[num][5]=0;
	}
}
int main(void)
{
	SPI_Flash_Init();
  while (1)
  {
		init();
		while(1)
		{
			play();
			res();
		}
  }
}


