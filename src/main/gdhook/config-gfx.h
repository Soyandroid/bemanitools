#ifndef GDHOOK_CONFIG_GFX_H
#define GDHOOK_CONFIG_GFX_H

#include "cconfig/cconfig.h"

/**
 * Config struct for d3d9hook
 *
 * Note: forced_refresh_rate sets the monitor's refresh rate
 * (it does not limit FPS or anything)
 */
struct gdhook_config_gfx {
    bool framed;
    bool windowed;
    bool confined;
};

/**
 * Add the gfx config set to the cconfig object
 */
void gdhook_config_gfx_init(struct cconfig *config);

/**
 * Fill the actual config_gfx from the given config
 */
void gdhook_config_gfx_get(
    struct gdhook_config_gfx *config_gfx, struct cconfig *config);

#endif
