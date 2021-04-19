#include "buzzer.h"

#include "save.h"

#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define PIN_BUZZER 26

namespace music
{

//// https://onlinesequencer.net/1675803#t0
const static int music_astronomia[] = {
    Bb4 | 8, Bb4 | 8, Bb4 | 8, Bb4 | 8, D_5 | 8, D_5 | 8, D_5 | 8, D_5 | 8,
    C_5 | 8, C_5 | 8, C_5 | 8, C_5 | 8, F_5 | 8, F_5 | 8, F_5 | 8, F_5 | 8,
    G_5 | 8, G_5 | 8, G_5 | 8, G_5 | 8, G_5 | 8, G_5 | 8, G_5 | 8, G_5 | 8,
    G_5 | 4, G_5 | 4, C_5 | 8, Bb4 | 8, A_4 | 8, F_4 | 8,

    G_4 | 4, G_4 | 8, D_5 | 8, C_5 | 4, Bb4 | 4, A_4 | 4, A_4 | 8, A_4 | 8,
    C_5 | 4, Bb4 | 8, A_4 | 8, G_4 | 4, G_4 | 8, Ab5 | 8, A_5 | 8, Ab5 | 8,
    A_5 | 8, Ab5 | 8, G_4 | 4, G_4 | 8, Ab5 | 8, A_5 | 8, Ab5 | 8, A_5 | 8,
    Ab5 | 8,

    G_4 | 4, G_4 | 8, D_5 | 8, C_5 | 4, Bb4 | 4, A_4 | 4, A_4 | 8, A_4 | 8,
    C_5 | 4, Bb4 | 8, A_4 | 8, G_4 | 4, G_4 | 8, Ab5 | 8, A_5 | 8, Ab5 | 8,
    A_5 | 8, Ab5 | 8, G_4 | 4, G_4 | 8, Ab5 | 8, A_5 | 8, Ab5 | 8, A_5 | 8,
    Ab5 | 8,

    0,
};

const static int music_nyan[] = {
    G_5 | 4, A_5 | 4, E_5 | 8, E_5 | 4, C_5 | 8, Eb5 | 8, D_5 | 8, C_5 | 2,
    D_5 | 4,

    Eb5 | 4, Eb5 | 8, D_5 | 8, C_5 | 8, D_5 | 8, E_5 | 8, G_5 | 8, A_5 | 8,
    E_5 | 8, G_5 | 8, D_5 | 8, E_5 | 8, C_5 | 8, D_5 | 8, C_5 | 8,

    G_5 | 4, A_5 | 4, E_5 | 8, E_5 | 4, C_5 | 8, Eb5 | 8, D_5 | 8, C_5 | 2,
    D_5 | 4,

    Eb5 | 4, Eb5 | 8, D_5 | 8, C_5 | 8, D_5 | 8, E_5 | 8, G_5 | 8, A_5 | 8,
    E_5 | 8, G_5 | 8, D_5 | 8, E_5 | 8, C_5 | 8, D_5 | 8, C_5 | 8,

    G_5 | 4, A_5 | 4, E_5 | 8, E_5 | 4, C_5 | 8, Eb5 | 8, D_5 | 8, C_5 | 2,
    D_5 | 4,

    Eb5 | 4, Eb5 | 8, D_5 | 8, C_5 | 8, D_5 | 8, E_5 | 8, G_5 | 8, A_5 | 8,
    E_5 | 8, G_5 | 8, D_5 | 8, E_5 | 8, C_5 | 8, D_5 | 8, C_5 | 8,

    E_5 | 4, G_5 | 4, A_5 | 8, E_5 | 8, G_5 | 8, D_5 | 8, E_5 | 8, C_5 | 8,
    Eb5 | 8, D_5 | 8, C_5 | 8, D_5 | 8,

    Eb5 | 4, C_5 | 8, D_5 | 8, E_5 | 8, G_5 | 8, D_5 | 8, E_5 | 8, D_5 | 8,
    C_5 | 8, D_5 | 4, C_5 | 4, D_5 | 4,

    C_5 | 4, G_4 | 8, A_4 | 8, C_5 | 4, G_4 | 8, A_4 | 8, C_5 | 8, D_5 | 8,
    E_5 | 8, C_5 | 8, F_5 | 8, E_5 | 8, F_5 | 8, G_5 | 8,

    C_5 | 8, 000 | 8, C_5 | 8, 000 | 8, G_4 | 8, A_4 | 8, C_5 | 8, G_4 | 8,
    F_5 | 8, E_5 | 8, D_5 | 8, C_5 | 8, G_4 | 8, E_4 | 8, F_4 | 8, G_4 | 8,

    C_5 | 4, G_4 | 8, A_4 | 8, C_5 | 4, G_4 | 8, A_4 | 8, C_5 | 8, C_5 | 8,
    D_5 | 8, E_5 | 8, C_5 | 8, G_4 | 8, A_4 | 8, G_4 | 8,

    C_5 | 8, C_5 | 8, C_5 | 8, B_5 | 8, C_5 | 8, G_4 | 8, A_4 | 8, C_5 | 8,
    F_5 | 8, E_5 | 8, F_5 | 8, G_5 | 8, C_5 | 8, B_5 | 8,

    C_5 | 4, G_4 | 8, A_4 | 8, C_5 | 4, G_4 | 8, A_5 | 8, C_5 | 8, D_5 | 8,
    E_5 | 8, C_5 | 8, F_5 | 8, E_5 | 8, F_5 | 8, G_5 | 8,

    C_5 | 4, C_5 | 4, G_4 | 8, A_4 | 8, C_5 | 8, G_4 | 8, F_5 | 8, E_5 | 8,
    D_5 | 8, C_5 | 8, G_4 | 8, E_4 | 8, F_4 | 8, G_4 | 8,

    C_5 | 4, G_4 | 8, A_4 | 8, C_5 | 4, G_4 | 8, A_4 | 8, C_5 | 8, C_5 | 8,
    D_5 | 8, E_5 | 8, C_5 | 8, G_4 | 8, A_4 | 8, G_4 | 8,

    C_5 | 4, C_5 | 8, B_5 | 8, C_5 | 8, G_4 | 8, A_4 | 8, C_5 | 8, F_5 | 8,
    E_5 | 8, F_5 | 8, G_5 | 8, C_5 | 4, D_5 | 4,

    0,
};

const static int music_starwars[] = {
    A_3 | 4,  A_3 | 4,  A_3 | 4,  F_3 | 24, C_4 | 16,

    A_3 | 4,  F_3 | 24, C_4 | 16, A_3 | 2,

    E_4 | 4,  E_4 | 4,  E_4 | 4,  F_4 | 24, C_4 | 16,

    Ab3 | 4,  F_3 | 24, C_4 | 16, A_3 | 2,

    A_4 | 4,  A_3 | 24, A_3 | 16, A_4 | 4,  Ab4 | 24, G_4 | 16,

    Gb4 | 16, E_4 | 16, F_4 | 8,  F_4 | 8,

    F_3 | 8,  Ab3 | 4,  F_3 | 24, A_3 | 16,

    C_4 | 4,  A_3 | 24, C_4 | 16, E_4 | 2,

    A_4 | 4,  A_3 | 24, A_3 | 16, A_4 | 4,  Ab4 | 24, G_4 | 16,

    Gb4 | 16, E_4 | 16, F_4 | 8,  F_4 | 8,  Bb3 | 8,  Eb4 | 4,
    D_4 | 24, Db4 | 16,

    C_4 | 16, B_3 | 16, C_4 | 8,  C_4 | 8,  F_3 | 8,  Ab3 | 4,
    F_3 | 24, C_4 | 16,

    A_3 | 4,  F_3 | 24, C_4 | 16, A_3 | 2,

    0,
};

const static int sfx_dialog[] = {
    D_3 | 1,
    E_3 | 1,
    0,
};

const static int sfx_failure[] = {
    Gb3 | 4,
    F_3 | 4,
    E_3 | 1,
    0,
};

const static int sfx_steps[] = {
    C_3 | 4,
    A_3 | 4,
    C_0 | 2,
    0,
};

const static int sfx_success[] = {
    A_4 | 4, A_4 | 8, D_5 | 8, F_5 | 8, A_5 | 8, D_5 | 8, F_5 | 8, A_5 | 2, 0,
};

} // namespace music

