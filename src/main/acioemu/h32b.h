#ifndef AC_IO_EMU_H32B_H
#define AC_IO_EMU_H32B_H

#include <stdbool.h>
#include <stdint.h>

#include "acioemu/emu.h"

struct ac_io_emu_h32b;

typedef void (*acio_h32b_dispatcher)(
    struct ac_io_emu_h32b *emu, const struct ac_io_message *req);

struct ac_io_emu_h32b {
    struct ac_io_emu *emu;
    acio_h32b_dispatcher light_dispatcher;
};

void ac_io_emu_h32b_init(
    struct ac_io_emu_h32b *h32b,
    struct ac_io_emu *emu,
    acio_h32b_dispatcher lights_dispatcher);

void ac_io_emu_h32b_dispatch_request(
    struct ac_io_emu_h32b *h32b, const struct ac_io_message *req);

#endif
