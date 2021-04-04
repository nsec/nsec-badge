#pragma once

// http://www.phy.mtu.edu/~suits/notefreqs.html
// Put NOTE_ as prefix to avoid redefinition
#define NOTE_C0 16.35
#define NOTE_Db0 17.32
#define NOTE_D0 18.35
#define NOTE_Eb0 19.45
#define NOTE_E0 20.60
#define NOTE_F0 21.83
#define NOTE_Gb0 23.12
#define NOTE_G0 24.50
#define NOTE_Ab0 25.96
#define NOTE_LA0 27.50
#define NOTE_Bb0 29.14
#define NOTE_B0 30.87
#define NOTE_C1 32.70
#define NOTE_Db1 34.65
#define NOTE_D1 36.71
#define NOTE_Eb1 38.89
#define NOTE_E1 41.20
#define NOTE_F1 43.65
#define NOTE_Gb1 46.25
#define NOTE_G1 49.00
#define NOTE_Ab1 51.91
#define NOTE_LA1 55.00
#define NOTE_Bb1 58.27
#define NOTE_B1 61.74
#define NOTE_C2 65.41
#define NOTE_Db2 69.30
#define NOTE_D2 73.42
#define NOTE_Eb2 77.78
#define NOTE_E2 82.41
#define NOTE_F2 87.31
#define NOTE_Gb2 92.50
#define NOTE_G2 98.00
#define NOTE_Ab2 103.83
#define NOTE_LA2 110.00
#define NOTE_Bb2 116.54
#define NOTE_B2 123.47
#define NOTE_C3 130.81
#define NOTE_Db3 138.59
#define NOTE_D3 146.83
#define NOTE_Eb3 155.56
#define NOTE_E3 164.81
#define NOTE_F3 174.61
#define NOTE_Gb3 185.00
#define NOTE_G3 196.00
#define NOTE_Ab3 207.65
#define NOTE_LA3 220.00
#define NOTE_Bb3 233.08
#define NOTE_B3 246.94
#define NOTE_C4 261.63
#define NOTE_Db4 277.18
#define NOTE_D4 293.66
#define NOTE_Eb4 311.13
#define NOTE_E4 329.63
#define NOTE_F4 349.23
#define NOTE_Gb4 369.99
#define NOTE_G4 392.00
#define NOTE_Ab4 415.30
#define NOTE_LA4 440.00
#define NOTE_Bb4 466.16
#define NOTE_B4 493.88
#define NOTE_C5 523.25
#define NOTE_Db5 554.37
#define NOTE_D5 587.33
#define NOTE_Eb5 622.25
#define NOTE_E5 659.26
#define NOTE_F5 698.46
#define NOTE_Gb5 739.99
#define NOTE_G5 783.99
#define NOTE_Ab5 830.61
#define NOTE_LA5 880.00
#define NOTE_Bb5 932.33
#define NOTE_B5 987.77
#define NOTE_C6 1046.50
#define NOTE_Db6 1108.73
#define NOTE_D6 1174.66
#define NOTE_Eb6 1244.51
#define NOTE_E6 1318.51
#define NOTE_F6 1396.91
#define NOTE_Gb6 1479.98
#define NOTE_G6 1567.98
#define NOTE_Ab6 1661.22
#define NOTE_LA6 1760.00
#define NOTE_Bb6 1864.66
#define NOTE_B6 1975.53
#define NOTE_C7 2093.00
#define NOTE_Db7 2217.46
#define NOTE_D7 2349.32
#define NOTE_Eb7 2489.02
#define NOTE_E7 2637.02
#define NOTE_F7 2793.83
#define NOTE_Gb7 2959.96
#define NOTE_G7 3135.96
#define NOTE_Ab7 3322.44
#define NOTE_LA7 3520.01
#define NOTE_Bb7 3729.31
#define NOTE_B7 3951.07
#define NOTE_C8 4186.01
#define NOTE_Db8 4434.92
#define NOTE_D8 4698.64
#define NOTE_Eb8 4978.03

#define BPM 103 // for the imperial march

// From the formula:
// 60,000 / Bpm = Ms We can break this down as 60'000 (the number of
// milliseconds in a minute).

#define Q 60000 / BPM // quarter 1/4
#define H 2 * Q       // half 2/4
#define E Q / 2       // eighth 1/8
#define S Q / 4       // sixteenth 1/16
#define W 4 * Q       // whole 4/4

typedef struct {
    int note;
    int duration;
} playback_t;

#ifdef __cplusplus
extern "C" {
#endif

void buzzer_init();
void buzzer_play_song(void * pvParameters);

#ifdef __cplusplus
}
#endif
