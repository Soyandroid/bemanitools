#define LOG_MODULE "wasapi-hook"

#include <audioclient.h>
#include <combaseapi.h>
#include <initguid.h>
#include <mmdeviceapi.h>
#include <windows.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "hook/com-proxy.h"
#include "hook/table.h"

#include "gdhook/wasapi.h"

#include "util/defs.h"
#include "util/log.h"
#include "util/str.h"
#include "util/time.h"

/* ------------------------------------------------------------------------- */

static HRESULT WINAPI my_GetBuffer(
    IAudioRenderClient *This, UINT32 NumFramesRequested, BYTE **ppData);

static HRESULT WINAPI my_ReleaseBuffer(
    IAudioRenderClient *This, UINT32 NumFramesWritten, DWORD dwFlags);

static HRESULT WINAPI my_Initialize(
    IAudioClient *This,
    AUDCLNT_SHAREMODE ShareMode,
    DWORD StreamFlags,
    REFERENCE_TIME hnsBufferDuration,
    REFERENCE_TIME hnsPeriodicity,
    const WAVEFORMATEX *pFormat,
    LPCGUID AudioSessionGuid);

static HRESULT WINAPI
my_GetBufferSize(IAudioClient *This, UINT32 *pNumBufferFrames);

static HRESULT WINAPI my_IsFormatSupported(
    IAudioClient *This,
    AUDCLNT_SHAREMODE ShareMode,
    const WAVEFORMATEX *pFormat,
    WAVEFORMATEX **ppClosestMatch);

static HRESULT WINAPI my_Start(IAudioClient *This);

static HRESULT WINAPI my_SetEventHandle(IAudioClient *This, HANDLE eventHandle);

static HRESULT WINAPI
my_GetService(IAudioClient *This, REFIID riid, void **ppv);

static HRESULT WINAPI my_Activate(
    IMMDevice *This,
    REFIID iid,
    DWORD dwClsCtx,
    PROPVARIANT *pActivationParams,
    void **ppInterface);

static HRESULT WINAPI my_GetDefaultAudioEndpoint(
    IMMDeviceEnumerator *This,
    EDataFlow dataFlow,
    ERole role,
    IMMDevice **ppEndpoint);

static HRESULT WINAPI my_CoCreateInstance(
    REFCLSID rclsid,
    LPUNKNOWN pUnkOuter,
    DWORD dwClsContext,
    REFIID riid,
    LPVOID *ppv);

static HRESULT(WINAPI *real_CoCreateInstance)(
    REFCLSID rclsid,
    LPUNKNOWN pUnkOuter,
    DWORD dwClsContext,
    REFIID riid,
    LPVOID *ppv);

/* ------------------------------------------------------------------------- */

/*
    for some reason, IID_IAudioClient and IID_IAudioRenderClient are not defined
   for me, so defines it here
 */
DEFINE_GUID(
    __IID_IAudioClient,
    0x1cb9ad4c,
    0xdbfa,
    0x4c32,
    0xb1,
    0x78,
    0xc2,
    0xf5,
    0x68,
    0xa7,
    0x03,
    0xb2);

DEFINE_GUID(
    __IID_IAudioRenderClient,
    0xf294acfc,
    0x3146,
    0x4483,
    0xa7,
    0xbf,
    0xad,
    0xdc,
    0xa7,
    0xc2,
    0x60,
    0xe2);

enum {
    GDHOOK_WASAPI_READJUST_NONE = 0,
    GDHOOK_WASAPI_READJUST_MODREADJUST_VAL = 1 << 0,
    GDHOOK_WASAPI_READJUST_MODRENDERBUFFER_SIZE = 1 << 1,
    GDHOOK_WASAPI_READJUST_ACQUIREREADBUFFER_PTR = 1 << 2,
    GDHOOK_WASAPI_READJUST_MODREADBUFFER_SIZE = 1 << 3,
};

static bool sound_wasapi_shared_mode;
static uint8_t sound_readjust_flag;
static uint32_t *p_sound_source_read_size;
static uint32_t adjust_frames;

static uint32_t bufferFrameCount;
static uint32_t numFramesPadding;
static IAudioClient *pAudioClient;

