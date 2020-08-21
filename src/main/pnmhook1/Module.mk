avsdlls		    += pnmhook1

ldflags_pnmhook1   := \
    -lws2_32 \
    
libs_pnmhook1     := \
    iidxhook-util \
    hook \
    util \

src_pnmhook1       := \
    d3d9.c \
    dllmain.c \
