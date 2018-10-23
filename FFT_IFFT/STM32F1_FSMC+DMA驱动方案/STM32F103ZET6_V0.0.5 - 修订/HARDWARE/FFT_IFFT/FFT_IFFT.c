
/*******************************************************************************
** �������ƣ����ٸ���Ҷ�任(FFT)
** ����������������ʵ�ֿ��ٸ���Ҷ�任������任
** ����������������FFT��bug,������������,���� N_FFT��Ϊ��ֵ̬
** ����汾��V6.6
** �������ߣ���Ԫ��
** ����޸ģ�2011��5��16��
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "sys.h"
#include "FFT_IFFT.h"
//#define OUTPRINT printf
//#define DEL /##/
#define RESULT(x) sqrt(data_of_N_FFT[x].real*data_of_N_FFT[x].real+data_of_N_FFT[x].imag*data_of_N_FFT[x].imag)
#define PI  3.14159265358979323846264338327950288419716939937510	//Բ���ʣ�50λС��
#define PI2 6.28318530717958647692528676655900576839433879875021
int N_FFT=0;				//����Ҷ�任�ĵ���
int M_of_N_FFT=0;			//��������ļ�����N = 2^M
int Npart2_of_N_FFT=0;		//�������Һ�����ʱȡPI��1/2
int Npart4_of_N_FFT=0;		//�������Һ�����ʱȡPI��1/4





ptr_complex_of_N_FFT data_of_N_FFT=NULL;//���ٴ洢��Ԫ��ԭʼ�����븺�������ʹ��֮
ElemType* SIN_TABLE_of_N_FFT=NULL;


//����ģ��ԭʼ��������
void InputData(u16 *data,float c)
{
    int i;
    for (i=0; i<N_FFT; i++)//������������
    {
        data_of_N_FFT[i].real = data[i];	//���Ҳ�
		//	  data_of_N_FFT[i].real = sin(i);	//���Ҳ�
        data_of_N_FFT[i].imag = 0;
       // printf("%f\n",data_of_N_FFT[i].real);
    }
}

//�������Һ�����
void CREATE_SIN_TABLE(void)
{
    int i=0;
    for (i=0; i<=Npart4_of_N_FFT; i++)
    {
        SIN_TABLE_of_N_FFT[i] = sin(PI*i/Npart2_of_N_FFT);//SIN_TABLE[i] = sin(PI2*i/N);
    }
}


ElemType Sin_find(ElemType x)
{
    int i = (int)(N_FFT*x);
    i = i>>1;
    if (i>Npart4_of_N_FFT)//ע�⣺i�Ѿ�ת��Ϊ0~N֮��������ˣ�
    {
        //���ᳬ��N/2
        i = Npart2_of_N_FFT - i;//i = i - 2*(i-Npart4);
    }
    return SIN_TABLE_of_N_FFT[i];
}

ElemType Cos_find(ElemType x)
{
    int i = (int)(N_FFT*x);
    i = i>>1;
    if (i<Npart4_of_N_FFT)//ע�⣺i�Ѿ�ת��Ϊ0~N֮��������ˣ�
    {
        //���ᳬ��N/2
        //i = Npart4 - i;
        return SIN_TABLE_of_N_FFT[Npart4_of_N_FFT - i];
    }
    else//i>Npart4 && i<N/2
    {
        //i = i - Npart4;
        return -SIN_TABLE_of_N_FFT[i - Npart4_of_N_FFT];
    }
}

//��ַ
void ChangeSeat(complex_of_N_FFT *DataInput)
{
    int nextValue,nextM,i,k,j=0;
    complex_of_N_FFT temp;

    nextValue=N_FFT/2;                  //��ַ���㣬������Ȼ˳���ɵ�λ�򣬲����׵��㷨
    nextM=N_FFT-1;
    for (i=0;i<nextM;i++)
    {
        if (i<j)					//���i<j,�����б�ַ
        {
            temp=DataInput[j];
            DataInput[j]=DataInput[i];
            DataInput[i]=temp;
        }
        k=nextValue;                //��j����һ����λ��
        while (k<=j)				//���k<=j,��ʾj�����λΪ1
        {
            j=j-k;					//�����λ���0
            k=k/2;					//k/2���Ƚϴθ�λ���������ƣ�����Ƚϣ�ֱ��ĳ��λΪ0
        }
        j=j+k;						//��0��Ϊ1
    }
}

//�����˷�
/*complex XX_complex(complex a, complex b)
{
	complex temp;

	temp.real = a.real * b.real-a.imag*b.imag;
	temp.imag = b.imag*a.real + a.imag*b.real;

	return temp;
}*/

//FFT���㺯��
void FFT(void)
{
    int L=0,B=0,J=0,K=0;
    int step=0, KB=0;
    //ElemType P=0;
    ElemType angle;
    complex_of_N_FFT W,Temp_XX;

    ChangeSeat(data_of_N_FFT);//��ַ
    //CREATE_SIN_TABLE();
    for (L=1; L<=M_of_N_FFT; L++)
    {
        step = 1<<L;//2^L
        B = step>>1;//B=2^(L-1)
        for (J=0; J<B; J++)
        {
            //P = (1<<(M-L))*J;//P=2^(M-L) *J
            angle = (double)J/B;			//���ﻹ�����Ż�
            W.imag =  -Sin_find(angle);		//��C++�ú���������Ϊinline
            W.real =   Cos_find(angle);		//��C++�ú���������Ϊinline
            //W.real =  cos(angle*PI);
            //W.imag = -sin(angle*PI);
            for (K=J; K<N_FFT; K=K+step)
            {
                KB = K + B;
                //Temp_XX = XX_complex(data[KB],W);
                //����������ֱ�Ӽ��㸴���˷���ʡȥ�������ÿ���
                Temp_XX.real = data_of_N_FFT[KB].real * W.real-data_of_N_FFT[KB].imag*W.imag;
                Temp_XX.imag = W.imag*data_of_N_FFT[KB].real + data_of_N_FFT[KB].imag*W.real;

                data_of_N_FFT[KB].real = data_of_N_FFT[K].real - Temp_XX.real;
                data_of_N_FFT[KB].imag = data_of_N_FFT[K].imag - Temp_XX.imag;

                data_of_N_FFT[K].real = data_of_N_FFT[K].real + Temp_XX.real;
                data_of_N_FFT[K].imag = data_of_N_FFT[K].imag + Temp_XX.imag;
            }
        }
    }
}

