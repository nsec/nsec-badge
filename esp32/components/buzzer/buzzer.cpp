#include "buzzer.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define PIN_BUZZER 26

const static playback_t music0[] = {
    {NOTE_LA3, Q},    {NOTE_LA3, Q},     {NOTE_LA3, Q},
    {NOTE_F3, E + S}, {NOTE_C4, S},

    {NOTE_LA3, Q},    {NOTE_F3, E + S},  {NOTE_C4, S},
    {NOTE_LA3, H},

    {NOTE_E4, Q},     {NOTE_E4, Q},      {NOTE_E4, Q},
    {NOTE_F4, E + S}, {NOTE_C4, S},

    {NOTE_Ab3, Q},    {NOTE_F3, E + S},  {NOTE_C4, S},
    {NOTE_LA3, H},

    {NOTE_LA4, Q},    {NOTE_LA3, E + S}, {NOTE_LA3, S},
    {NOTE_LA4, Q},    {NOTE_Ab4, E + S}, {NOTE_G4, S},

    {NOTE_Gb4, S},    {NOTE_E4, S},      {NOTE_F4, E},

    {0, E},           {NOTE_F3, E},      {NOTE_Ab3, Q},
    {NOTE_F3, E + S}, {NOTE_LA3, S},

    {NOTE_C4, Q},     {NOTE_LA3, E + S}, {NOTE_C4, S},
    {NOTE_E4, H},

    {NOTE_LA4, Q},    {NOTE_LA3, E + S}, {NOTE_LA3, S},
    {NOTE_LA4, Q},    {NOTE_Ab4, E + S}, {NOTE_G4, S},

    {NOTE_Gb4, S},    {NOTE_E4, S},      {NOTE_F4, E},
    {0, E},           {NOTE_Bb3, E},     {NOTE_Eb4, Q},
    {NOTE_D4, E + S}, {NOTE_Db4, S},

    {NOTE_C4, S},     {NOTE_B3, S},      {NOTE_C4, E},
    {0, E},           {NOTE_F3, E},      {NOTE_Ab3, Q},
    {NOTE_F3, E + S}, {NOTE_C4, S},

    {NOTE_LA3, Q},    {NOTE_F3, E + S},  {NOTE_C4, S},
    {NOTE_LA3, H},    {0, 2 * E}};

// https://onlinesequencer.net/1675803#t0
const static playback_t music1[] = {
    {NOTE_Bb4, E},
    {NOTE_Bb4, E},
    {NOTE_Bb4, E},
    {NOTE_Bb4, E},

    {NOTE_D5, E},
    {NOTE_D5, E},
    {NOTE_D5, E},
    {NOTE_D5, E},

    {NOTE_C5, E},
    {NOTE_C5, E},
    {NOTE_C5, E},
    {NOTE_C5, E},

    {NOTE_F5, E},
    {NOTE_F5, E},
    {NOTE_F5, E},
    {NOTE_F5, E},
    // --------------
    {NOTE_G5, E},
    {NOTE_G5, E},
    {NOTE_G5, E},
    {NOTE_G5, E},

    {NOTE_G5, E},
    {NOTE_G5, E},
    {NOTE_G5, E},
    {NOTE_G5, E},

    {NOTE_G5, Q},
    {NOTE_G5, Q},

    {NOTE_C5, E},
    {NOTE_Bb4, E},
    {NOTE_LA4, E},
    {NOTE_F4, E},
    // --------------
    {NOTE_G4, Q},
    {NOTE_G4, E},
    {NOTE_D5, E},

    {NOTE_C5, Q},
    {NOTE_Bb4, Q},

    {NOTE_LA4, Q},
    {NOTE_LA4, E},
    {NOTE_LA4, E},

    {NOTE_C5, Q},
    {NOTE_Bb4, E},
    {NOTE_LA4, E},
    // --------------
    {NOTE_G4, Q},
    {NOTE_G4, E},
    {NOTE_Ab4, E},

    {NOTE_LA4, E},
    {NOTE_Ab4, E},
    {NOTE_LA4, E},
    {NOTE_Ab4, E},

    {NOTE_G4, Q},
    {NOTE_G4, E},
    {NOTE_Ab4, E},

    {NOTE_LA4, E},
    {NOTE_Ab4, E},
    {NOTE_LA4, E},
    {NOTE_Ab4, E},
    // --------------
    {NOTE_G4, Q},
    {NOTE_G4, E},
    {NOTE_D5, E},

    {NOTE_C5, Q},
    {NOTE_Bb4, Q},

    {NOTE_LA4, Q},
    {NOTE_LA4, E},
    {NOTE_LA4, E},

    {NOTE_C5, Q},
    {NOTE_Bb4, E},
    {NOTE_LA4, E},
    // --------------
    {NOTE_G4, Q},
    {NOTE_G4, E},
    {NOTE_Ab4, E},

    {NOTE_LA4, E},
    {NOTE_Ab4, E},
    {NOTE_LA4, E},
    {NOTE_Ab4, E},

    {NOTE_G4, Q},
    {NOTE_G4, E},
    {NOTE_Ab4, E},

    {NOTE_LA4, E},
    {NOTE_Ab4, E},
    {NOTE_LA4, E},
    {NOTE_Ab4, E},
    // --------------
    {NOTE_G4, Q},

};

