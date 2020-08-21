dlls		    += ezusb-proxy

ldflags_ezusb-proxy   := \
    -lws2_32 \
    -liphlpapi \

libs_ezusb-proxy      := \

src_ezusb-proxy       := \
    dllmain.c \
