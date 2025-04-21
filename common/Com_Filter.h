#ifndef __COM_FILTER_H__
#define __COM_FILTER_H__
#include "stdint.h"

/* �������˲����ṹ�� */
typedef struct
{
    float LastP; // ��һʱ�̵�״̬�����Э���
    float Now_P; // ��ǰʱ�̵�״̬�����Э���
    float out;   // �˲��������ֵ�������Ƶ�״̬
    float Kg;    // ���������棬���ڵ���Ԥ��ֵ�Ͳ���ֵ֮���Ȩ��
    float Q;     // ���������ķ����ӳϵͳģ�͵Ĳ�ȷ����
    float R;     // ���������ķ����ӳ�������̵Ĳ�ȷ����
} KalmanFilter_Struct;

extern KalmanFilter_Struct kfs[3];
int16_t Com_Filter_LowPass(int16_t newValue, int16_t preFilteredValue);

double Com_Filter_KalmanFilter(KalmanFilter_Struct *kf, double input);

#endif /* __COM_FILTER_H__ */
