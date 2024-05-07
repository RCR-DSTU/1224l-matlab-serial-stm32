#pragma once
#ifndef _MATLAB_SERIAL_H_
#define _MATLAB_SERIAL_H_

/*!
 *
 * 	Matlab Simulink SDK for stm32 microcontrollers
 * 	Last change date: 03/05/24
 * 	Maintainer: @casonka
 * 	Mail: alexandrnazarov32@gmail.com
 *
 *	Library supports data types like: uint8_t, int8_t,
 *		uint16_t, int16_t, uint32_t, int32_t, float32
 *
 *	Todo list: char(ASCII), float64(double), uint64_t, int64_t
 */

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
	 * 	@info Operating data type map:
	 * 		0 - not initialized
	 * 		1 - uin8_t
	 * 		2 - int8_t
	 * 		3 - uint16_t
	 * 		4 - int16_t
	 * 		5 - uint32_t
	 * 		6 - int32_t
	 * 		7 - float
	 */
	unsigned int data_operating_type : 3; // 0-7

	/*!
	 * 	@info structure lock (simple thread safe implementation)
	 * 	0 - structure open
	 * 	1 - structure lock
	 */
	unsigned int structure_lock : 1; // 0-1
	unsigned int RESERVED : 4;
}matlab_serial_t;

#ifndef ROBOTIC_LIBRARY_STATIC
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"

/*!
 * Function states table
 */
typedef enum {
	STATUS_OK = 0x0,
	OBJECT_NULL = 0x1,
	INVALID_DATA_TYPE = 0x2,
	UNDEFINED_DATA_TYPE = 0x3,
	SENDING_ERROR = 0x4,
	RECEIVING_ERROR = 0x5,
	ALLOCATE_ERROR = 0x6,
	LIB_NOT_FOUND = 0x7,
	END_FUNCTION_WITH_ERROR = 0xF,
}status_t;

#ifndef NDEBUG
#define VALUE_CAN_BE_NULL(value)			assert(value != NULL)
#define VA_ARGS_SIZE_CAN_BE_INVALID(args)	assert(args != 0)
#define DATA_TYPE_CAN_BE_ZERO(type)			assert(type != 0)
#define DATA_TYPE_CAN_BE_NOT_U8(type)		assert(type == 1)
#define DATA_TYPE_CAN_BE_NOT_I8(type)		assert(type == 2)
#define DATA_TYPE_CAN_BE_NOT_U16(type)		assert(type == 3)
#define DATA_TYPE_CAN_BE_NOT_I16(type)		assert(type == 4)
#define DATA_TYPE_CAN_BE_NOT_U32(type)		assert(type == 5)
#define DATA_TYPE_CAN_BE_NOT_I32(type)		assert(type == 6)
#define DATA_TYPE_CAN_BE_NOT_FLOAT(type)	assert(type == 7)
#define MALLOC_CAN_RETURN_NULL(result)		assert(result != NULL)
#else /* NDEBUG */
#define VALUE_CAN_BE_NULL(expression)	do {if(expression == NULL) return OBJECT_NULL;} while(0)
#define DATA_TYPE_CAN_BE_ZERO(type)		do {if(type == 0) return INVALID_DATA_TYPE;} while(0)
#define DATA_TYPE_CAN_BE_NOT_U8(type)	do {if(type == 1) return INVALID_DATA_TYPE;} while(0)
#define DATA_TYPE_CAN_BE_NOT_I8(type)	do {if(type == 2) return INVALID_DATA_TYPE;} while(0)
#define DATA_TYPE_CAN_BE_NOT_U16(type)	do {if(type == 3) return INVALID_DATA_TYPE;} while(0)
#define DATA_TYPE_CAN_BE_NOT_I16(type)	do {if(type == 4) return INVALID_DATA_TYPE;} while(0)
#define DATA_TYPE_CAN_BE_NOT_U32(type)	do {if(type == 5) return INVALID_DATA_TYPE;} while(0)
#define DATA_TYPE_CAN_BE_NOT_I32(type)	 do {if(type == 6) return INVALID_DATA_TYPE;} while(0)
#define DATA_TYPE_CAN_BE_NOT_FLOAT(type) do {if(type == 7) return INVALID_DATA_TYPE;} while(0)
#define MALLOC_CAN_RETURN_NULL(result)	do {if(result == NULL) return ALLOCATE_ERROR;} while(0)
#endif /* NDEBUG */
/*!
 * Override function matlab_serial_init for various types
 *
 * Usage example
 *
 * 1) Init UART interface in Cube MX or CubeIDE creation wizard
 * 		UART_HandleTypeDef huart1;
 *
 * 2) Create matlab_serial_t object
 * 		matlab_serial_t object;
 *
 * 3) Create value buffer with necessary type
 * 		float value;
 *
 * 4) Call matlab_serial_init macro
 * 		matlab_serial_init(object, huart1, value, 0x3A, 0x0D0A);
 */
