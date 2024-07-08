#include "ReadADCSensor.h"

float Convert_ADC_To_Voltage(uint32_t adcValue) {
    float voltage = (adcValue * 5) / 1023.0;
    return voltage;
}
float  Convert_Vol_To_Turbidity(uint32_t vol){
		float turb = -1120.4*vol*vol+5742.3*vol-4352.9;
		return turb;
}