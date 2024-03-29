#pragma once

/**
 * @brief This function initialize the VOA driver base (timer, operators, comparators).
 * The function is initialize MCPWM pheripheral in the right order.
 * More details can be found in the ESP-IDF documentation.
 * 
 */
void voa_control_init();

/**
 * @brief This function enable the output signals.
 * 
 */
void voa_control_enable_output();

/**
 * @brief This function disable the ouput signals.
 * 
 */
void voa_control_disable_output();

/**
 * @brief This function initialize the ADC1 pheripheral in oneshot mode.
 * The input pin is on the VOA schematics.
 * 
 */
void voa_control_adc_init();

/**
 * @brief This function is mapping the zero attenuation end value.
 * 
 */
void voa_control_set_attenuation_zero();

/**
 * @brief This function is mapping the max attenuation end value.
 * 
 */
void voa_control_set_attenuation_max();

/**
 * @brief This function is the main VOA task. In this function fwd and rev
 * MCPWM pheriheral and moving VOA to the end positons. In the main loop if the attenuation
 * value is sent then this function is responsible to set the correct value.
 * 
 * @param pvParameters Unused - FreeRTOS parameter
 */
void voa_control_task(void *pvParameters);