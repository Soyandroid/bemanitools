#define LOG_MODULE "p4ioemu-uart"

// clang-format off
// Don't format because the order is important here
#include <windows.h>
#include <devioctl.h>
#include <ntdef.h>
#include <ntddser.h>
// clang-format on

#include <string.h>

#include "hook/iohook.h"

#include "p4io/cmd.h"

#include "p4ioemu/uart.h"

#include "util/iobuf.h"
#include "util/log.h"

static HRESULT
p4io_uart_open(const wchar_t *path, uint32_t baud_rate, HANDLE *fd);
static HRESULT p4io_uart_close(HANDLE fd);
static HRESULT p4io_uart_read(HANDLE fd, struct iobuf *iobuf);
static HRESULT p4io_uart_write(HANDLE fd, struct const_iobuf *iobuf);

static const wchar_t *p4io_uart_paths[2];
static HANDLE p4io_uart_fds[2];
static uint8_t p4io_break[2];

static const uint32_t p4io_uart_baud_codes[] = {
    9600, 19200, 38400, 57600, 115200};

void p4io_uart_set_path(size_t uart_no, const wchar_t *path)
{
    log_assert(uart_no <= 1);

    p4io_uart_paths[uart_no] = path;
    p4io_break[uart_no] = 0;
}

void p4io_uart_cmd_open(const struct p4io_sci_open_req *req)
{
    const wchar_t *path;
    uint32_t baud_rate;
    HRESULT hr;

    log_assert(req != NULL);

    if (req->port_sci > 1) {
        log_warning("Invalid UART number %i", req->port_sci);
        hr = E_INVALIDARG;

        goto end;
    }

    log_info("Opening remote RS232 port #%d", req->port_sci);

    if (req->baud_sci < lengthof(p4io_uart_baud_codes)) {
        baud_rate = p4io_uart_baud_codes[req->baud_sci];
    } else {
        baud_rate = 0;
    }

    if (baud_rate == 0) {
        log_warning("Invalid baud rate code: %02x", req->baud_sci);
        hr = E_FAIL;

        goto end;
    }

    path = p4io_uart_paths[req->port_sci];

    if (path == NULL) {
        log_warning("UART #%i: No downstream connection", req->port_sci);
        hr = E_FAIL;

        goto end;
    }

    hr = p4io_uart_open(path, baud_rate, &p4io_uart_fds[req->port_sci]);

    if (FAILED(hr)) {
        log_warning("p4io_uart_open() failed: %x", (int) hr);
    }

end:
    // we don't need to returns anything
    ;
}

void p4io_uart_cmd_break(const struct p4io_sci_break_req *req)
{
    log_assert(req != NULL);

    if (req->port_sci > 1) {
        log_warning("Invalid UART number %i", req->port_sci);

        goto end;
    }

    p4io_break[req->port_sci] = req->break_sci;

end:
    // we don't need to returns anything
    ;
}

uint8_t p4io_uart_cmd_update(
    const struct p4io_sci_update_req *req,
    uint8_t sz_req,
    struct p4io_sci_update_resp *resp)
{
    uint8_t ret;
    struct const_iobuf c_iobuf;
    struct iobuf iobuf;
    HRESULT hr;

    log_assert(req != NULL);
    log_assert(resp != NULL);

    resp->port_sci = req->port_sci;
    resp->break_sci = p4io_break[req->port_sci];
    ret = 2;

    if (resp->break_sci > 0) {
        /* break state, should hold write+read or just read? */
        goto end;
    }

    if (p4io_uart_fds[req->port_sci] == NULL) {
        /* try to update from an unopened port */
        goto end;
    }

    c_iobuf.bytes = req->payload_sci;
    c_iobuf.nbytes = sz_req;
    c_iobuf.pos = 0;

    if (req->port_sci > 1) {
        log_warning("Invalid UART number %i", req->port_sci);
        hr = E_INVALIDARG;

        goto end;
    }

    if (sz_req > 59) {
        log_warning("Too many bytes to write (%d)", sz_req);
        hr = E_INVALIDARG;

        goto end;
    }

