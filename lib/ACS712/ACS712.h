#include <mbed.h>

class ACS712 { 

    public:
        const double adc = 65536.0;
        const double Vref = 5000;
        const int mVperAmp = 185; 
        ACS712(PinName _pin,int ACSoffset = 2480);    
        /** Read the value of the measured current in amps
         *
         * @return current value in amps
         */
        float read(int samples = 1);
        
    private:
        AnalogIn sensor;
        double Voltage;
        double Amps; 
        int RawValue;
        int ACSoffset;
};
 
ACS712::ACS712(PinName _pin,int _ACSoffset) : sensor(_pin),ACSoffset(_ACSoffset){}

float ACS712::read(int samples) {     
    int64_t accumulated=0, average=0;
    for(int i= 0; i<samples; i++) {
      RawValue = sensor.read_u16();   //convert the analog-value (voltage) to digital one (0-1024)
      accumulated += RawValue*RawValue;      //accumulate all the digital-values repeat times   
    }
    average = accumulated / samples;
    RawValue = (int)(sqrt(average));
    Voltage = (Vref*RawValue )/ adc; // Gets you mV
    Amps = ((Voltage - ACSoffset) / mVperAmp);
    return Amps;
}