/* ------------------------------------------------------------------------- */

static const struct hook_symbol sound_hook_syms[] = {
    {.name = "CoCreateInstance",
     .patch = my_CoCreateInstance,
     .link = (void **) &real_CoCreateInstance},
};

/* ------------------------------------------------------------------------- */

static HRESULT WINAPI
my_GetBuffer(IAudioRenderClient *This, UINT32 NumFramesRequested, BYTE **ppData)
{
    IAudioRenderClient *real = com_proxy_downcast(This)->real;
    HRESULT ret;

    // log_info("IAudioRenderClient::GetBuffer hook hit");

    if (sound_readjust_flag & GDHOOK_WASAPI_READJUST_MODRENDERBUFFER_SIZE) {

        if (sound_readjust_flag & GDHOOK_WASAPI_READJUST_MODREADJUST_VAL) {
            /*
               setting re-adjust size for GetBuffer and ReleaseBuffer here
            */
            IAudioClient_GetCurrentPadding(pAudioClient, &numFramesPadding);
            adjust_frames = bufferFrameCount - numFramesPadding;
            if (p_sound_source_read_size != NULL &&
                sound_readjust_flag &
                    GDHOOK_WASAPI_READJUST_MODREADBUFFER_SIZE) {
                /*
                   setting the sound file read buffer size here
                   as mentioned below it may not works
                */
                *p_sound_source_read_size = adjust_frames;
            }
        }

        NumFramesRequested = adjust_frames;
    }

    ret = IAudioRenderClient_GetBuffer(real, NumFramesRequested, ppData);

    return ret;
}

static HRESULT WINAPI my_ReleaseBuffer(
    IAudioRenderClient *This, UINT32 NumFramesWritten, DWORD dwFlags)
{
    IAudioRenderClient *real = com_proxy_downcast(This)->real;
    HRESULT ret;

    // log_info("IAudioRenderClient::ReleaseBuffer hook hit");

    if (sound_readjust_flag & GDHOOK_WASAPI_READJUST_MODRENDERBUFFER_SIZE) {
        NumFramesWritten = adjust_frames;
    }

    ret = IAudioRenderClient_ReleaseBuffer(real, NumFramesWritten, dwFlags);

    return ret;
}

static HRESULT WINAPI my_Initialize(
    IAudioClient *This,
    AUDCLNT_SHAREMODE ShareMode,
    DWORD StreamFlags,
    REFERENCE_TIME hnsBufferDuration,
    REFERENCE_TIME hnsPeriodicity,
    const WAVEFORMATEX *pFormat,
    LPCGUID AudioSessionGuid)
{
    IAudioClient *real = com_proxy_downcast(This)->real;
    HRESULT ret;

    log_info("IAudioClient::Initialize hook hit");

    ShareMode = AUDCLNT_SHAREMODE_SHARED;
    StreamFlags |= AUDCLNT_STREAMFLAGS_RATEADJUST;
    hnsBufferDuration = 0;
    hnsPeriodicity = 0;

    ret = IAudioClient_Initialize(
        real,
        ShareMode,
        StreamFlags,
        hnsBufferDuration,
        hnsPeriodicity,
        pFormat,
        AudioSessionGuid);

    IAudioClient_GetBufferSize(real, &bufferFrameCount);

    return ret;
}

static HRESULT WINAPI
my_GetBufferSize(IAudioClient *This, UINT32 *pNumBufferFrames)
{
    IAudioClient *real = com_proxy_downcast(This)->real;
    HRESULT ret;

    log_info("IAudioClient::GetBufferSize hook hit");

    ret = IAudioClient_GetBufferSize(real, pNumBufferFrames);

    if (sound_readjust_flag & GDHOOK_WASAPI_READJUST_ACQUIREREADBUFFER_PTR) {
        /*
           get the pointer of sound buffer size (per frame?)

           in exclusive mode the game will get this value right after SetEventHandle
           as fixed buffer size for feeding audio source data into render client

           notice that if the game parse the value from this pointer to another
           variable it just won't work since we can't adjust the audio source
           read size by re-adjusting the value of this ptr

           but it works for GITADORA
        */
        p_sound_source_read_size = pNumBufferFrames;
        sound_readjust_flag &= ~GDHOOK_WASAPI_READJUST_ACQUIREREADBUFFER_PTR;
    }

    return ret;
}

