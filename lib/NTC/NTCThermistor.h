#ifndef NTCTHERMISTOR_H
#define NTCTHERMISTOR_H
#include "mbed.h"
#include "math.h"
/**
*NTC Thermistor class
*/
class NTCThermistor
{
    public:
        /** Create a NTC Thermistor instance
        *@param T0 basic temperature[degrees/celsious]
        *@param R0 basic resistance[ohm]. Thermistor takes this resistance at basic temperature T0.
        *@param B B-parameter. 
        */
        NTCThermistor(float T0,float R0,float B); 
        /** Transfer temperature from resistance.
        *@param R resitance of NTC thermistor
        */ 
        int64_t get_temperature_Beta(int64_t R);    //transfer resistance to temperature(degrees)
        int64_t resistance(int64_t vout);
        float read(int64_t input);//
    private:
        float R0_;     //basic resisitance of thremister at basic temperature 
        float T0_;     //basic temperature[°C]
        float K0_;     //basic temperature[K]
        float B_;      //B constant 
};
#endif

