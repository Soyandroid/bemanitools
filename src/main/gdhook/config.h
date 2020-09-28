#ifndef GDHOOK_CONFIG_H
#define GDHOOK_CONFIG_H

#include "cconfig/cconfig.h"

enum {
    GDHOOK_CONFIG_GAME_GUITAR = 0x10,
    GDHOOK_CONFIG_GAME_DRUM = 0x20,
    GDHOOK_CONFIG_CABTYPE_XG = 0x00,
    GDHOOK_CONFIG_CABTYPE_SD = 0x01,
    GDHOOK_CONFIG_CABTYPE_GD = 0x02,
};

struct gdhook_config {
    int32_t cab_type;
    bool is_windowed;
    bool disable_gf1_emu;
    bool disable_gf2_emu;
    bool disable_dm_emu;
    bool disable_eamio_emu;
    char cmdline_app[1024];
};

void gdhook_config_init(struct cconfig *config);

void gdhook_config_get(
    struct gdhook_config *config_gfx, struct cconfig *config);

#endif