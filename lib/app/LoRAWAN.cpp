/*
 * LoRAWAN.cpp
 *
 *  Created on: 27 de março de 2017
 *      Author: Leonan Saraiva
 */
#include "LoRAWAN.h"
#include "mbed.h"
myQueue * LoRAWAN::txQueue;
myQueue * LoRAWAN::rxQueue;
Frame *LoRAWAN::txFrame;
static const uint32_t FREQ = LoRaWAN_Config_868::EU868_F1;
uint32_t LoRaWAN_Config_868::_defalt_ch[9] = {FREQ, FREQ, FREQ, FREQ, FREQ, FREQ, FREQ, FREQ, FREQ}; // 
uint8_t LoRAWAN::rxBuffer [DATA_SIZE] = {0};
uint8_t rxLen = 0;
extern  void tx_done( void );

LoRAWAN::LoRAWAN(LoRaWAN_Config_915 * config) {
    txQueue = new myQueue(DATA_SIZE,100); 
    rxQueue = new myQueue(DATA_SIZE,100); 
    txFrame = new Frame();
      // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    printf("\rLMIC init\r\n");
    LMIC_reset();
    printf("\rLMIC reset\r\n");
    // Set static session parameters. Instead of dynamically establishing a session
    // by joining the network, precomputed session parameters are be provided.  
    LMIC_setSession(config->_netID, config->_devAddr, config->_nwkKey, config->_appKey);
    printf("\rLMIC LMIC_setSession\r\n");
    // Disable link check validation
    LMIC_setLinkCheckMode(0);
    /*Enable / disable link check validation. Link check mode is enabled by default and is used to periodically
    Verify network connectivity. Must be called only if a session is established.*/
    printf("\rSetup\r\n");
    static uint8_t fc;    
    fc = 10;
    // NA-US channels 0-71 are configured automatically
    // but only one group of 8 should (a subband) should be active
    // TTN recommends the second sub band, 1 in a zero based count.
    // https://github.com/TheThingsNetwork/gateway-conf/blob/master/US-global_conf.json
    #if defined(CFG_us915)
    printf("\rDefined frequency 915 MHz\r\n");
    printf("\rDefined SubBand %d\r\n", config->_nBand);
    LMIC_selectSubBand(config->_nBand);
    #endif
    // Set data rate and transmit power for uplink (note: txpow seems to be ignored by the library)
    LMIC_setDrTxpow(config->_txSf,14);
    printf("\rTX DR_SF%d\r\n", fc-config->_txSf);
    // TTN uses SF9 for its RX2 window.
    LMIC.dn2Dr = config->_rxSf;
    printf("\rRX DR_SF%d\r\n", fc-config->_rxSf);
    // Start job
    doSend(&_sendjob);
    run();
}

