#ifndef BEMANITOOLS_GDIO_H
#define BEMANITOOLS_GDIO_H

/* IO emulation provider for gitadora. */

#include <stdbool.h>
#include <stdint.h>

#include "bemanitools/glue.h"

/* input bit mappings. For guitar unit. */
enum gd_io_guitar_bit {
    GD_IO_GUITAR_R = 0x00,
    GD_IO_GUITAR_G = 0x01,
    GD_IO_GUITAR_B = 0x02,
    GD_IO_GUITAR_Y = 0x03,
    GD_IO_GUITAR_P = 0x04,
    GD_IO_GUITAR_PICK_UP = 0x05,
    GD_IO_GUITAR_PICK_DOWN = 0x06,
    GD_IO_GUITAR_WAIL_UP = 0x07,
    GD_IO_GUITAR_WAIL_DOWN = 0x08,
    GD_IO_GUITAR_WAIL_SIDE = 0x09,
};

enum gd_io_drum_bit {
    GD_IO_DRUM_LEFT_CYMBAL = 0x00,
    GD_IO_DRUM_HIGH_HAT = 0x01,
    GD_IO_DRUM_HIGH_TOM = 0x02,
    GD_IO_DRUM_SNARE = 0x03,
    GD_IO_DRUM_LOW_TOM = 0x04,
    GD_IO_DRUM_FLOOR_TOM = 0x05,
    GD_IO_DRUM_RIGHT_CYMBAL = 0x06,
    GD_IO_DRUM_LEFT_PEDAL = 0x07,
    GD_IO_DRUM_BASS_PEDAL = 0x08,
};

/* Bit mappings for "system" inputs */
enum gd_io_sys_bit {
    GD_IO_SYS_TEST = 0x00,
    GD_IO_SYS_SERVICE = 0x01,
    GD_IO_SYS_COIN = 0x02,
};

/* Bit mappings for panel inputs */
enum gd_io_panel_bit {
    GD_IO_PANEL_START = 0x00,
    GD_IO_PANEL_UP = 0x01,
    GD_IO_PANEL_DOWN = 0x02,
    GD_IO_PANEL_LEFT = 0x03,
    GD_IO_PANEL_RIGHT = 0x04,
    GD_IO_PANEL_HELP = 0x05,
    GD_IO_PANEL_EFF1 = 0x06,
    GD_IO_PANEL_EFF2 = 0x07,
    GD_IO_PANEL_EFF3 = 0x08,
    GD_IO_PANEL_PEDAL = 0x09,
};

/* PWM LED ENUM */
enum gd_io_rgb_led {
    GD_IO_RGB_LED_GF_CAB_CENTER_LOWER_LEFT = 0,
    GD_IO_RGB_LED_GF_CAB_CENTER_LOWER_RIGHT = 1,
    GD_IO_RGB_LED_GF_SPEAKER_LEFT_UPPER = 2,
    GD_IO_RGB_LED_GF_SPEAKER_LEFT_MIDDLE_UPPER_LEFT = 3,
    GD_IO_RGB_LED_GF_SPEAKER_LEFT_MIDDLE_UPPER_RIGHT = 4,
    GD_IO_RGB_LED_GF_SPEAKER_LEFT_MIDDLE_LOWER_LEFT = 5,
    GD_IO_RGB_LED_GF_SPEAKER_LEFT_MIDDLE_LOWER_RIGHT = 6,
    GD_IO_RGB_LED_GF_SPEAKER_LEFT_LOWER = 7,
    GD_IO_RGB_LED_GF_SPEAKER_RIGHT_UPPER = 8,
    GD_IO_RGB_LED_GF_SPEAKER_RIGHT_MIDDLE_UPPER_LEFT = 9,
    GD_IO_RGB_LED_GF_SPEAKER_RIGHT_MIDDLE_UPPER_RIGHT = 10,
    GD_IO_RGB_LED_GF_SPEAKER_RIGHT_MIDDLE_LOWER_LEFT = 11,
    GD_IO_RGB_LED_GF_SPEAKER_RIGHT_MIDDLE_LOWER_RIGHT = 12,
    GD_IO_RGB_LED_GF_SPEAKER_RIGHT_LOWER = 13,
    GD_IO_RGB_LED_DM_CAB_WOOFER = 14,
    GD_IO_RGB_LED_DM_CAB_STAGE = 15,
};

