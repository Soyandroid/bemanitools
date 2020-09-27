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
#include "acioemu/j32d.h"

#include "bemanitools/gdio.h"

#include "hook/iohook.h"

#include "gdhook/drumunit.h"

#include "imports/avs.h"

#include "util/defs.h"
#include "util/hex.h"
#include "util/iobuf.h"
#include "util/log.h"
#include "util/str.h"

#define GDHOOK_DMIO_TO_ACIO10BITS(x) (((x >> 6) & 0x000F) | ((x & 0x3F) << 10))

static struct ac_io_emu ac_io_emu;
static struct ac_io_emu_j32d ac_io_emu_j32d;
static uint8_t ac_io_drumunit_cnt;
static int drum_unit_thread_id;

static void drumunit_autoget_start(
    struct ac_io_emu_j32d *emu, const struct ac_io_message *req);
static int drumunit_autoget_thread(void *);

static struct acio_j32d_msg_hook drum_dispatcher = {.autoget_start =
                                                        drumunit_autoget_start};

enum {
    GDHOOK_DRUMUNIT_CABTYPE_XG = 0x00,
    GDHOOK_DRUMUNIT_CABTYPE_SD = 0x01,
    GDHOOK_DRUMUNIT_CABTYPE_GD = 0x02,
};

enum ac_io_j32d_cmd {
    AC_IO_CMD_J32D_IO_AUTOGET_START = 0x0120,
    AC_IO_CMD_J32D_IO_AUTOGET_DATA = 0x012F,
};

void drumunit_init(void)
{
    drum_unit_thread_id = 0;
    ac_io_drumunit_cnt = 0;

    ac_io_emu_init(&ac_io_emu, L"COM2");

    ac_io_emu_j32d_init(&ac_io_emu_j32d, &ac_io_emu, &drum_dispatcher);

    ac_io_drumunit_cnt++;

    log_misc("DRUM UNIT initiated");
}

void drumunit_fini(void)
{
    int thread_result;

    if (drum_unit_thread_id != 0) {
        avs_thread_join(drum_unit_thread_id, &thread_result);
        avs_thread_destroy(drum_unit_thread_id);
    }

    ac_io_emu_fini(&ac_io_emu);
}

HRESULT drumunit_dispatch_irp(struct irp *irp)
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
                ac_io_emu_cmd_assign_addrs(&ac_io_emu, msg, 1);

                break;

            case 1:
                ac_io_emu_j32d_dispatch_request(&ac_io_emu_j32d, msg);

                break;

            case AC_IO_BROADCAST:
                log_warning(
                    "Broadcast(?) message on GITADORA drumunit ACIO bus?");

                break;

            default:
                log_warning(
                    "ACIO message on unhandled bus address: %d", msg->addr);

                break;
        }

        ac_io_emu_request_pop(&ac_io_emu);
    }
}

/* GITADORA DrumMania XG~ IO */
static void drumunit_autoget_start(
    struct ac_io_emu_j32d *emu, const struct ac_io_message *req)
{
    /*
        This part is borrowed from log_server
        because no other game has an acio device
        that keep sending data to acio bus
        without getting request from the game.

        We need a seperate thread to do this job,
        but seems the only thing creates a new avs thread
        is the log_server.
    */
    HANDLE ready;

    /* start the actual autoget here */
    if (drum_unit_thread_id != 0) {
        log_warning("drumunit_autoget_thread already running");
        return;
    }

    ready = CreateEvent(NULL, TRUE, FALSE, NULL);

    drum_unit_thread_id =
        avs_thread_create(drumunit_autoget_thread, ready, 16384, 0);

    if (WaitForSingleObject(ready, INFINITE)) {
        // can't do any logging here, yet.
        log_warning(
            "ERROR drumunit_autoget_start: WaitForSingleObject failed: "
            "%08x",
            (unsigned int) GetLastError());
        return;
    }

    CloseHandle(ready);

    log_misc("Started drum unit autoget thread");
}

