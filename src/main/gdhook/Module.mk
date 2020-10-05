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
    p4io.c \
    ledunit.c \
    lednode.c \
    guitarunit.c \
    drumunit.c \
    d3d9.c \
    wasapi.c \
    config-gfx.c \
    config-emu.c \
    config-game.c \
    config-sound.c
