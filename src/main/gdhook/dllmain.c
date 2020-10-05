#include <windows.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bemanitools/eamio.h"
#include "bemanitools/gdio.h"

#include "cconfig/cconfig-hook.h"

#include "hook/iohook.h"

#include "hooklib/adapter.h"
#include "hooklib/app.h"
#include "hooklib/rs232.h"
#include "hooklib/setupapi.h"

#include "imports/avs.h"

#include "gdhook/cardunit.h"
#include "gdhook/config-emu.h"
#include "gdhook/config-game.h"
#include "gdhook/config-gfx.h"
#include "gdhook/config-sound.h"
#include "gdhook/d3d9.h"
#include "gdhook/drumunit.h"
#include "gdhook/guitarunit.h"
#include "gdhook/ledunit.h"
#include "gdhook/p4io.h"
#include "gdhook/wasapi.h"

#include "p4ioemu/device.h"
#include "p4ioemu/setupapi.h"
#include "p4ioemu/uart.h"

#include "util/defs.h"
#include "util/log.h"
#include "util/thread.h"

#define GDHOOK_INFO_HEADER \
    "gdhook for GITADORA"  \
    ", build " __DATE__ " " __TIME__ ", gitrev " STRINGIFY(GITREV) "\n"
#define GDHOOK_CMD_USAGE \
    "Usage: launcher.exe -K gdhook.dll <gdxg.dll> [options...]"

static uint8_t game_type;

static struct gdhook_config_emu gdhook_cfg_emu;
static struct gdhook_config_game gdhook_cfg_game;
static struct gdhook_config_gfx gdhook_cfg_gfx;
static struct gdhook_config_sound gdhook_cfg_sound;

static bool my_dll_entry_init(char *sidcode, struct property_node *param)
{
    bool eam_io_ok;
    bool gd_io_ok;
    bool dll_entry_init_result;
    uint8_t cabtype_override;
    char gdhook_param_cmdline[1024];

    uint8_t *my_property_object;
    struct property *p_myparam;
    struct property_node *cmdline;

    uint8_t cardunit_count, guitarunit_flag;

    p_myparam = NULL;
    my_property_object = NULL;
    eam_io_ok = false;
    gd_io_ok = false;

    cardunit_count = 2;
    guitarunit_flag = GDHOOK_GUITARUNIT_ENABLE_GUITAR_UNIT1 |
        GDHOOK_GUITARUNIT_ENABLE_GUITAR_UNIT2;
    game_type = GDHOOK_CONFIG_GAME_GUITAR;

    log_info("--- Begin gdhook dll_entry_init ---");

    /* modify /param here */
    gdhook_param_cmdline[0] = 0;
    switch (sidcode[4]) {
        case 'A':
            strcpy_s(gdhook_param_cmdline, 1024, "-g -GF ");
            game_type = GDHOOK_CONFIG_GAME_GUITAR;
            break;
        case 'B':
            strcpy_s(gdhook_param_cmdline, 1024, "-d -DM ");
            game_type = GDHOOK_CONFIG_GAME_DRUM;
            cardunit_count = 1;
            break;
        default:
            break;
    }

    if (gdhook_cfg_game.is_windowed)
        strcat_s(gdhook_param_cmdline, 1024, "-WINDOW ");

    strcat_s(gdhook_param_cmdline, 1024, gdhook_cfg_game.cmdline_app);
    strcat_s(gdhook_param_cmdline, 1024, " ");

    cmdline = NULL;
    if (param) {
        cmdline = property_search(0, param, "/cmdline");
    }
    if (cmdline) {
        property_node_refer(
            0,
            param,
            "/cmdline",
            PROPERTY_TYPE_STR,
            gdhook_param_cmdline + strlen(gdhook_param_cmdline),
            1024 - strlen(gdhook_param_cmdline));
        property_node_remove(cmdline);
        cmdline = NULL;
    }

    my_property_object = (uint8_t *) malloc(10 * 1024);
    if (my_property_object) {
        p_myparam = property_create(
            PROPERTY_FLAG_READ | PROPERTY_FLAG_WRITE | PROPERTY_FLAG_CREATE |
                PROPERTY_FLAG_APPEND,
            my_property_object,
            10 * 1024);
        cmdline = property_node_create(
            p_myparam,
            NULL,
            PROPERTY_TYPE_STR,
            "/param/cmdline",
            gdhook_param_cmdline);
        if (cmdline) {
            property_node_datasize(cmdline);
        }
        param = property_search(p_myparam, NULL, "/param");
    }

    /* p4ioemu init */
    cabtype_override = 0;
    switch (gdhook_cfg_game.cab_type) {
        case GDHOOK_CONFIG_CABTYPE_XG:
            game_type |= GDHOOK_CONFIG_CABTYPE_XG;
        default:
            break;
        case GDHOOK_CONFIG_CABTYPE_GD:
            cabtype_override |= 0x0C;
            cardunit_count = 1;
            guitarunit_flag = 1;
            game_type |= GDHOOK_CONFIG_CABTYPE_GD;
            break;
        case GDHOOK_CONFIG_CABTYPE_SD:
            cabtype_override |= 0x08;
            game_type |= GDHOOK_CONFIG_CABTYPE_SD;
            break;
    }

    /* add acio device to p4io sci ports */
    p4io_uart_set_path(0, L"COM4");
    p4io_uart_set_path(1, L"COM5");

    if (game_type & GDHOOK_CONFIG_GAME_DRUM) {
        p4ioemu_init(gdhook_p4io_dm_init(cabtype_override));
    } else {
        p4ioemu_init(gdhook_p4io_gf_init(cabtype_override));
    }

    log_info("Starting up GITADORA P4IO backend");

    gd_io_set_loggers(
        log_impl_misc, log_impl_info, log_impl_warning, log_impl_fatal);

    if (game_type & GDHOOK_CONFIG_GAME_DRUM) {
        gd_io_ok = gd_io_dm_init(
            avs_thread_create, avs_thread_join, avs_thread_destroy);
    } else {
        gd_io_ok = gd_io_gf_init(
            avs_thread_create, avs_thread_join, avs_thread_destroy);
    }

    if (!gd_io_ok) {
        goto fail;
    }

    /* cardunit init */

    if (!gdhook_cfg_emu.disable_eamio_emu) {
        cardunit_init(cardunit_count);

        log_info("Starting up card reader backend");

        eam_io_set_loggers(
            log_impl_misc, log_impl_info, log_impl_warning, log_impl_fatal);

        eam_io_ok =
            eam_io_init(avs_thread_create, avs_thread_join, avs_thread_destroy);

        if (!eam_io_ok) {
            goto fail;
        }
    }

    /* ledunit init */
    ledunit_init(game_type);

    log_info("Starting up led unit backend");

    /* game io init */
    guitarunit_flag &= (gdhook_cfg_emu.disable_gf1_emu ?
                            0 :
                            GDHOOK_GUITARUNIT_ENABLE_GUITAR_UNIT1) |
        (gdhook_cfg_emu.disable_gf2_emu ?
             0 :
             GDHOOK_GUITARUNIT_ENABLE_GUITAR_UNIT2);

    if (game_type & GDHOOK_CONFIG_GAME_DRUM) {
        if (!gdhook_cfg_emu.disable_dm_emu) {
            drumunit_init();
            log_info("Starting up drum unit backend");
        }
    } else {
        guitarunit_init(guitarunit_flag);
        log_info("Starting up guitar unit backend, flag %02x", guitarunit_flag);
    }

    log_info("---  End  gdhook dll_entry_init ---");

    dll_entry_init_result = app_hook_invoke_init(sidcode, param);

    if (p_myparam) {
        property_destroy(p_myparam);
    }
    if (my_property_object) {
        free(my_property_object);
    }

    return dll_entry_init_result;

fail:
    if (eam_io_ok) {
        eam_io_fini();
    }

    if (gd_io_ok) {
        gd_io_fini();
    }

    return false;
}

