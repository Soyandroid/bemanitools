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
#include "acioemu/j33i.h"

#include "bemanitools/gdio.h"

#include "hook/iohook.h"

#include "gdhook/guitarunit.h"

#include "imports/avs.h"

#include "util/defs.h"
#include "util/hex.h"
#include "util/iobuf.h"
#include "util/log.h"
#include "util/str.h"

#define GDHOOK_GDIO_TO_ACIO11BITS(x) (((x >> 3) & 0xFF) | ((x & 3) << 10))

static struct ac_io_emu ac_io_emu[2];
static struct ac_io_emu_j33i ac_io_emu_guitar[2];
static uint8_t ac_io_guitarunit_flag;
static int guitar_unit_thread_id[2];

static void guitarunit1_autoget_start(
    struct ac_io_emu_j33i *emu, const struct ac_io_message *req);
static int guitarunit1_autoget_thread(void *);

static void guitarunit2_autoget_start(
    struct ac_io_emu_j33i *emu, const struct ac_io_message *req);
static int guitarunit2_autoget_thread(void *);

/*
    bemanitools don't have vibration value output or something like that
    so we just set the .set_motor_value to NULL since it's unimplemented
*/

static struct acio_j33i_msg_hook guitar_p1_dispatcher = {
    .autoget_start = guitarunit1_autoget_start, .set_motor_value = NULL};

static struct acio_j33i_msg_hook guitar_p2_dispatcher = {
    .autoget_start = guitarunit2_autoget_start, .set_motor_value = NULL};

enum ac_io_j33i_cmd {
    AC_IO_CMD_J33I_IO_AUTOGET_START = 0x0120,
    AC_IO_CMD_J33I_IO_AUTOGET_DATA = 0x012F,
    AC_IO_CMD_J33I_IO_SET_MOTOR_VALUE = 0x0130,
};

static void guitarunit_init_unit_no(uint8_t unit_no)
{
    unit_no &= 1;

    ac_io_emu_init(&ac_io_emu[unit_no], unit_no == 0 ? L"COM2" : L"COM3");

    ac_io_emu_j33i_init(
        &ac_io_emu_guitar[unit_no],
        &ac_io_emu[unit_no],
        unit_no == 0 ? &guitar_p1_dispatcher : &guitar_p2_dispatcher);

    ac_io_guitarunit_flag |= 1 << unit_no;
}

void guitarunit_init(uint8_t guitarunit_type)
{
    memset(guitar_unit_thread_id, 0, sizeof(guitar_unit_thread_id));
    ac_io_guitarunit_flag = 0;

    if (guitarunit_type & GDHOOK_GUITARUNIT_ENABLE_GUITAR_UNIT1) {
        guitarunit_init_unit_no(0);
        log_misc("GUITAR UNIT 0 initiated");
    }

    if (guitarunit_type & GDHOOK_GUITARUNIT_ENABLE_GUITAR_UNIT2) {
        guitarunit_init_unit_no(1);
        log_misc("GUITAR UNIT 1 initiated");
    }
}

void guitarunit_fini(void)
{
    int thread_result;

    for (int unit_no = 0; unit_no < lengthof(guitar_unit_thread_id);
         unit_no++) {
        if (guitar_unit_thread_id[unit_no] != 0) {
            avs_thread_join(guitar_unit_thread_id[unit_no], &thread_result);
            avs_thread_destroy(guitar_unit_thread_id[unit_no]);
        }
    }

    for (int unit_no = 0; unit_no < 2; unit_no++) {
        if (ac_io_guitarunit_flag & (1 << unit_no)) {
            ac_io_emu_fini(&ac_io_emu[unit_no]);
        }
    }
}