#define matlab_serial_init(obj, interface, pointer, start_symbol, end_symbol) _Generic((&pointer),\
			float *: matlab_serial_init_f32((matlab_serial_t *)&obj, &interface, (float *)&pointer, start_symbol, end_symbol),\
			uint8_t *: matlab_serial_init_u8((matlab_serial_t *)&obj, &interface, (uint8_t *)&pointer, start_symbol, end_symbol),\
			uint16_t *: matlab_serial_init_u16((matlab_serial_t *)&obj, &interface, (uint16_t *)&pointer, start_symbol, end_symbol),\
			uint32_t *: matlab_serial_init_u32((matlab_serial_t *)&obj, &interface, (uint32_t *)&pointer, start_symbol, end_symbol),\
			int8_t *: matlab_serial_init_i8((matlab_serial_t *)&obj, &interface, (int8_t *)&pointer, start_symbol, end_symbol),\
			int16_t *: matlab_serial_init_i16((matlab_serial_t *)&obj, &interface, (int16_t *)&pointer, start_symbol, end_symbol),\
			int32_t *: matlab_serial_init_i32((matlab_serial_t *)&obj, &interface, (int32_t *)&pointer, start_symbol, end_symbol),\
			default: END_FUNCTION_WITH_ERROR)

#define matlab_serial_attach_rx(obj, buff) matlab_serial_allocate_rx(&obj, (void*)&buff, sizeof(buff));

#define matlab_serial_attach_tx(obj, buff) matlab_serial_allocate_tx(&obj, (void *)&buff, sizeof(buff));

/*!
 * Override function matlab_serial_init_array for various types
 *
 * Usage example
 *
 * 1) Init UART interface in Cube MX or CubeIDE creation wizard
 * 		UART_HandleTypeDef huart1;
 *
 * 2) Create matlab_serial_t object
 * 		matlab_serial_t object;
 *
 * 3) Create value array buffer with necessary type
 * 		float value[3];
 *
 * 4) Call matlab_serial_init macro
 * 		matlab_serial_init_array(object, huart1, value, 0x3A, 0x0D0A);
 *
 * 	@info value parameter automatically called with array indexing
 * 		value --> &value[0]
 */
#define matlab_serial_init_array(obj, interface, pointer, start_symbol, end_symbol) _Generic((&pointer[0]),\
			float *: matlab_serial_init_f32_array((matlab_serial_t *)&obj, &interface, (float *)&pointer[0], (uint8_t)(sizeof(pointer)/4), start_symbol, end_symbol),\
			uint8_t *: matlab_serial_init_u8_array((matlab_serial_t *)&obj, &interface, (uint8_t *)&pointer[0], (uint8_t)sizeof(pointer), start_symbol, end_symbol),\
			uint16_t *: matlab_serial_init_u16_array((matlab_serial_t *)&obj, &interface, (uint16_t *)&pointer[0], ((uint8_t)sizeof(pointer)/2), start_symbol, end_symbol),\
			uint32_t *: matlab_serial_init_u32_array((matlab_serial_t *)&obj, &interface, (uint32_t *)&pointer[0], ((uint8_t)sizeof(pointer)/4), start_symbol, end_symbol),\
			int8_t *: matlab_serial_init_i8_array((matlab_serial_t *)&obj, &interface, (int8_t *)&pointer[0], (uint8_t)sizeof(pointer), start_symbol, end_symbol),\
			int16_t *: matlab_serial_init_i16_array((matlab_serial_t *)&obj, &interface, (int16_t *)&pointer[0], ((uint8_t)sizeof(pointer)/2), start_symbol, end_symbol),\
			int32_t *: matlab_serial_init_i32_array((matlab_serial_t *)&obj, &interface, (int32_t *)&pointer[0], ((uint8_t)sizeof(pointer)/4), start_symbol, end_symbol),\
			default: END_FUNCTION_WITH_ERROR)

#endif

