#define LOG_MODULE "gdhook-p4io"

#include <string.h>

#include "bemanitools/gdio.h"

#include "imports/avs.h"

#include "gdhook/p4io.h"

#include "util/log.h"

static uint8_t gdhook_io_p4io_cabtype_override;

enum gdhook_io_p4io_command {
    GDHOOK_IO_P4IO_CMD_OUTPUTS = 0x18,
};

struct gdhook_io_p4io_outputs {
    uint32_t outputs;
};

static void gdhook_io_jamma2_read(void *resp, uint32_t nbytes);
static void gdhook_io_setled_gf(const void *portout, uint32_t portout_len);
static void gdhook_io_setled_dm(const void *portout, uint32_t portout_len);

static const struct p4ioemu_device_msg_hook gdhook_io_gf_msg = {
    .jamma2_read = gdhook_io_jamma2_read,
    .command_handle = NULL,
    .roundplug_read_id = NULL,
    .roundplug_read_mem = NULL,
    .get_coinstock = NULL,
    .set_portout = gdhook_io_setled_gf};

static const struct p4ioemu_device_msg_hook gdhook_io_dm_msg = {
    .jamma2_read = gdhook_io_jamma2_read,
    .command_handle = NULL,
    .roundplug_read_id = NULL,
    .roundplug_read_mem = NULL,
    .get_coinstock = NULL,
    .set_portout = gdhook_io_setled_dm};

/*
** p4io device input:
**                 bit8|bit7|bit6    |bit5   |bit4|bit3|bit2    |bit1
**       data[ 0]: EFF2|EFF1|HELP    |RIGHT  |LEFT|DOWN|UP      |START (1P)
**       data[ 1]: EFF2|EFF1|HELP    |RIGHT  |LEFT|DOWN|UP      |START (2P)
**       data[ 2]: 0   |0   |2P_PEDAL|2P_EFF3|0   |0   |1P_PEDAL|1P_EFF3
**       data[ 3]: 0   |0   |0       |TEST   |S2  |C2  |SERVICE |COIN
**
*/

static const uint32_t gdhook_io_sys_button_mappings[] = {
    (1 << 28),
    (1 << 25),
};

static void gdhook_io_jamma2_read(void *resp, uint32_t nbytes)
{
    uint8_t *inputs = (uint8_t *) resp;
    uint16_t panels[2];
    uint8_t buttons;

    if (!gd_io_read_p4io_inputs()) {
        log_warning("Reading p4io inputs from gdio failed");
        return;
    }

	memset(inputs, 0, 4);

    panels[0] = gd_io_get_gf_panel_inputs(0);
    panels[1] = gd_io_get_gf_panel_inputs(1);
    buttons = gd_io_get_sys_inputs();

	inputs[0] = panels[0] & 0xff;
	inputs[1] = panels[1] & 0xff;
    inputs[2] = ((panels[0] >> 8) & 0x03) | ((panels[1] >> 4) & 0x30);

    for (uint8_t i = 0; i < 2; i++) {
        if (buttons & (1 << i)) {
            *(uint32_t *)inputs |= gdhook_io_sys_button_mappings[i];
        }
    }

	/* 2 wow pedals are reading xored value */
	inputs[2] ^= 0x22;

	/* override cab type */
    inputs[3] |= gdhook_io_p4io_cabtype_override;

}

/*
	GF:
	P4PORTOUT LED
	PLAYER 1 START: BYTE 02
	PLAYER 1 UP+DOWN: BYTE 00
	PLAYER 1 LEFT+RIGHT: BYTE 01
	PLAYER 1 HELP: BYTE 03
	PLAYER 2 START: BYTE 06
	PLAYER 2 UP+DOWN: BYTE 04
	PLAYER 2 LEFT+RIGHT: BYTE 05
	PLAYER 2 HELP: BYTE 07

	DM:
	P4PORTOUT LED
	PAD LEFT CYMBAL: BYTE 08
	PAD HI-HAT: BYTE 10
	PAD HIGH TOM: BYTE 04
	PAD SNARE: BYTE 06
	PAD LOW TOM: BYTE 05
	PAD FLOOR TOM: BYTE 07
	PAD RIGHT CYMBAL: BYTE 09
	PLAYER START: BYTE 02
	PLAYER UP+DOWN: BYTE 00
	PLAYER LEFT+RIGHT: BYTE 01
	PLAYER HELP: BYTE 03
*/

static void gdhook_io_setled_gf(const void *portout, uint32_t portout_len)
{
    uint8_t *led_state = (uint8_t *) portout;

    if (portout_len != 16) {
        return;
    }

	gd_io_set_light(GD_IO_LIGHT_PANEL_START_P1, led_state[0x02]);
    gd_io_set_light(GD_IO_LIGHT_PANEL_UD_P1, led_state[0x00]);
    gd_io_set_light(GD_IO_LIGHT_PANEL_LR_P1, led_state[0x01]);
    gd_io_set_light(GD_IO_LIGHT_PANEL_HELP_P1, led_state[0x03]);

	gd_io_set_light(GD_IO_LIGHT_PANEL_START_P2, led_state[0x06]);
    gd_io_set_light(GD_IO_LIGHT_PANEL_UD_P2, led_state[0x04]);
    gd_io_set_light(GD_IO_LIGHT_PANEL_LR_P2, led_state[0x05]);
    gd_io_set_light(GD_IO_LIGHT_PANEL_HELP_P2, led_state[0x07]);
}

static void gdhook_io_setled_dm(const void *portout, uint32_t portout_len)
{
    uint8_t *led_state = (uint8_t *) portout;

    if (portout_len != 16) {
        return;
    }

    gd_io_set_light(GD_IO_LIGHT_PANEL_START_P1, led_state[0x02]);
    gd_io_set_light(GD_IO_LIGHT_PANEL_UD_P1, led_state[0x00]);
    gd_io_set_light(GD_IO_LIGHT_PANEL_LR_P1, led_state[0x01]);
    gd_io_set_light(GD_IO_LIGHT_PANEL_HELP_P1, led_state[0x03]);

    gd_io_set_light(GD_IO_LIGHT_DM_LEFT_CYMBAL, led_state[0x08]);
    gd_io_set_light(GD_IO_LIGHT_DM_HIHAT, led_state[0x0A]);
    gd_io_set_light(GD_IO_LIGHT_DM_HIGH_TOM, led_state[0x04]);
    gd_io_set_light(GD_IO_LIGHT_DM_SNARE, led_state[0x06]);
    gd_io_set_light(GD_IO_LIGHT_DM_LOW_TOM, led_state[0x05]);
    gd_io_set_light(GD_IO_LIGHT_DM_FLOOR_TOM, led_state[0x07]);
    gd_io_set_light(GD_IO_LIGHT_DM_RIGHT_CYMBAL, led_state[0x09]);
}

const struct p4ioemu_device_msg_hook *gdhook_p4io_gf_init(uint8_t cabtype)
{
    /* for switching to sdcab or gitadora cab */
    gdhook_io_p4io_cabtype_override = cabtype;

    return &gdhook_io_gf_msg;
}

const struct p4ioemu_device_msg_hook *gdhook_p4io_dm_init(uint8_t cabtype)
{
    /* for switching to sdcab or gitadora cab */
    gdhook_io_p4io_cabtype_override = cabtype;

    return &gdhook_io_dm_msg;
}
