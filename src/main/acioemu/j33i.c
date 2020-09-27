#define LOG_MODULE "acioemu-j33i"

#include "acioemu/j33i.h"

#include <windows.h> /* for _BitScanForward */

#include <stdint.h>
#include <string.h>

#include "acio/j33i.h"

#include "acioemu/emu.h"

#include "util/hex.h"

static void ac_io_emu_j33i_cmd_send_version(
    struct ac_io_emu_j33i *j33i, const struct ac_io_message *req);

static void ac_io_emu_j33i_send_status(
    struct ac_io_emu_j33i *j33i,
    const struct ac_io_message *req,
    uint8_t status);

void ac_io_emu_j33i_init(
    struct ac_io_emu_j33i *j33i,
    struct ac_io_emu *emu,
    struct acio_j33i_msg_hook *dispatcher)
{
    memset(j33i, 0, sizeof(*j33i));
    j33i->emu = emu;

	if (dispatcher == NULL) {
        log_warning("NULL dispatcher, J33I IO won't work");
    }
    j33i->cmd_dispatcher = dispatcher;
}

void ac_io_emu_j33i_dispatch_request(
    struct ac_io_emu_j33i *j33i, const struct ac_io_message *req)
{
    uint16_t cmd_code;

    cmd_code = ac_io_u16(req->cmd.code);

    switch (cmd_code) {
        case AC_IO_CMD_GET_VERSION:
            log_misc("AC_IO_CMD_GET_VERSION(%d)", req->addr);
            ac_io_emu_j33i_cmd_send_version(j33i, req);

            break;

        case AC_IO_CMD_START_UP:
            log_misc("AC_IO_CMD_START_UP(%d)", req->addr);
            ac_io_emu_j33i_send_status(j33i, req, 0x00);

            break;

        case AC_IO_CMD_J33I_IO_AUTOGET_START:
            if (j33i->cmd_dispatcher->autoget_start != NULL) {
                j33i->cmd_dispatcher->autoget_start(j33i, req);
            }

            break;

        case AC_IO_CMD_J33I_IO_SET_MOTOR_VALUE:
            if (j33i->cmd_dispatcher->set_motor_value != NULL) {
                j33i->cmd_dispatcher->set_motor_value(j33i, req);
            }

            break;

		case AC_IO_CMD_J33I_IO_AUTOGET_DATA:
            /* not called, only for loop response */
            break;

        default:
            log_warning(
                "Unknown ACIO message %04x on j33i node, addr=%d",
                cmd_code,
                req->addr);

            break;
    }
}

static void ac_io_emu_j33i_cmd_send_version(
    struct ac_io_emu_j33i *j33i, const struct ac_io_message *req)
{
    struct ac_io_message resp;

    resp.addr = req->addr | AC_IO_RESPONSE_FLAG;
    resp.cmd.code = req->cmd.code;
    resp.cmd.seq_no = req->cmd.seq_no;
    resp.cmd.nbytes = sizeof(resp.cmd.version);
    resp.cmd.version.type = ac_io_u32(AC_IO_NODE_TYPE_J33I);
    resp.cmd.version.flag = 0x01;
    resp.cmd.version.major = 0x01;
    resp.cmd.version.minor = 0x00;
    resp.cmd.version.revision = 0x04;
    memcpy(
        resp.cmd.version.product_code,
        "J33I",
        sizeof(resp.cmd.version.product_code));
    strncpy(resp.cmd.version.date, __DATE__, sizeof(resp.cmd.version.date));
    strncpy(resp.cmd.version.time, __TIME__, sizeof(resp.cmd.version.time));

    ac_io_emu_response_push(j33i->emu, &resp, 0);
}

static void ac_io_emu_j33i_send_status(
    struct ac_io_emu_j33i *j33i,
    const struct ac_io_message *req,
    uint8_t status)
{
    struct ac_io_message resp;

    resp.addr = req->addr | AC_IO_RESPONSE_FLAG;
    resp.cmd.code = req->cmd.code;
    resp.cmd.seq_no = req->cmd.seq_no;
    resp.cmd.nbytes = sizeof(resp.cmd.status);
    resp.cmd.status = status;

    ac_io_emu_response_push(j33i->emu, &resp, 0);
}
