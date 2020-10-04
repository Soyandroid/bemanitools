#ifndef JBHOOK1_P3IO_H
#define JBHOOK1_P3IO_H

#include <windows.h>

#include "hook/iohook.h"

#include "security/id.h"
#include "security/mcode.h"

/**
 * Initialize the jb01-04 specific p3io emulation backend.
 *
 * @param mcode Mcode of the target game to run. Required for dongle emulation.
 * @param pcbid PCBDID
 * @param eamid EAMID
 */
void jbhook_util_p3io_init(
    const struct security_mcode *mcode,
    const struct security_id *pcbid,
    const struct security_id *eamid);

/**
 * Early (02-04) versions of jubeat do irritating things with the security
 * plugs. Inside dll_entry_init they will modify the sidcode to have a different
 * spec if the game is an append version, then cache all the security dongle
 * values (due to the way device.dll works). Later on, they check the cached
 * security code against the modified one.
 *
 * When entering dll_entry_init, set the sidcode buffer to return the correct
 * spec for the cached values. When leaving dll_entry_init, set the sidcode
 * buffer to NULL to avoid accidentally dereferencing a stale pointer.
 *
 * @param sidcode sidcode to generate security plug responses from, or NULL
 */
void jbhook_util_p3io_set_sidcode(char *sidcode);

/**
 * Shutdown the p3io emulation backend.
 */
void jbhook_util_p3io_fini(void);

#endif
