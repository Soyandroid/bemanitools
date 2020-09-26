#include <string.h>

#include "cconfig/cconfig-util.h"

#include "gdhook/config.h"

#include "util/log.h"

#define GDHOOK_CONFIG_CAB_TYPE_KEY "game.type"
#define GDHOOK_CONFIG_IS_WINDOWED_KEY "game.windowed"

#define GDHOOK_CONFIG_DEFAULT_CAB_TYPE_VALUE 0
#define GDHOOK_CONFIG_DEFAULT_IS_WINDOWED_VALUE false

void gdhook_config_init(struct cconfig *config)
{
    cconfig_util_set_int(
        config,
        GDHOOK_CONFIG_CAB_TYPE_KEY,
        GDHOOK_CONFIG_DEFAULT_CAB_TYPE_VALUE,
        "Set cab type, 0 for XG cab, 1 for SD cab, 2 for GITADORA cab");

    cconfig_util_set_bool(
        config,
        GDHOOK_CONFIG_IS_WINDOWED_KEY,
        GDHOOK_CONFIG_DEFAULT_IS_WINDOWED_VALUE,
        "Set the game windowed");

}

void gdhook_config_get(
    struct gdhook_config *config_gfx, struct cconfig *config)
{
    if (!cconfig_util_get_int(
            config,
            GDHOOK_CONFIG_CAB_TYPE_KEY,
            &config_gfx->cab_type,
            GDHOOK_CONFIG_DEFAULT_CAB_TYPE_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            GDHOOK_CONFIG_CAB_TYPE_KEY,
            GDHOOK_CONFIG_DEFAULT_CAB_TYPE_VALUE);
    }

    if (!cconfig_util_get_bool(
            config,
            GDHOOK_CONFIG_IS_WINDOWED_KEY,
            &config_gfx->is_windowed,
            GDHOOK_CONFIG_DEFAULT_IS_WINDOWED_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            GDHOOK_CONFIG_IS_WINDOWED_KEY,
            GDHOOK_CONFIG_DEFAULT_IS_WINDOWED_VALUE);
    }

}