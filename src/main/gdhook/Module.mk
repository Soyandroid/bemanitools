avsdlls += gdhook

deplibs_gdhook    := \
    avs \

ldflags_gdhook   := \
    -lws2_32 \
    -liphlpapi \

libs_gdhook       := \
    acioemu \
    cconfig \
    eamio \
    gdio \
    p4ioemu \
    hook \
    hooklib \
    util \

src_gdhook        := \
    cardunit.c \
    dllmain.c \
    io.c \
    ledunit.c \
    config.c
