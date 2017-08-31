#include "NTCThermistor.h"

/** Create a NTCThermistor instance
*@param T0 basic temperature[degrees/celsious]
*@param R0 basic resistance[ohm]. Thermistor takes this resistance at basic temperature T0.
*@param B B-parameter. 
*/


#define Vref 5000.0
#define ADC  65536.0

NTCThermistor::NTCThermistor(float T0,float R0,float B){
    T0_ = T0;
    K0_ = T0_+273.0;
    R0_ = R0;
    B_ = B;
}

/** Transfer temperature from resistance 
*/
int64_t NTCThermistor::get_temperature_Beta(int64_t R){
    float TK = 1 / T0_ + (1 / B_) * int64_t(float(R) / R0_);
    return (1 / TK) - 273.15;
}

int64_t NTCThermistor::resistance(int64_t vout) {
    int64_t a = ((int64_t) ADC) * R0_;
    int64_t b = a - ((int64_t) R0_ * ((int64_t) vout));
    return (b / vout);   
}

float NTCThermistor::read(int64_t input) {   
    return  get_temperature_Beta(resistance(input));    
}

