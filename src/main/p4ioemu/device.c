#define LOG_MODULE "p4ioemu-device"

#include "p4ioemu/device.h"

#include <setupapi.h>
#include <usb100.h>
#include <windows.h>

#include "hook/iohook.h"
#include "util/hex.h"
#include "util/log.h"
#include "util/str.h"

#include "p4io/cmd.h"
#include "p4ioemu/uart.h"

//#define P4IOEMU_DEBUG_DUMP

/* can't seem to #include the requisite DDK headers from usermode code,
   so we have to redefine these macros here */

#define CTL_CODE(DeviceType, Function, Method, Access) \
    (((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method))

#define METHOD_BUFFERED 0

#define FILE_ANY_ACCESS 0x00

#define FILE_DEVICE_UNKNOWN 0x22

#define P4IO_FUNCTION_READ_JAMMA_2 0x801
#define P4IO_FUNCTION_GET_DEVICE_NAME 0x803

#define IOCTL_P4IO_GET_DEVICE_NAME     \
    CTL_CODE(                          \
        FILE_DEVICE_UNKNOWN,           \
        P4IO_FUNCTION_GET_DEVICE_NAME, \
        METHOD_BUFFERED,               \
        FILE_ANY_ACCESS)
#define IOCTL_P4IO_READ_JAMMA_2     \
    CTL_CODE(                       \
        FILE_DEVICE_UNKNOWN,        \
        P4IO_FUNCTION_READ_JAMMA_2, \
        METHOD_BUFFERED,            \
        FILE_ANY_ACCESS)

static const struct p4ioemu_device_msg_hook *p4ioemu_device_msg_hook;

static HANDLE p4ioemu_p4io_fd;
static uint8_t p4ioemu_p4io_cmd_read_buffer[4096];
static uint32_t p4ioemu_p4io_cmd_buffer_resp_len;
static uint8_t p4ioemu_p4io_last_cmd;
static uint8_t p4ioemu_p4io_last_seq_num;

static uint32_t p4ioemu_p4io_command_handle(
    uint8_t cmd,
    const void *payload,
    uint32_t payload_len,
    void *resp,
    uint32_t resp_max_len)
{
    switch (cmd) {
        case P4IO_CMD_INIT: {
            log_misc("P4IO_CMD_INIT");

            /* no data to send to host */
            memset(resp, 0, resp_max_len);
            /* driver expects this buffer size, fails otherwise */
            return 0;
        }

		case P4IO_CMD_REQ_COINSTOCK: {
            // log_misc("P4IO_CMD_REQ_COINSTOCK");

            if (p4ioemu_device_msg_hook->get_coinstock) {
                p4ioemu_device_msg_hook->get_coinstock(resp, 4);
            } else {
                memset(resp, 0, 4);
            }

			return 4;
		}

        case P4IO_CMD_GET_DEVICE_INFO: {
            log_misc("P4IO_CMD_GET_DEVICE_INFO");

            struct p4io_device_info_resp *info =
                (struct p4io_device_info_resp *) resp;

			memset(resp, 0, sizeof(struct p4io_device_info_resp));

            info->type = 0x37133713;
            info->flag = 1;
            info->version_major = 1;
            info->version_minor = 1;
            info->version_revision = 0;
            memcpy(info->product_code, "BMPU", 4);
            memcpy(info->build_date, __DATE__, strlen(__DATE__));
            memcpy(info->build_time, __TIME__, strlen(__TIME__));

            return sizeof(struct p4io_device_info_resp);
        }

        case P4IO_CMD_DALLAS_READ_ID: {
            const struct p4io_read_roundplug_req *req =
                (const struct p4io_read_roundplug_req *) payload;

            log_misc("P4IO_CMD_DALLAS_READ_ID: %d", req->type);

            if (p4ioemu_device_msg_hook->roundplug_read_id) {
                p4ioemu_device_msg_hook->roundplug_read_id(req->type, resp, 8);
            } else {
                memset(resp, 0, 8);
            }

            return 8;
        }

        case P4IO_CMD_DALLAS_READ_MEM: {
            const struct p4io_read_roundplug_req *req =
                (const struct p4io_read_roundplug_req *) payload;

            log_misc("P4IO_CMD_DALLAS_READ_MEM: %d", req->type);

            if (p4ioemu_device_msg_hook->roundplug_read_mem) {
                p4ioemu_device_msg_hook->roundplug_read_mem(
                    req->type, resp, 33);
            } else {
                memset(resp, 0, 33);
            }

            return 33;
        }

        case P4IO_CMD_SET_PORTOUT: {
            log_misc("P4IO_CMD_SET_PORTOUT: %d", payload_len);

            if (p4ioemu_device_msg_hook->set_portout) {
                p4ioemu_device_msg_hook->set_portout(payload, payload_len);
            }

            return 0;
        }

        case P4IO_CMD_SCI_MNG_OPEN: {
            // log_misc("P4IO_CMD_SCI_MNG_OPEN");

            const struct p4io_sci_open_req *req =
                (const struct p4io_sci_open_req *) payload;

            if (payload_len == 3) {
                p4io_uart_cmd_open(req);
			}

            return 0;
        }

        case P4IO_CMD_SCI_MNG_BREAK: {
            // log_misc("P4IO_CMD_SCI_MNG_BREAK");

            const struct p4io_sci_break_req *req =
                (const struct p4io_sci_break_req *) payload;

            if (payload_len == 2) {
                p4io_uart_cmd_break(req);
            }

            return 0;
        }

        case P4IO_CMD_SCI_UPDATE: {
            // log_misc("P4IO_CMD_SCI_UPDATE");

            const struct p4io_sci_update_req *req =
                (const struct p4io_sci_update_req *) payload;

			struct p4io_sci_update_resp *resp_payload =
                (struct p4io_sci_update_resp *) resp;

            return p4io_uart_cmd_update(req, payload_len - 1, resp_payload);
        }

        case P4IO_CMD_SET_WATCHDOG: {
            // log_misc("P4IO_CMD_SET_WATCHDOG");

            return 0;
        }

        default:
            /* forward to game specific handler */
            return 0xFFFFFFFF;
    }
}

static void p4ioemu_p4io_dump_buffer(const void *buffer, uint32_t len)
{
    char buffer_str[4096];
    hex_encode_uc(buffer, len, buffer_str, sizeof(buffer_str));
    log_warning("Package dump: %s", buffer_str);
}

static HRESULT p4ioemu_p4io_bulk_read(void *resp, uint32_t nbytes)
{
    struct p4io_cmd_package *package;
    void *payload;
    uint32_t max_payload_len;
    uint32_t payload_len;

    if (nbytes < sizeof(struct p4io_cmd_package)) {
        log_warning("Buffer for bulk read endpoint to short: %d", nbytes);

        return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }

    package = (struct p4io_cmd_package *) resp;
    payload = resp + sizeof(struct p4io_cmd_package);
    max_payload_len = nbytes - sizeof(struct p4io_cmd_package);

    if (max_payload_len < p4ioemu_p4io_cmd_buffer_resp_len) {
        log_warning(
            "Too much response data (%d) for buffer (%d), truncated",
            p4ioemu_p4io_cmd_buffer_resp_len,
            max_payload_len);
        payload_len = max_payload_len;
    } else {
        payload_len = p4ioemu_p4io_cmd_buffer_resp_len;
    }

    package->header_AA = 0xAA;
    package->cmd = p4ioemu_p4io_last_cmd;
    package->seq_num = p4ioemu_p4io_last_seq_num;
    package->payload_len = payload_len;

    memcpy(payload, p4ioemu_p4io_cmd_read_buffer, payload_len);

    return S_OK;
}

static HRESULT p4ioemu_p4io_bulk_write(const void *req, uint32_t nbytes)
{
    const struct p4io_cmd_package *package;
    const void *payload;

    if (nbytes < sizeof(struct p4io_cmd_package)) {
        log_warning("Command on bulk write endpoint to short: %d", nbytes);
        p4ioemu_p4io_dump_buffer(req, nbytes);

        return E_INVALIDARG;
    }

    package = (struct p4io_cmd_package *) req;
    payload = req + sizeof(struct p4io_cmd_package);

    if (package->header_AA != 0xAA) {
        log_warning("Command on bulk endpoint to short: %d", nbytes);
        p4ioemu_p4io_dump_buffer(req, nbytes);

        return E_INVALIDARG;
    }

    p4ioemu_p4io_last_cmd = package->cmd;
    p4ioemu_p4io_last_seq_num = package->seq_num;

    /* handle commands that are common p4io ones first */
    p4ioemu_p4io_cmd_buffer_resp_len = p4ioemu_p4io_command_handle(
        package->cmd,
        payload,
        package->payload_len,
        p4ioemu_p4io_cmd_read_buffer,
        sizeof(p4ioemu_p4io_cmd_read_buffer));

    /* forward to game specific handlers */
    if (p4ioemu_p4io_cmd_buffer_resp_len == 0xFFFFFFFF) {
        if (p4ioemu_device_msg_hook->command_handle) {
            p4ioemu_p4io_cmd_buffer_resp_len =
                p4ioemu_device_msg_hook->command_handle(
                    package->cmd,
                    payload,
                    package->payload_len,
                    p4ioemu_p4io_cmd_read_buffer,
                    sizeof(p4ioemu_p4io_cmd_read_buffer));
        }
    }

    if (p4ioemu_p4io_cmd_buffer_resp_len == 0xFFFFFFFF) {
        log_warning("Unhandled cmd 0x%X", package->cmd);

        return E_NOTIMPL;
    }

    return S_OK;
}

/* For debugging */
#ifdef P4IOEMU_DEBUG_DUMP
static void p4ioemu_device_log_ioctl_msg(
    const char *prefix,
    uint32_t ctl_code,
    const void *ctl,
    uint32_t ctl_size,
    void *header,
    uint32_t header_bytes,
    void *data,
    uint32_t data_bytes)
{
    char header_str[4096];
    char data_str[4096];
    const char *ctl_code_str;

    switch (ctl_code) {
        case IOCTL_P4IO_GET_DEVICE_NAME:
            ctl_code_str = "IOCTL_P4IO_GET_DEVICE_NAME";
            break;

        case IOCTL_P4IO_READ_JAMMA_2:
            ctl_code_str = "IOCTL_P4IO_READ_JAMMA_2";
            break;

        default:
            ctl_code_str = "UNKNOWN";
            break;
    }

    hex_encode_uc(header, header_bytes, header_str, sizeof(header_str));
    hex_encode_uc(data, data_bytes, data_str, sizeof(data_str));

    log_warning(
        "[P4IO IOCTL DUMP %s][%s] ctl_code 0x%X, ctl_size %d, header(%d) "
        "%s |||| data(%d) %s",
        prefix,
        ctl_code_str,
        ctl_code,
        ctl_size,
        header_bytes,
        header_str,
        data_bytes,
        data_str);
}

static void
p4ioemu_device_log(const char *prefix, const void *data, uint32_t data_bytes)
{
    char data_str[4096];

    hex_encode_uc(data, data_bytes, data_str, sizeof(data_str));

    log_warning("[P4IO DUMP %s] data(%d) %s", prefix, data_bytes, data_str);
}
#endif

static HRESULT p4ioemu_device_open(struct irp *irp)
{
    log_assert(irp != NULL);

    /* FIXME I don't know what kind of path the game expects to be returned
       on the setupapi calls, so i assumed it wants \\p4io. However, the
       game also adds the node \\p4io to that -> result: \\p4io\\p4io */

    if (!wstr_eq(irp->open_filename, L"\\p4io\\p4io")) {
        return iohook_invoke_next(irp);
    }

    irp->fd = p4ioemu_p4io_fd;
    log_info("P4IO opened");

    return S_OK;
}

static HRESULT p4ioemu_device_read(struct irp *irp)
{
    HRESULT hr;

    memset(irp->read.bytes, 0, irp->read.nbytes);
    hr = p4ioemu_p4io_bulk_read(irp->read.bytes, irp->read.nbytes);

    if (FAILED(hr)) {
        return hr;
    }

    /* game expects a read size of 0x40, always */
    irp->read.pos = 0x40;

#ifdef P4IOEMU_DEBUG_DUMP
    p4ioemu_device_log("POST READ", dest->bytes, dest->nbytes);
#endif

    return hr;
}

static HRESULT p4ioemu_device_write(struct irp *irp)
{
    HRESULT hr;

    /* For debugging */
#ifdef P4IOEMU_DEBUG_DUMP
    p4ioemu_device_log("WRITE", src->bytes, src->nbytes);
#endif

    hr = p4ioemu_p4io_bulk_write(irp->write.bytes, irp->write.nbytes);

    if (FAILED(hr)) {
        return hr;
    }

    irp->write.pos = irp->write.nbytes;

    return hr;
}

static HRESULT p4ioemu_device_ioctl(struct irp *irp)
{
    log_assert(irp != NULL);

    /* For debugging */
#ifdef P4IOEMU_DEBUG_DUMP
    p4ioemu_device_log_ioctl_msg(
        "BEFORE",
        code,
        in_bytes,
        in_nbytes,
        in_bytes,
        in_nbytes,
        out_bytes,
        out_nbytes);
#endif

    /* Cases are listed in order of first receipt */
    switch (irp->ioctl) {
        case IOCTL_P4IO_GET_DEVICE_NAME: {
            const char dev_name[] = "kactools p4ioemu";

            if (irp->read.nbytes < strlen(dev_name)) {
                log_fatal("Device name string does not fit into buffer");
            }

            memset(irp->read.bytes, 0, irp->read.nbytes);
            memcpy(irp->read.bytes, dev_name, strlen(dev_name));
            irp->read.pos = strlen(dev_name);

            return S_OK;
        }

        case IOCTL_P4IO_READ_JAMMA_2: {
            p4ioemu_device_msg_hook->jamma2_read(
                irp->read.bytes, irp->read.nbytes);
            irp->read.pos = irp->read.nbytes;

            return S_OK;
        }

        default:
            log_warning("Unknown ioctl %08x", irp->ioctl);

            return E_INVALIDARG;
    }

        /* For debugging */
#ifdef P4IOEMU_DEBUG_DUMP
    p4ioemu_device_log_ioctl_msg(
        "AFTER",
        code,
        in_bytes,
        in_nbytes,
        in_bytes,
        in_nbytes,
        out_bytes,
        out_nbytes);
#endif
}

static HRESULT p4ioemu_device_close(struct irp *irp)
{
    log_info("P4IO closed");

    return S_OK;
}

void p4ioemu_init(const struct p4ioemu_device_msg_hook *msg_hook)
{
    log_assert(p4ioemu_p4io_fd == NULL);

    HRESULT hr;

    hr = iohook_open_nul_fd(&p4ioemu_p4io_fd);

    if (hr != S_OK) {
        log_fatal("Opening nul fd failed: %08lx", hr);
    }

    p4ioemu_device_msg_hook = msg_hook;
}

void p4ioemu_fini(void)
{
    if (p4ioemu_p4io_fd != NULL) {
        CloseHandle(p4ioemu_p4io_fd);
    }

    p4ioemu_p4io_fd = NULL;
}

HRESULT
p4ioemu_dispatch_irp(struct irp *irp)
{
    log_assert(irp != NULL);

    if (irp->op != IRP_OP_OPEN && irp->fd != p4ioemu_p4io_fd) {
        return iohook_invoke_next(irp);
    }

    switch (irp->op) {
        case IRP_OP_OPEN:
            return p4ioemu_device_open(irp);
        case IRP_OP_CLOSE:
            return p4ioemu_device_close(irp);
        case IRP_OP_READ:
            return p4ioemu_device_read(irp);
        case IRP_OP_WRITE:
            return p4ioemu_device_write(irp);
        case IRP_OP_IOCTL:
            return p4ioemu_device_ioctl(irp);
        default:
            return E_NOTIMPL;
    }
}
