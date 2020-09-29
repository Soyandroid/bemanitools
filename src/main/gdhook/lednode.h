#ifndef GDHOOK_AC_IO_EMU_LEDNODE_H
#define GDHOOK_AC_IO_EMU_LEDNODE_H

#include <stdbool.h>
#include <stdint.h>

#include "acioemu/emu.h"

enum ac_io_lednode_cmd {
    AC_IO_LEDNODE_CMD_SEND_CUSTOM1 = 0x0130,
    AC_IO_LEDNODE_CMD_SEND_CUSTOM2 = 0x0131,
    AC_IO_LEDNODE_CMD_SEND_DIRECT = 0x0120,
};

struct ac_io_emu_lednode;

typedef void (*acio_lednode_dispatcher)(
    struct ac_io_emu_lednode *emu, const struct ac_io_message *req);

struct ac_io_emu_lednode {
    struct ac_io_emu *emu;
    acio_lednode_dispatcher light_dispatcher;
};

void ac_io_emu_lednode_init(
    struct ac_io_emu_lednode *h32b,
    struct ac_io_emu *emu,
    acio_lednode_dispatcher lights_dispatcher);

void ac_io_emu_lednode_dispatch_request(
    struct ac_io_emu_lednode *h32b, const struct ac_io_message *req);

#endif
