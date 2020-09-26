#define LOG_MODULE "j33i"

// clang-format off
// Don't format because the order is important here
#include <windows.h>
#include <devioctl.h>
#include <ntdef.h>
#include <ntddser.h>
// clang-format on

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "acioemu/addr.h"
#include "acioemu/emu.h"

#include "hook/iohook.h"

#include "util/defs.h"
#include "util/log.h"
#include "util/str.h"

static struct ac_io_emu guitar_xg_ac_io_emu;

static void guitar_xg_handle_broadcast(const struct ac_io_message *bcast);
static void guitar_xg_handle_get_version(const struct ac_io_message *req);
static void guitar_xg_handle_status(const struct ac_io_message *req);
static void guitar_xg_send_empty(const struct ac_io_message *req);

void guitar_xg_init(void)
{
    ac_io_emu_init(&guitar_xg_ac_io_emu, L"COM2");
}

void guitar_xg_fini(void)
{
    ac_io_emu_fini(&guitar_xg_ac_io_emu);
}

HRESULT
guitar_xg_dispatch_irp(struct irp *irp)
{
    const struct ac_io_message *msg;
    HRESULT hr;

    log_assert(irp != NULL);

    if (!ac_io_emu_match_irp(&guitar_xg_ac_io_emu, irp)) {
        return iohook_invoke_next(irp);
    }

    for (;;) {
        hr = ac_io_emu_dispatch_irp(&guitar_xg_ac_io_emu, irp);

        if (hr != S_OK) {
            return hr;
        }

        msg = ac_io_emu_request_peek(&guitar_xg_ac_io_emu);

        switch (msg->addr) {
            case 0:
                ac_io_emu_cmd_assign_addrs(&guitar_xg_ac_io_emu, msg, 1);

                break;

            case 1:
                switch (ac_io_u16(msg->cmd.code)) {
                    case AC_IO_CMD_GET_VERSION:
                        guitar_xg_handle_get_version(msg);

                        break;

                    case AC_IO_CMD_START_UP:
                        guitar_xg_handle_status(msg);

                        break;

                    case AC_IO_CMD_KEEPALIVE:
                        guitar_xg_send_empty(msg);

                        break;

                    case 0x100:
                    case 0x110:
                    case 0x112:
                    case 0x128:
                        guitar_xg_handle_status(msg);

                        break;

                    default:
                        log_warning(
                            "J32I ACIO unhandled cmd: %04X",
                            ac_io_u16(msg->cmd.code));
                }

                break;

            case AC_IO_BROADCAST:
                guitar_xg_handle_broadcast(msg);

                break;

            default:
                log_warning(
                    "J32I ACIO message on unhandled bus address: %d",
                    msg->addr);

                break;
        }

        ac_io_emu_request_pop(&guitar_xg_ac_io_emu);
    }
}

static void guitar_xg_handle_broadcast(const struct ac_io_message *bcast)
{
    ;
}

static void guitar_xg_handle_get_version(const struct ac_io_message *req)
{
    struct ac_io_message resp;

    resp.addr = req->addr | AC_IO_RESPONSE_FLAG;
    resp.cmd.code = req->cmd.code;
    resp.cmd.seq_no = req->cmd.seq_no;
    resp.cmd.nbytes = sizeof(resp.cmd.version);
    resp.cmd.version.type = ac_io_u32(AC_IO_NODE_TYPE_J33I);
    resp.cmd.version.flag = 0x00;
    resp.cmd.version.major = 0x01;
    resp.cmd.version.minor = 0x01;
    resp.cmd.version.revision = 0x00;
    memcpy(
        resp.cmd.version.product_code,
        "J33I",
        sizeof(resp.cmd.version.product_code));
    strncpy(resp.cmd.version.date, __DATE__, sizeof(resp.cmd.version.date));
    strncpy(resp.cmd.version.time, __TIME__, sizeof(resp.cmd.version.time));

    ac_io_emu_response_push(&guitar_xg_ac_io_emu, &resp, 0);
}

static void guitar_xg_handle_status(const struct ac_io_message *req)
{
    struct ac_io_message resp;

    resp.addr = req->addr | AC_IO_RESPONSE_FLAG;
    resp.cmd.code = req->cmd.code;
    resp.cmd.seq_no = req->cmd.seq_no;
    resp.cmd.nbytes = sizeof(resp.cmd.status);
    resp.cmd.status = 0x00;

    ac_io_emu_response_push(&guitar_xg_ac_io_emu, &resp, 0);
}

static void guitar_xg_send_empty(const struct ac_io_message *req)
{
    struct ac_io_message resp;

    resp.addr = req->addr | AC_IO_RESPONSE_FLAG;
    resp.cmd.code = req->cmd.code;
    resp.cmd.seq_no = req->cmd.seq_no;
    resp.cmd.nbytes = 0;

    ac_io_emu_response_push(&guitar_xg_ac_io_emu, &resp, 0);
}
