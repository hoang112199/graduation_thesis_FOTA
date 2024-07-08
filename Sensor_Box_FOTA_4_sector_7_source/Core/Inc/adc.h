#ifndef __adc_H
#define __adc_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"

extern ADC_HandleTypeDef hadc1;

void MX_ADC1_Init(void);

#ifdef __cplusplus
}
#endif
#endif /*__ adc_H */