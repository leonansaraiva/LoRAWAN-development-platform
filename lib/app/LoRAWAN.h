/*
 * LoRAWAN.h
 *
 *  Created on: 27 de março de 2017
 *      Author: Leonan Saraiva
 */

#ifndef LoRAWAN_H_
#define LoRAWAN_H_
#include <stdint.h>
#include "mbed.h"
#include "lmic.h"
#include "Singleton.h"
#include "myQueue.h"
#include "LoRAWANConfig.h"

class Frame {
    public:
      void assemble(uint8_t * data, int size, int port, bool msgOn = false) {
      	_data = data;
      	_size = size;
      	_port = port;
      	_msgOn = msgOn;
      }
      uint8_t * _data;
      int _size;
      int _port;
      bool _msgOn;
};

class LoRAWAN : public Singleton<LoRAWAN> {
public:
	static const uint8_t DATA_SIZE = 32;
	static uint8_t *(*aplicationFunc)(void);
	LoRAWAN(LoRaWAN_Config_915 * config);
	LoRAWAN(LoRaWAN_Config_868 * config);
	~LoRAWAN( void );
	void onEvent(ev_t ev);
	void run( void );
 	void send(Frame *frame);
 	uint8_t received(uint8_t * data);
	bool hasData( void );
	static void doSend(osjob_t* j);
private:
	void nextTask( void );
	osjob_t _sendjob;
    static myQueue * txQueue;
    static myQueue * rxQueue;
	static uint8_t rxBuffer [DATA_SIZE];
	static Frame *txFrame;
};


#endif /* LoRAWAN_H_ */
