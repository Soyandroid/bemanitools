#include <windows.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "hook/iohook.h"
#include "hook/table.h"

#include "imports/avs.h"

#include "iidxhook-util/eamuse.h"
#include "iidxhook-util/settings.h"

#include "pnmhook1/d3d9.h"

#include "util/defs.h"
#include "util/log.h"
#include "util/signal.h"
#include "util/str.h"

// TODOs
// - Write debug hook module that hooks Open call and logs to logger for debugging, make this easy
// to switch on/off on games
// - Write a path redirect module similar to pumptools -> can replace the settings module in iidxhook-utils
// - Implement proper proxy ezusb.dll
// - Add RtEffects stub dll
// - Add swapping of ezusb and RtEffects files to startup bat script
// - Make iidxhook-utils re-usable for popn
// - switch to process_hijack_startup once the missing bit inside it is implemented for XP support

typedef int (*ShowCursor_t)(BOOL bShow);
static ShowCursor_t real_ShowCursor;

static bool hook_initialized;

typedef HANDLE (*FindFirstFileA_t)(
  LPCSTR             lpFileName,
  LPWIN32_FIND_DATAA lpFindFileData
);

static FindFirstFileA_t real_FindFirstFileA;

HANDLE my_FindFirstFileA(
  LPCSTR             lpFileName,
  LPWIN32_FIND_DATAA lpFindFileData
)
{
    log_warning("&&& %s", lpFileName);

    HANDLE hnd;

    if (!strncmp(lpFileName, "D:\\popn15", 9)) {
        char curDir[1024];

        GetCurrentDirectoryA(1024, curDir);

        log_info("cur dir: %s", curDir);

        char buffer[1024];

        memset(buffer, 0, 1024);
        //strcpy(buffer, "..\\..");
        // TODO super hacky
        const char* path = "Z:\\SharedFolder\\popn\\15\\popn15\\d\\popn15";
        int len_path = strlen(path);
        strcpy(buffer, "Z:\\SharedFolder\\popn\\15\\popn15\\d\\popn15");
        strcpy(buffer + len_path, lpFileName + 9);

        log_info("redirect data: %s", buffer);

        hnd = real_FindFirstFileA(buffer, lpFindFileData);
    } else {
        hnd = real_FindFirstFileA(lpFileName, lpFindFileData);
    }

    log_warning("<<<< %p", hnd);

    return hnd;
}

HRESULT iohook_invoke_next_debug(struct irp *irp)
{
    // TODO this solution is not great, re-write and cleanup
    if (irp->op == IRP_OP_OPEN) {
        char* buf;

        wstr_narrow(irp->open_filename, &buf);

        log_warning("*** open: %s", buf);

        if (!strncmp(buf, "D:\\popn15", 9)) {
            char curDir[1024];

            GetCurrentDirectoryA(1024, curDir);

            log_info("cur dir: %s", curDir);

            char buffer[1024];

            memset(buffer, 0, 1024);
            strcpy(buffer, "..\\..");
            strcpy(buffer + 5, buf + 9);

            log_info("redirect data: %s", buffer);

            wchar_t* new_path = str_widen(buffer);

            irp->open_filename = new_path;

            HRESULT hr = iohook_invoke_next(irp);

            free(new_path);
            free(buf);

            log_info("result: %ld %ld", hr, S_OK);

            return hr;
        }

        free(buf);
    }

    return iohook_invoke_next(irp);
}

static const hook_d3d9_irp_handler_t iidxhook_d3d9_handlers[] = {
    iidxhook_util_d3d9_irp_handler,
};



