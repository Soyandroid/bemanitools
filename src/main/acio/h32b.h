#ifndef ACIO_H32B_H
#define ACIO_H32B_H

#include <stdint.h>

enum ac_io_h32b_cmd {
    AC_IO_H32B_CMD_SEND_CUSTOM1 = 0x0130,
    AC_IO_H32B_CMD_SEND_CUSTOM2 = 0x0131,
    AC_IO_H32B_CMD_SEND_DIRECT = 0x0120,
};

#endif