/*
*********************************************************************************************************
*	                                  
*	模块名称 : 串口驱动模块    
*	文件名称 : bsp_uart.c
*	版    本 : V1.0
*	说    明 : 实现printf和scanf函数重定向到串口1，即支持printf信息到USART1
*				实现重定向，只需要添加2个函数:
*				int fputc(int ch, FILE *f);
*				int fgetc(FILE *f);
*				对于KEIL MDK编译器，编译选项中需要在MicorLib前面打钩，否则不会有数据打印到USART1。
*	修改记录 :
*		版本号  日期         作者     说明
*		v1.0    2012-12-17  armfly  ST固件库V1.0.2版本。
*
*	Copyright (C), 2012-2013, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "stm32f4xx.h"
u8 RX_BUFFER[256];
u8 RX_FLAG=0;
/*
*********************************************************************************************************
*	函 数 名: bsp_InitUart
*	功能说明: 初始化CPU的USART1串口硬件设备。未启用中断。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitUart(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	/* 串口1 TX = PA9   RX = PA10 */

	/* 第1步： 配置GPIO */

	/* 打开 GPIO 时钟 */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

		/* 打开 UART 时钟 */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

		/* 将 PB6 映射为 USART1_TX */
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);

		/* 将 PB7 映射为 USART1_RX */
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

		/* 配置 USART Tx 为复用功能 */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* 复用模式 */

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		/* 配置 USART Rx 为复用功能 */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_Init(GPIOB, &GPIO_InitStructure);


NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器


	/* 第2步： 配置串口硬件参数 */
	USART_InitStructure.USART_BaudRate = 115200;	/* 波特率 */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_Cmd(USART1, ENABLE);		/* 使能串口 */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
		如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */  
}

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		if((RX_FLAG & 0x3F) > 63)
			RX_FLAG &= 0xC0;
		if(Res == '#' && ((RX_FLAG & 0xC0) == 0x00))
		{
			RX_FLAG |= 0x80;
			return;
		}
		else if(Res == '<' && ((RX_FLAG & 0xC0) == 0x80))
		{
			RX_FLAG |= 0x40;
			RX_BUFFER[RX_FLAG & 0x3F] = '#';
			RX_FLAG++;
			RX_BUFFER[RX_FLAG & 0x3F] = '<';
			RX_FLAG++;
			return;
		}
		if(Res != '<' && ((RX_FLAG & 0xC0) == 0x80))
		{
			RX_FLAG &= 0x3F;
			return;
		}
		
		if((RX_FLAG & 0xC0) == 0xC0)
		{
			RX_BUFFER[RX_FLAG & 0x3F] = Res;
			RX_FLAG++;
		}
		
		
		if(Res == '@')
		{
			RX_FLAG &= 0x3F;
		}

  } 

} 




/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
