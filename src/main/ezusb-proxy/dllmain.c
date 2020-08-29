#define LOG_MODULE "ezusb-proxy"

#include <windows.h>

#include <dbghelp.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "ezusb2-emu/desc.h"

#include "ezusb-proxy/ezusb.h"

#include "hook/iohook.h"
#include "hook/table.h"

#include "hooklib/setupapi.h"

#include "util/log.h"

static usbCheckAlive_t real_usbCheckAlive;
static usbCheckSecurityNew_t real_usbCheckSecurityNew;
static usbCoinGet_t real_usbCoinGet;
static usbCoinMode_t real_usbCoinMode;
static usbEnd_t real_usbEnd;
static usbFirmResult_t real_usbFirmResult;
static usbGetKEYID_t real_usbGetKEYID;
static usbGetSecurity_t real_usbGetSecurity;
static usbLamp_t real_usbLamp;
static usbPadRead_t real_usbPadRead;
static usbPadReadLast_t real_usbPadReadLast;
static usbSecurityInit_t real_usbSecurityInit;
static usbSecurityInitDone_t real_usbSecurityInitDone;
static usbSecuritySearch_t real_usbSecuritySearch;
static usbSecuritySearchDone_t real_usbSecuritySearchDone;
static usbSecuritySelect_t real_usbSecuritySelect;
static usbSecuritySelectDone_t real_usbSecuritySelectDone;
static usbSetExtIo_t real_usbSetExtIo;
static usbStart_t real_usbStart;
static usbWdtReset_t real_usbWdtReset;
static usbWdtStart_t real_usbWdtStart;
static usbWdtStartDone_t real_usbWdtStartDone;

static DllEntryPoint_t real_DllEntryPoint;

static bool real_ezusb_initialized;

static void init_real_ezusb();

typedef HDEVINFO __stdcall (*SetupDiGetClassDevsA_t)(const GUID *ClassGuid, PCSTR Enumerator, HWND hwndParent, DWORD Flags);

static SetupDiGetClassDevsA_t real_SetupDiGetClassDevsA;

HDEVINFO __stdcall my_SetupDiGetClassDevsA(const GUID *ClassGuid, PCSTR Enumerator, HWND hwndParent, DWORD Flags)
{
    log_warning("??????????????");

    return real_SetupDiGetClassDevsA(ClassGuid, Enumerator, hwndParent, Flags);
}



int usbCheckAlive() {
    log_misc("usbCheckAlive");

    // usbCheckAlive seems to be the first call to the ezusb API
    init_real_ezusb();

    return real_usbCheckAlive();
}

int usbCheckSecurityNew() {
    log_misc("usbCheckSecurityNew");

    return real_usbCheckSecurityNew();
}

int usbCoinGet() {
    log_misc("usbCoinGet");

    return real_usbCoinGet();
}

int usbCoinMode() {
    log_misc("usbCoinMode");

    return real_usbCoinMode();
}

int usbEnd() {
    log_misc("usbEnd");

    return real_usbEnd();
}

int usbFirmResult() {
    log_misc("usbFirmResult");

    return real_usbFirmResult();
}

int usbGetKEYID() {
    log_misc("usbGetKEYID");

    return real_usbGetKEYID();
}

int usbGetSecurity() {
    log_misc("usbGetSecurity");

    return real_usbGetSecurity();
}

int usbLamp(uint32_t lamp_bits) {
    log_misc("usbLamp");

    return real_usbLamp(lamp_bits);
}

int usbPadRead(unsigned int *pad_bits) {
    log_misc("usbPadRead");

    return real_usbPadRead(pad_bits);
}

int usbPadReadLast(uint8_t *a1) {
    log_misc("usbPadReadLast");

    return real_usbPadReadLast(a1);
}

int usbSecurityInit() {
    log_misc("usbSecurityInit");

    return real_usbSecurityInit();
}

int usbSecurityInitDone() {
    log_misc("usbSecurityInitDone");

    return real_usbSecurityInitDone();
}

int usbSecuritySearch() {
    log_misc("usbSecuritySearch");

    return real_usbSecuritySearch();
}

int usbSecuritySearchDone() {
    log_misc("usbSecuritySearchDone");

    return real_usbSecuritySearchDone();
}

int usbSecuritySelect() {
    log_misc("usbSecuritySelect");

    return real_usbSecuritySelect();
}

int usbSecuritySelectDone() {
    log_misc("usbSecuritySelectDone");

    return real_usbSecuritySelectDone();
}

int usbSetExtIo() {
    log_misc("usbSetExtIo");

    return real_usbSetExtIo();
}

int usbStart() {
    log_misc("usbStart");

    return real_usbStart();
}

int usbWdtReset() {
    log_misc("usbWdtReset");

    return real_usbWdtReset();
}

int usbWdtStart(int a1) {
    log_misc("usbWdtStart");

    return real_usbWdtStart(a1);
}

int usbWdtStartDone() {
    log_misc("usbWdtStartDone");

    return real_usbWdtStartDone();
}

