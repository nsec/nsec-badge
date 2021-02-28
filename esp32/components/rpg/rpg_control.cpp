#include "rpg_control.h"

#include "rpg/MainCharacter.h"
#include "rpg/Viewport.h"

extern "C" {
#include "buttons.h"
#include "graphics.h"
}

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <iostream>

namespace rpg
{

constexpr int rpg_control_priority_low = 10;
constexpr int rpg_control_priority_medium = 15;
constexpr int rpg_control_priority_high = 20;

struct RpgControlDevice {
    Scene *scene;
    TaskHandle_t task_animation_step;
    TaskHandle_t task_fps_report;
    TaskHandle_t task_main_character;
    TaskHandle_t task_render;
    TaskHandle_t task_status_bar;
    int fps_counter;
};

#if 1
/**
 * Smooth scrolling variant of the viewport scroller.
 */
static void rpg_control_render_scoll_viewport(Viewport &viewport,
                                              local_coordinates_t &coordinates)
{
    constexpr int easing = 3;

    constexpr int top = 2.5 * DISPLAY_TILE_HEIGHT;
    constexpr int left = 2.5 * DISPLAY_TILE_WIDTH;
    constexpr int right = viewport_width - 5 * DISPLAY_TILE_WIDTH;
    constexpr int bottom = viewport_height - 4.5 * DISPLAY_TILE_HEIGHT;

    if (coordinates.screen_x < left) {
        viewport.move_relative((coordinates.screen_x - left) / easing, 0);
    }

    if (coordinates.screen_y < top) {
        viewport.move_relative(0, (coordinates.screen_y - top) / easing);
    }

    if (coordinates.screen_x > right) {
        viewport.move_relative((coordinates.screen_x - right) / easing, 0);
    }

    if (coordinates.screen_y > bottom) {
        viewport.move_relative(0, (coordinates.screen_y - bottom) / easing);
    }
}
#else
/**
 * Stepping variant of the viewport scroller.
 */
static void rpg_control_render_scoll_viewport(Viewport &viewport,
                                              local_coordinates_t &coordinates)
{
    constexpr int top = 15;
    constexpr int left = 15;
    constexpr int right = viewport_width - 55;
    constexpr int bottom = viewport_height - 65;

    if (coordinates.screen_x < left) {
        viewport.move_relative(-130, 0);
    }

    if (coordinates.screen_y < top) {
        viewport.move_relative(0, -130);
    }

    if (coordinates.screen_x > right) {
        viewport.move_relative(130, 0);
    }

    if (coordinates.screen_y > bottom) {
        viewport.move_relative(0, 130);
    }
}
#endif

static void rpg_control_animation_step_task(void *arg)
{
    RpgControlDevice *control_device = static_cast<RpgControlDevice *>(arg);
    Scene *scene = control_device->scene;
    MainCharacter *mc = scene->get_main_character();

    for (;; vTaskDelay(15))
        mc->increment_animation_step();
}

static void rpg_control_fps_report_task(void *arg)
{
    RpgControlDevice *control_device = static_cast<RpgControlDevice *>(arg);
    int fps_counter_old = 0;

    do {
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        std::cout << "screen refresh ";
        std::cout << control_device->fps_counter - fps_counter_old;
        std::cout << " heap ";
        std::cout << esp_get_free_heap_size();
        std::cout << std::endl;

        fps_counter_old = control_device->fps_counter;
    } while (1);
}

static void rpg_control_main_character_task(void *arg)
{
    RpgControlDevice *control_device = static_cast<RpgControlDevice *>(arg);
    Scene *scene = control_device->scene;
    MainCharacter *mc = scene->get_main_character();

    button_t button;
    BaseType_t ret;

    int dx = 0, dy = 0;
    int speed = 1;

    while (true) {
        ret =
            xQueueReceive(button_event_queue, &button, 10 / portTICK_PERIOD_MS);
        if (ret == pdTRUE) {
            switch (button) {
            case BUTTON_DOWN:
                dx = 0;
                dy = speed;
                break;

            case BUTTON_LEFT:
                dx = -speed;
                dy = 0;
                break;

            case BUTTON_RIGHT:
                dx = speed;
                dy = 0;
                break;

            case BUTTON_UP:
                dx = 0;
                dy = -speed;
                break;

            case BUTTON_LEFT_RELEASE:
            case BUTTON_RIGHT_RELEASE:
                dx = 0;
                break;

            case BUTTON_DOWN_RELEASE:
            case BUTTON_UP_RELEASE:
                dy = 0;
                break;

            default:
                break;
            }
        }

        if (dx != 0 || dy != 0) {
            mc->move(mc->get_scene_x() + dx, mc->get_scene_y() + dy);
        }
    }
}

static void rpg_control_render_task(void *arg)
{
    RpgControlDevice *control_device = static_cast<RpgControlDevice *>(arg);
    Scene *scene = control_device->scene;
    MainCharacter *mc = scene->get_main_character();
    Viewport &viewport = scene->get_viewport();
    local_coordinates_t coordinates{};

    for (;; control_device->fps_counter++) {
        coordinates = viewport.get_local_coordinates(mc->get_scene_x(),
                                                     mc->get_scene_y());

        rpg_control_render_scoll_viewport(viewport, coordinates);

        scene->render();
        vTaskDelay(1);
    }
}

static void rpg_control_status_bar(void *arg)
{
    RpgControlDevice *control_device = static_cast<RpgControlDevice *>(arg);
    Scene *scene = control_device->scene;

    while (!scene->lock())
        ;

    graphics_draw_from_library(LIBRARY_IMAGE_STATUS_BAR, 0, 210);
    graphics_update_display();

    scene->unlock();
    vTaskPrioritySet(NULL, rpg_control_priority_low);

    while (1)
        vTaskDelay(100);
}

void rpg_control_take(Scene &scene)
{
    RpgControlDevice control_device{};
    control_device.scene = &scene;

    xTaskCreate(rpg_control_main_character_task, "rpg_main_char", 2500,
                &control_device, rpg_control_priority_high,
                &(control_device.task_main_character));

    xTaskCreate(rpg_control_status_bar, "rpg_status_bar", 4000, &control_device,
                rpg_control_priority_high, &(control_device.task_status_bar));

    xTaskCreate(rpg_control_render_task, "rpg_render", 4000, &control_device,
                rpg_control_priority_medium, &(control_device.task_render));

    xTaskCreate(rpg_control_animation_step_task, "rpg_animation", 1500,
                &control_device, rpg_control_priority_low,
                &(control_device.task_animation_step));

    xTaskCreate(rpg_control_fps_report_task, "rpg_fps_report", 2500,
                &control_device, rpg_control_priority_low,
                &(control_device.task_fps_report));

    while (true) {
        vTaskDelay(10);
        // TODO break
    }

    if (control_device.task_animation_step)
        vTaskDelete(control_device.task_animation_step);

    if (control_device.task_fps_report)
        vTaskDelete(control_device.task_fps_report);

    if (control_device.task_main_character)
        vTaskDelete(control_device.task_main_character);

    if (control_device.task_render)
        vTaskDelete(control_device.task_render);

    if (control_device.task_status_bar)
        vTaskDelete(control_device.task_status_bar);
}

} // namespace rpg
