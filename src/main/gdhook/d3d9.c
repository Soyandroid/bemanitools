#define LOG_MODULE "d3d9-hook"

#include <d3d9.h>
#include <d3dx9core.h>
#include <windows.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "hook/com-proxy.h"
#include "hook/table.h"

#include "gdhook/d3d9.h"

#include "util/defs.h"
#include "util/log.h"
#include "util/str.h"
#include "util/time.h"

/* ------------------------------------------------------------------------- */

static HWND STDCALL my_CreateWindowExA(
    DWORD dwExStyle,
    LPCSTR lpClassName,
    LPCSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam);

static HRESULT STDCALL my_CreateDevice(
    IDirect3D9 *self,
    UINT adapter,
    D3DDEVTYPE type,
    HWND hwnd,
    DWORD flags,
    D3DPRESENT_PARAMETERS *pp,
    IDirect3DDevice9 **pdev);

static IDirect3D9 *STDCALL my_Direct3DCreate9(UINT sdk_ver);

static WNDPROC real_WndProc;

static BOOL STDCALL
my_MoveWindow(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint);

static void calc_win_size_with_framed(
    HWND hwnd, DWORD x, DWORD y, DWORD width, DWORD height, LPWINDOWPOS wp);

static HWND(STDCALL *real_CreateWindowExA)(
    DWORD dwExStyle,
    LPCSTR lpClassName,
    LPCSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam);

static IDirect3D9 *(STDCALL *real_Direct3DCreate9)(UINT sdk_ver);

static BOOL(STDCALL *real_MoveWindow)(
    HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint);

/* ------------------------------------------------------------------------- */

static bool d3d9_windowed;
static bool d3d9_confined;
static bool d3d9_window_framed;

/* ------------------------------------------------------------------------- */

static const struct hook_symbol d3d9_hook_syms[] = {
    {.name = "Direct3DCreate9",
     .patch = my_Direct3DCreate9,
     .link = (void **) &real_Direct3DCreate9},
};

static const struct hook_symbol d3d9_hook_user32_syms[] = {
    {.name = "CreateWindowExA",
     .patch = my_CreateWindowExA,
     .link = (void **) &real_CreateWindowExA},
    {.name = "MoveWindow",
     .patch = my_MoveWindow,
     .link = (void **) &real_MoveWindow},
};

/* ------------------------------------------------------------------------- */

static HWND STDCALL my_CreateWindowExA(
    DWORD dwExStyle,
    LPCSTR lpClassName,
    LPCSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam)
{
    if (d3d9_windowed && d3d9_window_framed) {
        /* use a different style */
        dwStyle |= WS_OVERLAPPEDWINDOW;
        /* also show mouse cursor */
        ShowCursor(TRUE);
    }

    HWND hwnd = real_CreateWindowExA(
        dwExStyle,
        lpClassName,
        lpWindowName,
        dwStyle,
        X,
        Y,
        nWidth,
        nHeight,
        hWndParent,
        hMenu,
        hInstance,
        lpParam);

    return hwnd;
}

static BOOL STDCALL
my_MoveWindow(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint)
{
    if (d3d9_windowed && d3d9_window_framed) {
        /* we have to adjust the window size, because the window needs to be a
           slightly bigger than the rendering resolution (window caption and
           stuff is included in the window size) */

        WINDOWPOS wp;
        calc_win_size_with_framed(hWnd, X, Y, nWidth, nHeight, &wp);
        SetWindowPos(hWnd, 0, wp.x, wp.y, wp.cx, wp.cy, 0);
        X = wp.x;
        Y = wp.y;
        nWidth = wp.cx;
        nHeight = wp.cy;
    }

    BOOL result = real_MoveWindow(hWnd, X, Y, nWidth, nHeight, bRepaint);

    return result;
}

static LRESULT gfx_confine(HWND hwnd)
{
    POINT p;
    RECT r;

    log_misc("Confining mouse (ALT-TAB to release)");

    p.x = 0;
    p.y = 0;

    ClientToScreen(hwnd, &p);

    r.left = p.x;
    r.top = p.y;
    r.right = p.x + 100;
    r.bottom = p.y + 100;

    ClipCursor(&r);

    return TRUE;
}

