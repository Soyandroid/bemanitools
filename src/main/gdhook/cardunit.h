#ifndef GDHOOK_CARDUNIT_H
#define GDHOOK_CARDUNIT_H

void cardunit_init(uint8_t total_cardunit);
void cardunit_fini(void);
HRESULT cardunit_dispatch_irp(struct irp *irp);

#endif
