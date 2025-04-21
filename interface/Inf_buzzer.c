#include "inf_buzzer.h"

/**
 * @brief 设置蜂鸣器频率
 * @param freq: 频率(Hz)
 */
void Inf_Buzzer_SetFreq(uint16_t freq)
{
    if(freq == 0) {
        // 关闭蜂鸣器
        HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
        return;
    }
    
    // 设置定时器自动重装值
    uint16_t arr = 1000000 / freq;  // 假设定时器时钟为1MHz
    __HAL_TIM_SET_AUTORELOAD(&htim3, arr);
    
    // 设置PWM占空比为50%
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, arr/2);
    
    // 启动PWM输出
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

/**
 * @brief 发出警报声
 * @param times: 警报声重复次数，0表示持续报警
 */
void Inf_Buzzer_Alarm(uint8_t times)
{
    static uint32_t last_toggle = 0;
    static uint8_t is_high = 0;
    static uint8_t remain_times = 0;
    
    if(times > 0) {
        remain_times = times * 2;  // 每次报警需要高低音各一次
    }
    
    uint32_t now = HAL_GetTick();
    if(now - last_toggle >= ALARM_DURATION_MS)
    {
        if(is_high) {
            Inf_Buzzer_SetFreq(TONE_LOW);
            is_high = 0;
        } else {
            Inf_Buzzer_SetFreq(TONE_HIGH);
            is_high = 1;
            
            if(times > 0) {
                remain_times--;
                if(remain_times == 0) {
                    Inf_Buzzer_SetFreq(0);  // 停止报警
                }
            }
        }
        last_toggle = now;
    }
}
