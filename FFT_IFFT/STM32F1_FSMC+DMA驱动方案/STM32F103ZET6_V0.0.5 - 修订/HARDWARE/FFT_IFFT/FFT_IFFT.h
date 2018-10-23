#ifndef __FFT_IFFT_H
#define __FFT_IFFT_H	 
#include "sys.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef float ElemType;		//原始数据序列的数据类型,可以在这里设置
typedef struct				//定义复数结构体
{
    ElemType real,imag;
}complex_of_N_FFT,*ptr_complex_of_N_FFT;




void InputData(u16 *data,float c);
void Init_FFT(int N_of_FFT,float *data,float *table);
void FFT(void);
void IFFT(void);
void Close_FFT(void);
#endif
