#ifndef ACIO_J32D_H
#define ACIO_J32D_H

#include <stdint.h>

enum ac_io_j32d_cmd {
    AC_IO_CMD_J32D_IO_AUTOGET_START = 0x0120,
    AC_IO_CMD_J32D_IO_AUTOGET_DATA = 0x012F,
};

#endif