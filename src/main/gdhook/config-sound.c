#include <string.h>

#include "cconfig/cconfig-util.h"

#include "gdhook/config-sound.h"

#include "util/log.h"

#define GDHOOK_CONFIG_FORCE_SHARED_MODE_KEY "sound.shared_mode"

#define GDHOOK_CONFIG_DEFAULT_FORCE_SHARED_MODE_VALUE false

void gdhook_config_sound_init(struct cconfig *config)
{
    cconfig_util_set_bool(
        config,
        GDHOOK_CONFIG_FORCE_SHARED_MODE_KEY,
        GDHOOK_CONFIG_DEFAULT_FORCE_SHARED_MODE_VALUE,
        "Force the sound engine running in SHARED MODE, may increase latency");
}

void gdhook_config_sound_get(
    struct gdhook_config_sound *config_sound, struct cconfig *config)
{
    if (!cconfig_util_get_bool(
            config,
            GDHOOK_CONFIG_FORCE_SHARED_MODE_KEY,
            &config_sound->is_shared_mode,
            GDHOOK_CONFIG_DEFAULT_FORCE_SHARED_MODE_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            GDHOOK_CONFIG_FORCE_SHARED_MODE_KEY,
            GDHOOK_CONFIG_DEFAULT_FORCE_SHARED_MODE_VALUE);
    }
}