static HRESULT WINAPI my_IsFormatSupported(
    IAudioClient *This,
    AUDCLNT_SHAREMODE ShareMode,
    const WAVEFORMATEX *pFormat,
    WAVEFORMATEX **ppClosestMatch)
{
    IAudioClient *real = com_proxy_downcast(This)->real;
    HRESULT ret;

    log_info("IAudioClient::IsFormatSupported hook hit");

    /*
       force it checking for the shared mode of this format

       don't want to do something like 48000Hz 16bits to 44100Hz 32bit
       which is wayyyyyy more than this thing want to achieve

       in SDVX all 3 formats are not supported, didn't check why that happened
       yet, since all of the format actually looks fine to me
    */
    ShareMode = AUDCLNT_SHAREMODE_SHARED;

    ret = IAudioClient_IsFormatSupported(
        real, ShareMode, pFormat, ppClosestMatch);

    return ret;
}

static HRESULT WINAPI my_Start(IAudioClient *This)
{
    IAudioClient *real = com_proxy_downcast(This)->real;
    HRESULT ret;

    log_info("IAudioClient::Start hook hit");

    ret = IAudioClient_Start(real);

    /*
       set the flag of:

       setting re-adjust size for GetBuffer and ReleaseBuffer

       enable buffer size re-adjust for GetBuffer and ReleaseBuffer

       modify the sound file read buffer size (not 100% works depends on how the
       game codes)
    */
    sound_readjust_flag |= GDHOOK_WASAPI_READJUST_MODREADJUST_VAL |
        GDHOOK_WASAPI_READJUST_MODRENDERBUFFER_SIZE |
        GDHOOK_WASAPI_READJUST_MODREADBUFFER_SIZE;

    return ret;
}

static HRESULT WINAPI my_SetEventHandle(IAudioClient *This, HANDLE eventHandle)
{
    IAudioClient *real = com_proxy_downcast(This)->real;
    HRESULT ret;

    log_info("IAudioClient::SetEventHandle hook hit");

    ret = IAudioClient_SetEventHandle(real, eventHandle);

    /*
       in exclusive mode of GITADORA, the game will get the sound buffer size
       by calling GetBufferSize right after SetEventHandle

       so set the flag for it here to grab the pointer for the size of sound
       buffer
    */
    sound_readjust_flag |= GDHOOK_WASAPI_READJUST_ACQUIREREADBUFFER_PTR;

    return ret;
}

static HRESULT WINAPI my_GetService(IAudioClient *This, REFIID riid, void **ppv)
{
    IAudioClient *real = com_proxy_downcast(This)->real;
    HRESULT ret;
    HRESULT hr;
    IAudioRenderClientVtbl *api_vtbl;
    struct com_proxy *api_proxy;
    IAudioRenderClient *api_;

    log_info("IAudioClient::GetService hook hit");

    ret = IAudioClient_GetService(real, riid, ppv);

    if (*ppv && IsEqualIID(riid, &__IID_IAudioRenderClient)) {

        api_ = *ppv;

        hr = com_proxy_wrap(&api_proxy, api_, sizeof(*api_->lpVtbl));

        if (hr != S_OK) {
            log_warning("Wrapping com proxy failed: %08lx", hr);
            return ret;
        }

        api_vtbl = api_proxy->vptr;

        api_vtbl->GetBuffer = my_GetBuffer;
        api_vtbl->ReleaseBuffer = my_ReleaseBuffer;

        *ppv = (void *) api_proxy;
    }

    return ret;
}

