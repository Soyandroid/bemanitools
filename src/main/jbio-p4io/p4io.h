#include <stdbool.h>
#include <stdint.h>

bool p4io_open_device(void);
bool p4io_print_version(void);
bool p4io_read_jamma2(uint32_t jamma[4]);
void p4io_close_device(void);
