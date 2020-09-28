/* This is the source code for the JBIO.DLL that ships with Bemanitools 5.

   If you want to add on some minor functionality like a custom RGB LED setup
   then feel free to extend this code with support for your custom device.

   If you want to make a completely custom IO board that handles all input and
   lighting then you'd be better off writing your own from scratch. Consult
   the "bemanitools" header files included by this source file for detailed
   information about the API you'll need to implement. */

// clang-format off
// Don't format because the order is important here
#include <windows.h>
#include <mmsystem.h>
// clang-format on

#include "bemanitools/input.h"
#include "bemanitools/gdio.h"

static uint16_t gd_io_guitar[2], gd_io_p4io[2];

static int16_t gd_io_drum_pads[7];

static uint8_t gd_io_sys_buttons;

static uint16_t gd_io_drum;

/* Uncomment these if you need them. */

#if 0
static log_formatter_t gd_io_log_misc;
static log_formatter_t gd_io_log_info;
static log_formatter_t gd_io_log_warning;
static log_formatter_t gd_io_log_fatal;
#endif

#ifdef ENABLE_GDIO_LEDLOG
#include <stdio.h>

static char *gd_io_get_led_enum_to_str(uint8_t enum_val)
{
    switch (enum_val) {
        case GD_IO_RGB_LED_GF_CAB_CENTER_LOWER_LEFT:
            return "GD_IO_RGB_LED_GF_CAB_CENTER_LOWER_LEFT";
        case GD_IO_RGB_LED_GF_CAB_CENTER_LOWER_RIGHT:
            return "GD_IO_RGB_LED_GF_CAB_CENTER_LOWER_RIGHT";
        case GD_IO_RGB_LED_GF_SPEAKER_LEFT_UPPER:
            return "GD_IO_RGB_LED_GF_SPEAKER_LEFT_UPPER";
        case GD_IO_RGB_LED_GF_SPEAKER_LEFT_MIDDLE_UPPER_LEFT:
            return "GD_IO_RGB_LED_GF_SPEAKER_LEFT_MIDDLE_UPPER_LEFT";
        case GD_IO_RGB_LED_GF_SPEAKER_LEFT_MIDDLE_UPPER_RIGHT:
            return "GD_IO_RGB_LED_GF_SPEAKER_LEFT_MIDDLE_UPPER_RIGHT";
        case GD_IO_RGB_LED_GF_SPEAKER_LEFT_MIDDLE_LOWER_LEFT:
            return "GD_IO_RGB_LED_GF_SPEAKER_LEFT_MIDDLE_LOWER_LEFT";
        case GD_IO_RGB_LED_GF_SPEAKER_LEFT_MIDDLE_LOWER_RIGHT:
            return "GD_IO_RGB_LED_GF_SPEAKER_LEFT_MIDDLE_LOWER_RIGHT";
        case GD_IO_RGB_LED_GF_SPEAKER_LEFT_LOWER:
            return "GD_IO_RGB_LED_GF_SPEAKER_LEFT_LOWER";
        case GD_IO_RGB_LED_GF_SPEAKER_RIGHT_UPPER:
            return "GD_IO_RGB_LED_GF_SPEAKER_RIGHT_UPPER";
        case GD_IO_RGB_LED_GF_SPEAKER_RIGHT_MIDDLE_UPPER_LEFT:
            return "GD_IO_RGB_LED_GF_SPEAKER_RIGHT_MIDDLE_UPPER_LEFT";
        case GD_IO_RGB_LED_GF_SPEAKER_RIGHT_MIDDLE_UPPER_RIGHT:
            return "GD_IO_RGB_LED_GF_SPEAKER_RIGHT_MIDDLE_UPPER_RIGHT";
        case GD_IO_RGB_LED_GF_SPEAKER_RIGHT_MIDDLE_LOWER_LEFT:
            return "GD_IO_RGB_LED_GF_SPEAKER_RIGHT_MIDDLE_LOWER_LEFT";
        case GD_IO_RGB_LED_GF_SPEAKER_RIGHT_MIDDLE_LOWER_RIGHT:
            return "GD_IO_RGB_LED_GF_SPEAKER_RIGHT_MIDDLE_LOWER_RIGHT";
        case GD_IO_RGB_LED_GF_SPEAKER_RIGHT_LOWER:
            return "GD_IO_RGB_LED_GF_SPEAKER_RIGHT_LOWER";
        case GD_IO_RGB_LED_DM_CAB_WOOFER:
            return "GD_IO_RGB_LED_DM_CAB_WOOFER";
        case GD_IO_RGB_LED_DM_CAB_STAGE:
            return "GD_IO_RGB_LED_DM_CAB_STAGE";
        case GD_IO_LED_GF_CAB_SPOT_LEFT:
            return "GD_IO_LED_GF_CAB_SPOT_LEFT";
        case GD_IO_LED_GF_CAB_SPOT_CENTER_LEFT:
            return "GD_IO_LED_GF_CAB_SPOT_CENTER_LEFT";
        case GD_IO_LED_GF_CAB_SPOT_CENTER_RIGHT:
            return "GD_IO_LED_GF_CAB_SPOT_CENTER_RIGHT";
        case GD_IO_LED_GF_CAB_SPOT_RIGHT:
            return "GD_IO_LED_GF_CAB_SPOT_RIGHT";
        case GD_IO_LED_DM_CAB_SPOT_FRONT_LEFT:
            return "GD_IO_LED_DM_CAB_SPOT_FRONT_LEFT";
        case GD_IO_LED_DM_CAB_SPOT_FRONT_CENTER_LEFT:
            return "GD_IO_LED_DM_CAB_SPOT_FRONT_CENTER_LEFT";
        case GD_IO_LED_DM_CAB_SPOT_FRONT_CENTER_RIGHT:
            return "GD_IO_LED_DM_CAB_SPOT_FRONT_CENTER_RIGHT";
        case GD_IO_LED_DM_CAB_SPOT_FRONT_RIGHT:
            return "GD_IO_LED_DM_CAB_SPOT_FRONT_RIGHT";
        case GD_IO_LED_DM_CAB_SPOT_BACK_LEFT:
            return "GD_IO_LED_DM_CAB_SPOT_BACK_LEFT";
        case GD_IO_LED_DM_CAB_SPOT_BACK_RIGHT:
            return "GD_IO_LED_DM_CAB_SPOT_BACK_RIGHT";
        case GD_IO_LIGHT_PANEL_START_P1:
            return "GD_IO_LIGHT_PANEL_START_P1";
        case GD_IO_LIGHT_PANEL_UD_P1:
            return "GD_IO_LIGHT_PANEL_UD_P1";
        case GD_IO_LIGHT_PANEL_LR_P1:
            return "GD_IO_LIGHT_PANEL_LR_P1";
        case GD_IO_LIGHT_PANEL_HELP_P1:
            return "GD_IO_LIGHT_PANEL_HELP_P1";
        case GD_IO_LIGHT_PANEL_START_P2:
            return "GD_IO_LIGHT_PANEL_START_P2";
        case GD_IO_LIGHT_PANEL_UD_P2:
            return "GD_IO_LIGHT_PANEL_UD_P2";
        case GD_IO_LIGHT_PANEL_LR_P2:
            return "GD_IO_LIGHT_PANEL_LR_P2";
        case GD_IO_LIGHT_PANEL_HELP_P2:
            return "GD_IO_LIGHT_PANEL_HELP_P2";
        case GD_IO_LIGHT_DM_LEFT_CYMBAL:
            return "GD_IO_LIGHT_DM_LEFT_CYMBAL";
        case GD_IO_LIGHT_DM_HIHAT:
            return "GD_IO_LIGHT_DM_HIHAT";
        case GD_IO_LIGHT_DM_HIGH_TOM:
            return "GD_IO_LIGHT_DM_HIGH_TOM";
        case GD_IO_LIGHT_DM_SNARE:
            return "GD_IO_LIGHT_DM_SNARE";
        case GD_IO_LIGHT_DM_LOW_TOM:
            return "GD_IO_LIGHT_DM_LOW_TOM";
        case GD_IO_LIGHT_DM_FLOOR_TOM:
            return "GD_IO_LIGHT_DM_FLOOR_TOM";
        case GD_IO_LIGHT_DM_RIGHT_CYMBAL:
            return "GD_IO_LIGHT_DM_RIGHT_CYMBAL";
        default:
            return "";
    }
}
#endif

