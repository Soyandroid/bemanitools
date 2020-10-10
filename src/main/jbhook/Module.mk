avsdlls += jbhook

deplibs_jbhook    := \
    avs \

ldflags_jbhook   := \
    -lws2_32 \
    -liphlpapi \

libs_jbhook       := \
    acioemu \
    eamio \
    jbio \
    jbhook-util \
    p3ioemu \
    p3io \
    p4ioemu \
    hook \
    hooklib \
    security \
    util \

src_jbhook        := \
    dllmain.c \
    gfx.c \
    options.c \
    p4io.c
