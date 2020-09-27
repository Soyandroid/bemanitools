#define LOG_MODULE "acioemu-j32d"

#include "acioemu/j32d.h"

#include <windows.h> /* for _BitScanForward */

#include <stdint.h>
#include <string.h>

#include "acio/j32d.h"

#include "acioemu/emu.h"

#include "util/hex.h"

static void ac_io_emu_j32d_cmd_send_version(
    struct ac_io_emu_j32d *j32d, const struct ac_io_message *req);

static void ac_io_emu_j32d_send_status(
    struct ac_io_emu_j32d *j32d,
    const struct ac_io_message *req,
    uint8_t status);

void ac_io_emu_j32d_init(
    struct ac_io_emu_j32d *j32d,
    struct ac_io_emu *emu,
    struct acio_j32d_msg_hook *dispatcher)
{
    memset(j32d, 0, sizeof(*j32d));
    j32d->emu = emu;

	if (dispatcher == NULL) {
        log_warning("NULL dispatcher, J32D IO won't work");
    }
    j32d->cmd_dispatcher = dispatcher;
}

void ac_io_emu_j32d_dispatch_request(
    struct ac_io_emu_j32d *j32d, const struct ac_io_message *req)
{
    uint16_t cmd_code;

    cmd_code = ac_io_u16(req->cmd.code);

    switch (cmd_code) {
        case AC_IO_CMD_GET_VERSION:
            log_misc("AC_IO_CMD_GET_VERSION(%d)", req->addr);
            ac_io_emu_j32d_cmd_send_version(j32d, req);

            break;

        case AC_IO_CMD_START_UP:
            log_misc("AC_IO_CMD_START_UP(%d)", req->addr);
            ac_io_emu_j32d_send_status(j32d, req, 0x00);

            break;

        case AC_IO_CMD_J32D_IO_AUTOGET_START:
            if (j32d->cmd_dispatcher->autoget_start != NULL) {
                j32d->cmd_dispatcher->autoget_start(j32d, req);
            }

            ac_io_emu_j32d_send_status(j32d, req, 0x00);
            break;

		case AC_IO_CMD_J32D_IO_AUTOGET_DATA:
            /* not called, only for loop response */
            break;

        default:
            log_warning(
                "Unknown ACIO message %04x on j32d node, addr=%d",
                cmd_code,
                req->addr);

            break;
    }
}

static void ac_io_emu_j32d_cmd_send_version(
    struct ac_io_emu_j32d *j32d, const struct ac_io_message *req)
{
    struct ac_io_message resp;

    resp.addr = req->addr | AC_IO_RESPONSE_FLAG;
    resp.cmd.code = req->cmd.code;
    resp.cmd.seq_no = req->cmd.seq_no;
    resp.cmd.nbytes = sizeof(resp.cmd.version);
    resp.cmd.version.type = ac_io_u32(AC_IO_NODE_TYPE_J32D);
    resp.cmd.version.flag = 0x01;
    resp.cmd.version.major = 0x00;
    resp.cmd.version.minor = 0x01;
    resp.cmd.version.revision = 0x00;
    memcpy(
        resp.cmd.version.product_code,
        "J32D",
        sizeof(resp.cmd.version.product_code));
    strncpy(resp.cmd.version.date, __DATE__, sizeof(resp.cmd.version.date));
    strncpy(resp.cmd.version.time, __TIME__, sizeof(resp.cmd.version.time));

    ac_io_emu_response_push(j32d->emu, &resp, 0);
}

static void ac_io_emu_j32d_send_status(
    struct ac_io_emu_j32d *j32d,
    const struct ac_io_message *req,
    uint8_t status)
{
    struct ac_io_message resp;

    resp.addr = req->addr | AC_IO_RESPONSE_FLAG;
    resp.cmd.code = req->cmd.code;
    resp.cmd.seq_no = req->cmd.seq_no;
    resp.cmd.nbytes = sizeof(resp.cmd.status);
    resp.cmd.status = status;

    ac_io_emu_response_push(j32d->emu, &resp, 0);
}
