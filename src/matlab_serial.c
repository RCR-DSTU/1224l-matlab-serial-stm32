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

	// insert start byte
	object->message_buffer_tx[0] = object->start_symbol_buffer;

	// insert two end bytes
	object->message_buffer_tx[object->buffer_tx_size - 2] =
			object->end_symbol_buffer[0];
	object->message_buffer_tx[object->buffer_tx_size - 1] =
			object->end_symbol_buffer[1];

	// fill tx buffer value from data pointer
	for(int i = 0; i < (object->buffer_tx_size - 3); i++)
	{
		object->message_buffer_tx[i+1] = (*(((uint8_t *)object->data_tx_pointer) + i));
	}

#ifdef USE_HAL_DRIVER
	HAL_UART_Transmit(object->interface, (uint8_t *)object->message_buffer_tx,
			object->buffer_tx_size, timeout);
#endif /* USE_HAL_DRIVER */
	return STATUS_OK;
}

/*!
 * 	@status TESTING
 * 	@result DEVELOPING
 */
status_t matlab_serial_receive_common(
	matlab_serial_t *object,
	uint32_t timeout)
{
	VALUE_CAN_BE_NULL(object);

#ifdef USE_HAL_DRIVER
	HAL_StatusTypeDef response = HAL_ERROR;
	response = HAL_UART_Receive(object->interface, (uint8_t *)object->message_buffer_rx,
								object->buffer_rx_size, timeout);

	if(response != HAL_OK) return RECEIVING_ERROR;
#else /* USE_HAL_DRIVER */
	// todo CMSIS
#endif /* USE_HAL_DRIVER */

	uint8_t *start_buffer_pos = object->message_buffer_rx;
	uint8_t *end_buffer_pos = object->message_buffer_rx + (object->buffer_rx_size-1);

	// validating buffer frame start, end symbols
	if((*start_buffer_pos == object->start_symbol_buffer) &&
	   ((*(end_buffer_pos - 1)) == object->end_symbol_buffer[0]) &&
	   ((*end_buffer_pos) == object->end_symbol_buffer[1]))
	{
		uint8_t *data_buffer_pos = object->message_buffer_rx + 1;
		// byte swap from incoming buffer to value storage
		for(int i = 0; i < (object->buffer_rx_size - 3); i++)
		{
			*((uint8_t *)object->data_rx_pointer++) = *data_buffer_pos++;
		}
	}
	else return INVALID_RECEIVED_DATA_FRAME;
	return STATUS_OK;
}

/*!
 * @status TESTING
 */
status_t matlab_serial_init_common(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	uint8_t start_symbol,
	uint16_t end_symbol,
	void *value_pointer,
	uint8_t value_size)
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
	object->data_tx_pointer = (void *)value_pointer;
	object->message_buffer_tx = msg;
	object->message_buffer_rx = NULL;
	object->buffer_tx_size = value_size + 3;
	object->buffer_rx_size = 0;
	object->start_symbol_buffer = start_symbol;
	object->end_symbol_buffer[0] = (uint8_t)((end_symbol&0xFF00) >> 8);
	object->end_symbol_buffer[1] = (uint8_t)(end_symbol&0xFF);

	return STATUS_OK;
}



status_t matlab_serial_init_hil_common(
	matlab_serial_t *object,
	UART_HandleTypeDef *interface,
	uint8_t start_symbol,
	uint16_t end_symbol,
	void *value_pointer_tx,
	uint8_t value_size_tx,
	void *value_pointer_rx,
	uint8_t value_size_rx)
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
	object->data_tx_pointer = (void *)value_pointer_tx;
	object->message_buffer_tx = msg_tx;
	object->message_buffer_rx = msg_rx;
	object->buffer_tx_size = value_size_tx + 3;
	object->buffer_rx_size = value_size_rx + 3;
	object->start_symbol_buffer = start_symbol;
	object->end_symbol_buffer[0] = (uint8_t)((end_symbol&0xFF00) >> 8);
	object->end_symbol_buffer[1] = (uint8_t)(end_symbol&0xFF);

	return STATUS_OK;
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

	return STATUS_OK;
}