static LRESULT gfx_unconfine(HWND hwnd)
{
    log_misc("Un-confining mouse");

    ClipCursor(NULL);

    return TRUE;
}

static LRESULT CALLBACK
my_WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
        case WM_SETFOCUS:
            return gfx_confine(hwnd);

        case WM_KILLFOCUS:
            return gfx_unconfine(hwnd);

        default:
            return CallWindowProc(real_WndProc, hwnd, msg, wparam, lparam);
    }
}

static HRESULT WINAPI my_CreateDevice(
    IDirect3D9 *direct3d,
    UINT adapter,
    D3DDEVTYPE type,
    HWND hwnd,
    DWORD flag,
    D3DPRESENT_PARAMETERS *pp,
    IDirect3DDevice9 **pdev)
{
    IDirect3D9 *real = com_proxy_downcast(direct3d)->real;
    HRESULT hr;


    if (d3d9_windowed) {
        pp->Windowed = TRUE;
    }

    hr = IDirect3D9_CreateDevice(real, adapter, type, hwnd, flag, pp, pdev);

    if (SUCCEEDED(hr) && d3d9_confined) {
        real_WndProc = (void *) GetWindowLongPtr(hwnd, GWLP_WNDPROC);

        SetWindowLongPtr(hwnd, GWLP_WNDPROC, (uintptr_t) my_WndProc);
    }

    return hr;
}

static IDirect3D9 *STDCALL my_Direct3DCreate9(UINT sdk_ver)
{
    HRESULT hr;
    IDirect3D9Vtbl *api_vtbl;
    struct com_proxy *api_proxy;
    IDirect3D9 *api_, *ret;

    log_info("Direct3DCreate9 hook hit");

    api_ = real_Direct3DCreate9(sdk_ver);

    hr = com_proxy_wrap(&api_proxy, api_, sizeof(*api_->lpVtbl));

    if (hr != S_OK) {
        log_warning("Wrapping com proxy failed: %08lx", hr);
        return api_;
    }

    api_vtbl = api_proxy->vptr;

    api_vtbl->CreateDevice = my_CreateDevice;

    ret = (IDirect3D9 *) api_proxy;

    return ret;
}

void d3d9_hook_init(void)
{
    hook_table_apply(
        NULL, "d3d9.dll", d3d9_hook_syms, lengthof(d3d9_hook_syms));

    hook_table_apply(
        NULL,
        "user32.dll",
        d3d9_hook_user32_syms,
        lengthof(d3d9_hook_user32_syms));

    log_info("Inserted graphics hooks");
}

void d3d9_configure(struct gdhook_config_gfx *gfx_config)
{
    d3d9_windowed = gfx_config->windowed;
    d3d9_window_framed = gfx_config->framed;
    d3d9_confined = gfx_config->confined;
}

/* ------------------------------------------------------------------------- */

static void calc_win_size_with_framed(
    HWND hwnd, DWORD x, DWORD y, DWORD width, DWORD height, LPWINDOWPOS wp)
{
    /* taken from dxwnd */
    RECT rect;
    DWORD style;
    int max_x, max_y;
    HMENU menu;

    rect.left = x;
    rect.top = y;
    max_x = width;
    max_y = height;
    rect.right = x + max_x;
    rect.bottom = y + max_y;

    style = GetWindowLong(hwnd, GWL_STYLE);
    menu = GetMenu(hwnd);
    AdjustWindowRect(&rect, style, (menu != NULL));

    /* shift down-right so that the border is visible
       and also update the iPosX,iPosY upper-left coordinates
       of the client area */

    if (rect.left < 0) {
        rect.right -= rect.left;
        rect.left = 0;
    }

    if (rect.top < 0) {
        rect.bottom -= rect.top;
        rect.top = 0;
    }

    wp->x = rect.left;
    wp->y = rect.top;
    wp->cx = rect.right - rect.left;
    wp->cy = rect.bottom - rect.top;
}
