#include <sys/socket.h>

#include "FX.h"
#include "esp_console.h"
#include "esp_err.h"
#include "esp_spiffs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "FastLED.h"
#include "buttons.h"
#include "buzzer.h"
#include "cmd.h"
#include "cmd_wifi.h"
#include "graphics.h"
#include "infoscreen.h"
#include "neopixel.h"
#include "rpg.h"
#include "save.h"

static const char firmware_dump_flag[] =
    "Have you dumped the firmware? Here is your flag "
    "[FLAG-JTAGPower0verwhelming]. Now flip the right bit in memory to "
    "activate the last (10th) flag icon in the status bar on screen.";

static bool mount_spiffs()
{
    esp_vfs_spiffs_conf_t conf = {.base_path = "/spiffs",
                                  .partition_label = NULL,
                                  .max_files = 8,
                                  .format_if_mount_failed = false};

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    return ret == ESP_OK;
}

static void welcome_screen_exit()
{
    vTaskDelay(250 / portTICK_PERIOD_MS);
    nsec_buttons_flush();
}

static void welcome_screen()
{
    button_t button = BUTTON_NONE;
    nsec_buttons_flush();

    graphics_draw_jpeg("/spiffs/welcome/blank.jpeg", 0, 0);
    graphics_update_display();

    xQueueReceive(button_event_queue, &button, 750 / portTICK_PERIOD_MS);
    if (button != BUTTON_NONE)
        return welcome_screen_exit();

    graphics_draw_jpeg("/spiffs/welcome/nsec2021.jpeg", 0, 53);
    graphics_update_display();

    xQueueReceive(button_event_queue, &button, 750 / portTICK_PERIOD_MS);
    if (button != BUTTON_NONE)
        return welcome_screen_exit();

    int sponsored_y = 183;
    for (int fade = 0; fade <= 100; ++fade) {
        xQueueReceive(button_event_queue, &button, 0);
        if (button != BUTTON_NONE)
            return welcome_screen_exit();

        switch (fade) {
        case 0:
            graphics_draw_jpeg("/spiffs/welcome/sponsored-0.jpeg", 0,
                               sponsored_y);
            break;

        case 1:
            graphics_draw_jpeg("/spiffs/welcome/sponsored-1.jpeg", 0,
                               sponsored_y);
            break;

        case 2:
            graphics_draw_jpeg("/spiffs/welcome/sponsored-2.jpeg", 0,
                               sponsored_y);
            break;

        case 4:
            graphics_draw_jpeg("/spiffs/welcome/sponsored-4.jpeg", 0,
                               sponsored_y);
            break;

        case 8:
            graphics_draw_jpeg("/spiffs/welcome/sponsored-8.jpeg", 0,
                               sponsored_y);
            break;

        case 16:
            graphics_draw_jpeg("/spiffs/welcome/sponsored-16.jpeg", 0,
                               sponsored_y);
            break;

        case 32:
            graphics_draw_jpeg("/spiffs/welcome/sponsored-32.jpeg", 0,
                               sponsored_y);
            break;

        case 64:
            graphics_draw_jpeg("/spiffs/welcome/sponsored-64.jpeg", 0,
                               sponsored_y);
            break;

        case 80:
            graphics_draw_jpeg("/spiffs/welcome/sponsored-80.jpeg", 0,
                               sponsored_y);
            break;

        case 90:
            graphics_draw_jpeg("/spiffs/welcome/sponsored-90.jpeg", 0,
                               sponsored_y);
            break;

        case 100:
            graphics_draw_jpeg("/spiffs/welcome/sponsored-100.jpeg", 0,
                               sponsored_y);
            break;

        default:
            continue;
        }

        graphics_update_display();
    }

    xQueueReceive(button_event_queue, &button, 300 / portTICK_PERIOD_MS);
    if (button != BUTTON_NONE)
        return welcome_screen_exit();

    graphics_draw_jpeg("/spiffs/welcome/design.jpeg", 0, 11);
    graphics_update_display();

    xQueueReceive(button_event_queue, &button, 2000 / portTICK_PERIOD_MS);
    if (button != BUTTON_NONE)
        return welcome_screen_exit();

    for (int i = 0; i < 66; i += 7) {
        graphics_fadeout_display_buffer(i);
        graphics_update_display();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    welcome_screen_exit();
}

static void wifi_challenge_task(void *arg)
{
    int sock;
    struct sockaddr_in addr = {};

    addr.sin_addr.s_addr = inet_addr("198.51.100.42");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(4444);

    while (true) {
        vTaskDelay(15000 / portTICK_PERIOD_MS);

        if (!is_wifi_connected())
            continue;

        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
            continue;

        if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            close(sock);
            continue;
        }

        char key[] = {0xad, 0xa8, 0x32, 0x3a, 0x34, 0x77, 0xbb, 0xd3, 0x19,
                      0x28, 0xdf, 0x0b, 0x2c, 0x7a, 0x93, 0xe1, 0xc2, 0x58,
                      0xad, 0x68, 0x4e, 0xa5, 0x34, 0x72, 0x01, 0xc5, 0xb3,
                      0xf6, 0x89, 0x23, 0x1f, 0x85, 0xf1, 0x61, 0x92};

        char message[] = {0x2f, 0x4c, 0xf8, 0x7a, 0xde, 0x37, 0x63, 0x01, 0xff,
                          0xc0, 0x15, 0xd7, 0xfe, 0xa6, 0x5d, 0x9f, 0x82, 0xd4,
                          0x35, 0xea, 0xc0, 0xff, 0xd2, 0x92, 0x9f, 0x13, 0x79,
                          0x5e, 0x59, 0x45, 0x8f, 0x5b, 0x15, 0x87, 0x86, 0x00};

        for (int i = 0; i < 35; ++i)
            message[i] = (message[i] ^ key[i]) >> 1;

        send(sock, message, strlen(message), 0);
        close(sock);

        if (!Save::save_data.flag6) {
            Save::save_data.flag6 = true;
            Save::write_save();
        }
    }
}

extern "C" void app_main(void)
{
    button_t button = BUTTON_NONE;

    if (!mount_spiffs())
        abort();

    int *checksum = static_cast<int *>(calloc(1, sizeof(int)));
    for (int i = 0; i < 10; ++i)
        *checksum += firmware_dump_flag[i];

    graphics_start();
    initialize_nvs();
    nsec_buttons_init();

    Save::load_save();
    buzzer_init();
    NeoPixel::getInstance().init();

    xTaskCreate(console_task, "console task", 4096, NULL, 3, NULL);

    welcome_screen();
    infoscreen_display_bootwarning();

    xTaskCreate(wifi_challenge_task, " ", 2048, NULL, 2, NULL);

    for (int i = 100; i >= 0; i -= 25) {
        graphics_draw_jpeg("/spiffs/welcome/start.jpeg", 0, 0);
        graphics_fadeout_display_buffer(i);
        graphics_update_display();
    }

    do {
        vTaskDelay(1);
        xQueueReceive(button_event_queue, &button, 0);
    } while (button != BUTTON_ENTER_RELEASE);

    run_main_scene();
}
