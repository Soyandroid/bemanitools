#ifndef JBHOOK1_ACIO_H
#define JBHOOK1_ACIO_H

#include "hook/iohook.h"

/**
 * Initialize the ACIO backend for jubeat on the specified COM port.
 */
void jbhook_util_ac_io_port_init(const wchar_t *filename);

/**
 * Shutdown the ACIO backend.
 */
void jbhook_util_ac_io_port_fini(void);

/**
 * ACIO backend dispatch irp function. This needs to be hooked up to the iohook
 * module in order to receive system calls to dispatch for emulation.
 */
HRESULT jbhook_util_ac_io_port_dispatch_irp(struct irp *irp);

#endif
