#include "jbio-p4io/h44b.h"

#define H44B_PORT COM3
#define H44B_BAUD COM3

bool jb_io_h44b_init(void) {
    return true;
    // if (!aciodrv_device_open(config_kfca.port, config_kfca.baud)) {
    //     log_info("Opening acio device on [%s] failed", config_kfca.port);
    //     return 0;
    // }

    // log_info("Opening acio device successful");

    // uint8_t node_count = aciodrv_device_get_node_count();
    // log_info("Enumerated %d nodes", node_count);

    // kfca_node_id = -1;

    // for (uint8_t i = 0; i < node_count; i++) {
    //     char product[4];
    //     aciodrv_device_get_node_product_ident(i, product);
    //     log_info(
    //         "> %d: %c%c%c%c\n",
    //         i,
    //         product[0],
    //         product[1],
    //         product[2],
    //         product[3]);

    //     if (!memcmp(product, "KFCA", 4)) {
    //         if (kfca_node_id != -1) {
    //             log_warning("Multiple KFCA found! Using highest node id.");
    //         }
    //         kfca_node_id = i;
    //     }
    // }

    // if (kfca_node_id != -1) {
    //     log_warning("Using KFCA on node: %d", kfca_node_id);

    //     if (!aciodrv_kfca_init(kfca_node_id)) {
    //         log_warning("Unable to start KFCA on node: %d", kfca_node_id);
    //         return false;
    //     }

    //     running = true;
    //     log_warning("sdvxio-kfca now running");
    // } else {
    //     log_warning("No KFCA device found");
    // }

    // return running;
}

bool jb_io_h44b_fini(void) {
    return true;
}

bool jb_io_h44b_write_lights(struct ac_io_h44b_output *lights) {
    return true;
}