void gd_io_set_loggers(
    log_formatter_t misc,
    log_formatter_t info,
    log_formatter_t warning,
    log_formatter_t fatal)
{
    /* Pass logger functions on to geninput so that it has somewhere to write
       its own log output. */

    input_set_loggers(misc, info, warning, fatal);

    /* Uncomment this block if you have something you'd like to log.

       You should probably return false from the appropriate function instead
       of calling the fatal logger yourself though. */

#if 0
    gd_io_log_misc = misc;
    gd_io_log_info = info;
    gd_io_log_warning = warning;
    gd_io_log_fatal = fatal;
#endif
}

#if 0
static bool gd_io_init(
    thread_create_t thread_create,
    thread_join_t thread_join,
    thread_destroy_t thread_destroy)
{
    timeBeginPeriod(1);

    input_init(thread_create, thread_join, thread_destroy);
    mapper_config_load("gd");

    /* Initialize your own IO devices here. Log something and then return
       false if the initialization fails. */
    return true;
}
#endif

bool gd_io_gf_init(
    thread_create_t thread_create,
    thread_join_t thread_join,
    thread_destroy_t thread_destroy)
{
    timeBeginPeriod(1);

    input_init(thread_create, thread_join, thread_destroy);
    mapper_config_load("gfxg");

    /* Initialize your own IO devices here. Log something and then return
       false if the initialization fails. */
    return true;
}

