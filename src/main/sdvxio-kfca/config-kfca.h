#ifndef SDVXIO_KFCA_CONFIG_KFCA_H
#define SDVXIO_KFCA_CONFIG_KFCA_H

#include <windows.h>

#include "cconfig/cconfig.h"

struct sdvxio_kfca_config_kfca {
    char port[64];
    int32_t baud;
};

void sdvxio_kfca_config_kfca_init(struct cconfig *config);

void sdvxio_kfca_config_kfca_get(
    struct sdvxio_kfca_config_kfca *config_kfca, struct cconfig *config);

#endif