/**
 * @brief This function send data with selected data type
 *
 * This function call one of other function:
 * 		matlab_serial_send_f32,
 * 		matlab_serial_send_u8,
 * 		matlab_serial_send_i8,
 * 		matlab_serial_send_u16,
 * 		matlab_serial_send_i16,
 * 		matlab_serial_send_u32,
 * 		matlab_serial_send_i32,
 *
 * @param object: matlab_serial_t object pointer.
 * @param timeout:  transmission timeout in milliseconds.
 *
 * @return status_t: function result
 */
status_t matlab_serial_send(
	matlab_serial_t *object,
	uint32_t timeout);

status_t matlab_serial_receive(
	matlab_serial_t *object,
	uint32_t timeout);

status_t matlab_serial_init_hil(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	void *value_pointer_tx,
	uint8_t value_size_tx,
	void *value_pointer_rx,
	uint8_t value_size_rx,
	uint8_t start_symbol,
	uint16_t end_symbol);

status_t matlab_serial_allocate_rx(
	matlab_serial_t *object,
	void *pointer,
	uint8_t buffer_size);

status_t matlab_serial_allocate_tx(
	matlab_serial_t *object,
	void *pointer,
	uint8_t buffer_size);

status_t matlab_serial_init_new(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	void *value_pointer,
	uint8_t value_size,
	uint8_t start_symbol,
	uint16_t end_symbol);

status_t matlab_serial_send_new(
	matlab_serial_t *object,
	uint32_t timeout);

/**
 * @brief float initialization function
 *
 *	Allocating memory: Yes
 *	Implementation general call: Yes
 *	Thread-safe: No
 *
 * This function initialize matlab_serial_t structure for transmitting float data
 *
 * @param object: matlab_serial_t object pointer.
 * @param interface:  USART/UART handler pointer.
 * @param value_pointer: float data pointer.
 * @param start_symbol:	Start byte message frame (from 0x00 to 0xFF).
 * @param end_symbol: End 2 bytes message frame (from 0x0000 to 0xFFFF).
 *
 * @return status_t: function result
 *
 * @info Use general function / matlab_serial_init / with float value
 * 		for transmit
 */
status_t matlab_serial_init_f32(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	float *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol);

/**
 * @brief float array initialization function
 *
 *	Allocating memory: Yes
 *	Implementation general call: Yes
 *	Thread-safe: No
 *
 * This function initialize matlab_serial_t structure for transmitting float array data
 *
 * @param object: matlab_serial_t object pointer.
 * @param interface:  USART/UART handler pointer.
 * @param array_pointer: float array data pointer.
 * @param array_size: array_size.
 * @param start_symbol:	Start byte message frame (from 0x00 to 0xFF).
 * @param end_symbol: End 2 bytes message frame (from 0x0000 to 0xFFFF).
 *
 * @return status_t: function result
 *
 * @info Use general function / matlab_serial_init / with float value
 * 		for transmit
 */
status_t matlab_serial_init_f32_array(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	float *array_pointer,
	uint8_t array_size,
	uint8_t start_symbol,
	uint16_t end_symbol);

/**
 * @brief This function send data with selected data type
 *
 *	Allocating memory: No
 *	Implementation general call: Yes
 *	Thread-safe: No
 *
 * This function using initialized matlab_serial_t object for
 * 	transmitting float value or array
 *
 * @param object: matlab_serial_t object pointer.
 * @param timeout:  transmission timeout in milliseconds.
 *
 * @return status_t: function result
 *
 * @info Use general function / matlab_serial_send / with float initialized data
 */
status_t matlab_serial_send_f32(
	matlab_serial_t *object,
	uint32_t timeout);

/**
 * @brief uint8_t initialization function
 *
 *	Allocating memory: Yes
 *	Implementation general call: Yes
 *	Thread-safe: No
 *
 * This function initialize matlab_serial_t structure for transmitting uint8_t data
 *
 * @param object: matlab_serial_t object pointer.
 * @param interface:  USART/UART handler pointer.
 * @param value_pointer: uint8_t data pointer.
 * @param start_symbol:	Start byte message frame (from 0x00 to 0xFF).
 * @param end_symbol: End 2 bytes message frame (from 0x0000 to 0xFFFF).
 *
 * @return status_t: function result
 *
 * @info Use general function / matlab_serial_init / with uint8_t value
 * 		for transmit
 */
status_t matlab_serial_init_u8(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	uint8_t *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol);

