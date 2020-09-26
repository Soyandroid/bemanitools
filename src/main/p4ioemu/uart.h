#ifndef P4IO_UART_H
#define P4IO_UART_H

#include "p4io/cmd.h"

void p4io_uart_set_path(size_t uart_no, const wchar_t *path);

void p4io_uart_cmd_open(const struct p4io_sci_open_req *req);

void p4io_uart_cmd_break(const struct p4io_sci_break_req *req);

uint8_t p4io_uart_cmd_update(
    const struct p4io_sci_update_req *req,
    uint8_t sz_req,
    struct p4io_sci_update_resp *resp);

#endif