static void
iidxhook3_setup_d3d9_hooks()
{
    struct iidxhook_util_d3d9_config d3d9_config;

    iidxhook_util_d3d9_init_config(&d3d9_config);

    d3d9_config.windowed = true;
    d3d9_config.framed = true;
    // d3d9_config.override_window_width = -1;
    // d3d9_config.override_window_height = -1;
    // d3d9_config.framerate_limit = config_gfx->frame_rate_limit;
    // d3d9_config.pci_vid = config_gfx->pci_id_vid;
    // d3d9_config.pci_pid = config_gfx->pci_id_pid;

    /* Required for GOLD (and newer?) to not crash with NVIDIA cards */
    // d3d9_config.iidx11_to_17_fix_uvs_bg_videos = false;
    // d3d9_config.iidx14_to_19_nvidia_fix = false;

    // d3d9_config.scale_back_buffer_width = config_gfx->scale_back_buffer_width;
    // d3d9_config.scale_back_buffer_height = config_gfx->scale_back_buffer_height;
    // d3d9_config.scale_back_buffer_filter = config_gfx->scale_back_buffer_filter;
    // d3d9_config.forced_refresh_rate = config_gfx->forced_refresh_rate;
    // d3d9_config.device_adapter = config_gfx->device_adapter;

    // if (config_gfx->monitor_check == 0) {
    //     log_info("Auto monitor check enabled");

    //     d3d9_config.iidx09_to_19_monitor_check_cb =
    //         iidxhook_util_chart_patch_set_refresh_rate;
    //     iidxhook_util_chart_patch_init(
    //         IIDXHOOK_UTIL_CHART_PATCH_TIMEBASE_14_TO_18_VGA);
    // } else if (config_gfx->monitor_check > 0) {
    //     log_info(
    //         "Manual monitor check, resulting refresh rate: %f",
    //         config_gfx->monitor_check);

    //     iidxhook_util_chart_patch_init(
    //         IIDXHOOK_UTIL_CHART_PATCH_TIMEBASE_14_TO_18_VGA);
    //     iidxhook_util_chart_patch_set_refresh_rate(config_gfx->monitor_check);
    // }

    iidxhook_util_d3d9_configure(&d3d9_config);

    hook_d3d9_init(iidxhook_d3d9_handlers, lengthof(iidxhook_d3d9_handlers));
}

static int my_ShowCursor(BOOL bShow)
{
    log_misc("my_ShowCursor");

    // This function is called multiple times but is the earliest entry point for us to setup stuff
    // that requires configuration
    if (!hook_initialized) {
        log_info("-------------------------------------------------------------");
        log_info("---------------- Begin pnmhook my_ShowCursor ----------------");
        log_info("-------------------------------------------------------------");

        hook_initialized = true;

        log_info("!!!!!!!!!!!!!!");

        iidxhook3_setup_d3d9_hooks();

        iohook_push_handler(settings_hook_dispatch_irp);
        iohook_push_handler(iohook_invoke_next_debug);

        eamuse_hook_init();

        struct net_addr address;
        address.type = NET_ADDR_TYPE_IPV4;
        address.ipv4.addr = 0x0100007F;
        address.ipv4.port = 5730;

        eamuse_set_addr(&address);

        log_info("-------------------------------------------------------------");
        log_info("----------------- End pnmhook my_ShowCursor -----------------");
        log_info("-------------------------------------------------------------");
    }

    return real_ShowCursor(bShow);
}

static const struct hook_symbol init_hook_syms[] = {
    {.name = "ShowCursor",
     .patch = my_ShowCursor,
     .link = (void **) &real_ShowCursor},
};

static const struct hook_symbol init_hook_syms2[] = {
    {.name = "FindFirstFileA",
     .patch = my_FindFirstFileA,
     .link = (void **) &real_FindFirstFileA},
};

BOOL WINAPI DllMain(HMODULE mod, DWORD reason, void *ctx)
{
    if (reason == DLL_PROCESS_ATTACH) {
        log_to_writer(log_writer_debug, NULL);

        signal_exception_handler_init();

        hook_table_apply(
            NULL, "USER32.dll", init_hook_syms, lengthof(init_hook_syms));
                hook_table_apply(
            NULL, "Kernel32.dll", init_hook_syms2, lengthof(init_hook_syms2));

        settings_hook_init();
    }

    return TRUE;
}
