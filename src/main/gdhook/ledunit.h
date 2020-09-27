#ifndef GDHOOK_LEDUNIT_H
#define GDHOOK_LEDUNIT_H

void ledunit_init(uint8_t ledunit_type);
void ledunit_fini(void);
HRESULT ledunit_dispatch_irp(struct irp *irp);

#endif