static HRESULT WINAPI my_Activate(
    IMMDevice *This,
    REFIID iid,
    DWORD dwClsCtx,
    PROPVARIANT *pActivationParams,
    void **ppInterface)
{
    IMMDevice *real = com_proxy_downcast(This)->real;
    HRESULT ret;
    HRESULT hr;
    IAudioClientVtbl *api_vtbl;
    struct com_proxy *api_proxy;
    IAudioClient *api_;

    log_info("IMMDevice::Activate hook hit");

    ret =
        IMMDevice_Activate(real, iid, dwClsCtx, pActivationParams, ppInterface);

    if (*ppInterface && IsEqualIID(iid, &__IID_IAudioClient)) {

        api_ = *ppInterface;

        hr = com_proxy_wrap(&api_proxy, api_, sizeof(*api_->lpVtbl));

        if (hr != S_OK) {
            log_warning("Wrapping com proxy failed: %08lx", hr);
            return ret;
        }

        sound_readjust_flag = GDHOOK_WASAPI_READJUST_NONE;
        p_sound_source_read_size = NULL;

        api_vtbl = api_proxy->vptr;

        api_vtbl->Initialize = my_Initialize;
        api_vtbl->GetBufferSize = my_GetBufferSize;
        api_vtbl->IsFormatSupported = my_IsFormatSupported;
        api_vtbl->Start = my_Start;
        api_vtbl->SetEventHandle = my_SetEventHandle;
        api_vtbl->GetService = my_GetService;

        *ppInterface = (void *) api_proxy;

        pAudioClient = *ppInterface;
    }

    return ret;
}

static HRESULT WINAPI my_GetDefaultAudioEndpoint(
    IMMDeviceEnumerator *This,
    _In_ EDataFlow dataFlow,
    _In_ ERole role,
    _Out_ IMMDevice **ppEndpoint)
{
    IMMDeviceEnumerator *real = com_proxy_downcast(This)->real;
    HRESULT ret;
    HRESULT hr;
    IMMDeviceVtbl *api_vtbl;
    struct com_proxy *api_proxy;
    IMMDevice *api_;

    log_info("IMMDeviceEnumerator::GetDefaultAudioEndpoint hook hit");

    ret = IMMDeviceEnumerator_GetDefaultAudioEndpoint(
        real, dataFlow, role, ppEndpoint);

    if (*ppEndpoint) {

        api_ = *ppEndpoint;

        hr = com_proxy_wrap(&api_proxy, api_, sizeof(*api_->lpVtbl));

        if (hr != S_OK) {
            log_warning("Wrapping com proxy failed: %08lx", hr);
            return ret;
        }

        api_vtbl = api_proxy->vptr;

        api_vtbl->Activate = my_Activate;

        *ppEndpoint = (void *) api_proxy;
    }

    return ret;
}

static HRESULT WINAPI my_CoCreateInstance(
    REFCLSID rclsid,
    LPUNKNOWN pUnkOuter,
    DWORD dwClsContext,
    REFIID riid,
    LPVOID *ppv)
{

    HRESULT ret;
    HRESULT hr;
    IMMDeviceEnumeratorVtbl *api_vtbl;
    struct com_proxy *api_proxy;
    IMMDeviceEnumerator *api_;

    log_info("CoCreateInstance hook hit");

    ret = real_CoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);

    if (*ppv && IsEqualCLSID(rclsid, &CLSID_MMDeviceEnumerator) &&
        IsEqualIID(riid, &IID_IMMDeviceEnumerator)) {

        api_ = *ppv;

        hr = com_proxy_wrap(&api_proxy, api_, sizeof(*api_->lpVtbl));

        if (hr != S_OK) {
            log_warning("Wrapping com proxy failed: %08lx", hr);
            return ret;
        }

        api_vtbl = api_proxy->vptr;

        api_vtbl->GetDefaultAudioEndpoint = my_GetDefaultAudioEndpoint;

        *ppv = (void *) api_proxy;
    }

    return ret;
}

void wasapi_hook_init(void)
{
    if (sound_wasapi_shared_mode) {
        hook_table_apply(
            NULL, "ole32.dll", sound_hook_syms, lengthof(sound_hook_syms));

        log_info("Inserted sound hooks");
    }
}

void wasapi_configure(struct gdhook_config_sound *sound_config)
{
    sound_wasapi_shared_mode = sound_config->is_shared_mode;
}

/* ------------------------------------------------------------------------- */
