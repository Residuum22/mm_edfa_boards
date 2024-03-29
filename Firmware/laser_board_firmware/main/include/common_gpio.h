#pragma once
#include "hal/gpio_types.h"
#include "hal/dac_types.h"
#include "hal/adc_types.h"

#define PELTIER1_COOLIN_PIN     GPIO_NUM_21
#define PELTIER1_HEATING_PIN            GPIO_NUM_19

#define PELTIER2_COOLIN_PIN             GPIO_NUM_23
#define PELTIER2_HEATING_PIN            GPIO_NUM_22

#define LASER1_TEMP_PIN                 GPIO_NUM_35
#define LASER2_TEMP_PIN                 GPIO_NUM_34

#define LASER1_MONITOR_DIODE_PIN        GPIO_NUM_36
#define LASER2_MONITOR_DIODE_PIN        GPIO_NUM_39

#define LASER1_TEMP_CHANNEL             ADC_CHANNEL_7
#define LASER2_TEMP_CHANNEL             ADC_CHANNEL_6

#define LASER1_MONITOR_DIODE_CHANNEL    ADC_CHANNEL_0
#define LASER2_MONITOR_DIODE_CHANNEL    ADC_CHANNEL_3

#define LASER1_DIODE_CURRENT_CHANNEL    DAC_CHAN_1
#define LASER2_DIODE_CURRENT_CHANNEL    DAC_CHAN_0

#define INDICATOR_RED_GPIO              GPIO_NUM_18
#define INDICATOR_GREEN_GPIO            GPIO_NUM_5