#ifndef AC_IO_EMU_J32D_H
#define AC_IO_EMU_J32D_H

#include <stdbool.h>
#include <stdint.h>

#include "acioemu/emu.h"

struct ac_io_emu_j32d;

struct acio_j32d_msg_hook {
    void (*autoget_start)(
        struct ac_io_emu_j32d *emu, const struct ac_io_message *req);
};

struct ac_io_emu_j32d {
    struct ac_io_emu *emu;
    struct acio_j32d_msg_hook *cmd_dispatcher;
};

void ac_io_emu_j32d_init(
    struct ac_io_emu_j32d *j32d,
    struct ac_io_emu *emu,
    struct acio_j32d_msg_hook *dispatcher);

void ac_io_emu_j32d_dispatch_request(
    struct ac_io_emu_j32d *j32d, const struct ac_io_message *req);

#endif
