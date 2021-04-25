#include "challenges_screen.h"

#include "buttons.h"
#include "cmd_wifi.h"
#include "graphics.h"

#include <fcntl.h>
#include <sys/socket.h>

static void challenges_screen_wait()
{
    button_t button = BUTTON_NONE;
    while (true) {
        xQueueReceive(button_event_queue, &button, 60 / portTICK_PERIOD_MS);

        switch (button) {
        case BUTTON_BACK_RELEASE:
            return;

        default:
            continue;
        }
    }
}

static void challenges_screen_sockets()
{
    int sock101 = socket(AF_INET, SOCK_STREAM, 0);
    int sock102 = socket(AF_INET, SOCK_STREAM, 0);

    int flags = fcntl(sock101, F_GETFL);
    fcntl(sock101, F_SETFL, flags | O_NONBLOCK);
    fcntl(sock102, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in addr101 = {};
    addr101.sin_addr.s_addr = htonl(INADDR_ANY);
    addr101.sin_family = AF_INET;
    addr101.sin_port = htons(1101);

    struct sockaddr_in addr102 = {};
    addr102.sin_addr.s_addr = htonl(INADDR_ANY);
    addr102.sin_family = AF_INET;
    addr102.sin_port = htons(1102);

    bind(sock101, (struct sockaddr *)&addr101, sizeof(addr101));
    bind(sock102, (struct sockaddr *)&addr102, sizeof(addr102));
    listen(sock101, 1);
    listen(sock102, 1);

    uint8_t *buffer = static_cast<uint8_t *>(calloc(2048, sizeof(uint8_t)));
    button_t button = BUTTON_NONE;
    int conn_fd;
    FILE *download_fd;
    int length;

    while (true) {
        xQueueReceive(button_event_queue, &button, 60 / portTICK_PERIOD_MS);

        conn_fd = accept(sock101, 0, 0);
        if (conn_fd > 0) {
            download_fd = fopen("/spiffs/re/re101.zip", "rb");

            while (!feof(download_fd)) {
                length = fread(buffer, sizeof(uint8_t), 2048, download_fd);
                send(conn_fd, buffer, length, 0);
            }

            close(conn_fd);
            fclose(download_fd);
        }

        conn_fd = accept(sock102, 0, 0);
        if (conn_fd > 0) {
            download_fd = fopen("/spiffs/re/re102.zip", "rb");

            while (!feof(download_fd)) {
                length = fread(buffer, sizeof(uint8_t), 2048, download_fd);
                send(conn_fd, buffer, length, 0);
            }

            close(conn_fd);
            fclose(download_fd);
        }

        switch (button) {
        case BUTTON_BACK_RELEASE:
            break;

        default:
            continue;
        }

        break;
    }

    close(sock101);
    close(sock102);
}

void challenges_screen_display()
{
    int color = 0xFFFF;
    int x = 10;
    int y = 32;

    graphics_draw_jpeg("/spiffs/welcome/blank.jpeg", 0, 0);
    graphics_update_display();

    if (!is_wifi_connected()) {
        graphics_print_small("You need to connect\nthe badge to Wifi first.", x,
                             y, color, &x, &y);
        return challenges_screen_wait();
    }

    const char *ip_address = wifi_get_ip();
    if (ip_address[0] == 0) {
        graphics_print_small("Could not determine\nthe IPv4 address.\n\nCheck "
                             "that the Wifi\nis working.",
                             x, y, color, &x, &y);
        return challenges_screen_wait();
    }

    graphics_print_small("Welcome to the reverse\nengineering "
                         "challenge.\nFirst, download the\nbinaries:\n\n",
                         x, y, color, &x, &y);

    graphics_print_small("  nc ", x, y, color, &x, &y);
    graphics_print_small(ip_address, x, y, color, &x, &y);
    graphics_print_small(" 1101\n", x, y, color, &x, &y);

    x = 52;
    graphics_print_small("> rc101.zip\n\n", x, y, color, &x, &y);

    x = 10;
    graphics_print_small("  nc ", x, y, color, &x, &y);
    graphics_print_small(ip_address, x, y, color, &x, &y);
    graphics_print_small(" 1102\n", x, y, color, &x, &y);

    x = 52;
    graphics_print_small("> rc102.zip\n\n", x, y, color, &x, &y);

    x = 10;
    graphics_print_small("Validate flags using the\nUART CLI.", x, y, color, &x,
                         &y);

    return challenges_screen_sockets();
}
