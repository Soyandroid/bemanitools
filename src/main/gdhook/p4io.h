#ifndef GDHOOK_P4IO_H
#define GDHOOK_P4IO_H

#include "p4io/cmd.h"
#include "p4ioemu/device.h"

const struct p4ioemu_device_msg_hook *gdhook_p4io_gf_init(uint8_t cabtype);
const struct p4ioemu_device_msg_hook *gdhook_p4io_dm_init(uint8_t cabtype);

#endif