LoRAWAN::LoRAWAN(LoRaWAN_Config_868 * config) {
    txQueue = new myQueue(DATA_SIZE,100); 
    rxQueue = new myQueue(DATA_SIZE,100); 
    txFrame = new Frame();
    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    printf("\rLMIC init\r\n");
    LMIC_reset();
    printf("\rLMIC reset\r\n");
    // Set static session parameters. Instead of dynamically establishing a session
    // by joining the network, precomputed session parameters are be provided.  
    LMIC_setSession(config->_netID, config->_devAddr, config->_nwkKey, config->_appKey);
    printf("\rLMIC LMIC_setSession\r\n");
    // Disable link check validation
    LMIC_setLinkCheckMode(0);
    /*Enable / disable link check validation. Link check mode is enabled by default and is used to periodically
    Verify network connectivity. Must be called only if a session is established.*/
    printf("\rSetup\r\n");
    static uint8_t fc;
    #if defined(CFG_eu868)
    printf("\rDefined frequency 868 MHz\r\n");
    fc = 12;
    // Set up the channels used by the Things Network, which corresponds
    // to the defaults of most gateways. Without this, only three base
    // channels from the LoRAWAN specification are used, which certainly
    // works, so it is good for debugging, but can overload those
    // frequencies, so be sure to configure the full frequency range of
    // your network here (unless your network autoconfigures them).
    // Setting up channels should happen after LMIC_setSession, as that
    // configures the minimal channel set.
    // NA-US channels 0-71 are configured automatically
    if(config->_channels == NULL) config->_channels = LoRaWAN_Config_868::_defalt_ch;//default channel EU868_F1 868Mhz
    for(int i = 0;i < 8;i++) {
    LMIC_setupChannel(i, config->_channels[i], DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    printf("\r channel[%d] freq: %ld\r\n",i, config->_channels[i]);
    } 
    LMIC_setupChannel(8, config->_channels[8], DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band
    printf("\r channel[8] freq: %ld\r\n", config->_channels[8]);
    // TTN defines an additional channel at 869.525Mhz using SF9 for class B
    // devices' ping slots. LMIC does not have an easy way to define set this
    // frequency and support for class B is spotty and untested, so this
    // frequency is not configured here.
    #endif
    // Set data rate and transmit power for uplink (note: txpow seems to be ignored by the library)
    LMIC_setDrTxpow(config->_txSf,14);
    printf("\rTX DR_SF%d\r\n", fc-config->_txSf);
    // TTN uses SF9 for its RX2 window.
    LMIC.dn2Dr = config->_rxSf;
    printf("\rRX DR_SF%d\r\n", fc-config->_rxSf);
    // Start job
    doSend(&_sendjob);
    run();
}

LoRAWAN::~LoRAWAN( void ) {}

void LoRAWAN::run( void ) {
    os_runloop_once();
}

void LoRAWAN::send(Frame *frame) {
    txQueue->Put(frame);
    doSend(&_sendjob);
}

uint8_t LoRAWAN::received(uint8_t * data) {

    if ( !rxQueue->Get( data )) {
        return -1;
    } else {
        return rxLen; 
    }

}

bool LoRAWAN::hasData( void ) {
    if (!rxQueue->GetNumberOfItems()) {
        return false;
    } else {
        return true;
    }
}

void LoRAWAN::nextTask( void ) {
    if (txQueue->GetNumberOfItems()) doSend(&_sendjob);
}


void do_send(osjob_t* j) {LoRAWAN::singleton()->doSend(j);}
void LoRAWAN::doSend(osjob_t* j) {
    hal_disableIRQs(); //using hal_lmic interrupt handler
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        // printf("\rOP_TXRXPEND, not sending\r\n");
        printf("\rPacket queued\r\n");
    } else {
        // Prepare upstream data transmission at the next possible time.
        if ( !txQueue->GetNumberOfItems()) {
            // queue is empty
            printf("\rQueue empty!\r\n" );
        } else {
            txQueue->Get( txFrame );
            printf( "\rData: %s \r\n", txFrame->_data);   
            LMIC_setTxData2(txFrame->_port, txFrame->_data, txFrame->_size, txFrame->_msgOn);
        }
    }
    hal_enableIRQs(); 
    // Next TX is scheduled after TX_COMPLETE event.
}
//////////////////////////////////////////////////
// LMIC EVENT CALLBACK
//////////////////////////////////////////////////
void onEvent(ev_t ev) { LoRAWAN::singleton()->onEvent(ev);}
void LoRAWAN::onEvent(ev_t ev) {
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            printf("\rEV_SCAN_TIMEOUT\r\n");
            break;
        case EV_BEACON_FOUND:
            printf("\rEV_BEACON_FOUND\r\n");
            break;
        case EV_BEACON_MISSED:
            printf("\rEV_BEACON_MISSED\r\n");
            break;
        case EV_BEACON_TRACKED:
            printf("\rEV_BEACON_TRACKED\r\n");
            break;
        case EV_JOINING:
            printf("\rEV_JOINING\r\n");
            break;
        case EV_JOINED:
            printf("\rEV_JOINED\r\n");
            break;
        case EV_RFU1:
            printf("\rEV_RFU1\r\n");
            break;
        case EV_JOIN_FAILED:
            printf("\rEV_JOIN_FAILED\r\n");
            break;
        case EV_REJOIN_FAILED:
            printf("\rEV_REJOIN_FAILED\r\n");
            break;
        case EV_TXCOMPLETE:
            printf("\rEV_TXCOMPLETE (includes waiting for RX windows)\r\n");
            if (LMIC.txrxFlags & TXRX_ACK)
              printf("\rReceived ack\r\n");
            if (LMIC.dataLen) {
                memcpy(rxBuffer,&LMIC.frame[LMIC.dataBeg],LMIC.dataLen);
                rxLen = LMIC.dataLen;
                rxQueue->Put(rxBuffer);
            }
            tx_done();
            // Schedule next transmission
            nextTask();
             break;
        case EV_LOST_TSYNC:
            printf("\rEV_LOST_TSYNC\r\n");
            break;
        case EV_RESET:
            printf("\rEV_RESET\r\n");
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            printf("\rEV_RXCOMPLETE\r\n");
            break;
        case EV_LINK_DEAD:
            printf("\rEV_LINK_DEAD\r\n");
            break;
        case EV_LINK_ALIVE:
            printf("\rEV_LINK_ALIVE\r\n");
            break;
         default:
            printf("\rUnknown event\r\n");
            break;
    }
}

// These callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in config.h, otherwise the linker will complain).
// provide application router ID (8 bytes, LSBF)

void os_getArtEui( uint8_t *buf ) { }

// provide device ID (8 bytes, LSBF)
void os_getDevEui( uint8_t *buf ) { }

// provide device key (16 bytes)
void os_getDevKey( uint8_t *buf ) { }


