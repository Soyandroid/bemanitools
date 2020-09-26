// clang-format off
// Don't format because the order is important here
#include <windows.h>
#include <ntdef.h>
#include <devioctl.h>
#include <ntddser.h>
// clang-format on

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>

#include "acioemu/addr.h"
#include "acioemu/emu.h"
#include "acioemu/iccb.h"

#include "hook/iohook.h"

#include "gdhook/cardunit.h"

#include "imports/avs.h"

#include "util/defs.h"
#include "util/hex.h"
#include "util/iobuf.h"
#include "util/log.h"
#include "util/str.h"

static struct ac_io_emu ac_io_emu;
static struct ac_io_emu_iccb ac_io_emu_iccb[2];
static uint8_t ac_io_cardunit_cnt;

void cardunit_init(uint8_t total_cardunit)
{
    ac_io_emu_init(&ac_io_emu, L"COM4");
    ac_io_cardunit_cnt = total_cardunit;
    for (int i = 0; i < lengthof(ac_io_emu_iccb) && i < ac_io_cardunit_cnt; i++)
		ac_io_emu_iccb_init(&ac_io_emu_iccb[i], &ac_io_emu, i);
}

void cardunit_fini(void)
{
    ac_io_emu_fini(&ac_io_emu);
}

HRESULT
cardunit_dispatch_irp(struct irp *irp)
{
    const struct ac_io_message *msg;
    HRESULT hr;

    log_assert(irp != NULL);

    if (!ac_io_emu_match_irp(&ac_io_emu, irp)) {
        return iohook_invoke_next(irp);
    }

    for (;;) {
        hr = ac_io_emu_dispatch_irp(&ac_io_emu, irp);

        if (hr != S_OK) {
            return hr;
        }

        msg = ac_io_emu_request_peek(&ac_io_emu);

        switch (msg->addr) {
            case 0:
                ac_io_emu_cmd_assign_addrs(&ac_io_emu, msg, ac_io_cardunit_cnt);

                break;

            case 1:
                ac_io_emu_iccb_dispatch_request(&ac_io_emu_iccb[0], msg);

                break;

            case 2:
                ac_io_emu_iccb_dispatch_request(&ac_io_emu_iccb[1], msg);

                break;

            case AC_IO_BROADCAST:
                log_warning("Broadcast(?) message on GITADORA cardunit ACIO bus?");

                break;

            default:
                log_warning(
                    "ACIO message on unhandled bus address: %d", msg->addr);

                break;
        }

        ac_io_emu_request_pop(&ac_io_emu);
    }
}
