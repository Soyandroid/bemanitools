#include <d3d9.h>

#include <string.h>

#include "cconfig/cconfig-util.h"

#include "gdhook/config-gfx.h"

#include "util/log.h"

#define GDHOOK_CONFIG_GFX_FRAMED_KEY "gfx.framed"
#define GDHOOK_CONFIG_GFX_WINDOWED_KEY "gfx.windowed"
#define GDHOOK_CONFIG_GFX_CONFINED_KEY "gfx.confined"

#define GDHOOK_CONFIG_GFX_DEFAULT_FRAMED_VALUE false
#define GDHOOK_CONFIG_GFX_DEFAULT_WINDOWED_VALUE false
#define GDHOOK_CONFIG_GFX_DEFAULT_CONFINED_VALUE false

void gdhook_config_gfx_init(struct cconfig *config)
{
    cconfig_util_set_bool(
        config,
        GDHOOK_CONFIG_GFX_FRAMED_KEY,
        GDHOOK_CONFIG_GFX_DEFAULT_FRAMED_VALUE,
        "Run the game in a framed window (requires windowed option)");

    cconfig_util_set_bool(
        config,
        GDHOOK_CONFIG_GFX_CONFINED_KEY,
        GDHOOK_CONFIG_GFX_DEFAULT_CONFINED_VALUE,
        "Confine mouse coursor to window");
}

void gdhook_config_gfx_get(
    struct gdhook_config_gfx *config_gfx, struct cconfig *config)
{
    if (!cconfig_util_get_bool(
            config,
            GDHOOK_CONFIG_GFX_FRAMED_KEY,
            &config_gfx->framed,
            GDHOOK_CONFIG_GFX_DEFAULT_FRAMED_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            GDHOOK_CONFIG_GFX_FRAMED_KEY,
            GDHOOK_CONFIG_GFX_DEFAULT_FRAMED_VALUE);
    }

    if (!cconfig_util_get_bool(
            config,
            GDHOOK_CONFIG_GFX_CONFINED_KEY,
            &config_gfx->confined,
            GDHOOK_CONFIG_GFX_DEFAULT_CONFINED_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            GDHOOK_CONFIG_GFX_CONFINED_KEY,
            GDHOOK_CONFIG_GFX_DEFAULT_CONFINED_VALUE);
    }
}
