#include "matlab_serial.h"

static matlab_serial_t _default_matlab_serial = {
		.interface = &huart1
};


void matlab_serial_send_f32(matlab_serial_t *object, float value);

void matlab_serial_send_u32(matlab_serial_t *object, uint32_t value);
