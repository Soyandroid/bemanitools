#include <string.h>

#include "cconfig/cconfig-util.h"

#include "gdhook/config-emu.h"

#include "util/log.h"

#define GDHOOK_CONFIG_DISABLE_GF1_EMU_KEY "emu.disable_guitar1"
#define GDHOOK_CONFIG_DISABLE_GF2_EMU_KEY "emu.disable_guitar2"
#define GDHOOK_CONFIG_DISABLE_DM_EMU_KEY "emu.disable_drum"
#define GDHOOK_CONFIG_DISABLE_EAMIO_EMU_KEY "emu.disable_eamio"

#define GDHOOK_CONFIG_DEFAULT_DISABLE_GF1_EMU_VALUE false
#define GDHOOK_CONFIG_DEFAULT_DISABLE_GF2_EMU_VALUE false
#define GDHOOK_CONFIG_DEFAULT_DISABLE_DM_EMU_VALUE false
#define GDHOOK_CONFIG_DEFAULT_DISABLE_EAMIO_EMU_VALUE false

void gdhook_config_emu_init(struct cconfig *config)
{
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

void gdhook_config_emu_get(
    struct gdhook_config_emu *config_emu, struct cconfig *config)
{
    if (!cconfig_util_get_bool(
            config,
            GDHOOK_CONFIG_DISABLE_GF1_EMU_KEY,
            &config_emu->disable_gf1_emu,
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
            &config_emu->disable_gf2_emu,
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
            &config_emu->disable_dm_emu,
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
            &config_emu->disable_eamio_emu,
            GDHOOK_CONFIG_DEFAULT_DISABLE_EAMIO_EMU_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            GDHOOK_CONFIG_DISABLE_EAMIO_EMU_KEY,
            GDHOOK_CONFIG_DEFAULT_DISABLE_EAMIO_EMU_VALUE);
    }
}