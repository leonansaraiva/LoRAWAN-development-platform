/*
 * LoRAWAN_TIMER.h
 *
 *  Created on: 27 de março de 2017
 *      Author: Leonan Saraiva
 */

#ifndef LoRAWAN_TIMER_H_
#define LoRAWAN_TIMER_H_

#include "mbed.h"
#include <stdint.h>
#include <map>

class LoRAWAN_Dispatcher {
public:
	LoRAWAN_Dispatcher(void (*aplicationFunc)( void ), uint32_t tiks);
	~LoRAWAN_Dispatcher( void );
private:
	static Ticker _tick_time;
	typedef map<void (*)( void ), uint32_t> LoRaTikcsMap;
	static LoRaTikcsMap _ticks_map;
    static uint32_t _ticks;
    static void callBacks(void);
};

#endif /* LoRAWAN_Dispatcher */
