#include <string.h>

#include "cconfig/cconfig-util.h"

#include "gdhook/config.h"

#include "util/log.h"

#define GDHOOK_CONFIG_CAB_TYPE_KEY "game.type"
#define GDHOOK_CONFIG_IS_WINDOWED_KEY "game.windowed"
#define GDHOOK_CONFIG_CMDLINE_APPEND_KEY "game.cmdline"
#define GDHOOK_CONFIG_DISABLE_GF1_EMU_KEY "emu.disable_guitar1"
#define GDHOOK_CONFIG_DISABLE_GF2_EMU_KEY "emu.disable_guitar2"
#define GDHOOK_CONFIG_DISABLE_DM_EMU_KEY "emu.disable_drum"
#define GDHOOK_CONFIG_DISABLE_EAMIO_EMU_KEY "emu.disable_eamio"

#define GDHOOK_CONFIG_DEFAULT_CAB_TYPE_VALUE 0
#define GDHOOK_CONFIG_DEFAULT_IS_WINDOWED_VALUE false
#define GDHOOK_CONFIG_DEFAULT_CMDLINE_APPEND_VALUE ""
#define GDHOOK_CONFIG_DEFAULT_DISABLE_GF1_EMU_VALUE false
#define GDHOOK_CONFIG_DEFAULT_DISABLE_GF2_EMU_VALUE false
#define GDHOOK_CONFIG_DEFAULT_DISABLE_DM_EMU_VALUE false
#define GDHOOK_CONFIG_DEFAULT_DISABLE_EAMIO_EMU_VALUE false

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

    cconfig_util_set_str(
        config,
        GDHOOK_CONFIG_CMDLINE_APPEND_KEY,
        GDHOOK_CONFIG_DEFAULT_CMDLINE_APPEND_VALUE,
        "Add your own cmdline string");

    cconfig_util_set_bool(
        config,
        GDHOOK_CONFIG_DISABLE_GF1_EMU_KEY,
        GDHOOK_CONFIG_DEFAULT_DISABLE_GF1_EMU_VALUE,
        "Disable guitar unit 1 (COM2) emulation");

    cconfig_util_set_bool(
        config,
        GDHOOK_CONFIG_DISABLE_GF2_EMU_KEY,
        GDHOOK_CONFIG_DEFAULT_DISABLE_GF2_EMU_VALUE,
        "Disable guitar unit 2 (COM3) emulation");

    cconfig_util_set_bool(
        config,
        GDHOOK_CONFIG_DISABLE_DM_EMU_KEY,
        GDHOOK_CONFIG_DEFAULT_DISABLE_DM_EMU_VALUE,
        "Disable drum unit (COM2) emulation");

    cconfig_util_set_bool(
        config,
        GDHOOK_CONFIG_DISABLE_EAMIO_EMU_KEY,
        GDHOOK_CONFIG_DEFAULT_DISABLE_EAMIO_EMU_VALUE,
        "Disable eamio (COM4) emulation");
}

void gdhook_config_get(struct gdhook_config *config_gfx, struct cconfig *config)
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

    if (!cconfig_util_get_str(
            config,
            GDHOOK_CONFIG_CMDLINE_APPEND_KEY,
            config_gfx->cmdline_app,
            1024,
            GDHOOK_CONFIG_DEFAULT_CMDLINE_APPEND_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%s'",
            GDHOOK_CONFIG_CMDLINE_APPEND_KEY,
            GDHOOK_CONFIG_DEFAULT_CMDLINE_APPEND_VALUE);
    }

    if (!cconfig_util_get_bool(
            config,
            GDHOOK_CONFIG_DISABLE_GF1_EMU_KEY,
            &config_gfx->disable_gf1_emu,
            GDHOOK_CONFIG_DEFAULT_DISABLE_GF1_EMU_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            GDHOOK_CONFIG_DISABLE_GF1_EMU_KEY,
            GDHOOK_CONFIG_DEFAULT_DISABLE_GF1_EMU_VALUE);
    }

    if (!cconfig_util_get_bool(
            config,
            GDHOOK_CONFIG_DISABLE_GF2_EMU_KEY,
            &config_gfx->disable_gf2_emu,
            GDHOOK_CONFIG_DEFAULT_DISABLE_GF2_EMU_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            GDHOOK_CONFIG_DISABLE_GF2_EMU_KEY,
            GDHOOK_CONFIG_DEFAULT_DISABLE_GF2_EMU_VALUE);
    }

    if (!cconfig_util_get_bool(
            config,
            GDHOOK_CONFIG_DISABLE_DM_EMU_KEY,
            &config_gfx->disable_dm_emu,
            GDHOOK_CONFIG_DEFAULT_DISABLE_DM_EMU_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            GDHOOK_CONFIG_DISABLE_DM_EMU_KEY,
            GDHOOK_CONFIG_DEFAULT_DISABLE_DM_EMU_VALUE);
    }

    if (!cconfig_util_get_bool(
            config,
            GDHOOK_CONFIG_DISABLE_EAMIO_EMU_KEY,
            &config_gfx->disable_eamio_emu,
            GDHOOK_CONFIG_DEFAULT_DISABLE_EAMIO_EMU_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            GDHOOK_CONFIG_DISABLE_EAMIO_EMU_KEY,
            GDHOOK_CONFIG_DEFAULT_DISABLE_EAMIO_EMU_VALUE);
    }
}