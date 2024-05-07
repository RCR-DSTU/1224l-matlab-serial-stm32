#include "matlab_serial.h"

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
status_t matlab_serial_send(
	matlab_serial_t *object,
	uint32_t timeout)
{
	VALUE_CAN_BE_NULL(object);
	switch(object->data_operating_type)
	{
	case 0:
		return UNDEFINED_DATA_TYPE;
	case 1:
		return matlab_serial_send_u8(object, timeout);
	case 2:
		return matlab_serial_send_i8(object, timeout);
	case 3:
		return matlab_serial_send_u16(object, timeout);
	case 4:
		return matlab_serial_send_i16(object, timeout);
	case 5:
		return matlab_serial_send_u32(object, timeout);
	case 6:
		return matlab_serial_send_i32(object, timeout);
	case 7:
		return matlab_serial_send_f32(object, timeout);

	default: return END_FUNCTION_WITH_ERROR;
	}
}


status_t matlab_serial_receive(
	matlab_serial_t *object,
	uint32_t timeout)
{
	VALUE_CAN_BE_NULL(object);


#ifdef USE_HAL_DRIVER
	HAL_UART_Receive(object->interface, (uint8_t *)object->message_buffer_rx,
			object->buffer_rx_size, timeout);
#endif /* USE_HAL_DRIVER */

	return STATUS_OK;
}

/*!
 * @status TESTING
 */
status_t matlab_serial_init_new(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	void *value_pointer,
	uint8_t value_size,
	uint8_t start_symbol,
	uint16_t end_symbol)
{
	VALUE_CAN_BE_NULL(object);
	VALUE_CAN_BE_NULL(interface);
	VALUE_CAN_BE_NULL(value_pointer);

#ifdef _STDARG_H
	uint8_t *msg = (uint8_t *)malloc(value_size + 3);
	MALLOC_CAN_RETURN_NULL(msg);
#else
	return LIB_NOT_FOUND;
#endif

#ifdef _STRING_H_
	memset(&(*msg), 0x00, (value_size + 3));
#endif
	object->interface = interface;
	object->data_pointer = (void *)value_pointer;
	object->message_buffer_tx = msg;
	object->message_buffer_rx = NULL;
	object->buffer_tx_size = value_size + 3;
	object->buffer_rx_size = 0;
	object->start_symbol_buffer = start_symbol;
	object->end_symbol_buffer[0] = (uint8_t)((end_symbol&0xFF00) >> 8);
	object->end_symbol_buffer[1] = (uint8_t)(end_symbol&0xFF);
	// legacy
//	object->data_operating_type = 7;

	return STATUS_OK;
}

status_t matlab_serial_send_new(
	matlab_serial_t *object,
	uint32_t timeout)
{
	VALUE_CAN_BE_NULL(object);
	// legacy
	//DATA_TYPE_CAN_BE_NOT_U8(object->data_operating_type);

	// insert start byte
	object->message_buffer_tx[0] = object->start_symbol_buffer;

	// insert two end bytes
	object->message_buffer_tx[object->buffer_tx_size - 2] =
			object->end_symbol_buffer[0];
	object->message_buffer_tx[object->buffer_tx_size - 1] =
			object->end_symbol_buffer[1];

	// calculating float numbers and replace to tx buffer
	for(int i = 0; i < (object->buffer_tx_size - 3); i++)
	{
		object->message_buffer_tx[i+1] = (*(((uint8_t *)object->data_pointer) + i));
	}

#ifdef USE_HAL_DRIVER
	HAL_UART_Transmit(object->interface, (uint8_t *)object->message_buffer_tx,
			object->buffer_tx_size, timeout);
#endif /* USE_HAL_DRIVER */
	return STATUS_OK;
}