/**
 * @brief uint8_t initialization function
 *
 *	Allocating memory: Yes
 *	Implementation general call: Yes
 *	Thread-safe: No
 *
 * This function initialize matlab_serial_t structure for transmitting uint8_t data
 *
 * @param object: matlab_serial_t object pointer.
 * @param interface:  USART/UART handler pointer.
 * @param array_pointer: uint8_t data pointer.
 * @param array_size: array size.
 * @param start_symbol:	Start byte message frame (from 0x00 to 0xFF).
 * @param end_symbol: End 2 bytes message frame (from 0x0000 to 0xFFFF).
 *
 * @return status_t: function result
 *
 * @info Use general function / matlab_serial_init / with uint8_t value
 * 		for transmit
 */
status_t matlab_serial_init_u8_array(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	uint8_t *array_pointer,
	uint8_t array_size,
	uint8_t start_symbol,
	uint16_t end_symbol);

/**
 * @brief This function send data with selected data type
 *
 *	Allocating memory: No
 *	Implementation general call: Yes
 *	Thread-safe: No
 *
 * This function using initialized matlab_serial_t object for
 * 	transmitting uint8_t value or array
 *
 * @param object: matlab_serial_t object pointer.
 * @param timeout:  transmission timeout in milliseconds.
 *
 * @return status_t: function result
 *
 * @info Use general function / matlab_serial_send / with uint8_t initialized data
 */
status_t matlab_serial_send_u8(
	matlab_serial_t *object,
	uint32_t timeout);

/**
 * @brief int8_t initialization function
 *
 *	Allocating memory: Yes
 *	Implementation general call: Yes
 *	Thread-safe: No
 *
 * This function initialize matlab_serial_t structure for transmitting int8_t data
 *
 * @param object: matlab_serial_t object pointer.
 * @param interface:  USART/UART handler pointer.
 * @param value_pointer: int8_t data pointer.
 * @param start_symbol:	Start byte message frame (from 0x00 to 0xFF).
 * @param end_symbol: End 2 bytes message frame (from 0x0000 to 0xFFFF).
 *
 * @return status_t: function result
 *
 * @info Use general function / matlab_serial_init / with int8_t value
 * 		for transmit
 */
status_t matlab_serial_init_i8(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	int8_t *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol);

/**
 * @brief int8_t initialization function
 *
 *	Allocating memory: Yes
 *	Implementation general call: Yes
 *	Thread-safe: No
 *
 * This function initialize matlab_serial_t structure for transmitting int8_t data
 *
 * @param object: matlab_serial_t object pointer.
 * @param interface:  USART/UART handler pointer.
 * @param array_pointer: int8_t data pointer.
 * @param array_size: array size.
 * @param start_symbol:	Start byte message frame (from 0x00 to 0xFF).
 * @param end_symbol: End 2 bytes message frame (from 0x0000 to 0xFFFF).
 *
 * @return status_t: function result
 *
 * @info Use general function / matlab_serial_init / with int8_t value
 * 		for transmit
 */
status_t matlab_serial_init_i8_array(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	int8_t *array_pointer,
	uint8_t array_size,
	uint8_t start_symbol,
	uint16_t end_symbol);

/**
 * @brief This function send data with selected data type
 *
 *	Allocating memory: No
 *	Implementation general call: Yes
 *	Thread-safe: No
 *
 * This function using initialized matlab_serial_t object for
 * 	transmitting int8_t value or array
 *
 * @param object: matlab_serial_t object pointer.
 * @param timeout:  transmission timeout in milliseconds.
 *
 * @return status_t: function result
 *
 * @info Use general function / matlab_serial_send / with int8_t initialized data
 */
status_t matlab_serial_send_i8(
	matlab_serial_t *object,
	uint32_t timeout);

/**
 * @brief uint16_t initialization function
 *
 *	Allocating memory: Yes
 *	Implementation general call: Yes
 *	Thread-safe: No
 *
 * This function initialize matlab_serial_t structure for transmitting uint16_t data
 *
 * @param object: matlab_serial_t object pointer.
 * @param interface:  USART/UART handler pointer.
 * @param value_pointer: uint16_t data pointer.
 * @param start_symbol:	Start byte message frame (from 0x00 to 0xFF).
 * @param end_symbol: End 2 bytes message frame (from 0x0000 to 0xFFFF).
 *
 * @return status_t: function result
 *
 * @info Use general function / matlab_serial_init / with uint16_t value
 * 		for transmit
 */
