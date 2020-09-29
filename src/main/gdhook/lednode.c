#define LOG_MODULE "acioemu-lednode"

#include <windows.h> /* for _BitScanForward */

#include <stdint.h>
#include <string.h>

#include "gdhook/lednode.h"

#include "acioemu/emu.h"

#include "util/hex.h"

static void ac_io_emu_lednode_cmd_send_version(
    struct ac_io_emu_lednode *led_node, const struct ac_io_message *req);

static void ac_io_emu_lednode_send_status(
    struct ac_io_emu_lednode *led_node,
    const struct ac_io_message *req,
    uint8_t status);

void ac_io_emu_lednode_init(
    struct ac_io_emu_lednode *led_node,
    struct ac_io_emu *emu,
    acio_lednode_dispatcher lights_dispatcher)
{
    memset(led_node, 0, sizeof(*led_node));
    led_node->emu = emu;

    if (lights_dispatcher == NULL) {
        log_warning("NULL lights_dispatcher, LED light output won't work");
    }
    led_node->light_dispatcher = lights_dispatcher;
}

void ac_io_emu_lednode_dispatch_request(
    struct ac_io_emu_lednode *led_node, const struct ac_io_message *req)
{
    uint16_t cmd_code;

    cmd_code = ac_io_u16(req->cmd.code);

    switch (cmd_code) {
        case AC_IO_CMD_GET_VERSION:
            log_misc("AC_IO_CMD_GET_VERSION(%d)", req->addr);
            ac_io_emu_lednode_cmd_send_version(led_node, req);

            break;

        case AC_IO_CMD_START_UP:
            log_misc("AC_IO_CMD_START_UP(%d)", req->addr);
            ac_io_emu_lednode_send_status(led_node, req, 0x00);

            break;

        case AC_IO_LEDNODE_CMD_SEND_CUSTOM1:
            ac_io_emu_lednode_send_status(led_node, req, 0x00);
            break;

        case AC_IO_LEDNODE_CMD_SEND_CUSTOM2:
            if (led_node->light_dispatcher != NULL) {
                led_node->light_dispatcher(led_node, req);
            }

            ac_io_emu_lednode_send_status(led_node, req, 0x00);
            break;

        case AC_IO_LEDNODE_CMD_SEND_DIRECT:
            ac_io_emu_lednode_send_status(led_node, req, 0x00);

            break;

        default:
            log_warning(
                "Unknown ACIO message %04x on _led node, addr=%d",
                cmd_code,
                req->addr);

            break;
    }
}

static void ac_io_emu_lednode_cmd_send_version(
    struct ac_io_emu_lednode *led_node, const struct ac_io_message *req)
{
    struct ac_io_message resp;

    resp.addr = req->addr | AC_IO_RESPONSE_FLAG;
    resp.cmd.code = req->cmd.code;
    resp.cmd.seq_no = req->cmd.seq_no;
    resp.cmd.nbytes = sizeof(resp.cmd.version);
    resp.cmd.version.type = ac_io_u32(AC_IO_NODE_TYPE_LEDUNIT_GITADORA);
    resp.cmd.version.flag = 0x00;
    resp.cmd.version.major = 0x01;
    resp.cmd.version.minor = 0x00;
    resp.cmd.version.revision = 0x00;
    memcpy(
        resp.cmd.version.product_code,
        "_LED",
        sizeof(resp.cmd.version.product_code));
    strncpy(resp.cmd.version.date, __DATE__, sizeof(resp.cmd.version.date));
    strncpy(resp.cmd.version.time, __TIME__, sizeof(resp.cmd.version.time));

    ac_io_emu_response_push(led_node->emu, &resp, 0);
}

static void ac_io_emu_lednode_send_status(
    struct ac_io_emu_lednode *led_node,
    const struct ac_io_message *req,
    uint8_t status)
{
    struct ac_io_message resp;

    resp.addr = req->addr | AC_IO_RESPONSE_FLAG;
    resp.cmd.code = req->cmd.code;
    resp.cmd.seq_no = req->cmd.seq_no;
    resp.cmd.nbytes = sizeof(resp.cmd.status);
    resp.cmd.status = status;

    ac_io_emu_response_push(led_node->emu, &resp, 0);
}
