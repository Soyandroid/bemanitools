#ifndef GDHOOK_WASAPI_H
#define GDHOOK_WASAPI_H

#include <stdint.h>

#include "gdhook/config-sound.h"

/**
 * Hook some wasapi functions to patch sound related stuff
 */
void wasapi_hook_init(void);

/**
 * Configure this module by applying the provided config.
 *
 * @param sound_config Config to apply.
 */
void wasapi_configure(struct gdhook_config_sound *sound_config);

#endif
