#include <string.h>

#include "cconfig/cconfig-util.h"

#include "gdhook/config-game.h"

#include "util/log.h"

#define GDHOOK_CONFIG_CAB_TYPE_KEY "game.type"
#define GDHOOK_CONFIG_IS_WINDOWED_KEY "game.windowed"
#define GDHOOK_CONFIG_CMDLINE_APPEND_KEY "game.cmdline"

#define GDHOOK_CONFIG_DEFAULT_CAB_TYPE_VALUE 0
#define GDHOOK_CONFIG_DEFAULT_IS_WINDOWED_VALUE false
#define GDHOOK_CONFIG_DEFAULT_CMDLINE_APPEND_VALUE ""

void gdhook_config_game_init(struct cconfig *config)
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

    cconfig_util_set_str(
        config,
        GDHOOK_CONFIG_CMDLINE_APPEND_KEY,
        GDHOOK_CONFIG_DEFAULT_CMDLINE_APPEND_VALUE,
        "Add your own cmdline string");
}

void gdhook_config_game_get(
    struct gdhook_config_game *config_game, struct cconfig *config)
{
    if (!cconfig_util_get_int(
            config,
            GDHOOK_CONFIG_CAB_TYPE_KEY,
            &config_game->cab_type,
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
            &config_game->is_windowed,
            GDHOOK_CONFIG_DEFAULT_IS_WINDOWED_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            GDHOOK_CONFIG_IS_WINDOWED_KEY,
            GDHOOK_CONFIG_DEFAULT_IS_WINDOWED_VALUE);
    }

    if (!cconfig_util_get_str(
            config,
            GDHOOK_CONFIG_CMDLINE_APPEND_KEY,
            config_game->cmdline_app,
            1024,
            GDHOOK_CONFIG_DEFAULT_CMDLINE_APPEND_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%s'",
            GDHOOK_CONFIG_CMDLINE_APPEND_KEY,
            GDHOOK_CONFIG_DEFAULT_CMDLINE_APPEND_VALUE);
    }
}