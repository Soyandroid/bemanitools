#ifndef D3D9EXHOOK_D3D9EX_H
#define D3D9EXHOOK_D3D9EX_H

#include <stdint.h>

#include "gdhook/config-gfx.h"

/**
 * Hook some d3d9 functions to patch gfx related stuff
 * like enabling window mode.
 */
void d3d9_hook_init(void);

/**
 * Configure this module by applying the provided config.
 *
 * @param gfx_config Config to apply.
 */
void d3d9_configure(struct gdhook_config_gfx *gfx_config);

#endif
