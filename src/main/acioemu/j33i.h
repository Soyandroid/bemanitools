#ifndef AC_IO_EMU_J33I_H
#define AC_IO_EMU_J33I_H

#include <stdbool.h>
#include <stdint.h>

#include "acioemu/emu.h"

struct ac_io_emu_j33i;

struct acio_j33i_msg_hook {
    void (*autoget_start)(
        struct ac_io_emu_j33i *emu, const struct ac_io_message *req);
    void (*set_motor_value)(
        struct ac_io_emu_j33i *emu, const struct ac_io_message *req);
};

struct ac_io_emu_j33i {
    struct ac_io_emu *emu;
    struct acio_j33i_msg_hook *cmd_dispatcher;
};

void ac_io_emu_j33i_init(
    struct ac_io_emu_j33i *j33i,
    struct ac_io_emu *emu,
    struct acio_j33i_msg_hook *dispatcher);

void ac_io_emu_j33i_dispatch_request(
    struct ac_io_emu_j33i *j33i, const struct ac_io_message *req);

#endif
