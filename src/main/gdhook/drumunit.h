#ifndef GDHOOK_DRUMUNIT_H
#define GDHOOK_DRUMUNIT_H

void drumunit_init(void);
void drumunit_fini(void);
HRESULT drumunit_dispatch_irp(struct irp *irp);

#endif
