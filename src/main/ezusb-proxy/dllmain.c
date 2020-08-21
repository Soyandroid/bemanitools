#include <windows.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

int usbCheckAlive() {
    return 1;
}

int usbCheckSecurityNew() {
    return 0;
}

int usbCoinGet() {
    return 0;
}

int usbCoinMode() {
    return 0;
}

int usbEnd() {
    return 0;
}

int usbFirmResult() {
    return 0;
}

int usbGetKEYID() {
    return 0;
}

int usbGetSecurity() {
    return 0;
}

int usbLamp(uint32_t lamp_bits) {
    return 0;
}

int usbPadRead(unsigned int *pad_bits) {
    return 0;
}

int usbPadReadLast(uint8_t *a1) {
    memset(a1, 0, 40);
    return 0;
}

int usbSecurityInit() {
    return 0;
}

int usbSecurityInitDone() {
    return 0;
}

int usbSecuritySearch() {
    return 0;
}

int usbSecuritySearchDone() {
    return 0;
}

int usbSecuritySelect() {
    return 0;
}

int usbSecuritySelectDone() {
    return 0;
}

int usbSetExtIo() {
    return 0;
}

int usbStart() {
    return 0;
}

int usbWdtReset() {
    return 0;
}

int usbWdtStart(int a1) {
    return 0;
}

int usbWdtStartDone() {
    return 0;
}

BOOL WINAPI DllMain(HMODULE mod, DWORD reason, void *ctx)
{
    if (reason == DLL_PROCESS_ATTACH) {

    }

    return TRUE;
}
