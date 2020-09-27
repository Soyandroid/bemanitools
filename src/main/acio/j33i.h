#ifndef ACIO_J33I_H
#define ACIO_J33I_H

#include <stdint.h>

enum ac_io_j33i_cmd {
    AC_IO_CMD_J33I_IO_AUTOGET_START = 0x0120,
    AC_IO_CMD_J33I_IO_AUTOGET_DATA = 0x012F,
    AC_IO_CMD_J33I_IO_SET_MOTOR_VALUE = 0x0130,
};

#endif