static HRESULT guitarunit_dispatch_irp(struct irp *irp, uint8_t unit_no)
{
    const struct ac_io_message *msg;
    HRESULT hr;

    log_assert(irp != NULL);

    if (!ac_io_emu_match_irp(&ac_io_emu[unit_no], irp)) {
        return iohook_invoke_next(irp);
    }

    for (;;) {
        hr = ac_io_emu_dispatch_irp(&ac_io_emu[unit_no], irp);

        if (hr != S_OK) {
            return hr;
        }

        msg = ac_io_emu_request_peek(&ac_io_emu[unit_no]);

        switch (msg->addr) {
            case 0:
                ac_io_emu_cmd_assign_addrs(&ac_io_emu[unit_no], msg, 1);

                break;

            case 1:
                ac_io_emu_j33i_dispatch_request(&ac_io_emu_guitar[unit_no], msg);

                break;

            case AC_IO_BROADCAST:
                log_warning(
                    "Broadcast(?) message on GITADORA guitarunit ACIO bus?");

                break;

            default:
                log_warning(
                    "ACIO message on unhandled bus address: %d", msg->addr);

                break;
        }

        ac_io_emu_request_pop(&ac_io_emu[unit_no]);
    }
}

HRESULT
guitarunit1_dispatch_irp(struct irp *irp)
{
    return guitarunit_dispatch_irp(irp, 0);
}

HRESULT
guitarunit2_dispatch_irp(struct irp *irp)
{
    return guitarunit_dispatch_irp(irp, 1);
}

/* GITADORA GuitarFreaks XG~ IO */
static void guitarunit_autoget_start(
    struct ac_io_emu_j33i *emu,
    const struct ac_io_message *req,
    uint8_t unit_no)
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
    unit_no &= 1;
    if (guitar_unit_thread_id[unit_no] != 0) {
        log_warning("guitarunit%d_autoget_thread already running", unit_no);
        return;
    }

    ready = CreateEvent(NULL, TRUE, FALSE, NULL);

    guitar_unit_thread_id[unit_no] = avs_thread_create(
        unit_no == 0 ? guitarunit1_autoget_thread : guitarunit2_autoget_thread,
        ready,
        16384,
        0);

    if (WaitForSingleObject(ready, INFINITE)) {
        // can't do any logging here, yet.
        log_warning(
            "ERROR guitarunit%d_autoget_start: WaitForSingleObject failed: "
            "%08x",
            unit_no,
            (unsigned int) GetLastError());
        return;
    }

    CloseHandle(ready);

    log_misc("Started guitar unit%d autoget thread", unit_no);
}

static void guitarunit1_autoget_start(
    struct ac_io_emu_j33i *emu, const struct ac_io_message *req)
{
    guitarunit_autoget_start(emu, req, 0);
}

static void guitarunit2_autoget_start(
    struct ac_io_emu_j33i *emu, const struct ac_io_message *req)
{
    guitarunit_autoget_start(emu, req, 1);
}

