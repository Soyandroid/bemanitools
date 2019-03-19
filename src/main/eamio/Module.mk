eamio_curdir     := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

dlls             += eamio
libs_eamio       := geninput util
abslibs_eamio_32 := $(eamio_curdir)/nfc/lib/i686/libnfc.a
abslibs_eamio_64 := $(eamio_curdir)/nfc/lib/x86_64/libnfc.a
ldflags_eamio    := -lwinscard
cflags_eamio     := -I"$(eamio_curdir)" -DLIBNFC_STATIC
src_eamio        := \
    eam-api.c \
    eam-impl.c \
    eam-s11n.c \
