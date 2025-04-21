#ifndef __INF_BUZZER_H__
#define __INF_BUZZER_H__

// #include "gpio.h"
#include "tim.h"
// 定义不同音调的频率
#define TONE_HIGH   2000  // 高音频率 2000Hz
#define TONE_LOW    1000  // 低音频率 1000Hz

// 警报声音持续时间
#define ALARM_DURATION_MS  300  // 每个音调持续300ms

void Inf_Buzzer_Alarm(uint8_t times);
#endif /* __INF_BUZZER_H__ */
