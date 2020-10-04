avsdlls += jbhook

deplibs_jbhook    := \
    avs \

ldflags_jbhook   := \
    -lws2_32 \
    -liphlpapi \

libs_jbhook       := \
    jbhook-util \
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
    eamuse.c \
    gfx.c \
    options.c \
    p4io.c
