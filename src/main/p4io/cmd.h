#ifndef P3IO_CMD_H
#define P3IO_CMD_H

#include <stdint.h>

#include "p3io/frame.h"


enum {
    P4IO_CMD_INIT = 0x00,
    P4IO_CMD_GET_DEVICE_INFO = 0x01,
    P4IO_CMD_SET_PORTOUT = 0x12,
    P4IO_CMD_REQ_COINSTOCK = 0x18,
    P4IO_CMD_SET_WATCHDOG = 0x1C,
    P4IO_CMD_SCI_MNG_OPEN = 0x20,
    P4IO_CMD_SCI_UPDATE = 0x21,
    /* SCI = serial communication interface */
    P4IO_CMD_SCI_MNG_BREAK = 0x24,
    /* Read round plug id over one-wire */
    P4IO_CMD_DALLAS_READ_ID = 0x40,
    /* Read round plug mem over one-wire */
    P4IO_CMD_DALLAS_READ_MEM = 0x41
};

enum {
    P4IO_RS232_BAUD_9600 = 0x00,
    P4IO_RS232_BAUD_19200 = 0x01,
    P4IO_RS232_BAUD_38400 = 0x02,
    P4IO_RS232_BAUD_57600 = 0x03,
    P4IO_RS232_BAUD_115200 = 0x04,
};

#pragma pack(push, 1)
struct p4io_cmd_package {
    uint8_t header_AA;
    uint8_t cmd;
    uint8_t seq_num;
    uint8_t payload_len;
};

struct p4io_read_roundplug_req {
    /* 0 = black, 1 = white */
    uint8_t type;
};

struct p4io_sci_open_req {
    /* 0 or 1 since there're only 2 ports on p4io */
    uint8_t port_sci;
    /* 0 <= baud <= 4 */
    uint8_t baud_sci;
    /* 0 / 2 / 3 ? */
    uint8_t unknown_sci;
};

struct p4io_sci_break_req {
    /* 0 or 1 since there're only 2 ports on p4io */
    uint8_t port_sci;
    /* 0 or 1? */
    uint8_t break_sci;
};

struct p4io_sci_update_req {
    /* 0 or 1 since there're only 2 ports on p4io */
    uint8_t port_sci;
    /* data here */
    const uint8_t payload_sci[1];
};

union p4io_req_any {
    struct p4io_read_roundplug_req read_roundplug;
    struct p4io_sci_open_req sci_open;
    struct p4io_sci_break_req sci_break;
    struct p4io_sci_update_req sci_update;
    uint8_t raw[64];
};

struct p4io_sci_update_resp {
    uint8_t port_sci;
    uint8_t break_sci;
    uint8_t payload_sci[1];
};

struct p4io_device_info_resp {
    uint32_t type;
    uint8_t flag;
    uint8_t version_major;
    uint8_t version_minor;
    uint8_t version_revision;
    char product_code[4];
    char build_date[16];
    char build_time[16];
};

union p4io_resp_any {
    struct p4io_device_info_resp device_info;
    struct p4io_sci_update_resp sci_update;
    uint8_t raw[64];
};

#pragma pack(pop)

//uint8_t p3io_req_cmd(const union p3io_req_any *src);
//
//void p3io_resp_init(
//    struct p3io_hdr *dest, size_t nbytes, const struct p3io_hdr *req);

#endif
