// clang-format off
// Don't format because the order is important here
#include <windows.h>
#include <setupapi.h>
#include <stdlib.h>
// clang-format on

#include "jbio-p4io/jbio.h"
#include "jbio-p4io/p4io.h"
#include "util/str.h"

/* can't seem to #include the requisite DDK headers from usermode code,
   so we have to redefine these macros here */

#define CTL_CODE(DeviceType, Function, Method, Access) \
    (((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method))

#define METHOD_BUFFERED 0

#define FILE_ANY_ACCESS 0x00

#define FILE_DEVICE_UNKNOWN 0x22

#define P4IO_FUNCTION_READ_JAMMA_2 0x801
#define P4IO_FUNCTION_GET_DEVICE_NAME 0x803

// 0x22200Cu sent to bulk handle
#define IOCTL_P4IO_GET_DEVICE_NAME     \
    CTL_CODE(                          \
        FILE_DEVICE_UNKNOWN,           \
        P4IO_FUNCTION_GET_DEVICE_NAME, \
        METHOD_BUFFERED,               \
        FILE_ANY_ACCESS)
// 0x222004 sent to int handle
#define IOCTL_P4IO_READ_JAMMA_2     \
    CTL_CODE(                       \
        FILE_DEVICE_UNKNOWN,        \
        P4IO_FUNCTION_READ_JAMMA_2, \
        METHOD_BUFFERED,            \
        FILE_ANY_ACCESS)

static const GUID p4io_guid = {
    0x8B7250A5,
    0x4F61,
    0x46C9,
    {0x84, 0x3A, 0xE6, 0x68, 0x06, 0x47, 0x6A, 0x20}
};

static HANDLE p4io_handle = INVALID_HANDLE_VALUE;

bool p4io_open_device(void) {
    bool success = false;
    wchar_t p4io_filename[MAX_PATH]; // game uses 1024, but it shouldn't be that long
    PSP_DEVICE_INTERFACE_DETAIL_DATA_W detail_data = NULL;
    HDEVINFO dev_info_set;

    dev_info_set = SetupDiGetClassDevsW(&p4io_guid, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

    if(dev_info_set == INVALID_HANDLE_VALUE) {
        jb_io_log_warning("p4io", "SetupDiGetClassDevs fail, is p4io device connected and driver installed?");
        return false;
    }

    SP_DEVICE_INTERFACE_DATA interface_data;
    interface_data.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    if(!SetupDiEnumDeviceInterfaces(dev_info_set, NULL, &p4io_guid, 0, &interface_data)) {
        jb_io_log_warning("p4io", "SetupDiEnumDeviceInterfaces fail");
        goto CLEANUP;
    }

    DWORD required_size;
    SetupDiGetDeviceInterfaceDetailW(dev_info_set, &interface_data, NULL, 0, &required_size, NULL);

    detail_data = malloc(required_size);
    detail_data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

    if(!SetupDiGetDeviceInterfaceDetailW(dev_info_set, &interface_data, detail_data, required_size, NULL, NULL)) {
        jb_io_log_warning("p4io", "SetupDiGetDeviceInterfaceDetailW fail");
        goto CLEANUP;
    }

    wstr_cpy(p4io_filename, MAX_PATH, detail_data->DevicePath);
    wstr_cat(p4io_filename, MAX_PATH, L"\\p4io");

    jb_io_log_info("p4io", "p4io found at path %ls", p4io_filename);

    p4io_handle = CreateFileW(p4io_filename, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if(p4io_handle == INVALID_HANDLE_VALUE) {
        jb_io_log_warning("p4io", "CreateFileW fail");
        goto CLEANUP;
    }

    success = true;
    jb_io_log_info("p4io", "p4io device opened!");

    CLEANUP:
    free(detail_data);
    SetupDiDestroyDeviceInfoList(dev_info_set);

    return success;
}

void p4io_close_device(void) {
    if(p4io_handle != INVALID_HANDLE_VALUE) {
        CloseHandle(p4io_handle);
    }
    p4io_handle = INVALID_HANDLE_VALUE;
}

bool p4io_print_version(void) {
    char p4io_version[128];

    DWORD bytes_returned;
    if(!DeviceIoControl(p4io_handle, IOCTL_P4IO_GET_DEVICE_NAME, NULL, 0, p4io_version, sizeof(p4io_version), &bytes_returned, NULL)) {
        jb_io_log_warning("p4io", "p4io does not support get_version cmd");
        return false;
    }

    jb_io_log_info("p4io", "p4io version: %s", p4io_version);
    return true;
}

bool p4io_read_jamma2(uint32_t jamma[4]) {
    DWORD bytes_returned;
    if(!DeviceIoControl(p4io_handle, IOCTL_P4IO_READ_JAMMA_2, NULL, 0, jamma, sizeof(uint32_t[4]), &bytes_returned, NULL)) {
        jb_io_log_warning("p4io", "jamma read failed");
        return false;
    }

    return true;
}
