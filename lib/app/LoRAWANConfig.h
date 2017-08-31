/*
 * LoRAWan.h
 *
 *  Created on: 27 de março de 2017
 *      Author: Leonan Saraiva
 */

#ifndef LORAWAN_CONFIG_H_
#define LORAWAN_CONFIG_H_
#include "mbed.h"

#define LoRa915
// #define LoRa868

class LoRAWANConfig {
public:
		
	LoRAWANConfig (
		uint8_t netID , 
		uint32_t devAddr,
		uint8_t * nwkKey, 
		uint8_t * appKey
	) 
	{
		 _netID = netID;
		 _devAddr = devAddr;
		 _nwkKey = nwkKey;
		 _appKey = appKey;
	}
	~LoRAWANConfig(){}

	uint8_t _netID;
	uint32_t _devAddr;
	uint8_t * _nwkKey;
	uint8_t * _appKey;
};

class LoRaWAN_Config_915 : public LoRAWANConfig {
	public:
		enum drSF { DR_SF10 = 0, DR_SF9, DR_SF8, DR_SF7, DR_SF8C, DR_NONE, DR_SF12CR=8, DR_SF11CR, DR_SF10CR, DR_SF9CR, DR_SF8CR, DR_SF7CR };
		enum subBand {band0 = 0, band1, band2, band3, band4, band5, band6, band7};	
		
		LoRaWAN_Config_915 (
		uint8_t netID , 
		uint32_t devAddr,
		uint8_t * nwkKey, 
		uint8_t * appKey,
		drSF txSf,
		drSF rxSf,
		subBand nBand = band1
		) : LoRAWANConfig(netID, devAddr, nwkKey, appKey)
		{
		 _txSf = txSf;
		 _rxSf = rxSf;
		 _nBand = nBand;
		}

		~LoRaWAN_Config_915() {}

		drSF _txSf;
		drSF _rxSf;
		subBand _nBand; 
};

class LoRaWAN_Config_868 : public LoRAWANConfig {
	public:
		enum { EU868_F1 = 868100000,      // g1   SF7-12
		       EU868_F2 = 868300000,      // g1   SF7-12 FSK SF7/250
		       EU868_F3 = 868500000,      // g1   SF7-12
		       EU868_F4 = 868850000,      // g2   SF7-12
		       EU868_F5 = 869050000,      // g2   SF7-12
		       EU868_F6 = 869525000,      // g3   SF7-12
		       EU868_J4 = 864100000,      // g2   SF7-12  used during join
		       EU868_J5 = 864300000,      // g2   SF7-12   ditto
		       EU868_J6 = 864500000,      // g2   SF7-12   ditto
			};

		enum drSF { DR_SF12=0, DR_SF11, DR_SF10, DR_SF9, DR_SF8, DR_SF7, DR_SF7B, DR_FSK, DR_NONE };

		LoRaWAN_Config_868 (
		uint8_t netID , 
		uint32_t devAddr,
		uint8_t * nwkKey, 
		uint8_t * appKey,
		drSF txSf,
		drSF rxSf,
		uint32_t *channels = NULL
		) : LoRAWANConfig(netID, devAddr, nwkKey, appKey)
		{
			_txSf = txSf;
			_rxSf = rxSf;
			_channels = channels; 
		}

		~LoRaWAN_Config_868() {}

		drSF _txSf;
		drSF _rxSf;
		uint32_t *_channels; 
		static uint32_t _defalt_ch[9];
};
#endif /* LORAWAN_CONFIG_H_*/
