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
	/*!
	 * 	int8_t, uint8_t, int16_t, uint16_t, uint32_t, int32_t, float
	 * 	@todo char, double
	 */
	unsigned int data_operating_type: 3; // 0-7
	unsigned int RESERVED: 5;
}matlab_serial_t;

#ifndef ROBOTIC_LIBRARY_STATIC
#include "stdlib.h"
#include "string.h"
typedef enum {
	STATUS_OK = 0,
	OBJECT_NULL = 1,
	INVALID_DATA_TYPE = 2,
}status_t;
#ifndef NDEBUG
#define VALUE_CAN_BE_NULL(expression)	assert(expression != NULL);
#define DATA_CAN_BE_NOT_U8(type)		assert(type == 0);
#define DATA_CAN_BE_NOT_U16(type)		assert(type == 1);
#define DATA_CAN_BE_NOT_U32(type)		assert(type == 2);
#define DATA_CAN_BE_NOT_FLOAT(type)		assert(type == 7);
#else /* NDEBUG */
#define VALUE_CAN_BE_NULL(expression)	if(expression == NULL) return;
#endif /* NDEBUG */

#define matlab_serial_init(obj, pointer, start_symbol, end_symbol) _Generic((pointer),\
		float *: matlab_serial_init_f32((matlab_serial_t *)obj, (float *)pointer, start_symbol, end_symbol),\
		uint8_t *: matlab_serial_init_u8((matlab_serial_t *)obj, (uint8_t *)pointer, start_symbol, end_symbol),\
		uint16_t *: matlab_serial_init_u16((matlab_serial_t *)obj, (uint16_t *)pointer, start_symbol, end_symbol),\
		uint32_t *: matlab_serial_init_u32((matlab_serial_t *)obj, (uint32_t *)pointer, start_symbol, end_symbol),\
		int8_t *: matlab_serial_init_i8((matlab_serial_t *)obj, (int8_t *)pointer, start_symbol, end_symbol),\
		int16_t *: matlab_serial_init_i16((matlab_serial_t *)obj, (int16_t *)pointer, start_symbol, end_symbol),\
		int32_t *: matlab_serial_init_i32((matlab_serial_t *)obj, (int32_t *)pointer, start_symbol, end_symbol),\
		default: (void)0U)

#define matlab_serial_send(obj, data, timeout) _Generic((data),\
		float *: matlab_serial_send_f32((matlab_serial_t *)obj, (float *)data), timeout),\
		default: (void)0U
#endif

/**
 * @brief This function calculates the sum of two integers.
 *
 * This function takes two integers as input and returns their sum.
 *
 * @param a The first integer.
 * @param b The second integer.
 * @return The sum of the two integers.
 */
status_t matlab_serial_init_f32(
	matlab_serial_t *object,
	float *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol);

/**
 * @brief This function calculates the sum of two integers.
 *
 * This function takes two integers as input and returns their sum.
 *
 * @param a The first integer.
 * @param b The second integer.
 * @return The sum of the two integers.
 */
status_t matlab_serial_send_f32(
	matlab_serial_t *object,
	float *data,
	uint32_t timeout);

/**
 * @brief This function calculates the sum of two integers.
 *
 * This function takes two integers as input and returns their sum.
 *
 * @param a The first integer.
 * @param b The second integer.
 * @return The sum of the two integers.
 */
status_t matlab_serial_init_u8(
	matlab_serial_t *object,
	uint8_t *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol);

/**
 * @brief This function calculates the sum of two integers.
 *
 * This function takes two integers as input and returns their sum.
 *
 * @param a The first integer.
 * @param b The second integer.
 * @return The sum of the two integers.
 */
status_t matlab_serial_send_u8(
	matlab_serial_t *object,
	uint8_t *data,
	uint32_t timeout);

/**
 * @brief This function calculates the sum of two integers.
 *
 * This function takes two integers as input and returns their sum.
 *
 * @param a The first integer.
 * @param b The second integer.
 * @return The sum of the two integers.
 */
status_t matlab_serial_init_i8(
	matlab_serial_t *object,
	int8_t *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol);

/**
 * @brief This function calculates the sum of two integers.
 *
 * This function takes two integers as input and returns their sum.
 *
 * @param a The first integer.
 * @param b The second integer.
 * @return The sum of the two integers.
 */
status_t matlab_serial_send_i8(
	matlab_serial_t *object,
	int8_t *data,
	uint32_t timeout);

/**
 * @brief This function calculates the sum of two integers.
 *
 * This function takes two integers as input and returns their sum.
 *
 * @param a The first integer.
 * @param b The second integer.
 * @return The sum of the two integers.
 */
status_t matlab_serial_init_u16(
	matlab_serial_t *object,
	uint16_t *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol);

/**
 * @brief This function calculates the sum of two integers.
 *
 * This function takes two integers as input and returns their sum.
 *
 * @param a The first integer.
 * @param b The second integer.
 * @return The sum of the two integers.
 */
status_t matlab_serial_init_i16(
	matlab_serial_t *object,
	int16_t *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol);

/**
 * @brief This function calculates the sum of two integers.
 *
 * This function takes two integers as input and returns their sum.
 *
 * @param a The first integer.
 * @param b The second integer.
 * @return The sum of the two integers.
 */
status_t matlab_serial_init_u32(
	matlab_serial_t *object,
	uint32_t *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol);

/**
 * @brief This function calculates the sum of two integers.
 *
 * This function takes two integers as input and returns their sum.
 *
 * @param a The first integer.
 * @param b The second integer.
 * @return The sum of the two integers.
 */
status_t matlab_serial_init_i32(
	matlab_serial_t *object,
	int32_t *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol);

/**
 * @brief This function calculates the sum of two integers.
 *
 * This function takes two integers as input and returns their sum.
 *
 * @param a The first integer.
 * @param b The second integer.
 * @return The sum of the two integers.
 */
status_t matlab_serial_send_u32(
	matlab_serial_t *object,
	float *data,
	uint32_t timeout);

/**
 * @brief This function calculates the sum of two integers.
 *
 * This function takes two integers as input and returns their sum.
 *
 * @param a The first integer.
 * @param b The second integer.
 * @return The sum of the two integers.
 */
status_t matlab_serial_send_u16(
	matlab_serial_t *object,
	float *data,
	uint32_t timeout);

/**
 * @brief This function calculates the sum of two integers.
 *
 * This function takes two integers as input and returns their sum.
 *
 * @param a The first integer.
 * @param b The second integer.
 * @return The sum of the two integers.
 */
status_t matlab_serial_finish(matlab_serial_t *object);


#endif /* USE_HAL_DRIVER */
#endif /*_MATLAB_SERIAL_H_*/
