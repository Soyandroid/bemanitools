dlls                += jbio-p4io

ldflags_jbio-p4io      := \
    -lsetupapi

src_jbio-p4io          := \
    h44b.c \
    jbio.c \
    p4io.c \

libs_jbio-p4io      := \
    util \
