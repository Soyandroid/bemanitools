#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "bemanitools/glue.h"
#include "bemanitools/sdvxio.h"

#include "cconfig/cconfig-main.h"

#include "aciodrv/device.h"
#include "aciodrv/kfca.h"

#include "sdvxio-kfca/config-kfca.h"

#define LOG_MODULE "sdvxio-kfca"

#define log_misc(...) sdvx_io_log_misc(LOG_MODULE, __VA_ARGS__)
#define log_info(...) sdvx_io_log_info(LOG_MODULE, __VA_ARGS__)
#define log_warning(...) sdvx_io_log_warning(LOG_MODULE, __VA_ARGS__)
#define log_fatal(...) sdvx_io_log_fatal(LOG_MODULE, __VA_ARGS__)

static log_formatter_t sdvx_io_log_misc;
static log_formatter_t sdvx_io_log_info;
static log_formatter_t sdvx_io_log_warning;
static log_formatter_t sdvx_io_log_fatal;

static uint16_t sdvx_io_gpio[2];
static uint8_t sdvx_io_gpio_sys;
static uint16_t sdvx_io_analog[2];

static atomic_bool running;
static atomic_bool processing_io;
static int16_t kfca_node_id;

struct ac_io_kfca_poll_out pout_staging;
struct ac_io_kfca_poll_out pout_ready;

static struct aciodrv_device_ctx *acio_device_ctx;

void sdvx_io_set_loggers(
    log_formatter_t misc,
    log_formatter_t info,
    log_formatter_t warning,
    log_formatter_t fatal)
{
    sdvx_io_log_misc = misc;
    sdvx_io_log_info = info;
    sdvx_io_log_warning = warning;
    sdvx_io_log_fatal = fatal;
}

bool sdvx_io_init(
    thread_create_t thread_create,
    thread_join_t thread_join,
    thread_destroy_t thread_destroy)
{

    struct cconfig *config;
    struct sdvxio_kfca_config_kfca config_kfca;

    config = cconfig_init();

    sdvxio_kfca_config_kfca_init(config);

    if (!cconfig_main_config_init(
            config,
            "--kfca-config",
            "sdvxio-kfca.conf",
            "--help",
            "-h",
            "sdvxio-kfca",
            CCONFIG_CMD_USAGE_OUT_STDOUT)) {
        cconfig_finit(config);
        exit(EXIT_FAILURE);
    }

    sdvxio_kfca_config_kfca_get(&config_kfca, config);

    cconfig_finit(config);

    acio_device_ctx = aciodrv_device_open(config_kfca.port, config_kfca.baud);

    if (acio_device_ctx == NULL) {
        log_info("Opening acio device on [%s] failed", config_kfca.port);
        return 0;
    }

    log_info("Opening acio device successful");

    uint8_t node_count = aciodrv_device_get_node_count(acio_device_ctx);
    log_info("Enumerated %d nodes", node_count);

    kfca_node_id = -1;

    for (uint8_t i = 0; i < node_count; i++) {
        char product[4];
        aciodrv_device_get_node_product_ident(acio_device_ctx, i, product);
        log_info(
            "> %d: %c%c%c%c\n",
            i,
            product[0],
            product[1],
            product[2],
            product[3]);

        if (!memcmp(product, "KFCA", 4)) {
            if (kfca_node_id != -1) {
                log_warning("Multiple KFCA found! Using highest node id.");
            }
            kfca_node_id = i;
        }
    }

    if (kfca_node_id != -1) {
        log_warning("Using KFCA on node: %d", kfca_node_id);

        if (!aciodrv_kfca_init(acio_device_ctx, kfca_node_id)) {
            log_warning("Unable to start KFCA on node: %d", kfca_node_id);
            return false;
        }

        running = true;
        log_warning("sdvxio-kfca now running");
    } else {
        log_warning("No KFCA device found");
    }

    return running;
}

void sdvx_io_fini(void)
{
    running = false;
    while (processing_io) {
        // avoid banging
        Sleep(1);
    }
}

void sdvx_io_set_gpio_lights(uint32_t gpio_lights)
{
    pout_staging.gpio = gpio_lights;
    pout_staging.gpio |= 1 << 0x16;

    pout_staging.gpio = ac_io_u32(pout_staging.gpio);
}

void sdvx_io_set_pwm_light(uint8_t light_no, uint8_t intensity)
{
    pout_staging.pwm[light_no] = intensity;
}

bool sdvx_io_write_output(void)
{
    memcpy(&pout_ready, &pout_staging, sizeof(struct ac_io_kfca_poll_out));
    return true;
}

bool sdvx_io_read_input(void)
{
    struct ac_io_kfca_poll_in pin;

    if (!running) {
        return false;
    }
    processing_io = true;

    if (!aciodrv_kfca_poll(acio_device_ctx, kfca_node_id, &pout_ready, &pin)) {
        return false;
    }

    processing_io = false;

    pin.adc[0] = ac_io_u16(pin.adc[0]);
    pin.adc[1] = ac_io_u16(pin.adc[1]);

    pin.gpio[0] = ac_io_u16(pin.gpio[0]);
    pin.gpio[1] = ac_io_u16(pin.gpio[1]);

    sdvx_io_gpio_sys = pin.gpio_sys;

    sdvx_io_analog[0] = (pin.adc[0] >> 6) & 0x3FF;
    sdvx_io_analog[1] = (pin.adc[1] >> 6) & 0x3FF;

    sdvx_io_gpio[0] = pin.gpio[0];
    sdvx_io_gpio[1] = pin.gpio[1];

    return true;
}

uint8_t sdvx_io_get_input_gpio_sys(void)
{
    return sdvx_io_gpio_sys;
}

uint16_t sdvx_io_get_input_gpio(uint8_t gpio_bank)
{
    if (gpio_bank > 1) {
        return 0;
    }

    return sdvx_io_gpio[gpio_bank];
}

uint16_t sdvx_io_get_spinner_pos(uint8_t spinner_no)
{
    if (spinner_no >= 2) {
        return 0;
    }
    return sdvx_io_analog[spinner_no];
}

bool sdvx_io_set_amp_volume(
    uint8_t primary,
    uint8_t headphone,
    uint8_t subwoofer)
{
    if (!running) {
        return false;
    }

    if (!aciodrv_kfca_amp(
            acio_device_ctx,
            kfca_node_id,
            primary, 96, headphone, subwoofer)) {
        return false;
    }

    return true;
}
