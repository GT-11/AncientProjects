#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "adc.h"
#include "dac.h"

u16 ADC_Value[1024];
u16 DAC_Value[1024];
int main(void)
{
	u32 t=0;
	uart_init(115200);
	delay_init(84);

	
	Adc_Init();
	ADCDMA_Config(DMA2_Stream0,DMA_Channel_0,(u32)&ADC1->DR,(u32)&ADC_Value,1024);
	
	
	Dac1_Init();
	DACDMA_Config(DMA1_Stream5,DMA_Channel_7,(u32)&DAC->DHR12R1,(u32)&ADC_Value,1024);
	
  while(1){
		//ADC_Value[t]=ADC_GetConversionValue(ADC1);
    printf("t:%d\r\n",ADC_Value[t]);
		delay_ms(500);
		t++;
		if(t==8) t=0;
	}
}


