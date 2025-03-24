#include "matlab_serial.h"

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
matlab_status_t matlab_serial_send(
	matlab_serial_t *object,
	uint32_t timeout)
{
	VALUE_CAN_BE_NULL(object);

	// insert start byte
	object->message_buffer_tx[0] = object->start_symbol_buffer_tx;

	// insert two end bytes
	object->message_buffer_tx[object->buffer_tx_size - 2] =
			object->end_symbol_buffer_tx[0];
	object->message_buffer_tx[object->buffer_tx_size - 1] =
			object->end_symbol_buffer_tx[1];

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
 * 	@result This function with HAL_UART_Receive not
 * 		working. May in next iteration, this function will be delete
 */
matlab_status_t matlab_serial_receive(
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
	if((*start_buffer_pos == object->start_symbol_buffer_tx) &&
	   ((*(end_buffer_pos - 1)) == object->end_symbol_buffer_tx[0]) &&
	   ((*end_buffer_pos) == object->end_symbol_buffer_tx[1]))
	{
		uint8_t *data_buffer_pos = object->message_buffer_rx + 1;
		// byte swap from incoming buffer to value storage
		for(int i = 0; i < (object->buffer_rx_size - 3); i++)
		{
			*(((uint8_t *)object->data_rx_pointer) + i) = data_buffer_pos[i];
		}
	}
	else return INVALID_RECEIVED_DATA_FRAME;
	return STATUS_OK;
}

matlab_status_t matlab_serial_parse_rx(matlab_serial_t *object)
{
	uint8_t *start_buffer_pos = object->message_buffer_rx;
	uint8_t *end_buffer_pos = object->message_buffer_rx + (object->buffer_rx_size-1);

	// validating buffer frame start, end symbols
	if((*start_buffer_pos == object->start_symbol_buffer_tx) &&
	   ((*(end_buffer_pos - 1)) == object->end_symbol_buffer_tx[0]) &&
	   ((*end_buffer_pos) == object->end_symbol_buffer_tx[1]))
	{
		uint8_t *data_buffer_pos = object->message_buffer_rx + 1;
		// byte swap from incoming buffer to value storage
		for(int i = 0; i < (object->buffer_rx_size - 3); i++)
		{
			*(((uint8_t *)object->data_rx_pointer) + i) = data_buffer_pos[i];
		}
	}
	else return INVALID_RECEIVED_DATA_FRAME;
	return STATUS_OK;
}


/*!
 * @status TESTING
 */
matlab_status_t matlab_serial_init_common(
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
	object->start_symbol_buffer_tx = start_symbol;
	object->end_symbol_buffer_tx[0] = (uint8_t)((end_symbol&0xFF00) >> 8);
	object->end_symbol_buffer_tx[1] = (uint8_t)(end_symbol&0xFF);

	return STATUS_OK;
}


/*!
 * @status TESTING
 */
matlab_status_t matlab_serial_init_hil_common(
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
	object->data_rx_pointer = (void *)value_pointer_rx;
	object->message_buffer_tx = msg_tx;
	object->message_buffer_rx = msg_rx;
	object->buffer_tx_size = value_size_tx + 3;
	object->buffer_rx_size = value_size_rx + 3;
	object->start_symbol_buffer_tx = start_symbol;
	object->end_symbol_buffer_tx[0] = (uint8_t)((end_symbol&0xFF00) >> 8);
	object->end_symbol_buffer_tx[1] = (uint8_t)(end_symbol&0xFF);

	if(object->interface->hdmarx != NULL)
	{
		HAL_UARTEx_ReceiveToIdle_DMA(object->interface, 
			(uint8_t *)object->message_buffer_rx, object->buffer_rx_size);
	}

	return STATUS_OK;
}

matlab_status_t matlab_serial_change_end_symbols(
	matlab_serial_t *object,
	uint16_t end_symbol_tx,
	uint16_t end_symbol_rx)
{
	VALUE_CAN_BE_NULL(object);	

	object->end_symbol_buffer_tx[0] = (uint8_t)((end_symbol_tx&0xFF00) >> 8);
	object->end_symbol_buffer_tx[1] = (uint8_t)(end_symbol_tx&0xFF);

	object->end_symbol_buffer_rx[0] = (uint8_t)((end_symbol_rx&0xFF00) >> 8);
	object->end_symbol_buffer_rx[1] = (uint8_t)(end_symbol_rx&0xFF);

	return STATUS_OK;
}

matlab_status_t matlab_serial_change_start_symbols(
	matlab_serial_t *object,
	uint8_t start_symbol_tx,
	uint8_t start_symbol_rx)
{
	VALUE_CAN_BE_NULL(object);	

	object->start_symbol_buffer_tx = start_symbol_tx;
	object->start_symbol_buffer_rx = start_symbol_rx;

	return STATUS_OK;
}

/*!
 * 	@status TESTED ON TARGET
 * 	@result COMPLETED
 */
matlab_status_t matlab_serial_finish(matlab_serial_t *object)
{
	VALUE_CAN_BE_NULL(object);

	free(object->message_buffer_rx);
	free(object->message_buffer_tx);
	object->end_symbol_buffer_tx[0] = (uint8_t)0x00;
	object->end_symbol_buffer_tx[1] = (uint8_t)0x00;
	object->end_symbol_buffer_rx[0] = (uint8_t)0x00;
	object->end_symbol_buffer_rx[1] = (uint8_t)0x00;
	object->start_symbol_buffer_tx = 0x00;
	object->start_symbol_buffer_rx = 0x00;
	object->interface = NULL;

	return STATUS_OK;
}
