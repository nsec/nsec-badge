#include "rpg_control.h"

#include "rpg_action.h"

#include "rpg/Coordinates.h"
#include "rpg/Viewport.h"
#include "rpg/characters/MainCharacter.h"

#include "buttons.h"
#include "graphics.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <iostream>

#define CONTINUE_RUNNING_TASK                                                  \
    (control_device->exit_action == ControlExitAction::nothing)

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
    ControlExitAction exit_action;
    int fps_counter;
};

static void rpg_control_center_on_character(Scene *scene, MainCharacter *mc)
{
    auto coordinates = mc->get_coordinates();
    coordinates.change_xy_by(mc->get_width() / 2, mc->get_height() / 2);
    coordinates.change_xy_by(-(viewport_width / 2), -(viewport_height / 2));

    scene->lock();
    scene->get_viewport().move(coordinates);
    scene->unlock();
}

#if 0
/**
 * Smooth scrolling variant of the viewport scroller.
 */
static void rpg_control_render_scoll_viewport(Viewport &viewport,
                                              ScreenCoordinates coordinates)
{
    constexpr int clipping = 11;
    constexpr int easing = 2;

    constexpr int top = 2.5 * DISPLAY_TILE_HEIGHT;
    constexpr int left = 2.5 * DISPLAY_TILE_WIDTH;
    constexpr int right = viewport_width - 5 * DISPLAY_TILE_WIDTH;
    constexpr int bottom = viewport_height - 4.5 * DISPLAY_TILE_HEIGHT;

    if (coordinates.x() < left) {
        int move = (coordinates.x() - left) / easing;
        if (move > -clipping)
            move = -clipping;

        viewport.move_relative(GlobalCoordinates::xy(move, 0));
    }

    if (coordinates.y() < top) {
        int move = (coordinates.y() - top) / easing;
        if (move > -clipping)
            move = -clipping;

        viewport.move_relative(GlobalCoordinates::xy(0, move));
    }

    if (coordinates.x() > right) {
        int move = (coordinates.x() - right) / easing;
        if (move < clipping)
            move = clipping;

        viewport.move_relative(GlobalCoordinates::xy(move, 0));
    }

    if (coordinates.y() > bottom) {
        int move = (coordinates.y() - bottom) / easing;
        if (move < clipping)
            move = clipping;

        viewport.move_relative(GlobalCoordinates::xy(0, move));
    }
}
#else
/**
 * Stepping variant of the viewport scroller.
 */
static void rpg_control_render_scoll_viewport(Viewport &viewport,
                                              ScreenCoordinates coordinates)
{
    constexpr int top = 15;
    constexpr int left = 15;
    constexpr int right = viewport_width - 55;
    constexpr int bottom = viewport_height - 65;

    if (coordinates.x() < left) {
        viewport.move_relative(GlobalCoordinates::xy(-130, 0));
    }

    if (coordinates.y() < top) {
        viewport.move_relative(GlobalCoordinates::xy(0, -130));
    }

    if (coordinates.x() > right) {
        viewport.move_relative(GlobalCoordinates::xy(130, 0));
    }

    if (coordinates.y() > bottom) {
        viewport.move_relative(GlobalCoordinates::xy(0, 130));
    }
}
#endif

static void rpg_control_animation_step_task(void *arg)
{
    RpgControlDevice *control_device = static_cast<RpgControlDevice *>(arg);
    Scene *scene = control_device->scene;

    for (; CONTINUE_RUNNING_TASK; vTaskDelay(5)) {
        for (auto character : scene->get_characters())
            character->increment_animation_step();
    }

    // Self-destruct.
    vTaskDelete(NULL);
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
    } while (CONTINUE_RUNNING_TASK);

    // Self-destruct.
    vTaskDelete(NULL);
}

static void rpg_control_main_character_task(void *arg)
{
    RpgControlDevice *control_device = static_cast<RpgControlDevice *>(arg);
    Scene *scene = control_device->scene;
    MainCharacter *mc = scene->get_main_character();

    button_t button;
    int dx = 0;
    int dy = 0;
    int speed = 1;

    nsec_buttons_flush();

    while (CONTINUE_RUNNING_TASK) {
        button = BUTTON_NONE;
        xQueueReceive(button_event_queue, &button, 10 / portTICK_PERIOD_MS);

        if (button != BUTTON_NONE) {
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

            case BUTTON_ENTER:
                rpg_control_center_on_character(scene, mc);
                break;

            default:
                break;
            }

            control_device->exit_action =
                rpg_action_main_handle(scene, button, nullptr);
        }

        if (dx != 0 || dy != 0) {
            auto coordinates = mc->get_coordinates();
            coordinates.change_xy_by(dx, dy);
            mc->move(coordinates);
        }
    }

    // Self-destruct.
    vTaskDelete(NULL);
}

static void rpg_control_render_task(void *arg)
{
    RpgControlDevice *control_device = static_cast<RpgControlDevice *>(arg);
    Scene *scene = control_device->scene;
    MainCharacter *mc = scene->get_main_character();
    Viewport &viewport = scene->get_viewport();

    viewport.mark_for_full_refresh();

    for (; CONTINUE_RUNNING_TASK; control_device->fps_counter++) {
        rpg_control_render_scoll_viewport(
            viewport, viewport.to_screen(mc->get_coordinates()));

        scene->render();
        vTaskDelay(1);
    }

    // Self-destruct.
    vTaskDelete(NULL);
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

    while (CONTINUE_RUNNING_TASK)
        vTaskDelay(100);

    // Self-destruct.
    vTaskDelete(NULL);
}

ControlExitAction rpg_control_take(Scene &scene)
{
    RpgControlDevice control_device{};
    control_device.exit_action = ControlExitAction::nothing;
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

#ifdef LOG_REFRESH
    xTaskCreate(rpg_control_fps_report_task, "rpg_fps_report", 2500,
                &control_device, rpg_control_priority_low,
                &(control_device.task_fps_report));
#endif

    while (control_device.exit_action == ControlExitAction::nothing) {
        vTaskDelay(10);
    }

    if (control_device.task_animation_step)
        while (eTaskGetState(control_device.task_animation_step) != eDeleted)
            vTaskDelay(1);

    if (control_device.task_fps_report)
        while (eTaskGetState(control_device.task_fps_report) != eDeleted)
            vTaskDelay(1);

    if (control_device.task_main_character)
        while (eTaskGetState(control_device.task_main_character) != eDeleted)
            vTaskDelay(1);

    if (control_device.task_render)
        while (eTaskGetState(control_device.task_render) != eDeleted)
            vTaskDelay(1);

    if (control_device.task_status_bar)
        while (eTaskGetState(control_device.task_status_bar) != eDeleted)
            vTaskDelay(1);

    return control_device.exit_action;
}

} // namespace rpg
