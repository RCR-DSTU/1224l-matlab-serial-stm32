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
	uint8_t *message_buffer_tx;
	uint8_t *message_buffer_rx;
	uint8_t start_symbol_buffer;
	uint16_t end_symbol_buffer;
}matlab_serial_t;

#ifndef ROBOTIC_LIBRARY_STATIC
#include "stdlib.h"
#include "string.h"
#ifndef NDEBUG
#define VALUE_CAN_BE_NULL(expression)	assert(expression != NULL);
#else /* NDEBUG */
#define VALUE_CAN_BE_NULL(expression)	if(expression == NULL) return;
#endif /* NDEBUG */

#define matlab_serial_init(obj, value, start_symbol, end_symbol) _Generic((value),\
		float *: matlab_serial_init_f32((matlab_serial_t *)obj, (float *)value, start_symbol, end_symbol),\
		uint8_t *: matlab_serial_init_u8((matlab_serial_t *)obj, (uint8_t *)value, start_symbol, end_symbol),\
		uint16_t *: matlab_serial_init_u16((matlab_serial_t *)obj, (uint16_t *)value, start_symbol, end_symbol),\
		uint32_t *: matlab_serial_init_u32((matlab_serial_t *)obj, (uint32_t *)value, start_symbol, end_symbol),\
		default: (void)0U)
#endif

void matlab_serial_init_f32(
	matlab_serial_t *object,
	float *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol);

void matlab_serial_init_f32_hardware_in_the_loop(
	matlab_serial_t *object,
	float *value_tx_pointer,
	float *value_rx_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol);

void matlab_serial_init_f32_array(
	matlab_serial_t *object,
	float *array_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol);

void matlab_serial_init_u8(
	matlab_serial_t *object,
	uint8_t *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol);

void matlab_serial_init_u8_array(
	matlab_serial_t *object,
	uint8_t *array_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol);

void matlab_serial_init_u16(
	matlab_serial_t *object,
	uint16_t *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol);

void matlab_serial_init_u16_array(
	matlab_serial_t *object,
	uint16_t *array_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol);

void matlab_serial_init_u32(
	matlab_serial_t *object,
	uint32_t *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol);

void matlab_serial_init_u32_array(
	matlab_serial_t *object,
	uint32_t *array_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol);

void matlab_serial_finish(matlab_serial_t *object);


#endif /* USE_HAL_DRIVER */
#endif /*_MATLAB_SERIAL_H_*/