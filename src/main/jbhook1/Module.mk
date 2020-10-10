avsdlls += jbhook1

deplibs_jbhook1	:= \
	avs \

ldflags_jbhook1   := \
    -lws2_32 \
    -liphlpapi \

libs_jbhook1       := \
    acioemu \
    cconfig \
    eamio \
    jbio \
	jbhook-util \
    p3ioemu \
    p3io \
    hook \
    hooklib \
    security \
    util \

src_jbhook1        := \
    avs-boot.c \
    config-gfx.c \
    config-eamuse.c \
    config-security.c \
    dllmain.c \
    log-gftools.c \
