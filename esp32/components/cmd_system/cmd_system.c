/* Console example — various system commands

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "cmd_system.h"
#include "argtable3/argtable3.h"
#include "driver/rtc_io.h"
#include "driver/uart.h"
#include "esp_console.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#ifdef CONFIG_FREERTOS_USE_STATS_FORMATTING_FUNCTIONS
#define WITH_TASKS_INFO 1
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

static const char *TAG = "cmd_system";

static void register_free(void);
static void register_heap(void);
static void register_version(void);
static void register_restart(void);
#if WITH_TASKS_INFO
static void register_tasks(void);
#endif

void register_system(void)
{
    register_free();
    register_heap();
    register_version();
    register_restart();
#if WITH_TASKS_INFO
    register_tasks();
#endif
}

/* 'version' command */
static int get_version(int argc, char **argv)
{
    esp_chip_info_t info;
    esp_chip_info(&info);
    printf("IDF Version:%s\r\n", esp_get_idf_version());
    printf("Chip info:\r\n");
    printf("\tmodel:%s\r\n", info.model == CHIP_ESP32 ? "ESP32" : "Unknow");
    printf("\tcores:%d\r\n", info.cores);
    printf("\tfeature:%s%s%s%s%d%s\r\n",
           info.features & CHIP_FEATURE_WIFI_BGN ? "/802.11bgn" : "",
           info.features & CHIP_FEATURE_BLE ? "/BLE" : "",
           info.features & CHIP_FEATURE_BT ? "/BT" : "",
           info.features & CHIP_FEATURE_EMB_FLASH ? "/Embedded-Flash:"
                                                  : "/External-Flash:",
           spi_flash_get_chip_size() / (1024 * 1024), " MB");
    printf("\trevision number:%d\r\n", info.revision);
    return 0;
}

static void register_version(void)
{
    const esp_console_cmd_t cmd = {
        .command = "version",
        .help = "Get version of chip and SDK",
        .hint = NULL,
        .func = &get_version,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}
/** 'restart' command restarts the program */

static int restart(int argc, char **argv)
{
    ESP_LOGI(TAG, "Restarting");
    esp_restart();
}

static void register_restart(void)
{
    const esp_console_cmd_t cmd = {
        .command = "restart",
        .help = "Software reset of the chip",
        .hint = NULL,
        .func = &restart,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'free' command prints available heap memory */

static int free_mem(int argc, char **argv)
{
    printf("%d\n", esp_get_free_heap_size());
    return 0;
}

static void register_free(void)
{
    const esp_console_cmd_t cmd = {
        .command = "free",
        .help = "Get the current size of free heap memory",
        .hint = NULL,
        .func = &free_mem,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/* 'heap' command prints minumum heap size */
static int heap_size(int argc, char **argv)
{
    uint32_t heap_size = heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT);
    ESP_LOGI(TAG, "min heap size: %u", heap_size);
    return 0;
}

static void register_heap(void)
{
    const esp_console_cmd_t heap_cmd = {
        .command = "heap",
        .help = "Get minimum size of free heap memory that was available "
                "during program execution",
        .hint = NULL,
        .func = &heap_size,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&heap_cmd));
}

/** 'tasks' command prints the list of tasks and related information */
#if WITH_TASKS_INFO

static int tasks_info(int argc, char **argv)
{
    TaskStatus_t tasks[20];
    uint32_t total_run_time;
    UBaseType_t ntasks;

    ntasks = uxTaskGetSystemState(tasks, ARRAY_SIZE(tasks), &total_run_time);

    assert(ntasks > 0);

    ESP_LOGI(TAG, "#\tName\t\tState\tPrio\tRuntime\t\tRuntime%%\tStackMin");

    for (UBaseType_t i = 0; i < ntasks; i++) {
        TaskStatus_t *task = &tasks[i];
        char state;
        uint32_t run_time_per_thousand =
            (task->ulRunTimeCounter * 1000) / total_run_time;
        unsigned int run_time_percent_whole = run_time_per_thousand / 10;
        unsigned int run_time_percent_fract = run_time_per_thousand % 10;

        switch (task->eCurrentState) {
        case eRunning:
            state = '*';
            break;
        case eReady:
            state = 'R';
            break;
        case eBlocked:
            state = 'B';
            break;
        case eSuspended:
            state = 'S';
            break;
        case eDeleted:
            state = 'D';
            break;
        default:
            state = '?';
            break;
        }

        ESP_LOGI(TAG, "%d\t%-10s\t%c\t%d\t%d\t\t%2d.%01d\t\t%d",
                 task->xTaskNumber, task->pcTaskName, state,
                 task->uxBasePriority, task->ulRunTimeCounter,
                 run_time_percent_whole, run_time_percent_fract,
                 task->usStackHighWaterMark);
    }
    return 0;
}

static void register_tasks(void)
{
    const esp_console_cmd_t cmd = {
        .command = "tasks",
        .help = "Get information about running tasks",
        .hint = NULL,
        .func = &tasks_info,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

#endif // WITH_TASKS_INFO