enum gd_io_led {
    GD_IO_LED_GF_CAB_SPOT_LEFT = 48,
    GD_IO_LED_GF_CAB_SPOT_CENTER_LEFT = 49,
    GD_IO_LED_GF_CAB_SPOT_CENTER_RIGHT = 50,
    GD_IO_LED_GF_CAB_SPOT_RIGHT = 51,
    GD_IO_LED_DM_CAB_SPOT_FRONT_LEFT = 52,
    GD_IO_LED_DM_CAB_SPOT_FRONT_CENTER_LEFT = 53,
    GD_IO_LED_DM_CAB_SPOT_FRONT_CENTER_RIGHT = 54,
    GD_IO_LED_DM_CAB_SPOT_FRONT_RIGHT = 55,
    GD_IO_LED_DM_CAB_SPOT_BACK_LEFT = 56,
    GD_IO_LED_DM_CAB_SPOT_BACK_RIGHT = 57,
};

/* NON-PWM LED ENUM */
enum gd_io_light {
    GD_IO_LIGHT_PANEL_START_P1 = 58,
    GD_IO_LIGHT_PANEL_UD_P1 = 59,
    GD_IO_LIGHT_PANEL_LR_P1 = 60,
    GD_IO_LIGHT_PANEL_HELP_P1 = 61,
    GD_IO_LIGHT_PANEL_START_P2 = 62,
    GD_IO_LIGHT_PANEL_UD_P2 = 63,
    GD_IO_LIGHT_PANEL_LR_P2 = 64,
    GD_IO_LIGHT_PANEL_HELP_P2 = 65,
    GD_IO_LIGHT_DM_LEFT_CYMBAL = 66,
    GD_IO_LIGHT_DM_HIHAT = 67,
    GD_IO_LIGHT_DM_HIGH_TOM = 68,
    GD_IO_LIGHT_DM_SNARE = 69,
    GD_IO_LIGHT_DM_LOW_TOM = 70,
    GD_IO_LIGHT_DM_FLOOR_TOM = 71,
    GD_IO_LIGHT_DM_RIGHT_CYMBAL = 72,
};

/* The first function that will be called on your DLL. You will be supplied
   with four function pointers that may be used to log messages to the game's
   log file. See comments in glue.h for further information. */

void gd_io_set_loggers(
    log_formatter_t misc,
    log_formatter_t info,
    log_formatter_t warning,
    log_formatter_t fatal);

/* Initialize your GD IO emulation DLL. Thread management functions are
   provided to you; you must use these functions to create your own threads if
   you want to make use of the logging functions that are provided to
   gd_io_set_loggers(). You will also need to pass these thread management
   functions on to geninput if you intend to make use of that library.

   See glue.h and geninput.h for further details. */

bool gd_io_init(
    thread_create_t thread_create,
    thread_join_t thread_join,
    thread_destroy_t thread_destroy);

/* Shut down your GD IO emulation DLL */

void gd_io_fini(void);

/* TODO doc */

bool gd_io_read_inputs(void);

bool gd_io_write_outputs(void);

uint8_t gd_io_get_sys_inputs(void);

uint16_t gd_io_get_gf_panel_inputs(uint8_t player_no);

uint16_t gd_io_get_gf_guitar_inputs(uint8_t player_no);

uint8_t gd_io_get_dm_panel_inputs(void);

uint8_t gd_io_get_dm_drum_pedals(void);

int16_t gd_io_get_dm_drum_pads(uint8_t pad_type);

void gd_io_set_rgb_led(
    enum gd_io_rgb_led unit, uint8_t r, uint8_t g, uint8_t b);

void gd_io_set_led(enum gd_io_led unit, uint8_t brightness);

void gd_io_set_light(enum gd_io_light unit, uint8_t is_on);

#endif