status_t matlab_serial_init_hil(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	void *value_pointer_tx,
	uint8_t value_size_tx,
	void *value_pointer_rx,
	uint8_t value_size_rx,
	uint8_t start_symbol,
	uint16_t end_symbol)
{
	VALUE_CAN_BE_NULL(object);
	VALUE_CAN_BE_NULL(interface);
	VALUE_CAN_BE_NULL(value_pointer_tx);
	VALUE_CAN_BE_NULL(value_pointer_rx);

#ifdef _STDARG_H
	uint8_t *msg_tx = (uint8_t *)malloc(value_size_tx + 3);
	MALLOC_CAN_RETURN_NULL(msg_tx);

	uint8_t *msg_rx = (uint8_t *)malloc(value_size_rx + 3);
	MALLOC_CAN_RETURN_NULL(msg_rx);
#else
	return LIB_NOT_FOUND;
#endif

#ifdef _STRING_H_
	memset(&(*msg_tx), 0x00, (value_size_tx + 3));
	memset(&(*msg_rx), 0x00, (value_size_rx + 3));
#endif
	object->interface = interface;
	object->data_pointer = (void *)value_pointer_tx;
	object->message_buffer_tx = msg_tx;
	object->message_buffer_rx = msg_rx;
	object->buffer_tx_size = value_size_tx + 3;
	object->buffer_rx_size = value_size_rx + 3;
	object->start_symbol_buffer = start_symbol;
	object->end_symbol_buffer[0] = (uint8_t)((end_symbol&0xFF00) >> 8);
	object->end_symbol_buffer[1] = (uint8_t)(end_symbol&0xFF);
	// legacy
//	object->data_operating_type = 7;

	return STATUS_OK;
}

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
status_t matlab_serial_init_f32(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	float *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol)
{
	VALUE_CAN_BE_NULL(object);
	VALUE_CAN_BE_NULL(interface);
	VALUE_CAN_BE_NULL(value_pointer);

	// start byte + float 4 byte + end 2 byte
	uint8_t *msg = (uint8_t *)malloc(sizeof(float) + 3);
	MALLOC_CAN_RETURN_NULL(msg);
#ifdef _STRING_H_
	memset(&(*msg), 0x00, 7);
#endif
	object->interface = interface;
	object->data_pointer = (void *)value_pointer;
	object->message_buffer_tx = msg;
	object->message_buffer_rx = NULL;
	object->buffer_tx_size = 7;
	object->buffer_rx_size = 0;
	object->start_symbol_buffer = start_symbol;
	object->end_symbol_buffer[0] = (uint8_t)((end_symbol&0xFF00) >> 8);
	object->end_symbol_buffer[1] = (uint8_t)(end_symbol&0xFF);
	object->data_operating_type = 7;

	return STATUS_OK;
}

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
status_t matlab_serial_init_f32_array(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	float *array_pointer,
	uint8_t array_size,
	uint8_t start_symbol,
	uint16_t end_symbol)
{
	VALUE_CAN_BE_NULL(object);
	VALUE_CAN_BE_NULL(interface);
	VALUE_CAN_BE_NULL(array_pointer);

	uint8_t byte_size = 3 + (sizeof(float) * array_size);

	// start byte + float 4 byte * sizeof + end 2 byte
	uint8_t *msg = (uint8_t *)malloc(byte_size);
	MALLOC_CAN_RETURN_NULL(msg);
#ifdef _STRING_H_
	memset(&(*msg), 0x00, byte_size);
#endif
	object->interface = interface;
	object->data_pointer = (void *)array_pointer;
	object->message_buffer_tx = msg;
	object->message_buffer_rx = NULL;
	object->buffer_tx_size = byte_size;
	object->buffer_rx_size = 0;
	object->start_symbol_buffer = start_symbol;
	object->end_symbol_buffer[0] = (uint8_t)((end_symbol&0xFF00) >> 8);
	object->end_symbol_buffer[1] = (uint8_t)(end_symbol&0xFF);
	object->data_operating_type = 7;

	return STATUS_OK;
}

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
status_t matlab_serial_send_f32(
	matlab_serial_t *object,
	uint32_t timeout)
{
	VALUE_CAN_BE_NULL(object);
	DATA_TYPE_CAN_BE_NOT_FLOAT(object->data_operating_type);

	// insert start byte
	object->message_buffer_tx[0] = object->start_symbol_buffer;

	// insert two end bytes
	object->message_buffer_tx[object->buffer_tx_size - 2] =
			object->end_symbol_buffer[0];
	object->message_buffer_tx[object->buffer_tx_size - 1] =
			object->end_symbol_buffer[1];

	// calculating float numbers and replace to tx buffer
	for(int i = 0; i < ((object->buffer_tx_size - 3) / 4); i++)
	{
		union {
			float data;
			uint8_t bytes[4];
		}convert_float_to_u8;

		convert_float_to_u8.data = (*(((float *)object->data_pointer) + i));

		for(int j = 0; j < sizeof(convert_float_to_u8.bytes); j++)
		{
			object->message_buffer_tx[(i*4)+(j+1)] = convert_float_to_u8.bytes[j];
		}
	}

#ifdef USE_HAL_DRIVER
	HAL_UART_Transmit(object->interface, (uint8_t *)object->message_buffer_tx,
			object->buffer_tx_size, timeout);
#endif /* USE_HAL_DRIVER */
	return STATUS_OK;
}

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
status_t matlab_serial_init_u8(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	uint8_t *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol)
{
	VALUE_CAN_BE_NULL(object);
	VALUE_CAN_BE_NULL(interface);
	VALUE_CAN_BE_NULL(value_pointer);

	// start byte + uint8_t 1 byte + end 2 byte
	uint8_t *msg = (uint8_t *)malloc(sizeof(uint8_t) + 3);
	MALLOC_CAN_RETURN_NULL(msg);
#ifdef _STRING_H_
	memset(&(*msg), 0x00, 4);
#endif
	object->interface = interface;
	object->data_pointer = (void *)value_pointer;
	object->message_buffer_tx = msg;
	object->message_buffer_rx = NULL;
	object->buffer_tx_size = 4;
	object->buffer_rx_size = 0;
	object->start_symbol_buffer = start_symbol;
	object->end_symbol_buffer[0] = (uint8_t)((end_symbol&0xFF00) >> 8);
	object->end_symbol_buffer[1] = (uint8_t)(end_symbol&0xFF);
	object->data_operating_type = 1;

	return STATUS_OK;
}

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
status_t matlab_serial_init_u8_array(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	uint8_t *array_pointer,
	uint8_t array_size,
	uint8_t start_symbol,
	uint16_t end_symbol)
{
	VALUE_CAN_BE_NULL(object);
	VALUE_CAN_BE_NULL(interface);
	VALUE_CAN_BE_NULL(array_pointer);

	// start byte + uint8_t 1 byte * sizeof + end 2 byte
	uint8_t byte_size = 3 + (sizeof(uint8_t) * array_size);

	uint8_t *msg = (uint8_t *)malloc(byte_size);
	MALLOC_CAN_RETURN_NULL(msg);
#ifdef _STRING_H_
	memset(&(*msg), 0x00, byte_size);
#endif
	object->interface = interface;
	object->data_pointer = (void *)array_pointer;
	object->message_buffer_tx = msg;
	object->message_buffer_rx = NULL;
	object->buffer_tx_size = byte_size;
	object->buffer_rx_size = 0;
	object->start_symbol_buffer = start_symbol;
	object->end_symbol_buffer[0] = (uint8_t)((end_symbol&0xFF00) >> 8);
	object->end_symbol_buffer[1] = (uint8_t)(end_symbol&0xFF);
	object->data_operating_type = 1;

	return STATUS_OK;
}

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
status_t matlab_serial_send_u8(
	matlab_serial_t *object,
	uint32_t timeout)
{
	VALUE_CAN_BE_NULL(object);
	DATA_TYPE_CAN_BE_NOT_U8(object->data_operating_type);

	// insert start byte
	object->message_buffer_tx[0] = object->start_symbol_buffer;

	// insert two end bytes
	object->message_buffer_tx[object->buffer_tx_size - 2] =
			object->end_symbol_buffer[0];
	object->message_buffer_tx[object->buffer_tx_size - 1] =
			object->end_symbol_buffer[1];

	// calculating float numbers and replace to tx buffer
	for(int i = 0; i < (object->buffer_tx_size - 3); i++)
	{
		object->message_buffer_tx[i+1] = (*(((uint8_t *)object->data_pointer) + i));
	}

#ifdef USE_HAL_DRIVER
	HAL_UART_Transmit(object->interface, (uint8_t *)object->message_buffer_tx,
			object->buffer_tx_size, timeout);
#endif /* USE_HAL_DRIVER */
	return STATUS_OK;
}

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
status_t matlab_serial_init_i8(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	int8_t *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol)
{
	VALUE_CAN_BE_NULL(object);
	VALUE_CAN_BE_NULL(interface);
	VALUE_CAN_BE_NULL(value_pointer);

	// start byte + int8_t 1 byte + end 2 byte
	uint8_t *msg = (uint8_t *)malloc(sizeof(int8_t) + 3);
	MALLOC_CAN_RETURN_NULL(msg);
#ifdef _STRING_H_
	memset(&(*msg), 0x00, 4);
#endif
	object->interface = interface;
	object->data_pointer = (void *)value_pointer;
	object->message_buffer_tx = msg;
	object->message_buffer_rx = NULL;
	object->buffer_tx_size = 4;
	object->buffer_rx_size = 0;
	object->start_symbol_buffer = start_symbol;
	object->end_symbol_buffer[0] = (uint8_t)((end_symbol&0xFF00) >> 8);
	object->end_symbol_buffer[1] = (uint8_t)(end_symbol&0xFF);
	object->data_operating_type = 2;

	return STATUS_OK;
}

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
status_t matlab_serial_init_i8_array(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	int8_t *array_pointer,
	uint8_t array_size,
	uint8_t start_symbol,
	uint16_t end_symbol)
{
	VALUE_CAN_BE_NULL(object);
	VALUE_CAN_BE_NULL(interface);
	VALUE_CAN_BE_NULL(array_pointer);

	// start byte + int8_t 1 byte * sizeof + end 2 byte
	uint8_t byte_size = 3 + (sizeof(int8_t) * array_size);

	uint8_t *msg = (uint8_t *)malloc(byte_size);
	MALLOC_CAN_RETURN_NULL(msg);
#ifdef _STRING_H_
	memset(&(*msg), 0x00, byte_size);
#endif
	object->interface = interface;
	object->data_pointer = (void *)array_pointer;
	object->message_buffer_tx = msg;
	object->message_buffer_rx = NULL;
	object->buffer_tx_size = byte_size;
	object->buffer_rx_size = 0;
	object->start_symbol_buffer = start_symbol;
	object->end_symbol_buffer[0] = (uint8_t)((end_symbol&0xFF00) >> 8);
	object->end_symbol_buffer[1] = (uint8_t)(end_symbol&0xFF);
	object->data_operating_type = 2;

	return STATUS_OK;
}

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
status_t matlab_serial_send_i8(
	matlab_serial_t *object,
	uint32_t timeout)
{
	VALUE_CAN_BE_NULL(object);
	DATA_TYPE_CAN_BE_NOT_I8(object->data_operating_type);

	// insert start byte
	object->message_buffer_tx[0] = object->start_symbol_buffer;

	// insert two end bytes
	object->message_buffer_tx[object->buffer_tx_size - 2] =
			object->end_symbol_buffer[0];
	object->message_buffer_tx[object->buffer_tx_size - 1] =
			object->end_symbol_buffer[1];

	// calculating float numbers and replace to tx buffer
	for(int i = 0; i < (object->buffer_tx_size - 3); i++)
	{
		object->message_buffer_tx[i+1] = (*(((int8_t *)object->data_pointer) + i));
	}

#ifdef USE_HAL_DRIVER
	HAL_UART_Transmit(object->interface, (uint8_t *)object->message_buffer_tx,
			object->buffer_tx_size, timeout);
#endif /* USE_HAL_DRIVER */
	return STATUS_OK;
}

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
status_t matlab_serial_init_u16(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	uint16_t *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol)
{
	VALUE_CAN_BE_NULL(object);
	VALUE_CAN_BE_NULL(interface);
	VALUE_CAN_BE_NULL(value_pointer);

	// start byte + uint16_t 2 bytes + end 2 byte
	uint8_t *msg = (uint8_t *)malloc(sizeof(uint16_t) + 3);
	MALLOC_CAN_RETURN_NULL(msg);
#ifdef _STRING_H_
	memset(&(*msg), 0x00, 5);
#endif
	object->interface = interface;
	object->data_pointer = (void *)value_pointer;
	object->message_buffer_tx = msg;
	object->message_buffer_rx = NULL;
	object->buffer_tx_size = 5;
	object->buffer_rx_size = 0;
	object->start_symbol_buffer = start_symbol;
	object->end_symbol_buffer[0] = (uint8_t)((end_symbol&0xFF00) >> 8);
	object->end_symbol_buffer[1] = (uint8_t)(end_symbol&0xFF);
	object->data_operating_type = 3;

	return STATUS_OK;
}

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
status_t matlab_serial_init_u16_array(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	uint16_t *array_pointer,
	uint8_t array_size,
	uint8_t start_symbol,
	uint16_t end_symbol)
{
	VALUE_CAN_BE_NULL(object);
	VALUE_CAN_BE_NULL(interface);
	VALUE_CAN_BE_NULL(array_pointer);

	// start byte + uint16_t 2 bytes * sizeof + end 2 byte
	uint8_t byte_size = 3 + (sizeof(uint16_t) * array_size);

	uint8_t *msg = (uint8_t *)malloc(byte_size);
	MALLOC_CAN_RETURN_NULL(msg);
#ifdef _STRING_H_
	memset(&(*msg), 0x00, byte_size);
#endif
	object->interface = interface;
	object->data_pointer = (void *)array_pointer;
	object->message_buffer_tx = msg;
	object->message_buffer_rx = NULL;
	object->buffer_tx_size = byte_size;
	object->buffer_rx_size = 0;
	object->start_symbol_buffer = start_symbol;
	object->end_symbol_buffer[0] = (uint8_t)((end_symbol&0xFF00) >> 8);
	object->end_symbol_buffer[1] = (uint8_t)(end_symbol&0xFF);
	object->data_operating_type = 3;

	return STATUS_OK;
}

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
status_t matlab_serial_send_u16(
	matlab_serial_t *object,
	uint32_t timeout)
{
	VALUE_CAN_BE_NULL(object);
	DATA_TYPE_CAN_BE_NOT_U16(object->data_operating_type);

	// insert start byte
	object->message_buffer_tx[0] = object->start_symbol_buffer;

	// insert two end bytes
	object->message_buffer_tx[object->buffer_tx_size - 2] =
			object->end_symbol_buffer[0];
	object->message_buffer_tx[object->buffer_tx_size - 1] =
			object->end_symbol_buffer[1];

	// calculating float numbers and replace to tx buffer
	for(int i = 0; i < ((object->buffer_tx_size - 3)/2); i++)
	{
		object->message_buffer_tx[(i*2)+1] = ((*(((uint16_t *)object->data_pointer) + i))&0xFF);
		object->message_buffer_tx[(i*2)+2] = (((*(((uint16_t *)object->data_pointer) + i))&0xFF00) >> 8);
	}

#ifdef USE_HAL_DRIVER
	HAL_StatusTypeDef result = HAL_ERROR;
	result = HAL_UART_Transmit(object->interface, (uint8_t *)object->message_buffer_tx,
			object->buffer_tx_size, timeout);
#endif /* USE_HAL_DRIVER */
	return (result == HAL_OK) ? STATUS_OK : SENDING_ERROR;
}

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
status_t matlab_serial_init_i16(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	int16_t *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol)
{
	VALUE_CAN_BE_NULL(object);
	VALUE_CAN_BE_NULL(interface);
	VALUE_CAN_BE_NULL(value_pointer);

	// start byte + uint16_t 2 bytes + end 2 byte
	uint8_t *msg = (uint8_t *)malloc(sizeof(int16_t) + 3);
	MALLOC_CAN_RETURN_NULL(msg);
#ifdef _STRING_H_
	memset(&(*msg), 0x00, 5);
#endif
	object->interface = interface;
	object->data_pointer = (void *)value_pointer;
	object->message_buffer_tx = msg;
	object->message_buffer_rx = NULL;
	object->buffer_tx_size = 5;
	object->buffer_rx_size = 0;
	object->start_symbol_buffer = start_symbol;
	object->end_symbol_buffer[0] = (uint8_t)((end_symbol&0xFF00) >> 8);
	object->end_symbol_buffer[1] = (uint8_t)(end_symbol&0xFF);
	object->data_operating_type = 4;

	return STATUS_OK;
}

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
status_t matlab_serial_init_i16_array(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	int16_t *array_pointer,
	uint8_t array_size,
	uint8_t start_symbol,
	uint16_t end_symbol)
{
	VALUE_CAN_BE_NULL(object);
	VALUE_CAN_BE_NULL(interface);
	VALUE_CAN_BE_NULL(array_pointer);

	// start byte + int16_t 2 bytes * sizeof + end 2 byte
	uint8_t byte_size = 3 + (sizeof(int16_t) * array_size);

	uint8_t *msg = (uint8_t *)malloc(byte_size);
	MALLOC_CAN_RETURN_NULL(msg);
#ifdef _STRING_H_
	memset(&(*msg), 0x00, byte_size);
#endif
	object->interface = interface;
	object->data_pointer = (void *)array_pointer;
	object->message_buffer_tx = msg;
	object->message_buffer_rx = NULL;
	object->buffer_tx_size = byte_size;
	object->buffer_rx_size = 0;
	object->start_symbol_buffer = start_symbol;
	object->end_symbol_buffer[0] = (uint8_t)((end_symbol&0xFF00) >> 8);
	object->end_symbol_buffer[1] = (uint8_t)(end_symbol&0xFF);
	object->data_operating_type = 4;

	return STATUS_OK;
}

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
status_t matlab_serial_send_i16(
	matlab_serial_t *object,
	uint32_t timeout)
{
	VALUE_CAN_BE_NULL(object);
	DATA_TYPE_CAN_BE_NOT_I16(object->data_operating_type);

	// insert start byte
	object->message_buffer_tx[0] = object->start_symbol_buffer;

	// insert two end bytes
	object->message_buffer_tx[object->buffer_tx_size - 2] =
			object->end_symbol_buffer[0];
	object->message_buffer_tx[object->buffer_tx_size - 1] =
			object->end_symbol_buffer[1];

	// calculating float numbers and replace to tx buffer
	for(int i = 0; i < ((object->buffer_tx_size - 3)/2); i++)
	{
		object->message_buffer_tx[(i*2)+1] = ((*(((int16_t *)object->data_pointer) + i))&0xFF);
		object->message_buffer_tx[(i*2)+2] = (((*(((int16_t *)object->data_pointer) + i))&0xFF00) >> 8);
	}

#ifdef USE_HAL_DRIVER
	HAL_StatusTypeDef result = HAL_ERROR;
	result = HAL_UART_Transmit(object->interface, (uint8_t *)object->message_buffer_tx,
			object->buffer_tx_size, timeout);
#endif /* USE_HAL_DRIVER */
	return (result == HAL_OK) ? STATUS_OK : SENDING_ERROR;
}

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
status_t matlab_serial_init_u32(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	uint32_t *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol)
{
	VALUE_CAN_BE_NULL(object);
	VALUE_CAN_BE_NULL(interface);
	VALUE_CAN_BE_NULL(value_pointer);

	// start byte + uint32_t 4 bytes + end 2 byte
	uint8_t *msg = (uint8_t *)malloc(sizeof(uint32_t) + 3);
	MALLOC_CAN_RETURN_NULL(msg);
#ifdef _STRING_H_
	memset(&(*msg), 0x00, 7);
#endif
	object->interface = interface;
	object->data_pointer = (void *)value_pointer;
	object->message_buffer_tx = msg;
	object->message_buffer_rx = NULL;
	object->buffer_tx_size = 7;
	object->buffer_rx_size = 0;
	object->start_symbol_buffer = start_symbol;
	object->end_symbol_buffer[0] = (uint8_t)((end_symbol&0xFF00) >> 8);
	object->end_symbol_buffer[1] = (uint8_t)(end_symbol&0xFF);
	object->data_operating_type = 5;

	return STATUS_OK;
}

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
status_t matlab_serial_init_u32_array(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	uint32_t *array_pointer,
	uint8_t array_size,
	uint8_t start_symbol,
	uint16_t end_symbol)
{
	VALUE_CAN_BE_NULL(object);
	VALUE_CAN_BE_NULL(interface);
	VALUE_CAN_BE_NULL(array_pointer);

	// start byte + uint32_t 4 bytes * sizeof + end 2 byte
	uint8_t byte_size = 3 + (sizeof(uint32_t) * array_size);

	uint8_t *msg = (uint8_t *)malloc(byte_size);
	MALLOC_CAN_RETURN_NULL(msg);
#ifdef _STRING_H_
	memset(&(*msg), 0x00, byte_size);
#endif
	object->interface = interface;
	object->data_pointer = (void *)array_pointer;
	object->message_buffer_tx = msg;
	object->message_buffer_rx = NULL;
	object->buffer_tx_size = byte_size;
	object->buffer_rx_size = 0;
	object->start_symbol_buffer = start_symbol;
	object->end_symbol_buffer[0] = (uint8_t)((end_symbol&0xFF00) >> 8);
	object->end_symbol_buffer[1] = (uint8_t)(end_symbol&0xFF);
	object->data_operating_type = 5;

	return STATUS_OK;
}

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
status_t matlab_serial_send_u32(
	matlab_serial_t *object,
	uint32_t timeout)
{
	VALUE_CAN_BE_NULL(object);
	DATA_TYPE_CAN_BE_NOT_U32(object->data_operating_type);

	// insert start byte
	object->message_buffer_tx[0] = object->start_symbol_buffer;

	// insert two end bytes
	object->message_buffer_tx[object->buffer_tx_size - 2] =
			object->end_symbol_buffer[0];
	object->message_buffer_tx[object->buffer_tx_size - 1] =
			object->end_symbol_buffer[1];

	// calculating float numbers and replace to tx buffer
	for(int i = 0; i < ((object->buffer_tx_size - 3)/4); i++)
	{
		union {
			uint32_t data;
			uint8_t bytes[4];
		}convert_u32_to_u8;

		convert_u32_to_u8.data = (*(((uint32_t *)object->data_pointer) + i));

		for(int j = 0; j < sizeof(convert_u32_to_u8.bytes); j++)
		{
			object->message_buffer_tx[(i*4)+(j+1)] = convert_u32_to_u8.bytes[j];
		}
	}

#ifdef USE_HAL_DRIVER
	HAL_StatusTypeDef result = HAL_ERROR;
	result = HAL_UART_Transmit(object->interface, (uint8_t *)object->message_buffer_tx,
			object->buffer_tx_size, timeout);
#endif /* USE_HAL_DRIVER */
	return (result == HAL_OK) ? STATUS_OK : SENDING_ERROR;
}

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
status_t matlab_serial_init_i32(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	int32_t *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol)
{
	VALUE_CAN_BE_NULL(object);
	VALUE_CAN_BE_NULL(interface);
	VALUE_CAN_BE_NULL(value_pointer);

	// start byte + int32_t 4 bytes + end 2 byte
	uint8_t *msg = (uint8_t *)malloc(sizeof(int32_t) + 3);
	MALLOC_CAN_RETURN_NULL(msg);
#ifdef _STRING_H_
	memset(&(*msg), 0x00, 7);
#endif
	object->interface = interface;
	object->data_pointer = (void *)value_pointer;
	object->message_buffer_tx = msg;
	object->message_buffer_rx = NULL;
	object->buffer_tx_size = 7;
	object->buffer_rx_size = 0;
	object->start_symbol_buffer = start_symbol;
	object->end_symbol_buffer[0] = (uint8_t)((end_symbol&0xFF00) >> 8);
	object->end_symbol_buffer[1] = (uint8_t)(end_symbol&0xFF);
	object->data_operating_type = 6;

	return STATUS_OK;
}

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
status_t matlab_serial_init_i32_array(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	int32_t *array_pointer,
	uint8_t array_size,
	uint8_t start_symbol,
	uint16_t end_symbol)
{
	VALUE_CAN_BE_NULL(object);
	VALUE_CAN_BE_NULL(interface);
	VALUE_CAN_BE_NULL(array_pointer);

	// start byte + int32_t 4 bytes * sizeof + end 2 byte
	uint8_t byte_size = 3 + (sizeof(int32_t) * array_size);

	uint8_t *msg = (uint8_t *)malloc(byte_size);
	MALLOC_CAN_RETURN_NULL(msg);
#ifdef _STRING_H_
	memset(&(*msg), 0x00, byte_size);
#endif
	object->interface = interface;
	object->data_pointer = (void *)array_pointer;
	object->message_buffer_tx = msg;
	object->message_buffer_rx = NULL;
	object->buffer_tx_size = byte_size;
	object->buffer_rx_size = 0;
	object->start_symbol_buffer = start_symbol;
	object->end_symbol_buffer[0] = (uint8_t)((end_symbol&0xFF00) >> 8);
	object->end_symbol_buffer[1] = (uint8_t)(end_symbol&0xFF);
	object->data_operating_type = 6;

	return STATUS_OK;
}

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
status_t matlab_serial_send_i32(
	matlab_serial_t *object,
	uint32_t timeout)
{
	VALUE_CAN_BE_NULL(object);
	DATA_TYPE_CAN_BE_NOT_I32(object->data_operating_type);

	// insert start byte
	object->message_buffer_tx[0] = object->start_symbol_buffer;

	// insert two end bytes
	object->message_buffer_tx[object->buffer_tx_size - 2] =
			object->end_symbol_buffer[0];
	object->message_buffer_tx[object->buffer_tx_size - 1] =
			object->end_symbol_buffer[1];

	// calculating float numbers and replace to tx buffer
	for(int i = 0; i < ((object->buffer_tx_size - 3)/4); i++)
	{
		union {
			int32_t data;
			uint8_t bytes[4];
		}convert_u32_to_u8;

		convert_u32_to_u8.data = (*(((int32_t *)object->data_pointer) + i));

		for(int j = 0; j < sizeof(convert_u32_to_u8.bytes); j++)
		{
			object->message_buffer_tx[(i*4)+(j+1)] = convert_u32_to_u8.bytes[j];
		}
	}

#ifdef USE_HAL_DRIVER
	HAL_StatusTypeDef result = HAL_ERROR;
	result = HAL_UART_Transmit(object->interface, (uint8_t *)object->message_buffer_tx,
			object->buffer_tx_size, timeout);
#endif /* USE_HAL_DRIVER */
	return (result == HAL_OK) ? STATUS_OK : SENDING_ERROR;
}

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
status_t matlab_serial_finish(matlab_serial_t *object)
{
	VALUE_CAN_BE_NULL(object);

	free(object->message_buffer_rx);
	free(object->message_buffer_tx);
	object->end_symbol_buffer[0] = (uint8_t)0x00;
	object->end_symbol_buffer[1] = (uint8_t)0x00;
	object->start_symbol_buffer = 0x00;
	object->interface = NULL;
	object->data_operating_type = 0;

	return STATUS_OK;
}
