/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2024 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#include <badge-persistence/exception.hpp>
#include <badge-persistence/utils.hpp>

#include <utils/logging.hpp>

#include <fmt/format.h>

#include <nvs_flash.h>

namespace np = nsec::persistence;

void np::utils::initialize_storage()
{
    nsec::logging::logger logger("Persistence");

    logger.debug("Initializing flash");

    bool tried_to_recover = false;

    while (true) {
        const auto init_ret = nvs_flash_init();
        if (init_ret == ESP_OK) {
            logger.info("Flash successfully initialized");
            return;
        }

        if (tried_to_recover) {
            // Already tried to recover before... give up!
            NSEC_PERSISTENCE_THROW_ERROR_CORRUPTED(fmt::format(
                "Failed to initialize flash: nvs_flash_init returned {}",
                init_ret));
        }

        logger.error("Failed to initialize flash: nvs_flash_init returned {}",
                     init_ret);
        if (init_ret == ESP_ERR_NVS_NO_FREE_PAGES ||
            init_ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            // NVS partition was truncated and needs to be erased. Retry
            // nvs_flash_init.

            logger.info("Erasing flash to attempt recovery", init_ret);

            const auto erase_ret = nvs_flash_erase();
            if (erase_ret != ESP_OK) {
                logger.warn("Failed to erase flash: nvs_flash_erase returned {}", erase_ret);
            }

            tried_to_recover = true;
        }
    }
}
