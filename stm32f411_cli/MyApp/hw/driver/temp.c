#include "temp.h"
#include "adc.h"
#include "stm32f4xx_hal_adc.h"

#include <stdint.h>
extern ADC_HandleTypeDef hadc1;

bool tempInit(void) {
    // Implementation for temperature sensor initialization
    
    return true;
}

float tempRead(void) {
    // Implementation for reading temperature
    uint32_t adc_val = 0; // Replace with actual ADC reading code
    float temp_celsius = 0.0f; // Example conversion, adjust as needed
    HAL_ADC_Start(&hadc1); // Start ADC conversion
    // return 0.0f;

    if(HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK) {
        adc_val = HAL_ADC_GetValue(&hadc1);
        float vsense = (adc_val / 4095.0f) * 3.3f; // Assuming a 12-bit ADC and 3.3V reference
        temp_celsius = (vsense - 0.76f) / 0.0025f + 25.0f; // Example conversion formula for STM32 internal temperature sensor
    }

    HAL_ADC_Stop(&hadc1); // Stop ADC conversion
    
    return temp_celsius;
}