static void* get_proc_address(HMODULE module, const char* name)
{
    void* addr;

    log_warning("33333");

    addr = GetProcAddress(module, name);

    log_warning("1234");

    if (!addr) {
        log_fatal("Getting function address %s in ezusb-orig.dll failed: %08lX", name, GetLastError());
    }

    return addr;
}

static void load_real_ezusb_functions()
{
    HMODULE module;

    module = LoadLibraryEx("ezusb-orig.dll", NULL, DONT_RESOLVE_DLL_REFERENCES);

    log_warning("**** %p", module);

    // This triggers a call to DllMain
    // module = LoadLibraryA("ezusb-orig.dll");

    // log_warning("111**** %p", module);

    if (!module) {
        log_fatal("Could not load ezusb-orig.dll (%08lX).", GetLastError());
    }

    // real_usbCheckAlive = (usbCheckAlive_t) get_proc_address(module, "usbCheckAlive");
    // real_usbCheckSecurityNew = (usbCheckSecurityNew_t) get_proc_address(module, "usbCheckSecurityNew");
    // real_usbCoinGet = (usbCoinGet_t) get_proc_address(module, "usbCoinGet");
    // real_usbCoinMode = (usbCoinMode_t) get_proc_address(module, "usbCoinMode");
    // real_usbEnd = (usbEnd_t) get_proc_address(module, "usbEnd");
    // real_usbFirmResult = (usbFirmResult_t) get_proc_address(module, "usbFirmResult");
    // real_usbGetKEYID = (usbGetKEYID_t) get_proc_address(module, "usbGetKEYID");
    // real_usbGetSecurity = (usbGetSecurity_t) get_proc_address(module, "usbGetSecurity");
    // real_usbLamp = (usbLamp_t) get_proc_address(module, "usbLamp");
    // real_usbPadRead = (usbPadRead_t) get_proc_address(module, "usbPadRead");
    // real_usbPadReadLast = (usbPadReadLast_t) get_proc_address(module, "usbPadReadLast");
    // real_usbSecurityInit = (usbSecurityInit_t) get_proc_address(module, "usbSecurityInit");
    // real_usbSecurityInitDone = (usbSecurityInitDone_t) get_proc_address(module, "usbSecurityInitDone");
    // real_usbSecuritySearch = (usbSecuritySearch_t) get_proc_address(module, "usbSecuritySearch");
    // real_usbSecuritySearchDone = (usbSecuritySearchDone_t) get_proc_address(module, "usbSecuritySearchDone");
    // real_usbSecuritySelect = (usbSecuritySelect_t) get_proc_address(module, "usbSecuritySelect");
    // real_usbSecuritySelectDone = (usbSecuritySelectDone_t) get_proc_address(module, "usbSecuritySelectDone");
    // real_usbSetExtIo = (usbSetExtIo_t) get_proc_address(module, "usbSetExtIo");
    // real_usbStart = (usbStart_t) get_proc_address(module, "usbStart");
    // real_usbWdtReset = (usbWdtReset_t) get_proc_address(module, "usbWdtReset");
    // real_usbWdtStart = (usbWdtStart_t) get_proc_address(module, "usbWdtStart");
    // real_usbWdtStartDone = (usbWdtStartDone_t) get_proc_address(module, "usbWdtStartDone");

    log_warning("!!!!!");

    typedef BOOL(WINAPI *DllMainFunPtr)(HMODULE, DWORD, LPVOID);

    PIMAGE_NT_HEADERS header = ImageNtHeader(module);

    DllMainFunPtr dllMain = (DllMainFunPtr)(header->OptionalHeader.AddressOfEntryPoint + (DWORD_PTR)module);



    // real_DllEntryPoint = (DllEntryPoint_t) get_proc_address(module, "DllEntryPoint");

    log_warning("Calling DLLMain....: %p", dllMain);

    BOOL res = dllMain(module, DLL_PROCESS_ATTACH, 0);

    log_warning("res DLLMain: %d", res);
}

static void init_real_ezusb()
{
    if (!real_ezusb_initialized) {
        real_ezusb_initialized = true;

        log_info("Initalizing real ezusb");

        load_real_ezusb_functions();
    }
}

static const struct hook_symbol init_hook_syms[] = {
    {.name = "SetupDiGetClassDevsA",
     .patch = my_SetupDiGetClassDevsA,
     .link = (void **) &real_SetupDiGetClassDevsA},
};

BOOL WINAPI DllMain(HMODULE mod, DWORD reason, void *ctx)
{
    if (reason != DLL_PROCESS_ATTACH) {
        return TRUE;
    }

    log_to_writer(log_writer_debug, NULL);

    log_misc("DllMain");

    SetupDiGetClassDevsA(NULL, "asdf", NULL, 1);

    // hook_table_apply(
    //         NULL, "setupapi.dll", init_hook_syms, lengthof(init_hook_syms));

    // log_warning("proxy SetupDiGetClassDevsA: real %p hook %p", real_SetupDiGetClassDevsA, my_SetupDiGetClassDevsA);

    return TRUE;
}