const static playback_t music2[] = {
// Part 1
    {NOTE_G5, Q},
    {NOTE_LA5, Q},
    {NOTE_E5, E},
    {NOTE_E5, Q},
    {NOTE_C5, E},
    {NOTE_Eb5, E},
    {NOTE_D5, E},
    {NOTE_C5, H},
    {NOTE_D5, Q},

    {NOTE_Eb5, Q},
    {NOTE_Eb5, E},
    {NOTE_D5, E},
    {NOTE_C5, E},
    {NOTE_D5, E},
    {NOTE_E5, E},
    {NOTE_G5, E},
    {NOTE_LA5, E},
    {NOTE_E5, E},
    {NOTE_G5, E},
    {NOTE_D5, E},
    {NOTE_E5, E},
    {NOTE_C5, E},
    {NOTE_D5, E},
    {NOTE_C5, E},

// Part 1 repeat
    {NOTE_G5, Q},
    {NOTE_LA5, Q},
    {NOTE_E5, E},
    {NOTE_E5, Q},
    {NOTE_C5, E},
    {NOTE_Eb5, E},
    {NOTE_D5, E},
    {NOTE_C5, H},
    {NOTE_D5, Q},

    {NOTE_Eb5, Q},
    {NOTE_Eb5, E},
    {NOTE_D5, E},
    {NOTE_C5, E},
    {NOTE_D5, E},
    {NOTE_E5, E},
    {NOTE_G5, E},
    {NOTE_LA5, E},
    {NOTE_E5, E},
    {NOTE_G5, E},
    {NOTE_D5, E},
    {NOTE_E5, E},
    {NOTE_C5, E},
    {NOTE_D5, E},
    {NOTE_C5, E},

// Part 1 repeat again
    {NOTE_G5, Q},
    {NOTE_LA5, Q},
    {NOTE_E5, E},
    {NOTE_E5, Q},
    {NOTE_C5, E},
    {NOTE_Eb5, E},
    {NOTE_D5, E},
    {NOTE_C5, H},
    {NOTE_D5, Q},

    {NOTE_Eb5, Q},
    {NOTE_Eb5, E},
    {NOTE_D5, E},
    {NOTE_C5, E},
    {NOTE_D5, E},
    {NOTE_E5, E},
    {NOTE_G5, E},
    {NOTE_LA5, E},
    {NOTE_E5, E},
    {NOTE_G5, E},
    {NOTE_D5, E},
    {NOTE_E5, E},
    {NOTE_C5, E},
    {NOTE_D5, E},
    {NOTE_C5, E},

// Part 2
    {NOTE_E5, Q},
    {NOTE_G5, Q},
    {NOTE_LA5, E},
    {NOTE_E5, E},
    {NOTE_G5, E},
    {NOTE_D5, E},
    {NOTE_E5, E},
    {NOTE_C5, E},
    {NOTE_Eb5, E},
    {NOTE_D5, E},
    {NOTE_C5, E},
    {NOTE_D5, E},

    {NOTE_Eb5, Q},
    {NOTE_C5, E},
    {NOTE_D5, E},
    {NOTE_E5, E},
    {NOTE_G5, E},
    {NOTE_D5, E},
    {NOTE_E5, E},
    {NOTE_D5, E},
    {NOTE_C5, E},
    {NOTE_D5, Q},
    {NOTE_C5, Q},
    {NOTE_D5, Q},

// Part 3
    {NOTE_C5, Q},
    {NOTE_G4, E},
    {NOTE_LA4, E},
    {NOTE_C5, Q},
    {NOTE_G4, E},
    {NOTE_LA4, E},
    {NOTE_C5, E},
    {NOTE_D5, E},
    {NOTE_E5, E},
    {NOTE_C5, E},
    {NOTE_F5, E},
    {NOTE_E5, E},
    {NOTE_F5, E},
    {NOTE_G5, E},

    {NOTE_C5, E},
    {0, E},
    {NOTE_C5, E},
    {0, E},
    {NOTE_G4, E},
    {NOTE_LA4, E},
    {NOTE_C5, E},
    {NOTE_G4, E},
    {NOTE_F5, E},
    {NOTE_E5, E},
    {NOTE_D5, E},
    {NOTE_C5, E},
    {NOTE_G4, E},
    {NOTE_E4, E},
    {NOTE_F4, E},
    {NOTE_G4, E},

    {NOTE_C5, Q},
    {NOTE_G4, E},
    {NOTE_LA4, E},
    {NOTE_C5, Q},
    {NOTE_G4, E},
    {NOTE_LA4, E},
    {NOTE_C5, E},
    {NOTE_C5, E},
    {NOTE_D5, E},
    {NOTE_E5, E},
    {NOTE_C5, E},
    {NOTE_G4, E},
    {NOTE_LA4, E},
    {NOTE_G4, E},

    {NOTE_C5, E},
    {NOTE_C5, E},
    {NOTE_C5, E},
    {NOTE_B5, E},
    {NOTE_C5, E},
    {NOTE_G4, E},
    {NOTE_LA4, E},
    {NOTE_C5, E},
    {NOTE_F5, E},
    {NOTE_E5, E},
    {NOTE_F5, E},
    {NOTE_G5, E},
    {NOTE_C5, E},
    {NOTE_B5, E},

    {NOTE_C5, Q},
    {NOTE_G4, E},
    {NOTE_LA4, E},
    {NOTE_C5, Q},
    {NOTE_G4, E},
    {NOTE_LA5, E},
    {NOTE_C5, E},
    {NOTE_D5, E},
    {NOTE_E5, E},
    {NOTE_C5, E},
    {NOTE_F5, E},
    {NOTE_E5, E},
    {NOTE_F5, E},
    {NOTE_G5, E},

    {NOTE_C5, Q},
    {NOTE_C5, Q},
    {NOTE_G4, E},
    {NOTE_LA4, E},
    {NOTE_C5, E},
    {NOTE_G4, E},
    {NOTE_F5, E},
    {NOTE_E5, E},
    {NOTE_D5, E},
    {NOTE_C5, E},
    {NOTE_G4, E},
    {NOTE_E4, E},
    {NOTE_F4, E},
    {NOTE_G4, E},

    {NOTE_C5, Q},
    {NOTE_G4, E},
    {NOTE_LA4, E},
    {NOTE_C5, Q},
    {NOTE_G4, E},
    {NOTE_LA4, E},
    {NOTE_C5, E},
    {NOTE_C5, E},
    {NOTE_D5, E},
    {NOTE_E5, E},
    {NOTE_C5, E},
    {NOTE_G4, E},
    {NOTE_LA4, E},
    {NOTE_G4, E},

    {NOTE_C5, Q},
    {NOTE_C5, E},
    {NOTE_B5, E},
    {NOTE_C5, E},
    {NOTE_G4, E},
    {NOTE_LA4, E},
    {NOTE_C5, E},
    {NOTE_F5, E},
    {NOTE_E5, E},
    {NOTE_F5, E},
    {NOTE_G5, E},
    {NOTE_C5, Q},
    {NOTE_D5, Q},
};

ledc_timer_config_t ledc_timer;
ledc_channel_config_t ledc_channel;

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
}

void buzzer_play_tone(int note, int duration)
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

void buzzer_play_song(void *pvParameters)
{
    while (true) {
        for (int i = 0; i < sizeof(music2) / sizeof(playback_t); i++) {
            buzzer_play_tone(music2[i].note, music2[i].duration);
        }
        for (int i = 0; i < sizeof(music0) / sizeof(playback_t); i++) {
            buzzer_play_tone(music0[i].note, music0[i].duration);
        }
        buzzer_play_tone(0, Q);
        for (int i = 0; i < sizeof(music1) / sizeof(playback_t); i++) {
            buzzer_play_tone(music1[i].note, music1[i].duration);
        }
    }
}
