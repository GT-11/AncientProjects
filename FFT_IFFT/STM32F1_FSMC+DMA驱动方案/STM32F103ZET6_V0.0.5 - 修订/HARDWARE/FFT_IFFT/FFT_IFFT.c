
/*******************************************************************************
** 程序名称：快速傅里叶变换(FFT)
** 程序描述：本程序实现快速傅里叶变换及其逆变换
** 性能提升：修正了FFT的bug,变量重新命名,并将 N_FFT改为动态值
** 程序版本：V6.6
** 程序作者：宋元瑞
** 最后修改：2011年5月16日
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "sys.h"
#include "FFT_IFFT.h"
//#define OUTPRINT printf
//#define DEL /##/
#define RESULT(x) sqrt(data_of_N_FFT[x].real*data_of_N_FFT[x].real+data_of_N_FFT[x].imag*data_of_N_FFT[x].imag)
#define PI  3.14159265358979323846264338327950288419716939937510	//圆周率，50位小数
#define PI2 6.28318530717958647692528676655900576839433879875021
int N_FFT=0;				//傅里叶变换的点数
int M_of_N_FFT=0;			//蝶形运算的级数，N = 2^M
int Npart2_of_N_FFT=0;		//创建正弦函数表时取PI的1/2
int Npart4_of_N_FFT=0;		//创建正弦函数表时取PI的1/4





ptr_complex_of_N_FFT data_of_N_FFT=NULL;//开辟存储单元，原始数据与负数结果均使用之
ElemType* SIN_TABLE_of_N_FFT=NULL;


//产生模拟原始数据输入
void InputData(u16 *data,float c)
{
    int i;
    for (i=0; i<N_FFT; i++)//制造输入序列
    {
        data_of_N_FFT[i].real = data[i];	//正弦波
		//	  data_of_N_FFT[i].real = sin(i);	//正弦波
        data_of_N_FFT[i].imag = 0;
       // printf("%f\n",data_of_N_FFT[i].real);
    }
}

//创建正弦函数表
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
    if (i>Npart4_of_N_FFT)//注意：i已经转化为0~N之间的整数了！
    {
        //不会超过N/2
        i = Npart2_of_N_FFT - i;//i = i - 2*(i-Npart4);
    }
    return SIN_TABLE_of_N_FFT[i];
}

ElemType Cos_find(ElemType x)
{
    int i = (int)(N_FFT*x);
    i = i>>1;
    if (i<Npart4_of_N_FFT)//注意：i已经转化为0~N之间的整数了！
    {
        //不会超过N/2
        //i = Npart4 - i;
        return SIN_TABLE_of_N_FFT[Npart4_of_N_FFT - i];
    }
    else//i>Npart4 && i<N/2
    {
        //i = i - Npart4;
        return -SIN_TABLE_of_N_FFT[i - Npart4_of_N_FFT];
    }
}

//变址
void ChangeSeat(complex_of_N_FFT *DataInput)
{
    int nextValue,nextM,i,k,j=0;
    complex_of_N_FFT temp;

    nextValue=N_FFT/2;                  //变址运算，即把自然顺序变成倒位序，采用雷德算法
    nextM=N_FFT-1;
    for (i=0;i<nextM;i++)
    {
        if (i<j)					//如果i<j,即进行变址
        {
            temp=DataInput[j];
            DataInput[j]=DataInput[i];
            DataInput[i]=temp;
        }
        k=nextValue;                //求j的下一个倒位序
        while (k<=j)				//如果k<=j,表示j的最高位为1
        {
            j=j-k;					//把最高位变成0
            k=k/2;					//k/2，比较次高位，依次类推，逐个比较，直到某个位为0
        }
        j=j+k;						//把0改为1
    }
}

//复数乘法
/*complex XX_complex(complex a, complex b)
{
	complex temp;

	temp.real = a.real * b.real-a.imag*b.imag;
	temp.imag = b.imag*a.real + a.imag*b.real;

	return temp;
}*/