bool gd_io_dm_init(
    thread_create_t thread_create,
    thread_join_t thread_join,
    thread_destroy_t thread_destroy)
{
    timeBeginPeriod(1);

    input_init(thread_create, thread_join, thread_destroy);
    mapper_config_load("dmxg");

    /* Initialize your own IO devices here. Log something and then return
       false if the initialization fails. */
    return true;
}

void gd_io_fini(void)
{
    /* This function gets called as JB shuts down after an Alt-F4. Close your
       connections to your IO devices here. */

    input_fini();
    timeEndPeriod(1);
}

bool gd_io_read_p4io_inputs(void)
{
    uint64_t inputs;
    /* Sleep first: input is timestamped immediately AFTER the ioctl returns.

       Which is the right thing to do, for once. We sleep here because
       the game polls input in a tight loop. Can't complain, at there isn't
       an artificial limit on the poll frequency. */

    Sleep(1);

    /* Update python 4 input state here. */
    inputs = (uint64_t) mapper_update();

    gd_io_p4io[0] = (inputs >> 20) & 0x3FF;
    gd_io_p4io[1] = (inputs >> 30) & 0x3FF;

    gd_io_sys_buttons = (inputs >> 49) & 0x07;

    return true;
}

bool gd_io_read_dm_inputs(void)
{
    uint64_t inputs;

    Sleep(1);

    /* Update all of our input state here. */
    inputs = (uint64_t) mapper_update();

    gd_io_drum = (inputs >> 40) & 0x1FF;

    /* Setting pads, needs MIDI support for GITADORA */
    memset(gd_io_drum_pads, 0, sizeof(gd_io_drum_pads));

#if 0
    /* Use key binding for setting pads value */
    for (int pad_type = 0; pad_type < 7; pad_type++)
        gd_io_drum_pads[pad_type] |=
            gd_io_drum_pads_key & (1 << pad_type) ? 512 : 0;
#endif

    return true;
}

bool gd_io_read_gf_inputs(uint8_t player_no)
{
    uint64_t inputs;

    Sleep(1);

    /* Update all of our input state here. */

    inputs = (uint64_t) mapper_update();
    player_no = player_no & 1;

    gd_io_guitar[player_no] = (inputs >> (10 * player_no)) & 0x3FF;

    return true;
}

bool gd_io_write_outputs(void)
{
    /* The generic input stack currently initiates lighting sends and input
       reads simultaneously, though this might change later. Perform all of our
       I/O immediately before reading out the inputs so that the input state is
       as fresh as possible. */

    return true;
}

uint8_t gd_io_get_sys_inputs(void)
{
    return gd_io_sys_buttons;
}

uint16_t gd_io_get_gf_panel_inputs(uint8_t player_no)
{
    return gd_io_p4io[player_no & 1];
}

uint16_t gd_io_get_gf_guitar_inputs(uint8_t player_no)
{
    return gd_io_guitar[player_no & 1];
}

uint8_t gd_io_get_dm_panel_inputs(void)
{
    return (uint8_t)(gd_io_p4io[0] & 0x3F);
}

uint16_t gd_io_get_dm_drum_inputs(void)
{
    return gd_io_drum;
}

int16_t gd_io_get_dm_drum_pads(uint8_t pad_type)
{
    if (pad_type >= 0 && pad_type <= 6) {
        return gd_io_drum_pads[pad_type];
    }
    return 0;
}

void gd_io_set_rgb_led(enum gd_io_rgb_led unit, uint8_t r, uint8_t g, uint8_t b)
{
    if (unit >= 0 && unit <= 15) {
#ifdef ENABLE_GDIO_LEDLOG
        printf(
            "Set LED '%s': %02X %02X %02X\n",
            gd_io_get_led_enum_to_str(unit),
            r,
            g,
            b);
#endif
        mapper_write_light(unit * 3, r);
        mapper_write_light(unit * 3 + 1, g);
        mapper_write_light(unit * 3 + 2, b);
    }
}

void gd_io_set_led(enum gd_io_led unit, uint8_t brightness)
{
    if (unit >= 48 && unit <= 57) {
#ifdef ENABLE_GDIO_LEDLOG
        printf(
            "Set LED '%s': %02X\n",
            gd_io_get_led_enum_to_str(unit),
            brightness);
#endif
        mapper_write_light(unit, brightness);
    }
}

void gd_io_set_light(enum gd_io_light unit, uint8_t is_on)
{
    if (unit >= 58 && unit <= 72) {
#ifdef ENABLE_GDIO_LEDLOG
        printf(
            "Set LED '%s': %s\n",
            gd_io_get_led_enum_to_str(unit),
            is_on ? "on" : "off");
#endif
        mapper_write_light(unit, is_on ? 255 : 0);
    }
}