/*
** GF ACIO:
**       bit 8  |bit7 |bit6 |bit5 |bit4|bit3|bit2|bit1
** [00]: X -1024|X 512|X 256|X 128|X 64|X 32|X 16|X 8
** [01]: 0      |X   4|X   2|X   1|0   |0   |0   |0
** [02]: Y -1024|Y 512|Y 256|Y 128|Y 64|Y 32|Y 16|Y 8
** [03]: 0      |Y   4|Y   2|Y   1|0   |0   |0   |0
** [04]: Z -1024|Z 512|Z 256|Z 128|Z 64|Z 32|Z 16|Z 8
** [05]: 0      |Z   4|Z   2|Z   1|0   |0   |0   |0
** [06]: R      |G    |B    |Y    |P   |0   |0   |0
** [07]: 0      |0    |P_UP |P_DN |KB0 |KB1 |KB2 |KB3
** [08]: 0      |MOTOR (7 bits)
*/
static int guitarunit_autoget_thread(uint8_t unit_no)
{
    uint8_t autoget_buffer[9];
    uint16_t guitar_inputs;
    int16_t x, y, z;
    uint8_t pick_value, motor_value, seq_no_autoget, autoget_unit_no;

    struct ac_io_message resp;

    memset(autoget_buffer, 0, sizeof(autoget_buffer));
    seq_no_autoget = 0;
    autoget_unit_no = unit_no & 1;

    while (true) {

        /*
            it just runs too fast, slow it here
            real guitar io runs at about 250 ticks per second @ baudrate 115200
            so adding the sleep from gd_io_read_gf_inputs will get us to ~2ms
            delay which is pretty close to 250 ticks per second
        */
        Sleep(1);

        if (!gd_io_read_gf_inputs(autoget_unit_no)) {
            log_warning(
                "Reading guitar unit%d inputs from gdio failed",
                autoget_unit_no);
            return -1;
        }

        /* read inputs here */
        guitar_inputs = gd_io_get_gf_guitar_inputs(autoget_unit_no);

        /*
            the range of these 3 sensors should be -180 to 180
            can't solid confirm 'cuz i don't want to swing the controller too much
            since i'm in a small room
        */
        x = 0;
        y = 0;
        z = 0;

        motor_value = 0;
        pick_value = 0;

        /* process inputs */
        if (guitar_inputs & (1 << GD_IO_GUITAR_PICK_UP)) {
            pick_value = 0x20;
        } else if (guitar_inputs & (1 << GD_IO_GUITAR_PICK_DOWN)) {
            pick_value = 0x10;
        }

        if (guitar_inputs & (1 << GD_IO_GUITAR_WAIL_UP)) {
            y -= 128;
        } else if (guitar_inputs & (1 << GD_IO_GUITAR_WAIL_DOWN)) {
            y += 128;
        }

        if (guitar_inputs & (1 << GD_IO_GUITAR_WAIL_SIDE)) {
            z += 128;
        }

        /* write acio buffer */
        *(uint16_t *) (autoget_buffer + 0) =
            (uint16_t) GDHOOK_GDIO_TO_ACIO11BITS(x);
        *(uint16_t *) (autoget_buffer + 2) =
            (uint16_t) GDHOOK_GDIO_TO_ACIO11BITS(y);
        *(uint16_t *) (autoget_buffer + 4) =
            (uint16_t) GDHOOK_GDIO_TO_ACIO11BITS(z);

        autoget_buffer[6] = (guitar_inputs & (1 << GD_IO_GUITAR_R) ? 0x80 : 0) |
            (guitar_inputs & (1 << GD_IO_GUITAR_G) ? 0x40 : 0) |
            (guitar_inputs & (1 << GD_IO_GUITAR_B) ? 0x20 : 0) |
            (guitar_inputs & (1 << GD_IO_GUITAR_Y) ? 0x10 : 0) |
            (guitar_inputs & (1 << GD_IO_GUITAR_P) ? 0x08 : 0);

        autoget_buffer[7] = pick_value | 0x08;

        autoget_buffer[8] = motor_value & 0x7F;

        /* send to acio bus */
        resp.addr = 1 | AC_IO_RESPONSE_FLAG;
        resp.cmd.code = ac_io_u16(AC_IO_CMD_J33I_IO_AUTOGET_DATA);
        resp.cmd.seq_no = seq_no_autoget++;
        resp.cmd.nbytes = sizeof(autoget_buffer);
        memcpy(resp.cmd.raw, autoget_buffer, sizeof(autoget_buffer));

        ac_io_emu_response_push(
            (&ac_io_emu_guitar[autoget_unit_no])->emu, &resp, 0);
    }
    return 0;
}

static int guitarunit1_autoget_thread(void *dummy_ctx)
{
    SetEvent((HANDLE) dummy_ctx);

    return guitarunit_autoget_thread(0);
}

static int guitarunit2_autoget_thread(void *dummy_ctx)
{
    SetEvent((HANDLE) dummy_ctx);

    return guitarunit_autoget_thread(1);
}