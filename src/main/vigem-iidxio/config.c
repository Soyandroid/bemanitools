#include "cconfig/cconfig-main.h"
#include "cconfig/cconfig-util.h"

#include "vigem-iidxio/config.h"

#include "util/log.h"

#define VIGEM_IIDXIO_CONFIG_ENABLE_KEYLIGHT_KEY "vigem.iidxio.enable_keylight"
#define VIGEM_IIDXIO_CONFIG_RELATIVE_ANALOG_KEY "vigem.iidxio.use_relative_analog"
#define VIGEM_IIDXIO_CONFIG_ENABLE_CAB_LIGHT_SEQ_KEY "vigem.iidxio.enable_cab_light_seq"
#define VIGEM_IIDXIO_CONFIG_TEXT_16SEG_KEY "vigem.iidxio.text_16seg"

#define VIGEM_IIDXIO_CONFIG_DEFAULT_ENABLE_KEYLIGHT_VALUE true
#define VIGEM_IIDXIO_CONFIG_DEFAULT_RELATIVE_ANALOG_VALUE false
#define VIGEM_IIDXIO_CONFIG_DEFAULT_ENABLE_CAB_LIGHT_SEQ_VALUE false
#define VIGEM_IIDXIO_CONFIG_DEFAULT_TEXT_16SEG_VALUE ""

static void _vigem_iidxio_config_init(struct cconfig *config)
{
    cconfig_util_set_bool(
        config,
        VIGEM_IIDXIO_CONFIG_ENABLE_KEYLIGHT_KEY,
        VIGEM_IIDXIO_CONFIG_DEFAULT_ENABLE_KEYLIGHT_VALUE,
        "Enable input based key lighting");

    cconfig_util_set_bool(
        config,
        VIGEM_IIDXIO_CONFIG_RELATIVE_ANALOG_KEY,
        VIGEM_IIDXIO_CONFIG_DEFAULT_RELATIVE_ANALOG_VALUE,
        "Use relative mode analog mapping");

    cconfig_util_set_bool(
        config,
        VIGEM_IIDXIO_CONFIG_ENABLE_CAB_LIGHT_SEQ_KEY,
        VIGEM_IIDXIO_CONFIG_DEFAULT_ENABLE_CAB_LIGHT_SEQ_VALUE,
        "Enable built in cabinet light sequence");

    cconfig_util_set_str(
        config,
        VIGEM_IIDXIO_CONFIG_TEXT_16SEG_KEY,
        VIGEM_IIDXIO_CONFIG_DEFAULT_TEXT_16SEG_VALUE,
        "Display text on 16seg. If text exceeds 9 char display limit, it will scroll");
}

static void _vigem_iidxio_config_get(
    struct vigem_iidxio_config *vigem_config, struct cconfig *config)
{
    if (!cconfig_util_get_bool(
            config,
            VIGEM_IIDXIO_CONFIG_ENABLE_KEYLIGHT_KEY,
            &vigem_config->enable_keylight,
            VIGEM_IIDXIO_CONFIG_DEFAULT_ENABLE_KEYLIGHT_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            VIGEM_IIDXIO_CONFIG_ENABLE_KEYLIGHT_KEY,
            VIGEM_IIDXIO_CONFIG_DEFAULT_ENABLE_KEYLIGHT_VALUE);
    }

    if (!cconfig_util_get_bool(
            config,
            VIGEM_IIDXIO_CONFIG_RELATIVE_ANALOG_KEY,
            &vigem_config->relative_analog,
            VIGEM_IIDXIO_CONFIG_DEFAULT_RELATIVE_ANALOG_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            VIGEM_IIDXIO_CONFIG_RELATIVE_ANALOG_KEY,
            VIGEM_IIDXIO_CONFIG_DEFAULT_RELATIVE_ANALOG_VALUE);
    }

    if (!cconfig_util_get_bool(
            config,
            VIGEM_IIDXIO_CONFIG_ENABLE_CAB_LIGHT_SEQ_KEY,
            &vigem_config->enable_cab_light_seq,
            VIGEM_IIDXIO_CONFIG_DEFAULT_ENABLE_CAB_LIGHT_SEQ_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            VIGEM_IIDXIO_CONFIG_ENABLE_CAB_LIGHT_SEQ_KEY,
            VIGEM_IIDXIO_CONFIG_DEFAULT_ENABLE_CAB_LIGHT_SEQ_VALUE);
    }

    if (!cconfig_util_get_str(
            config,
            VIGEM_IIDXIO_CONFIG_TEXT_16SEG_KEY,
            vigem_config->text_16seg,
            sizeof(vigem_config->text_16seg),
            VIGEM_IIDXIO_CONFIG_DEFAULT_TEXT_16SEG_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%s'",
            VIGEM_IIDXIO_CONFIG_TEXT_16SEG_KEY,
            VIGEM_IIDXIO_CONFIG_DEFAULT_TEXT_16SEG_VALUE);
    }
}

bool vigem_iidxio_config_get(struct vigem_iidxio_config *config_out)
{
    struct cconfig *config;

    config = cconfig_init();

    _vigem_iidxio_config_init(config);

    if (!cconfig_main_config_init(
            config,
            "--config",
            "vigem-iidxio.conf",
            "--help",
            "-h",
            "vigem-iidxio",
            CCONFIG_CMD_USAGE_OUT_STDOUT)) {
        cconfig_finit(config);
        return false;
    }

    _vigem_iidxio_config_get(config_out, config);

    cconfig_finit(config);

    return true;
}
