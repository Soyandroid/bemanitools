// clang-format off
// Don't format because the order is important here
#include <windows.h>
#include <ntdef.h>
#include <devioctl.h>
#include <ntddser.h>
// clang-format on

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>

#include "acioemu/addr.h"
#include "acioemu/emu.h"
#include "acioemu/h32b.h"

#include "bemanitools/gdio.h"

#include "hook/iohook.h"

#include "gdhook/ledunit.h"

#include "imports/avs.h"

#include "util/defs.h"
#include "util/hex.h"
#include "util/iobuf.h"
#include "util/log.h"
#include "util/str.h"

static struct ac_io_emu ac_io_emu;
static struct ac_io_emu_h32b ac_io_emu_h32b[3];
static uint8_t ac_io_ledunit_cnt;

enum {
    GDHOOK_LEDUNIT_GAME_GUITAR_XG = 0x10,
    GDHOOK_LEDUNIT_GAME_GUITAR_SD = 0x11,
    GDHOOK_LEDUNIT_GAME_GUITAR_GD = 0x12,
    GDHOOK_LEDUNIT_GAME_DRUM_XG = 0x20,
    GDHOOK_LEDUNIT_GAME_DRUM_SD = 0x21,
    GDHOOK_LEDUNIT_GAME_DRUM_GD = 0x22,
};

static void lights_dispatcher_gf_main(
    struct ac_io_emu_h32b *emu, const struct ac_io_message *req)
{
    uint8_t *output = (uint8_t *) &req->cmd.raw;

    if (req->cmd.nbytes != 24) {
        return;
    }

    gd_io_set_rgb_led(
        GD_IO_RGB_LED_GF_CAB_CENTER_LOWER_LEFT,
        output[0x00],
        output[0x01],
        output[0x02]);

    gd_io_set_rgb_led(
        GD_IO_RGB_LED_GF_CAB_CENTER_LOWER_RIGHT,
        output[0x08],
        output[0x09],
        output[0x0A]);

    gd_io_set_led(GD_IO_LED_GF_CAB_SPOT_LEFT, output[0x03]);
    gd_io_set_led(GD_IO_LED_GF_CAB_SPOT_CENTER_LEFT, output[0x07]);
    gd_io_set_led(GD_IO_LED_GF_CAB_SPOT_CENTER_RIGHT, output[0x0B]);
    gd_io_set_led(GD_IO_LED_GF_CAB_SPOT_RIGHT, output[0x0F]);
}

static void lights_dispatcher_gf_speakers(
    struct ac_io_emu_h32b *emu,
    const struct ac_io_message *req,
    bool is_right_speaker)
{
    uint8_t *output = (uint8_t *) &req->cmd.raw;
    uint8_t offset = is_right_speaker ? GD_IO_RGB_LED_GF_SPEAKER_RIGHT_UPPER -
            GD_IO_RGB_LED_GF_SPEAKER_LEFT_UPPER :
                                        0;

    if (req->cmd.nbytes != 24) {
        return;
    }

    gd_io_set_rgb_led(
        GD_IO_RGB_LED_GF_SPEAKER_LEFT_UPPER + offset,
        output[0x00],
        output[0x01],
        output[0x02]);

    gd_io_set_rgb_led(
        GD_IO_RGB_LED_GF_SPEAKER_LEFT_MIDDLE_UPPER_LEFT + offset,
        output[0x04],
        output[0x05],
        output[0x06]);

    gd_io_set_rgb_led(
        GD_IO_RGB_LED_GF_SPEAKER_LEFT_MIDDLE_UPPER_RIGHT + offset,
        output[0x08],
        output[0x09],
        output[0x0A]);

    gd_io_set_rgb_led(
        GD_IO_RGB_LED_GF_SPEAKER_LEFT_MIDDLE_LOWER_LEFT + offset,
        output[0x0C],
        output[0x0D],
        output[0x0E]);

    gd_io_set_rgb_led(
        GD_IO_RGB_LED_GF_SPEAKER_LEFT_MIDDLE_LOWER_RIGHT + offset,
        output[0x10],
        output[0x11],
        output[0x12]);

    gd_io_set_rgb_led(
        GD_IO_RGB_LED_GF_SPEAKER_LEFT_LOWER + offset,
        output[0x14],
        output[0x15],
        output[0x16]);
}

static void lights_dispatcher_gf_left_speaker(
    struct ac_io_emu_h32b *emu, const struct ac_io_message *req)
{
    lights_dispatcher_gf_speakers(emu, req, false);
}

