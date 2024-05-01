#include "matlab_serial.h"

/*!
 * 	matlab_serial_init_f32
 */
status_t matlab_serial_init_f32(
	matlab_serial_t *object,
	float *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol)
{
	VALUE_CAN_BE_NULL(object);
	VALUE_CAN_BE_NULL(value_pointer);

	// start byte + float 4 byte + end 2 byte
	uint8_t *msg = (uint8_t *)malloc(sizeof(float) + 3);
#ifdef _STRING_H_
	memset(&msg, 0x00, sizeof(msg));
#endif
	object->message_buffer_tx = msg;
	object->start_symbol_buffer = start_symbol;
	object->end_symbol_buffer = end_symbol;
	object->data_operating_type = 7;

	return STATUS_OK;
}

status_t matlab_serial_send_f32(
	matlab_serial_t *object,
	float *data,
	uint32_t timeout)
{
	VALUE_CAN_BE_NULL(object);
	VALUE_CAN_BE_NULL(data);
	DATA_CAN_BE_NOT_FLOAT(object->data_operating_type);

#ifdef USE_HAL_DRIVER
	union {
		float data;
		uint8_t bytes[4];
	}convert_float_to_u8;

	convert_float_to_u8.data = *data;
	HAL_UART_Transmit(object->interface, (uint8_t *)convert_float_to_u8.bytes,
			sizeof(convert_float_to_u8.bytes), timeout);

#endif /* USE_HAL_DRIVER */
	return STATUS_OK;
}

status_t matlab_serial_init_u8(
	matlab_serial_t *object,
	uint8_t *value_pointer,
	uint8_t start_symbol,
	uint16_t end_symbol)
{
	VALUE_CAN_BE_NULL(object);
	VALUE_CAN_BE_NULL(value_pointer);

	// start byte + uint8_t 1 byte + end 2 byte
	uint8_t *msg = (uint8_t *)malloc(sizeof(uint8_t) + 3);
#ifdef _STRING_H_
	memset(&msg, 0x00, sizeof(msg));
#endif
	object->message_buffer_tx = msg;
	object->start_symbol_buffer = start_symbol;
	object->end_symbol_buffer = end_symbol;
	object->data_operating_type = 0;

	return STATUS_OK;
}

status_t matlab_serial_send_u8(
	matlab_serial_t *object,
	uint8_t *data,
	uint32_t timeout)
{
	VALUE_CAN_BE_NULL(object);
	VALUE_CAN_BE_NULL(data);
	DATA_CAN_BE_NOT_U8(object->data_operating_type);

#ifdef USE_HAL_DRIVER
	HAL_UART_Transmit(object->interface, (uint8_t *)data,
			sizeof(data), timeout);
#endif /* USE_HAL_DRIVER */
	return STATUS_OK;
}



status_t matlab_serial_finish(matlab_serial_t *object)
{
	VALUE_CAN_BE_NULL(object);

	free(object->message_buffer_rx);
	free(object->message_buffer_tx);
	object->end_symbol_buffer = (uint16_t)0x00;
	object->start_symbol_buffer = 0x00;
	object->interface = NULL;
	object->data_operating_type = 0;

	return STATUS_OK;
}
