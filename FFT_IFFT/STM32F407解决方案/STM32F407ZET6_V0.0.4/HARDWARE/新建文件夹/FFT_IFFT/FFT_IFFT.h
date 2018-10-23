#ifndef __FFT_IFFT_H
#define __FFT_IFFT_H	 
#include "sys.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef float ElemType;		//ԭʼ�������е���������,��������������
typedef struct				//���帴���ṹ��
{
    ElemType real,imag;
}complex_of_N_FFT,*ptr_complex_of_N_FFT;




void InputData(u16 *data,float c);
void Init_FFT(int N_of_FFT,float *data,float *table);
void FFT(void);
void IFFT(void);
void Close_FFT(void);
#endif