    hr = p4io_uart_write(p4io_uart_fds[req->port_sci], &c_iobuf);

    iobuf.bytes = resp->payload_sci;
    iobuf.nbytes = 59;
    iobuf.pos = 0;

    if (req->port_sci > 1) {
        log_warning("Invalid UART number %i", req->port_sci);
        hr = E_INVALIDARG;

        goto end;
    }

    hr = p4io_uart_read(p4io_uart_fds[req->port_sci], &iobuf);

    if (!FAILED(S_OK)) {
        /* Read success */
        ret += iobuf.pos;
    }

end:
    return ret;
}

static HRESULT
p4io_uart_open(const wchar_t *path, uint32_t baud_rate, HANDLE *fd)
{
    struct irp irp;
    uint32_t comm_mask;
    uint32_t flags;
    SERIAL_QUEUE_SIZE qs;
    SERIAL_TIMEOUTS timeouts;
    SERIAL_LINE_CONTROL lc;
    SERIAL_BAUD_RATE baud;
    SERIAL_HANDFLOW handflow;
    HRESULT hr;

    if (*fd != NULL) {
        log_warning("Port is already open");

        return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
    }

    memset(&irp, 0, sizeof(irp));
    irp.op = IRP_OP_OPEN;
    irp.open_filename = path;
    irp.open_access = GENERIC_READ | GENERIC_WRITE;
    irp.open_share = 0;
    irp.open_sa = NULL;
    irp.open_creation = OPEN_EXISTING;
    irp.open_flags = 0;
    irp.open_tmpl = NULL;

    hr = iohook_invoke_next(&irp);

    if (FAILED(hr)) {
        goto fail;
    }

    *fd = irp.fd;

    memset(&irp, 0, sizeof(irp));
    irp.op = IRP_OP_IOCTL;
    irp.fd = *fd;
    irp.ioctl = IOCTL_SERIAL_SET_WAIT_MASK;
    irp.write.bytes = (const void *) &comm_mask;
    irp.write.nbytes = sizeof(comm_mask);
    comm_mask = EV_RXCHAR;

    hr = iohook_invoke_next(&irp);

    if (FAILED(hr)) {
        goto fail;
    }

    memset(&irp, 0, sizeof(irp));
    irp.op = IRP_OP_IOCTL;
    irp.fd = *fd;
    irp.ioctl = IOCTL_SERIAL_SET_QUEUE_SIZE;
    irp.write.bytes = (const void *) &qs;
    irp.write.nbytes = sizeof(qs);
    qs.InSize = 0x4000;
    qs.OutSize = 0x4000;

    hr = iohook_invoke_next(&irp);

    if (FAILED(hr)) {
        goto fail;
    }

    memset(&irp, 0, sizeof(irp));
    irp.op = IRP_OP_IOCTL;
    irp.fd = *fd;
    irp.ioctl = IOCTL_SERIAL_PURGE;
    irp.write.bytes = (const void *) &flags;
    irp.write.nbytes = sizeof(flags);
    flags = PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR;

    hr = iohook_invoke_next(&irp);

    if (FAILED(hr)) {
        goto fail;
    }

    memset(&irp, 0, sizeof(irp));
    irp.op = IRP_OP_IOCTL;
    irp.fd = *fd;
    irp.ioctl = IOCTL_SERIAL_SET_TIMEOUTS;
    irp.write.bytes = (const void *) &timeouts;
    irp.write.nbytes = sizeof(timeouts);
    timeouts.ReadIntervalTimeout = -1;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.ReadTotalTimeoutConstant = 0;
    timeouts.WriteTotalTimeoutMultiplier = 100;
    timeouts.WriteTotalTimeoutConstant = 0;

    hr = iohook_invoke_next(&irp);

    if (FAILED(hr)) {
        goto fail;
    }

    memset(&irp, 0, sizeof(irp));
    irp.op = IRP_OP_IOCTL;
    irp.fd = *fd;
    irp.ioctl = IOCTL_SERIAL_SET_LINE_CONTROL;
    irp.write.bytes = (const void *) &lc;
    irp.write.nbytes = sizeof(lc);
    lc.WordLength = 8;
    lc.Parity = NO_PARITY;
    lc.StopBits = STOP_BIT_1;

    hr = iohook_invoke_next(&irp);

    if (FAILED(hr)) {
        goto fail;
    }

    memset(&irp, 0, sizeof(irp));
    irp.op = IRP_OP_IOCTL;
    irp.fd = *fd;
    irp.ioctl = IOCTL_SERIAL_SET_BAUD_RATE;
    irp.write.bytes = (const void *) &baud;
    irp.write.nbytes = sizeof(baud);
    baud.BaudRate = baud_rate;

    hr = iohook_invoke_next(&irp);

    if (FAILED(hr)) {
        goto fail;
    }

    memset(&irp, 0, sizeof(irp));
    irp.op = IRP_OP_IOCTL;
    irp.fd = *fd;
    irp.ioctl = IOCTL_SERIAL_SET_HANDFLOW;
    irp.write.bytes = (const void *) &handflow;
    irp.write.nbytes = sizeof(handflow);
    handflow.ControlHandShake = 0;
    handflow.FlowReplace = 0;
    handflow.XonLimit = 0;
    handflow.XoffLimit = 0;

    hr = iohook_invoke_next(&irp);

    if (FAILED(hr)) {
        goto fail;
    }

    return S_OK;

fail:
    if (*fd != NULL) {
        irp.op = IRP_OP_CLOSE;
        irp.fd = *fd;

        iohook_invoke_next(&irp);

        *fd = NULL;
    }

    return hr;
}

