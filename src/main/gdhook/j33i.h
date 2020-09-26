#ifndef GDHOOK_J33I_H
#define GDHOOK_J33I_H

#include <windows.h>

#include "hook/iohook.h"

void guitar_xg_init(void);
void guitar_xg_fini(void);
HRESULT guitar_xg_dispatch_irp(struct irp *irp);

#endif
