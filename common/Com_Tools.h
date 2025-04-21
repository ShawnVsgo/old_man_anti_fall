#ifndef __COM_TOOLS_H__
#define __COM_TOOLS_H__
#include "main.h"
#include "time.h"
#include "Com_Debug.h"
#include <stdio.h>
void Com_Delay_us(uint16_t us);

void Com_Delay_ms(uint16_t ms);

void Com_Delay_s(uint16_t s);

void Com_UTC_to_Bj_Time(uint8_t * UTC_Time, uint8_t * Bj_Time);

#endif /* __COM_TOOLS_H__ */
