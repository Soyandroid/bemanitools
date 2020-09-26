#ifndef GDHOOK_LEDUNIT_H
#define GDHOOK_LEDUNIT_H

enum {
	GDHOOK_LEDUNIT_GAME_GUITAR_XG = 0x10,
	GDHOOK_LEDUNIT_GAME_GUITAR_SD = 0x11,
	GDHOOK_LEDUNIT_GAME_GUITAR_GD = 0x12,
	GDHOOK_LEDUNIT_GAME_DRUM_XG = 0x20,
	GDHOOK_LEDUNIT_GAME_DRUM_SD = 0x21,
	GDHOOK_LEDUNIT_GAME_DRUM_GD = 0x22,
};

void ledunit_init(uint8_t ledunit_type);
void ledunit_fini(void);
HRESULT ledunit_dispatch_irp(struct irp *irp);

#endif