static bool my_dll_entry_main(void)
{
    bool result;

    result = app_hook_invoke_main();

    log_info("Shutting down card reader backend");
    eam_io_fini();

    log_info("Shutting down GITADORA IO backend");
    gd_io_fini();

    if (game_type & GDHOOK_CONFIG_GAME_DRUM) {
        if (!gdhook_cfg_emu.disable_dm_emu) {
            drumunit_fini();
        }
    } else {
        guitarunit_fini();
    }

    ledunit_fini();

    if (!gdhook_cfg_emu.disable_eamio_emu) {
        cardunit_fini();
    }

    p4ioemu_fini();

    return result;
}

BOOL WINAPI DllMain(HMODULE mod, DWORD reason, void *ctx)
{
    struct cconfig *config;

    if (reason != DLL_PROCESS_ATTACH) {
        return TRUE;
    }

    config = cconfig_init();

    gdhook_config_emu_init(config);
    gdhook_config_game_init(config);
    gdhook_config_gfx_init(config);
    gdhook_config_sound_init(config);

    if (!cconfig_hook_config_init(
            config,
            GDHOOK_INFO_HEADER "\n" GDHOOK_CMD_USAGE,
            CCONFIG_CMD_USAGE_OUT_DBG)) {
        cconfig_finit(config);
        exit(EXIT_FAILURE);
    }

    gdhook_config_emu_get(&gdhook_cfg_emu, config);
    gdhook_config_game_get(&gdhook_cfg_game, config);
    gdhook_config_gfx_get(&gdhook_cfg_gfx, config);
    gdhook_config_sound_get(&gdhook_cfg_sound, config);

    cconfig_finit(config);

    gdhook_cfg_gfx.windowed = gdhook_cfg_game.is_windowed;

    log_to_external(
        log_body_misc, log_body_info, log_body_warning, log_body_fatal);

    app_hook_init(my_dll_entry_init, my_dll_entry_main);

    iohook_push_handler(p4ioemu_dispatch_irp);

    if (!gdhook_cfg_emu.disable_eamio_emu) {
        iohook_push_handler(cardunit_dispatch_irp);
    }

    iohook_push_handler(ledunit_dispatch_irp);

    if (!gdhook_cfg_emu.disable_gf1_emu) {
        iohook_push_handler(guitarunit1_dispatch_irp);
    }

    if (!gdhook_cfg_emu.disable_gf2_emu) {
        iohook_push_handler(guitarunit2_dispatch_irp);
    }

    if (!gdhook_cfg_emu.disable_dm_emu) {
        iohook_push_handler(drumunit_dispatch_irp);
    }

    adapter_hook_init();
    rs232_hook_init();

    d3d9_configure(&gdhook_cfg_gfx);
    d3d9_hook_init();

	wasapi_configure(&gdhook_cfg_sound);
    wasapi_hook_init();

    hook_setupapi_init(&p4ioemu_setupapi_data);

    return TRUE;
}