//IFFT���㺯��
void IFFT(void)
{
    int L=0,B=0,J=0,K=0;
    int step=0, KB=0;
    //ElemType P=0;
    ElemType angle;
    complex_of_N_FFT W,Temp_XX;

    ChangeSeat(data_of_N_FFT);//��ַ
    //CREATE_SIN_TABLE();
    for (L=1; L<=M_of_N_FFT; L++)
    {
        step = 1<<L;//2^L
        B = step>>1;//B=2^(L-1)
        for (J=0; J<B; J++)
        {
            //P = (1<<(M-L))*J;//P=2^(M-L) *J
            angle = (double)J/B;			//���ﻹ�����Ż�

            W.imag =   Sin_find(angle);		//��C++�ú���������Ϊinline
            W.real =   Cos_find(angle);		//��C++�ú���������Ϊinline
            //W.real =  cos(angle*PI);
            //W.imag = -sin(angle*PI);
            for (K=J; K<N_FFT; K=K+step)
            {
                KB = K + B;
                //Temp_XX = XX_complex(data[KB],W);
                //����������ֱ�Ӽ��㸴���˷���ʡȥ�������ÿ���
                Temp_XX.real = data_of_N_FFT[KB].real * W.real-data_of_N_FFT[KB].imag*W.imag;
                Temp_XX.imag = W.imag*data_of_N_FFT[KB].real + data_of_N_FFT[KB].imag*W.real;

                data_of_N_FFT[KB].real = data_of_N_FFT[K].real - Temp_XX.real;
                data_of_N_FFT[KB].imag = data_of_N_FFT[K].imag - Temp_XX.imag;

                data_of_N_FFT[K].real = data_of_N_FFT[K].real + Temp_XX.real;
                data_of_N_FFT[K].imag = data_of_N_FFT[K].imag + Temp_XX.imag;
            }
        }
    }
}

//��ʼ��FFT����
//N_FFT�� FFT�ĵ�����������2�Ĵη�
void Init_FFT(int N_of_FFT,float *data,float *table)
{
    int i=0;
    int temp_N_FFT=1;
    N_FFT = N_of_FFT;					//����Ҷ�任�ĵ��� �������� 2�Ĵη�
    M_of_N_FFT = 0;					//��������ļ�����N = 2^M
    for (i=0; temp_N_FFT<N_FFT; i++)
    {
        temp_N_FFT = 2*temp_N_FFT;
        M_of_N_FFT++;
    }
   // printf("\n%d\n",M_of_N_FFT);
    Npart2_of_N_FFT = N_FFT/2;		//�������Һ�����ʱȡPI��1/2
    Npart4_of_N_FFT = N_FFT/4;		//�������Һ�����ʱȡPI��1/4

//    //ptr_complex_of_N_FFT data_of_N_FFT=NULL;//���ٴ洢��Ԫ��ԭʼ�����븺�������ʹ��֮
//    data_of_N_FFT = (ptr_complex_of_N_FFT)malloc(N_FFT * sizeof(complex_of_N_FFT));
//    //ptr_complex_of_N_FFT SIN_TABLE_of_N_FFT=NULL;
//    SIN_TABLE_of_N_FFT = (ElemType *)malloc((Npart4_of_N_FFT+1) * sizeof(ElemType));
		
    data_of_N_FFT = (ptr_complex_of_N_FFT)data;
    SIN_TABLE_of_N_FFT = (ElemType *)table;

    CREATE_SIN_TABLE();				//�������Һ�����
}

//���� FFT���㣬�ͷ�����ڴ�
void Close_FFT(void)
{
    if (data_of_N_FFT != NULL)
    {
        free(data_of_N_FFT);          //�ͷ��ڴ�
        data_of_N_FFT = NULL;
    }
    if (SIN_TABLE_of_N_FFT != NULL)
    {
        free(SIN_TABLE_of_N_FFT);     //�ͷ��ڴ�
        SIN_TABLE_of_N_FFT = NULL;
    }
}

////������
//int main(int argc, char *argv[])
//{
//    int i = 0;
//
//    Init_FFT(64);		//��ʼ������������˺���ֻ��ִ��һ��
//
//    InputData();		//����ԭʼ����
//    FFT();				//���� FFT����
//
//    printf("\n\n");
//    for (i=0; i<N_FFT; i++)
//    {
//        printf("%f\n",RESULT(i));
//    }
//
//    IFFT();//���� IFFT����
//    printf("\n\n");
//    for (i=0; i<N_FFT; i++)
//    {
//        printf("%f\n",data_of_N_FFT[i].real/N_FFT);
//    }
//
//    Close_FFT();		//���� FFT���㣬�ͷ�����ڴ�
//
//    return 0;
//}



