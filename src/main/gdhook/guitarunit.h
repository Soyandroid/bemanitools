#ifndef GDHOOK_GUITARUNIT_H
#define GDHOOK_GUITARUNIT_H

void guitarunit_init(uint8_t ledunit_type);
void guitarunit_fini(void);
HRESULT guitarunit1_dispatch_irp(struct irp *irp);
HRESULT guitarunit2_dispatch_irp(struct irp *irp);

#endif