static HRESULT p4io_uart_close(HANDLE fd)
{
    struct irp irp;
    HRESULT hr;

    if (fd != NULL) {
        memset(&irp, 0, sizeof(irp));
        irp.op = IRP_OP_CLOSE;
        irp.fd = fd;

        hr = iohook_invoke_next(&irp);

        if (FAILED(hr)) {
            log_warning("Error closing port: %x", (int) hr);
        }
    } else {
        log_warning("Port is already closed");
        hr = S_OK;
    }

    return hr;
}

static HRESULT p4io_uart_read(HANDLE fd, struct iobuf *iobuf)
{
    struct irp irp;
    SERIAL_STATUS status;
    HRESULT hr;

    if (fd == NULL) {
        log_warning("Read from unopened port");

        return E_FAIL;
    }

    /* Peek RX buffer */

    memset(&irp, 0, sizeof(irp));
    irp.op = IRP_OP_IOCTL;
    irp.fd = fd;
    irp.ioctl = IOCTL_SERIAL_GET_COMMSTATUS;
    irp.read.bytes = (uint8_t *) &status;
    irp.read.nbytes = sizeof(status);

    hr = iohook_invoke_next(&irp);

    if (FAILED(hr)) {
        log_warning("UART FIFO peek failed: %x", (int) hr);

        return hr;
    }

    /* Return immediately if no data available */

    if (status.AmountInInQueue == 0) {
        return S_OK;
    }

    /* Issue read */

    memset(&irp, 0, sizeof(irp));
    irp.op = IRP_OP_READ;
    irp.fd = fd;
    memcpy(&irp.read, iobuf, sizeof(*iobuf));

    hr = iohook_invoke_next(&irp);

    if (FAILED(hr)) {
        log_warning("Read error: %x", (int) hr);

        return hr;
    }

    memcpy(iobuf, &irp.read, sizeof(*iobuf));

    return S_OK;
}

static HRESULT p4io_uart_write(HANDLE fd, struct const_iobuf *iobuf)
{
    struct irp irp;
    HRESULT hr;

    if (fd == NULL) {
        log_warning("Write to unopened port");

        return E_FAIL;
    }

    memset(&irp, 0, sizeof(irp));
    irp.op = IRP_OP_WRITE;
    irp.fd = fd;
    memcpy(&irp.write, iobuf, sizeof(*iobuf));

    hr = iohook_invoke_next(&irp);

    if (SUCCEEDED(hr)) {
        memcpy(iobuf, &irp.write, sizeof(*iobuf));
    } else {
        log_warning("Write error: %x", (int) hr);
    }

    return hr;
}
