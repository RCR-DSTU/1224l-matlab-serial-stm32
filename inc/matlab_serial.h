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
	/* data pointers */
	void *data_tx_pointer;
	void *data_rx_pointer;
	/*!
	 * 	@info structure lock (simple thread safe implementation)
	 * 	0 - structure open
	 * 	1 - structure lock
	 */
	unsigned int structure_lock : 1; // 0-1
	unsigned int RESERVED : 7;
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
#define MALLOC_CAN_RETURN_NULL(result)		assert(result != NULL)
#else /* NDEBUG */
#define VALUE_CAN_BE_NULL(expression)	do {if(expression == NULL) return OBJECT_NULL;} while(0)
#define DATA_TYPE_CAN_BE_ZERO(type)		do {if(type == 0) return INVALID_DATA_TYPE;} while(0)
#define MALLOC_CAN_RETURN_NULL(result)	do {if(result == NULL) return ALLOCATE_ERROR;} while(0)
#endif /* NDEBUG */

/*!
 * General function matlab_serial_init for various types
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
 * 		float value; //example #1
 * 		float value[3];  //example #2
 *
 * 4) Call matlab_serial_init macro
 * 		matlab_serial_init(object, huart1, 0x3A, 0x0D0A, value);
 */
#define matlab_serial_init(obj, interface, start_symbol, end_symbol, pointer) \
			matlab_serial_init_common((matlab_serial_t *)&obj, (UART_HandleTypeDef *)&interface, \
										(uint8_t)start_symbol, (uint16_t)end_symbol,\
										(void *)&pointer, (uint8_t)sizeof(pointer))

/*!
 * General function matlab_serial_init for various types
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
 * 		float value; //example #1
 * 		float value[3];  //example #2
 *
 * 4) Call matlab_serial_init macro
 * 		matlab_serial_init(object, huart1, value, 0x3A, 0x0D0A);
 */
#define matlab_serial_init_hil(obj, interface, start_symbol, end_symbol, pointer_tx, pointer_rx)\
		matlab_serial_init_hil_common((matlab_serial_t *)&obj, (UART_HandleTypeDef *)&interface,\
										(uint8_t) start_symbol, (uint16_t) end_symbol,\
										(void *)&pointer_tx, (uint8_t) sizeof(pointer_tx),\
										(void *)&pointer_rx, (uint8_t) sizeof(pointer_rx))


#define matlab_serial_attach_rx(obj, buff) matlab_serial_allocate_rx(&obj, (void*)&buff, sizeof(buff));

#define matlab_serial_attach_tx(obj, buff) matlab_serial_allocate_tx(&obj, (void *)&buff, sizeof(buff));

#define matlab_serial_receive(obj, timeout, cast) (void)matlab_serial_receive_value(&obj, timeout, sizeof(cast))

#define matlab_serial_receive_get_casted(obj, timeout, cast) (cast)matlab_serial_receive_value(&obj, timeout, sizeof(cast))

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

status_t matlab_serial_receive_cast_common(
	matlab_serial_t *object,
	uint32_t timeout,
	uint8_t cast_size);

status_t matlab_serial_init_hil_common(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	uint8_t start_symbol,
	uint16_t end_symbol,
	void *value_pointer_tx,
	uint8_t value_size_tx,
	void *value_pointer_rx,
	uint8_t value_size_rx);

status_t matlab_serial_allocate_rx(
	matlab_serial_t *object,
	void *pointer,
	uint8_t buffer_size);

status_t matlab_serial_allocate_tx(
	matlab_serial_t *object,
	void *pointer,
	uint8_t buffer_size);

status_t matlab_serial_init_common(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	uint8_t start_symbol,
	uint16_t end_symbol,
	void *value_pointer,
	uint8_t value_size);

status_t matlab_serial_send_new(
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