status_t matlab_serial_init_u16(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	uint16_t *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol);

/**
 * @brief uint16_t initialization function
 *
 *	Allocating memory: Yes
 *	Implementation general call: Yes
 *	Thread-safe: No
 *
 * This function initialize matlab_serial_t structure for transmitting uint16_t data
 *
 * @param object: matlab_serial_t object pointer.
 * @param interface:  USART/UART handler pointer.
 * @param array_pointer: uint16_t data pointer.
 * @param array_size: array size.
 * @param start_symbol:	Start byte message frame (from 0x00 to 0xFF).
 * @param end_symbol: End 2 bytes message frame (from 0x0000 to 0xFFFF).
 *
 * @return status_t: function result
 *
 * @info Use general function / matlab_serial_init / with uint16_t value
 * 		for transmit
 */
status_t matlab_serial_init_u16_array(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	uint16_t *array_pointer,
	uint8_t array_size,
	uint8_t start_symbol,
	uint16_t end_symbol);

/**
 * @brief This function send data with selected data type
 *
 *	Allocating memory: No
 *	Implementation general call: Yes
 *	Thread-safe: No
 *
 * This function using initialized matlab_serial_t object for
 * 	transmitting uint16_t value or array
 *
 * @param object: matlab_serial_t object pointer.
 * @param timeout:  transmission timeout in milliseconds.
 *
 * @return status_t: function result
 *
 * @info Use general function / matlab_serial_send / with uint16_t initialized data
 */
status_t matlab_serial_send_u16(
	matlab_serial_t *object,
	uint32_t timeout);

/**
 * @brief int16_t initialization function
 *
 *	Allocating memory: Yes
 *	Implementation general call: Yes
 *	Thread-safe: No
 *
 * This function initialize matlab_serial_t structure for transmitting int16_t data
 *
 * @param object: matlab_serial_t object pointer.
 * @param interface:  USART/UART handler pointer.
 * @param value_pointer: int16_t data pointer.
 * @param start_symbol:	Start byte message frame (from 0x00 to 0xFF).
 * @param end_symbol: End 2 bytes message frame (from 0x0000 to 0xFFFF).
 *
 * @return status_t: function result
 *
 * @info Use general function / matlab_serial_init / with int16_t value
 * 		for transmit
 */
status_t matlab_serial_init_i16(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	int16_t *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol);

/**
 * @brief int16_t initialization function
 *
 *	Allocating memory: Yes
 *	Implementation general call: Yes
 *	Thread-safe: No
 *
 * This function initialize matlab_serial_t structure for transmitting int16_t data
 *
 * @param object: matlab_serial_t object pointer.
 * @param interface:  USART/UART handler pointer.
 * @param array_pointer: int16_t data pointer.
 * @param array_size: array size.
 * @param start_symbol:	Start byte message frame (from 0x00 to 0xFF).
 * @param end_symbol: End 2 bytes message frame (from 0x0000 to 0xFFFF).
 *
 * @return status_t: function result
 *
 * @info Use general function / matlab_serial_init / with int16_t value
 * 		for transmit
 */
status_t matlab_serial_init_i16_array(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	int16_t *array_pointer,
	uint8_t array_size,
	uint8_t start_symbol,
	uint16_t end_symbol);

/**
 * @brief This function send data with selected data type
 *
 *	Allocating memory: No
 *	Implementation general call: Yes
 *	Thread-safe: No
 *
 * This function using initialized matlab_serial_t object for
 * 	transmitting int16_t value or array
 *
 * @param object: matlab_serial_t object pointer.
 * @param timeout:  transmission timeout in milliseconds.
 *
 * @return status_t: function result
 *
 * @info Use general function / matlab_serial_send / with int16_t initialized data
 */
status_t matlab_serial_send_i16(
	matlab_serial_t *object,
	uint32_t timeout);

/**
 * @brief uint32_t initialization function
 *
 *	Allocating memory: Yes
 *	Implementation general call: Yes
 *	Thread-safe: No
 *
 * This function initialize matlab_serial_t structure for transmitting uint32_t data
 *
 * @param object: matlab_serial_t object pointer.
 * @param interface:  USART/UART handler pointer.
 * @param value_pointer: uint32_t data pointer.
 * @param start_symbol:	Start byte message frame (from 0x00 to 0xFF).
 * @param end_symbol: End 2 bytes message frame (from 0x0000 to 0xFFFF).
 *
 * @return status_t: function result
 *
 * @info Use general function / matlab_serial_init / with uint32_t value
 * 		for transmit
 */