static TaskHandle_t buzzer_task_handle;
static ledc_timer_config_t ledc_timer;
static ledc_channel_config_t ledc_channel;
static music::Playlist playlist{};

static void buzzer_play_tone(int note, int duration)
{
    // Set the note's frequency
    if (note != 0) {
        ledc_set_freq(ledc_channel.speed_mode, ledc_timer.timer_num, note);
    }

    // Set the note's duration
    ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, duration);
    // update
    ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
    vTaskDelay(duration / portTICK_PERIOD_MS);

    // Set back to the default values
    ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, 0);
    ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
}

static void buzzer_play_music()
{
    const int *music = playlist.music;
    int duration;
    int note;

    if (!music)
        return;

    while (playlist.repeat > 0) {
        for (int i = 0; music[i] != 0; ++i) {
            if (music != playlist.music || playlist.repeat < 1)
                return;

            duration = 0;
            duration += (playlist.bpm * (music[i] & 1));
            duration += (playlist.bpm * (music[i] & 2)) / 4;
            duration += (playlist.bpm * (music[i] & 4)) / 16;
            duration += (playlist.bpm * (music[i] & 8)) / 64;
            duration += (playlist.bpm * (music[i] & 16)) / 256;

            if (duration == 0)
                break;

            note = (music[i] >> 5) / 100;
            buzzer_play_tone(note, duration);
        }

        playlist.repeat--;
    }
}

