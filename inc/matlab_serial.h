#pragma once
#ifndef _MATLAB_SERIAL_H_
#define _MATLAB_SERIAL_H_

#ifdef USE_HAL_DRIVER
#if defined(STM32F407xx) || defined(STM32F401xC) || defined(STM32F411xE)
#include "stm32f4xx_hal.h"
#elif defined(STM32F103xx)
#include "stm32f1xx_hal.h"
#endif


typedef struct {
	UART_HandleTypeDef *interface;

}matlab_serial_t;

void matlab_serial_init_default(matlab_serial_t *object, );

void matlab_serial_send_f32(matlab_serial_t *object, float value);

void matlab_serial_send_u32(matlab_serial_t *object, uint32_t value);

void matlab_serial_send_f32_array(matlab_serial_t *object, float *array_pointer);

#endif /* USE_HAL_DRIVER */
#endif /*_MATLAB_SERIAL_H_*/