status_t matlab_serial_init_u32(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	uint32_t *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol);

/**
 * @brief int16_t initialization function
 *
 *	Allocating memory: Yes
 *	Implementation general call: Yes
 *	Thread-safe: No
 *
 * This function initialize matlab_serial_t structure for transmitting uint32_t data
 *
 * @param object: matlab_serial_t object pointer.
 * @param interface:  USART/UART handler pointer.
 * @param array_pointer: uint32_t data pointer.
 * @param array_size: array size.
 * @param start_symbol:	Start byte message frame (from 0x00 to 0xFF).
 * @param end_symbol: End 2 bytes message frame (from 0x0000 to 0xFFFF).
 *
 * @return status_t: function result
 *
 * @info Use general function / matlab_serial_init / with uint32_t value
 * 		for transmit
 */
status_t matlab_serial_init_u32_array(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	uint32_t *array_pointer,
	uint8_t array_size,
	uint8_t start_symbol,
	uint16_t end_symbol);

/**
 * @brief This function send data with selected data type
 *
 *	Allocating memory: No
 *	Implementation general call: Yes
 *	Thread-safe: No
 *
 * This function using initialized matlab_serial_t object for
 * 	transmitting uint32_t value or array
 *
 * @param object: matlab_serial_t object pointer.
 * @param timeout:  transmission timeout in milliseconds.
 *
 * @return status_t: function result
 *
 * @info Use general function / matlab_serial_send / with uint32_t initialized data
 */
status_t matlab_serial_send_u32(
	matlab_serial_t *object,
	uint32_t timeout);

/**
 * @brief int32_t initialization function
 *
 *	Allocating memory: Yes
 *	Implementation general call: Yes
 *	Thread-safe: No
 *
 * This function initialize matlab_serial_t structure for transmitting int32_t data
 *
 * @param object: matlab_serial_t object pointer.
 * @param interface:  USART/UART handler pointer.
 * @param value_pointer: int32_t data pointer.
 * @param start_symbol:	Start byte message frame (from 0x00 to 0xFF).
 * @param end_symbol: End 2 bytes message frame (from 0x0000 to 0xFFFF).
 *
 * @return status_t: function result
 *
 * @info Use general function / matlab_serial_init / with int32_t value
 * 		for transmit
 */
status_t matlab_serial_init_i32(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	int32_t *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol);

/**
 * @brief int32_t initialization function
 *
 *	Allocating memory: Yes
 *	Implementation general call: Yes
 *	Thread-safe: No
 *
 * This function initialize matlab_serial_t structure for transmitting int32_t data
 *
 * @param object: matlab_serial_t object pointer.
 * @param interface:  USART/UART handler pointer.
 * @param array_pointer: int32_t data pointer.
 * @param array_size: array size.
 * @param start_symbol:	Start byte message frame (from 0x00 to 0xFF).
 * @param end_symbol: End 2 bytes message frame (from 0x0000 to 0xFFFF).
 *
 * @return status_t: function result
 *
 * @info Use general function / matlab_serial_init / with int32_t value
 * 		for transmit
 */
status_t matlab_serial_init_i32_array(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	int32_t *array_pointer,
	uint8_t array_size,
	uint8_t start_symbol,
	uint16_t end_symbol);

/**
 * @brief This function send data with selected data type
 *
 *	Allocating memory: No
 *	Implementation general call: Yes
 *	Thread-safe: No
 *
 * This function using initialized matlab_serial_t object for
 * 	transmitting int32_t value or array
 *
 * @param object: matlab_serial_t object pointer.
 * @param timeout:  transmission timeout in milliseconds.
 *
 * @return status_t: function result
 *
 * @info Use general function / matlab_serial_send / with int32_t initialized data
 */
status_t matlab_serial_send_i32(
	matlab_serial_t *object,
	uint32_t timeout);

/**
 * @brief This function finish working with object and free allocated memory
 *
 *	Allocating memory: No
 *	Implementation general call: No
 *	Thread-safe: No
 *
 * This function delete allocated memory and reset all object values
 *
 * @param object: matlab_serial_t object pointer.
 *
 * @return status_t: function result
 *
 */
status_t matlab_serial_finish(matlab_serial_t *object);


#endif /* USE_HAL_DRIVER */
#endif /*_MATLAB_SERIAL_H_*/
