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
	uint8_t	buffer_tx_size;
	uint8_t *message_buffer_rx;
	uint8_t	buffer_rx_size;
	uint8_t start_symbol_buffer;
	uint8_t end_symbol_buffer[2];
	/* data pointer */
	void *data_pointer;
	/*!
	 * 	int8_t, uint8_t, int16_t, uint16_t, uint32_t, int32_t, float
	 * 	@todo char, double, uint64_t, int64_t
	 */
	unsigned int data_operating_type : 3; // 0-7
	unsigned int structure_lock : 1; // 0-1
	unsigned int RESERVED : 4;
}matlab_serial_t;

#ifndef ROBOTIC_LIBRARY_STATIC
#include "stdlib.h"
#include "string.h"
typedef enum {
	STATUS_OK = 0x0,
	OBJECT_NULL = 0x1,
	INVALID_DATA_TYPE = 0x2,
	UNDEFINED_DATA_TYPE = 0x3,
	END_FUNCTION_WITH_ERROR = 0xF,
}status_t;

#ifndef NDEBUG
#define VALUE_CAN_BE_NULL(expression)		assert(expression != NULL);
#define DATA_TYPE_CAN_BE_ZERO(type)			assert(type == 0);
#define DATA_TYPE_CAN_BE_NOT_U8(type)		assert(type == 1);
#define DATA_TYPE_CAN_BE_NOT_U16(type)		assert(type == 2);
#define DATA_TYPE_CAN_BE_NOT_U32(type)		assert(type == 3);
#define DATA_TYPE_CAN_BE_NOT_FLOAT(type)	assert(type == 7);
#define MALLOC_CAN_RETURN_NULL(result)		assert(result != NULL);
#else /* NDEBUG */
#define VALUE_CAN_BE_NULL(expression)	if(expression == NULL) return OBJECT_NULL;
#endif /* NDEBUG */
#define matlab_serial_init(obj, interface, pointer, start_symbol, end_symbol) _Generic((&pointer[0]),\
			float *: matlab_serial_init_f32((matlab_serial_t *)obj, interface, (float *)&pointer[0], start_symbol, end_symbol),\
			uint8_t *: matlab_serial_init_u8((matlab_serial_t *)obj, interface, (uint8_t *)pointer, start_symbol, end_symbol),\
			uint16_t *: matlab_serial_init_u16((matlab_serial_t *)obj, interface, (uint16_t *)pointer, start_symbol, end_symbol),\
			uint32_t *: matlab_serial_init_u32((matlab_serial_t *)obj, interface, (uint32_t *)pointer, start_symbol, end_symbol),\
			int8_t *: matlab_serial_init_i8((matlab_serial_t *)obj, interface, (int8_t *)pointer, start_symbol, end_symbol),\
			int16_t *: matlab_serial_init_i16((matlab_serial_t *)obj, interface, (int16_t *)pointer, start_symbol, end_symbol),\
			int32_t *: matlab_serial_init_i32((matlab_serial_t *)obj, interface, (int32_t *)pointer, start_symbol, end_symbol),\
			default: END_FUNCTION_WITH_ERROR)

#define matlab_serial_init_array(obj, interface, pointer, start_symbol, end_symbol) _Generic((&pointer[0]),\
			float *: matlab_serial_init_f32_array((matlab_serial_t *)obj, interface, (float *)&pointer[0], (uint8_t)(sizeof(pointer)/4), start_symbol, end_symbol),\
			uint8_t *: matlab_serial_init_u8_array((matlab_serial_t *)obj, interface, (uint8_t *)pointer, (uint8_t)sizeof(pointer), start_symbol, end_symbol),\
			uint16_t *: matlab_serial_init_u16_array((matlab_serial_t *)obj, interface, (uint16_t *)pointer, (uint8_t)sizeof(pointer), start_symbol, end_symbol),\
			uint32_t *: matlab_serial_init_u32_array((matlab_serial_t *)obj, interface, (uint32_t *)pointer, (uint8_t)sizeof(pointer), start_symbol, end_symbol),\
			int8_t *: matlab_serial_init_i8_array((matlab_serial_t *)obj, interface, (int8_t *)pointer, (uint8_t)sizeof(pointer), start_symbol, end_symbol),\
			int16_t *: matlab_serial_init_i16_array((matlab_serial_t *)obj, interface, (int16_t *)pointer, (uint8_t)sizeof(pointer), start_symbol, end_symbol),\
			int32_t *: matlab_serial_init_i32_array((matlab_serial_t *)obj, interface, (int32_t *)pointer, (uint8_t)sizeof(pointer), start_symbol, end_symbol),\
			default: END_FUNCTION_WITH_ERROR)

#endif

status_t matlab_serial_send(
	matlab_serial_t *object,
	uint32_t timeout);

/**
 * @brief This function
 *
 * This function
 *
 * @param a The first integer.
 * @param b The second integer.
 * @return The sum of the two integers.
 */
status_t matlab_serial_init_f32(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	float *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol);

/**
 * @brief This function
 *
 * This function
 *
 * @param a The first integer.
 * @param b The second integer.
 * @return The sum of the two integers.
 */
status_t matlab_serial_init_f32_array(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	float *array_pointer,
	uint8_t array_size,
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
	UART_HandleTypeDef *interface,
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
	UART_HandleTypeDef *interface,
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
	UART_HandleTypeDef *interface,
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
	UART_HandleTypeDef *interface,
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
	UART_HandleTypeDef *interface,
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
	UART_HandleTypeDef *interface,
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
