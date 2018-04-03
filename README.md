# LoRaIFSC
IFSC-Api for end devices using the LoRaWAN communication protocol.

This project uses classes in C ++ to do a LMIC library wraper. Facilitating the communication of end devices in a LoRaWAN network.
# Dependecy 
To execute this project is necessary to install the gcc4mbed compressed archive from https://github.com/adamgreen/gcc4mbed/zipball/master

# API Layers

[![N|Solid](https://lh3.googleusercontent.com/-o5Myg76JNAA/WahF2GRoXCI/AAAAAAAAFiE/lxNsvZSHBU8XAhR97LtNPJcb8PPqVmdKACJoC/w530-h441-p-rw/camadas_plataformas.png)](https://nodesource.com/products/nsolid)

# End Device - FRDM-KL27Z
[![N|Solid](https://lh3.googleusercontent.com/-utC9pMVNQyg/WahHOrEzL0I/AAAAAAAAFik/Rw7j5JtRH2szlKYKPjTi4OnoWcZodg0JgCJoC/w530-h563-p-rw/frdm-kl27z.png)](https://nodesource.com/products/nsolid)

``` c++
// // /* Test which brings default HelloWorld project from mbed online compiler
//    to be built under GCC.
// Author: Leonan da Silva Saraiva 
// Email: leonandasilvasaraiva@gmail.com
// */
#include "mbed.h"
#include "LoRAWAN.h"
#include "LoRAWANConfig.h"
#include "LoRAWAN_Dispatcher.h"
#include "DS1820.h"
#include "Dht11.h"
#include "ACS712.h"

DigitalOut myled(LED1);
extern  void tx_done( void );
// DS1820 sensor_temperature_1(PTE5);
// DS1820 sensor_temperature_2(PTE4);
// Dht11 sensor_temperature_3(PTE3);
// ACS712 sensor_corrente(PTB0);

void tx_done( void ) {
        myled = 0;
}
void tx_led_send( void ) {
        myled = 1;
}
// LoRaWAN NwkSKey, network session key
// This is the default Semtech key, which is used by the early prototype TTN
// network.
static  uint8_t NWKSKEY[16] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
// LoRaWAN AppSKey, application session key
// This is the default Semtech key, which is used by the early prototype TTN
// network.
static  uint8_t APPSKEY[16] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
// LoRaWAN end-device address (DevAddr)
static  uint32_t DEVADDR = 0x03FF0001; // <-- Change this address for every node!
static const uint32_t FREQ = LoRaWAN_Config_868::EU868_F1;
static  uint32_t CHANNELS[9] = {FREQ, FREQ, FREQ, FREQ, FREQ, FREQ, FREQ, FREQ, FREQ}; // <-- Change this address for every node!
/*!
 * Defines the network ID when using personalization activation procedure
 */
#define NET_ID  1
/*!
 * LoRaWAN application port
 */
#define APP_PORT_1    3
#define APP_PORT_2    3

#define TX_TIME_1 30
#define TX_TIME_2 120
uint8_t data_1[9] = "app1";
uint8_t data_2[9] = "app2";
uint8_t buffer[16] = {0};
Frame *frame1 = new Frame();
Frame *frame2 = new Frame();
LoRaWAN_Config_915 * config_915 = new LoRaWAN_Config_915 ( 
        NET_ID,
        DEVADDR,
        NWKSKEY,
        APPSKEY,
        LoRaWAN_Config_915::DR_SF7,
        LoRaWAN_Config_915::DR_SF9,
        LoRaWAN_Config_915::band1
);
LoRAWAN *lora = new LoRAWAN(config_915);
void appFunc1( void ) {
    tx_led_send();
    frame1->assemble(data_1, sizeof(data_1), APP_PORT_1); 
    lora->send(frame1);  
}
void appFunc2( void ) {
    tx_led_send();
    frame2->assemble(data_2, sizeof(data_2), APP_PORT_2); 
    lora->send(frame2);  
}
int main( void ) {   
    LoRAWAN_Dispatcher Tpp1(&appFunc1,TX_TIME_1);
    LoRAWAN_Dispatcher Tpp2(&appFunc2,TX_TIME_2);      
    printf("\rstart\r\n");
    while(true) {
       lora->run();
        if(lora->hasData()) {
            printf("\rReceived %d bytes of Payload %s\r\n",lora->received(buffer),buffer);
        }
    }
}
```
