#include "matlab_serial.h"

void matlab_serial_init_default(
	matlab_serial_t *object,
	float *value_pointer)
{
	VALUE_CAN_BE_NULL(object);
	VALUE_CAN_BE_NULL(value_pointer);

	// start byte + float 4 byte + end 2 byte
	uint8_t *msg = (uint8_t *)malloc(sizeof(float) + 3);
#ifdef _STRING_H_
	memset(&msg, 0x00, sizeof(msg));
#endif

	object->message_buffer_rx = msg;
	object->start_symbol_buffer = 0x3A;
	object->end_symbol_buffer = 0x0D0A;
}

void matlab_serial_init_f32(
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
	object->message_buffer_rx = msg;
	object->start_symbol_buffer = start_symbol;
	object->end_symbol_buffer = end_symbol;
}

void matlab_serial_finish(matlab_serial_t *object)
{
	VALUE_CAN_BE_NULL(object);

	free(object->message_buffer_rx);
	free(object->message_buffer_tx);
	object->end_symbol_buffer = (uint16_t)0x00;
	object->start_symbol_buffer = 0x00;
	object->interface = NULL;
}