static void lights_dispatcher_gf_right_speaker(
    struct ac_io_emu_h32b *emu, const struct ac_io_message *req)
{
    lights_dispatcher_gf_speakers(emu, req, true);
}

static void lights_dispatcher_dm_main(
    struct ac_io_emu_h32b *emu, const struct ac_io_message *req)
{
    uint8_t *output = (uint8_t *) &req->cmd.raw;

    if (req->cmd.nbytes != 24) {
        return;
    }

    gd_io_set_rgb_led(
        GD_IO_RGB_LED_DM_CAB_WOOFER, output[0x00], output[0x01], output[0x02]);

    gd_io_set_rgb_led(
        GD_IO_RGB_LED_DM_CAB_STAGE, output[0x0C], output[0x0D], output[0x0E]);

    gd_io_set_led(GD_IO_LED_DM_CAB_SPOT_FRONT_LEFT, output[0x0B]);
    gd_io_set_led(GD_IO_LED_DM_CAB_SPOT_FRONT_CENTER_LEFT, output[0x0F]);
    gd_io_set_led(GD_IO_LED_DM_CAB_SPOT_FRONT_CENTER_RIGHT, output[0x13]);
    gd_io_set_led(GD_IO_LED_DM_CAB_SPOT_FRONT_RIGHT, output[0x17]);
    gd_io_set_led(GD_IO_LED_DM_CAB_SPOT_BACK_LEFT, output[0x03]);
    gd_io_set_led(GD_IO_LED_DM_CAB_SPOT_BACK_RIGHT, output[0x07]);
}

void ledunit_init(uint8_t ledunit_type)
{
    ac_io_emu_init(&ac_io_emu, L"COM5");
    ac_io_ledunit_cnt = 0;

    switch (ledunit_type) {
        case GDHOOK_LEDUNIT_GAME_GUITAR_GD:
        case GDHOOK_LEDUNIT_GAME_DRUM_GD:
        default:
            ac_io_ledunit_cnt = 0;
            break;

        case GDHOOK_LEDUNIT_GAME_GUITAR_XG:
            ac_io_emu_h32b_init(
                &ac_io_emu_h32b[1],
                &ac_io_emu,
                lights_dispatcher_gf_left_speaker);
            ac_io_emu_h32b_init(
                &ac_io_emu_h32b[2],
                &ac_io_emu,
                lights_dispatcher_gf_right_speaker);
            ac_io_ledunit_cnt = 2;
        case GDHOOK_LEDUNIT_GAME_GUITAR_SD:
            ac_io_emu_h32b_init(
                &ac_io_emu_h32b[0], &ac_io_emu, lights_dispatcher_gf_main);
            ac_io_ledunit_cnt++;
            break;

        case GDHOOK_LEDUNIT_GAME_DRUM_XG:
        case GDHOOK_LEDUNIT_GAME_DRUM_SD:
            ac_io_emu_h32b_init(
                &ac_io_emu_h32b[0], &ac_io_emu, lights_dispatcher_dm_main);
            ac_io_ledunit_cnt++;
            break;
    }
}

void ledunit_fini(void)
{
    ac_io_emu_fini(&ac_io_emu);
}

HRESULT
ledunit_dispatch_irp(struct irp *irp)
{
    const struct ac_io_message *msg;
    HRESULT hr;

    log_assert(irp != NULL);

    if (!ac_io_emu_match_irp(&ac_io_emu, irp)) {
        return iohook_invoke_next(irp);
    }

    for (;;) {
        hr = ac_io_emu_dispatch_irp(&ac_io_emu, irp);

        if (hr != S_OK) {
            return hr;
        }

        msg = ac_io_emu_request_peek(&ac_io_emu);

        switch (msg->addr) {
            case 0:
                ac_io_emu_cmd_assign_addrs(&ac_io_emu, msg, ac_io_ledunit_cnt);

                break;

            case 1:
                ac_io_emu_h32b_dispatch_request(&ac_io_emu_h32b[0], msg);

                break;

            case 2:
                ac_io_emu_h32b_dispatch_request(&ac_io_emu_h32b[1], msg);

                break;

            case 3:
                ac_io_emu_h32b_dispatch_request(&ac_io_emu_h32b[2], msg);

                break;

            case AC_IO_BROADCAST:
                log_warning(
                    "Broadcast(?) message on GITADORA ledunit ACIO bus?");

                break;

            default:
                log_warning(
                    "ACIO message on unhandled bus address: %d", msg->addr);

                break;
        }

        ac_io_emu_request_pop(&ac_io_emu);
    }
}
