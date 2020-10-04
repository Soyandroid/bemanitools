// clang-format off
// Don't format because the order is important here
#include <windows.h>
#include <setupapi.h>
#include <stdlib.h>
// clang-format on

#include "bemanitools/jbio.h"
#include "jbio-p4io/p4io.h"

static uint16_t jb_io_panels;
static uint8_t jb_io_sys_buttons;

log_formatter_t jb_io_log_misc;
log_formatter_t jb_io_log_info;
log_formatter_t jb_io_log_warning;
log_formatter_t jb_io_log_fatal;

void jb_io_set_loggers(
    log_formatter_t misc,
    log_formatter_t info,
    log_formatter_t warning,
    log_formatter_t fatal)
{
    jb_io_log_misc = misc;
    jb_io_log_info = info;
    jb_io_log_warning = warning;
    jb_io_log_fatal = fatal;
}

bool jb_io_init(
    thread_create_t thread_create,
    thread_join_t thread_join,
    thread_destroy_t thread_destroy)
{
    if(!p4io_open_device()) {
        return false;
    }

    if(!p4io_print_version()) {
        return false;
    }

    return true;
}

void jb_io_fini(void)
{
    p4io_close_device();
}

static const uint32_t jb_io_panel_mappings[] = {
    (1 << 5),
    (1 << 1),
    (1 << 13),
    (1 << 9),
    (1 << 6),
    (1 << 2),
    (1 << 14),
    (1 << 10),
    (1 << 7),
    (1 << 3),
    (1 << 15),
    (1 << 11),
    (1 << 16),
    (1 << 4),
    (1 << 20),
    (1 << 12),
};

static const uint32_t jb_io_sys_button_mappings[] = {
    (1 << 28),
    (1 << 25),
};

bool jb_io_read_inputs(void)
{
    uint32_t jamma[4];
    if(!p4io_read_jamma2(jamma)) {
        return false;
    }

    jb_io_panels = 0;
    jb_io_sys_buttons = 0;

    // panel is active low
    uint32_t panel_in = ~jamma[0];

    for (uint8_t i = 0; i < 16; i++) {
        if (panel_in & jb_io_panel_mappings[i]) {
            jb_io_panels |= 1 << i;
        }
    }

    // sys is active high
    for (uint8_t i = 0; i < 2; i++) {
        if (jamma[0] & jb_io_sys_button_mappings[i]) {
            jb_io_sys_buttons |= 1 << i;
        }
    }

    return true;
}

bool jb_io_write_outputs(void)
{
    return true;
}

uint8_t jb_io_get_sys_inputs(void)
{
    return jb_io_sys_buttons;
}

uint16_t jb_io_get_panel_inputs(void)
{
    return jb_io_panels;
}

void jb_io_set_rgb_led(enum jb_io_rgb_led unit, uint8_t r, uint8_t g, uint8_t b)
{
}
