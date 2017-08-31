#include "mbed.h"
#include "LoRAWAN_Dispatcher.h"
#include <map>

uint32_t LoRAWAN_Dispatcher::_ticks = 0;
LoRAWAN_Dispatcher::LoRaTikcsMap LoRAWAN_Dispatcher::_ticks_map;
Ticker LoRAWAN_Dispatcher::_tick_time;

LoRAWAN_Dispatcher::LoRAWAN_Dispatcher(void (*aplicationFunc)( void ), uint32_t tiks) {
	_ticks_map[aplicationFunc] = tiks;
	_tick_time.attach(&callBacks, 1.0);
}

LoRAWAN_Dispatcher::~LoRAWAN_Dispatcher() {}

void LoRAWAN_Dispatcher::callBacks( void ) {
	_ticks++;
	std::map<void (*)( void ), uint32_t>::iterator it = _ticks_map.begin();
	for (it=_ticks_map.begin(); it!=_ticks_map.end(); ++it) {
		if(_ticks % it->second == 0) {
			it->first();
		}
	}
}