//FFT运算函数
void FFT(void)
{
    int L=0,B=0,J=0,K=0;
    int step=0, KB=0;
    //ElemType P=0;
    ElemType angle;
    complex_of_N_FFT W,Temp_XX;

    ChangeSeat(data_of_N_FFT);//变址
    //CREATE_SIN_TABLE();
    for (L=1; L<=M_of_N_FFT; L++)
    {
        step = 1<<L;//2^L
        B = step>>1;//B=2^(L-1)
        for (J=0; J<B; J++)
        {
            //P = (1<<(M-L))*J;//P=2^(M-L) *J
            angle = (double)J/B;			//这里还可以优化
            W.imag =  -Sin_find(angle);		//用C++该函数课声明为inline
            W.real =   Cos_find(angle);		//用C++该函数课声明为inline
            //W.real =  cos(angle*PI);
            //W.imag = -sin(angle*PI);
            for (K=J; K<N_FFT; K=K+step)
            {
                KB = K + B;
                //Temp_XX = XX_complex(data[KB],W);
                //用下面两行直接计算复数乘法，省去函数调用开销
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

//IFFT运算函数
void IFFT(void)
{
    int L=0,B=0,J=0,K=0;
    int step=0, KB=0;
    //ElemType P=0;
    ElemType angle;
    complex_of_N_FFT W,Temp_XX;

    ChangeSeat(data_of_N_FFT);//变址
    //CREATE_SIN_TABLE();
    for (L=1; L<=M_of_N_FFT; L++)
    {
        step = 1<<L;//2^L
        B = step>>1;//B=2^(L-1)
        for (J=0; J<B; J++)
        {
            //P = (1<<(M-L))*J;//P=2^(M-L) *J
            angle = (double)J/B;			//这里还可以优化

            W.imag =   Sin_find(angle);		//用C++该函数课声明为inline
            W.real =   Cos_find(angle);		//用C++该函数课声明为inline
            //W.real =  cos(angle*PI);
            //W.imag = -sin(angle*PI);
            for (K=J; K<N_FFT; K=K+step)
            {
                KB = K + B;
                //Temp_XX = XX_complex(data[KB],W);
                //用下面两行直接计算复数乘法，省去函数调用开销
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

//初始化FFT程序
//N_FFT是 FFT的点数，必须是2的次方
void Init_FFT(int N_of_FFT,float *data,float *table)
{
    int i=0;
    int temp_N_FFT=1;
    N_FFT = N_of_FFT;					//傅里叶变换的点数 ，必须是 2的次方
    M_of_N_FFT = 0;					//蝶形运算的级数，N = 2^M
    for (i=0; temp_N_FFT<N_FFT; i++)
    {
        temp_N_FFT = 2*temp_N_FFT;
        M_of_N_FFT++;
    }
   // printf("\n%d\n",M_of_N_FFT);
    Npart2_of_N_FFT = N_FFT/2;		//创建正弦函数表时取PI的1/2
    Npart4_of_N_FFT = N_FFT/4;		//创建正弦函数表时取PI的1/4

//    //ptr_complex_of_N_FFT data_of_N_FFT=NULL;//开辟存储单元，原始数据与负数结果均使用之
//    data_of_N_FFT = (ptr_complex_of_N_FFT)malloc(N_FFT * sizeof(complex_of_N_FFT));
//    //ptr_complex_of_N_FFT SIN_TABLE_of_N_FFT=NULL;
//    SIN_TABLE_of_N_FFT = (ElemType *)malloc((Npart4_of_N_FFT+1) * sizeof(ElemType));
		
    data_of_N_FFT = (ptr_complex_of_N_FFT)data;
    SIN_TABLE_of_N_FFT = (ElemType *)table;

    CREATE_SIN_TABLE();				//创建正弦函数表
}

//结束 FFT运算，释放相关内存
void Close_FFT(void)
{
    if (data_of_N_FFT != NULL)
    {
        free(data_of_N_FFT);          //释放内存
        data_of_N_FFT = NULL;
    }
    if (SIN_TABLE_of_N_FFT != NULL)
    {
        free(SIN_TABLE_of_N_FFT);     //释放内存
        SIN_TABLE_of_N_FFT = NULL;
    }
}

////主函数
//int main(int argc, char *argv[])
//{
//    int i = 0;
//
//    Init_FFT(64);		//初始化各项参数，此函数只需执行一次
//
//    InputData();		//输入原始数据
//    FFT();				//进行 FFT计算
//
//    printf("\n\n");
//    for (i=0; i<N_FFT; i++)
//    {
//        printf("%f\n",RESULT(i));
//    }
//
//    IFFT();//进行 IFFT计算
//    printf("\n\n");
//    for (i=0; i<N_FFT; i++)
//    {
//        printf("%f\n",data_of_N_FFT[i].real/N_FFT);
//    }
//
//    Close_FFT();		//结束 FFT运算，释放相关内存
//
//    return 0;
//}