static void buzzer_task(void *arg)
{
    while (true) {
        if (playlist.music && playlist.repeat > 0) {
            buzzer_play_music();
            continue;
        }

        vTaskDelay(5);
    }
}

void buzzer_init()
{
    // Pulse Width Modulation (PWM) initialization
    // https://circuitdigest.com/tutorial/what-is-pwm-pulse-width-modulation
    ledc_timer.duty_resolution = LEDC_TIMER_13_BIT;
    ledc_timer.freq_hz = 3000;
    ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_timer.timer_num = LEDC_TIMER_0;

    ledc_channel.channel = LEDC_CHANNEL_0;
    ledc_channel.duty = 0;
    ledc_channel.gpio_num = PIN_BUZZER;
    ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel.timer_sel = LEDC_TIMER_0;

    // Set configuration of timer0 for high speed channels
    ledc_timer_config(&ledc_timer);

    ledc_channel_config(&ledc_channel);

    // Initialize fade service.
    ledc_fade_func_install(0);

    if (Save::save_data.buzzer_enable_music)
        buzzer_request_music(
            static_cast<music::Music>(Save::save_data.buzzer_enable_music_id));

    xTaskCreate(buzzer_task, "Buzzer", 4096, NULL, 99, &buzzer_task_handle);
}

void buzzer_request_music(music::Music music_id)
{
    switch (music_id) {
    case music::Music::music_astronomia:
        if (Save::save_data.buzzer_enable_music) {
            playlist.bpm = 1800;
            playlist.id = music_id;
            playlist.music = music::music_astronomia;
            playlist.repeat = 1000;
        }
        return;

    case music::Music::music_nyan:
        if (Save::save_data.buzzer_enable_music) {
            playlist.bpm = 1111;
            playlist.id = music_id;
            playlist.music = music::music_nyan;
            playlist.repeat = 1000;
        }
        return;

    case music::Music::music_starwars:
        if (Save::save_data.buzzer_enable_music) {
            playlist.bpm = 2200;
            playlist.id = music_id;
            playlist.music = music::music_starwars;
            playlist.repeat = 1000;
        }
        return;

    case music::Music::sfx_dialog:
        if (Save::save_data.buzzer_enable_sfx) {
            playlist.bpm = 200;
            playlist.id = music_id;
            playlist.music = music::sfx_dialog;
            playlist.repeat = 1000;
        }
        return;

    case music::Music::sfx_failure:
        if (Save::save_data.buzzer_enable_sfx) {
            playlist.bpm = 1000;
            playlist.id = music_id;
            playlist.music = music::sfx_failure;
            playlist.repeat = 1;
        }
        return;

    case music::Music::sfx_steps:
        if (Save::save_data.buzzer_enable_steps) {
            playlist.bpm = 200;
            playlist.id = music_id;
            playlist.music = music::sfx_steps;
            playlist.repeat = 1000;
        }
        return;

    case music::Music::sfx_success:
        if (Save::save_data.buzzer_enable_sfx) {
            playlist.bpm = 1000;
            playlist.id = music_id;
            playlist.music = music::sfx_success;
            playlist.repeat = 1;
        }
        return;

    default:
        buzzer_stop_music();
    }
}

void buzzer_stop_music()
{
    playlist.id = music::Music::NONE;
    playlist.music = nullptr;
    playlist.repeat = 0;
}

void buzzer_stop_music(music::Music music_id)
{
    if (playlist.id == music_id)
        buzzer_stop_music();
}
