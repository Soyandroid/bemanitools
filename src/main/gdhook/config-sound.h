#ifndef GDHOOK_CONFIG_SOUND_H
#define GDHOOK_CONFIG_SOUND_H

#include "cconfig/cconfig.h"

struct gdhook_config_sound {
    bool is_shared_mode;
};

void gdhook_config_sound_init(struct cconfig *config);

void gdhook_config_sound_get(
    struct gdhook_config_sound *config_game, struct cconfig *config);

#endif