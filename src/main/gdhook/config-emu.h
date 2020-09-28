#ifndef GDHOOK_CONFIG_EMU_H
#define GDHOOK_CONFIG_EMU_H

#include "cconfig/cconfig.h"

struct gdhook_config_emu {
    bool disable_gf1_emu;
    bool disable_gf2_emu;
    bool disable_dm_emu;
    bool disable_eamio_emu;
};

void gdhook_config_emu_init(struct cconfig *config);

void gdhook_config_emu_get(
    struct gdhook_config_emu *config_emu, struct cconfig *config);

#endif