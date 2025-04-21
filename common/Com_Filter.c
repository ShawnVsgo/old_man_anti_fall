#include "Com_Filter.h"

#define ALPHA 0.15 /* һ�׵�ͨ�˲� ָ����Ȩϵ�� */
/**
 * @description: һ�׵�ͨ�˲�
 *  ��һ�ֳ��õ��˲���������ȥ����Ƶ�������Ƶ�ɷ֣������ź��еĵ�Ƶ�ɷ֡�
 *  �ڵ�Ƭ��Ӧ���У�һ�ּ��ҳ����ĵ�ͨ�˲�����һ������������Ӧ��IIR����ͨ�˲�����
 *  ͨ��ʵ��Ϊָ����Ȩ�ƶ�ƽ���˲�����
 * @param {int16_t} newValue ��Ҫ�˲���ֵ
 * @param {int16_t} preFilteredValue ��һ���˲�����ֵ
 * @return {*}
 */
int16_t Com_Filter_LowPass(int16_t newValue, int16_t preFilteredValue)
{
    return ALPHA * newValue + (1 - ALPHA) * preFilteredValue;
}

/* �������˲� https://www.mwrf.net/tech/basic/2023/30081.html */

/* �������˲����� */
KalmanFilter_Struct kfs[3] = {
    {0.02, 0, 0, 0, 0.001, 0.543},
    {0.02, 0, 0, 0, 0.001, 0.543},
    {0.02, 0, 0, 0, 0.001, 0.543}};
double Com_Filter_KalmanFilter(KalmanFilter_Struct *kf, double input)
{
    kf->Now_P = kf->LastP + kf->Q;
    kf->Kg    = kf->Now_P / (kf->Now_P + kf->R);
    kf->out   = kf->out + kf->Kg * (input - kf->out);
    kf->LastP = (1 - kf->Kg) * kf->Now_P;
    return kf->out;
}
