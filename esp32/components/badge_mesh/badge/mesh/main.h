#pragma once

#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/semphr.h"
#include <stdint.h>
#include <stdio.h>

#include "badge/mesh/network.h"

#ifdef __cplusplus
extern "C" {
#endif

class BadgeMesh
{
  public:
    static BadgeMesh &getInstance()
    {
        static BadgeMesh instance;
        return instance;
    }

  private:
    BadgeMesh()
    {
    }

    static TaskHandle_t _taskHandle;
    SemaphoreHandle_t _bt_semaphore;

  public:
    BadgeMesh(BadgeMesh const &) = delete;
    void operator=(BadgeMesh const &) = delete;

    void init();
    static void task(BadgeMesh *instance) {
        instance->taskHandler();
    }
    void taskHandler();
    esp_err_t clientSend(uint16_t dst_addr, uint32_t op, uint8_t *msg, unsigned int length, bool needsResponse);
    esp_err_t serverSend(uint16_t dst_addr, uint32_t op, uint8_t *msg, unsigned int length);
};

esp_err_t mesh_client_send(uint16_t dst_addr, uint32_t op, uint8_t *msg, unsigned int length, bool needsResponse);
esp_err_t mesh_server_send(uint16_t dst_addr, uint32_t op, uint8_t *msg, unsigned int length);

#ifdef __cplusplus
}
#endif
