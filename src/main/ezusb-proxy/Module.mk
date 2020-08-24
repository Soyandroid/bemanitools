dlls		    += ezusb-proxy

ldflags_ezusb-proxy   := \
    -lws2_32 \
    -liphlpapi \

libs_ezusb-proxy      := \
    util \

src_ezusb-proxy       := \
    dllmain.c \