/*
** DM ACIO:
**       bit 8|bit7|bit6|bit5|bit4|bit3|bit2|bit1
** [00]: 0    |0   |0   |0   | 512| 256| 128|  64 HIGH TOM
** [01]:    32|  16|   8|   4|   2|   1|0   |0
** [02]: 0    |0   |0   |0   | 512| 256| 128|  64 LOW TOM
** [03]:    32|  16|   8|   4|   2|   1|0   |0
** [04]: 0    |0   |0   |0   | 512| 256| 128|  64 SNARE
** [05]:    32|  16|   8|   4|   2|   1|0   |0
** [06]: 0    |0   |0   |0   | 512| 256| 128|  64 FLOOR TOM
** [07]:    32|  16|   8|   4|   2|   1|0   |0
** [08]: 0    |0   |0   |0   | 512| 256| 128|  64 LEFT CYMBAL
** [09]:    32|  16|   8|   4|   2|   1|0   |0
** [10]: 0    |0   |0   |0   | 512| 256| 128|  64 RIGHT CYMBAL
** [11]:    32|  16|   8|   4|   2|   1|0   |0
** [12]: 0    |0   |0   |0   | 512| 256| 128|  64 HIGH HAT
** [13]:    32|  16|   8|   4|   2|   1|0   |0
** [14]: 0    |0   |0   |0   | 512| 256| 128|  64 ??????
** [15]:    32|  16|   8|   4|   2|   1|0   |0
** [16]: 0    |0   |0   |0   |0   |0   |L-P |B-P
*/
static int drumunit_autoget_thread(void *dummy_ctx)
{
    uint8_t autoget_buffer[17];
    uint8_t seq_no_autoget;
    int16_t drum_pad[7];
    uint16_t drum_inputs;

    struct ac_io_message resp;

    SetEvent((HANDLE) dummy_ctx);

    memset(autoget_buffer, 0, sizeof(autoget_buffer));
    seq_no_autoget = 0;

    while (true) {

        /*
            it just runs too fast, slow it here
            real guitar io runs at about 250 ticks per second @ baudrate 115200
            so adding the sleep from gd_io_read_dm_inputs will get us to ~2ms
            delay which is pretty close to 250 ticks per second
        */
        Sleep(1);

        if (!gd_io_read_dm_inputs()) {
            log_warning("Reading drum unit inputs from gdio failed");
            return -1;
        }

        /* read inputs here */
        drum_inputs = gd_io_get_dm_drum_inputs();
        for (int pad_no = 0; pad_no < 7; pad_no++) {
            drum_pad[pad_no] = gd_io_get_dm_drum_pads(pad_no);
        }

        /* process inputs */
        for (int pad_no = 0; pad_no < 7; pad_no++) {
            /* add drum_inputs value */
            drum_pad[pad_no] |= drum_inputs & (1 << pad_no) ? 128 : 0;
        }

        /* write acio buffer */
        *(uint16_t *) (autoget_buffer + 0x00) =
            (uint16_t) GDHOOK_DMIO_TO_ACIO10BITS(drum_pad[GD_IO_DRUM_HIGH_TOM]);
        *(uint16_t *) (autoget_buffer + 0x02) =
            (uint16_t) GDHOOK_DMIO_TO_ACIO10BITS(drum_pad[GD_IO_DRUM_LOW_TOM]);
        *(uint16_t *) (autoget_buffer + 0x04) =
            (uint16_t) GDHOOK_DMIO_TO_ACIO10BITS(drum_pad[GD_IO_DRUM_SNARE]);
        *(uint16_t *) (autoget_buffer + 0x06) =
            (uint16_t) GDHOOK_DMIO_TO_ACIO10BITS(
                drum_pad[GD_IO_DRUM_FLOOR_TOM]);
        *(uint16_t *) (autoget_buffer + 0x08) =
            (uint16_t) GDHOOK_DMIO_TO_ACIO10BITS(
                drum_pad[GD_IO_DRUM_LEFT_CYMBAL]);
        *(uint16_t *) (autoget_buffer + 0x0A) =
            (uint16_t) GDHOOK_DMIO_TO_ACIO10BITS(
                drum_pad[GD_IO_DRUM_RIGHT_CYMBAL]);
        *(uint16_t *) (autoget_buffer + 0x0C) =
            (uint16_t) GDHOOK_DMIO_TO_ACIO10BITS(drum_pad[GD_IO_DRUM_HIGH_HAT]);
        autoget_buffer[0x10] =
            (drum_inputs & (1 << GD_IO_DRUM_BASS_PEDAL) ? 1 : 0) |
            (drum_inputs & (1 << GD_IO_DRUM_LEFT_PEDAL) ? 2 : 0);

        /* send to acio bus */
        resp.addr = 1 | AC_IO_RESPONSE_FLAG;
        resp.cmd.code = ac_io_u16(AC_IO_CMD_J32D_IO_AUTOGET_DATA);
        resp.cmd.seq_no = seq_no_autoget++;
        resp.cmd.nbytes = sizeof(autoget_buffer);
        memcpy(resp.cmd.raw, autoget_buffer, sizeof(autoget_buffer));

        ac_io_emu_response_push((&ac_io_emu_j32d)->emu, &resp, 0);
    }
    return 0;
}
