#include "matlab_serial.h"

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
//	case 2:
//		return matlab_serial_send_u16(object, timeout);
//	case 3:
//		return matlab_serial_send_u32(object, timeout);
	case 7:
		return matlab_serial_send_f32(object, timeout);

	default: return END_FUNCTION_WITH_ERROR;
	}
}

/*!
 * 	matlab_serial_init_f32
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
	object->data_operating_type = 7;

	return STATUS_OK;
}

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
