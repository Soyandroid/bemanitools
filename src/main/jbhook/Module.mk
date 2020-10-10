avsdlls += jbhook

deplibs_jbhook    := \
    avs \
    jbhook-util \

ldflags_jbhook   := \
    -lws2_32 \
    -liphlpapi \

libs_jbhook       := \
    acioemu \
    eamio \
    jbio \
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
