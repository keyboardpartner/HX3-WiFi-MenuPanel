/*
// #############################################################################
//       __ ________  _____  ____  ___   ___  ___
//      / //_/ __/\ \/ / _ )/ __ \/ _ | / _ \/ _ \
//     / ,< / _/   \  / _  / /_/ / __ |/ , _/ // /
//    /_/|_/___/_  /_/____/\____/_/_|_/_/|_/____/
//      / _ \/ _ | / _ \/_  __/ |/ / __/ _ \
//     / ___/ __ |/ , _/ / / /    / _// , _/
//    /_/  /_/ |_/_/|_| /_/ /_/|_/___/_/|_|
//
// #############################################################################
*/


#ifndef Menu_Items_h
#define Menu_Items_h

#include <Arduino.h>
#include "global_vars.h"
#include "Free_Fonts.h"
#include <Ticker.h>
#include "drawing.h"

// Forward declaration of menu action routines
void savePreset();
void saveWifi();
void saveUpper();
void saveLower();
void savePedal();
void saveOrgan();
void saveSpeaker();
void saveDefaults();
void enterBootloader();
void enterPresetName();

// ------------------------------------------------------------------------------
// Hier Daten aus Excel-Tabelle "MIDI_menuItems.xlsx" einfügen
// ------------------------------------------------------------------------------

// Menu System Variables
#define MENU_ITEMCOUNT 204
#define MAIN_MENU_END  18  // Index des letzten Hauptmenüeintrags, danach folgen die Untermenüs

// Diese Tabelle enthält die Menüstruktur, die in der Excel-Tabelle MIDI_menuItems.xlsx definiert ist
// Menü-Text, Link zu Untermenüs, Zeiger auf Werte, die bei Änderung geändert werden sollen, 
// Action-Routine bei Änderung, Min- und Maximalwerte für die Editierung
const menuEntryType MenuItems[MENU_ITEMCOUNT] PROGMEM = { 
  {"Preset", 199, 203, 268, &savePreset, tm_preset, 0, 99, 0},  // Idx 0
  {"Upper Voice", 25, 36, 269, &saveUpper, tm_numeric, 0, 15, 0},  // Idx 1
  {"Lower Voice", 73, 84, 270, &saveLower, tm_numeric, 0, 15, 0},  // Idx 2
  {"Pedal Voice", 109, 113, 271, &savePedal, tm_numeric, 0, 15, 0},  // Idx 3
  {"Tabs", 126, 137, -1, NULL, tm_none, -1, -1, 0},  // Idx 4
  {"Rotary Speed", 21, 23, 256, NULL, tm_halfmoon, 0, 2, 0},  // Idx 5
  {"Master Volume", 19, 20, 80, NULL, tm_pot, 0, 127, 0},  // Idx 6
  {"Amp Gain", 24, 24, 81, NULL, tm_pot, 0, 127, 0},  // Idx 7
  {"Reverb Prgm", 160, 162, 263, NULL, tm_numeric, 0, 3, 0},  // Idx 15
  {"Equalizer", 163, 173, -1, NULL, tm_none, -1, -1, 0},  // Idx 14
  {"Upper ADSR", 44, 72, -1, NULL, tm_none, -1, -1, 1},  // Idx 8
  {"Upper GM Synth", 37, 43, -1, NULL, tm_none, -1, -1, 0},  // Idx 9
  {"Lower ADSR", 92, 108, -1, NULL, tm_none, -1, -1, 1},  // Idx 10
  {"Lower GM Synth", 85, 91, -1, NULL, tm_none, -1, -1, 0},  // Idx 11
  {"Pedal ADSR", 121, 125, -1, NULL, tm_none, -1, -1, 0},  // Idx 12
  {"Pedal GM Synth", 114, 120, -1, NULL, tm_none, -1, -1, 0},  // Idx 13
  {"Organ", 138, 159, 265, &saveOrgan, tm_organ, 0, 15, 0},  // Idx 16
  {"Speaker", 174, 186, 266, &saveSpeaker, tm_speaker, 0, 15, 0},  // Idx 17
  {"Keybd Setup", 187, 198, -1, &saveDefaults, tm_none, -1, -1, 0},  // Idx 18
  {"AO28 Tone Pot", -1, -1, 87, NULL, tm_pot, 0, 127, 0},  // Idx 19
  {"AO28 Gain Cap", -1, -1, 88, NULL, tm_pot, 0, 127, 0},  // Idx 20
  {"Ext FX Insert", -1, -1, 172, NULL, tm_tab, 0, 1, 0},  // Idx 21
  {"Rotary Bypass", -1, -1, 137, NULL, tm_tab, 0, 1, 0},  // Idx 22
  {"Separate Pedal", -1, -1, 142, NULL, tm_tab, 0, 1, 0},  // Idx 23
  {"Amp Bypass", -1, -1, 136, NULL, tm_tab, 0, 1, 0},  // Idx 24
  {"Drawbar 16", -1, -1, 0, NULL, tm_drawbar, 0, 127, 0},  // Idx 25
  {"Drawbar 5 1/3", -1, -1, 1, NULL, tm_drawbar, 0, 127, 0},  // Idx 26
  {"Drawbar 8", -1, -1, 2, NULL, tm_drawbar, 0, 127, 0},  // Idx 27
  {"Drawbar 4", -1, -1, 3, NULL, tm_drawbar, 0, 127, 0},  // Idx 28
  {"Drawbar 2 2/3", -1, -1, 4, NULL, tm_drawbar, 0, 127, 0},  // Idx 29
  {"Drawbar 2", -1, -1, 5, NULL, tm_drawbar, 0, 127, 0},  // Idx 30
  {"Drawbar 1 3/5", -1, -1, 6, NULL, tm_drawbar, 0, 127, 0},  // Idx 31
  {"Drawbar 1 1/3", -1, -1, 7, NULL, tm_drawbar, 0, 127, 0},  // Idx 32
  {"Drawbar 1", -1, -1, 8, NULL, tm_drawbar, 0, 127, 0},  // Idx 33
  {"Drawbar Mix 1", -1, -1, 9, NULL, tm_drawbar, 0, 127, 0},  // Idx 34
  {"Drawbar Mix 2", -1, -1, 10, NULL, tm_drawbar, 0, 127, 0},  // Idx 35
  {"Drawbar Mix 3", -1, -1, 11, NULL, tm_drawbar, 0, 127, 0},  // Idx 36
  {"GM Prg 1", -1, -1, 224, NULL, tm_numeric, 0, 126, 0},  // Idx 37
  {"GM Level 1", -1, -1, 225, NULL, tm_pot, 0, 127, 0},  // Idx 38
  {"GM Hrm 1", -1, -1, 226, NULL, tm_numeric, 0, 5, 0},  // Idx 39
  {"GM Prg 2", -1, -1, 227, NULL, tm_numeric, 0, 126, 1},  // Idx 40
  {"GM Level 2", -1, -1, 228, NULL, tm_pot, 0, 127, 1},  // Idx 41
  {"GM Harm2", -1, -1, 229, NULL, tm_numeric, 0, 5, 1},  // Idx 42
  {"GM Detune 2", -1, -1, 230, NULL, tm_pot, 0, 15, 1},  // Idx 43
  {"Env/Perc 16", -1, -1, 96, NULL, tm_drawbar, 0, 127, 1},  // Idx 44
  {"Env/Perc 5 1/3", -1, -1, 97, NULL, tm_drawbar, 0, 127, 1},  // Idx 45
  {"Env/Perc 8", -1, -1, 98, NULL, tm_drawbar, 0, 127, 1},  // Idx 46
  {"Env/Perc 4", -1, -1, 99, NULL, tm_drawbar, 0, 127, 1},  // Idx 47
  {"Env/Perc 2 2/3", -1, -1, 100, NULL, tm_drawbar, 0, 127, 1},  // Idx 48
  {"Env/Perc 2", -1, -1, 101, NULL, tm_drawbar, 0, 127, 1},  // Idx 49
  {"Env/Perc 1 3/5", -1, -1, 102, NULL, tm_drawbar, 0, 127, 1},  // Idx 50
  {"Env/Perc 1 1/3", -1, -1, 103, NULL, tm_drawbar, 0, 127, 1},  // Idx 51
  {"Env/Perc 1", -1, -1, 104, NULL, tm_drawbar, 0, 127, 1},  // Idx 52
  {"Env/Perc Mix 1", -1, -1, 105, NULL, tm_drawbar, 0, 127, 1},  // Idx 53
  {"Env/Perc Mix 2", -1, -1, 106, NULL, tm_drawbar, 0, 127, 1},  // Idx 54
  {"Env/Perc Mix 3", -1, -1, 107, NULL, tm_drawbar, 0, 127, 1},  // Idx 55
  {"Attack", -1, -1, 107, NULL, tm_pot, 0, 127, 1},  // Idx 56
  {"Decay", -1, -1, 107, NULL, tm_pot, 0, 127, 1},  // Idx 57
  {"Sustain", -1, -1, 107, NULL, tm_pot, 0, 127, 1},  // Idx 58
  {"Release", -1, -1, 107, NULL, tm_pot, 0, 127, 1},  // Idx 59
  {"ADSR Harmonic", -1, -1, 107, NULL, tm_pot, 0, 127, 1},  // Idx 60
  {"ADSR on 16", -1, -1, 160, NULL, tm_tab, 0, 1, 1},  // Idx 61
  {"ADSR on 5 1/3", -1, -1, 161, NULL, tm_tab, 0, 1, 1},  // Idx 62
  {"ADSR on 8", -1, -1, 162, NULL, tm_tab, 0, 1, 1},  // Idx 63
  {"ADSR on 4", -1, -1, 163, NULL, tm_tab, 0, 1, 1},  // Idx 64
  {"ADSR on 2 2/3", -1, -1, 164, NULL, tm_tab, 0, 1, 1},  // Idx 65
  {"ADSR on 2", -1, -1, 165, NULL, tm_tab, 0, 1, 1},  // Idx 66
  {"ADSR on 1 3/5", -1, -1, 166, NULL, tm_tab, 0, 1, 1},  // Idx 67
  {"ADSR on 1 1/3", -1, -1, 167, NULL, tm_tab, 0, 1, 1},  // Idx 68
  {"ADSR on 1", -1, -1, 168, NULL, tm_tab, 0, 1, 1},  // Idx 69
  {"ADSR on Mix 1", -1, -1, 169, NULL, tm_tab, 0, 1, 1},  // Idx 70
  {"ADSR on Mix 2", -1, -1, 170, NULL, tm_tab, 0, 1, 1},  // Idx 71
  {"ADSR on Mix 3", -1, -1, 171, NULL, tm_tab, 0, 1, 1},  // Idx 72
  {"Drawbar 16", -1, -1, 16, NULL, tm_drawbar, 0, 127, 0},  // Idx 73
  {"Drawbar 5 1/3", -1, -1, 17, NULL, tm_drawbar, 0, 127, 0},  // Idx 74
  {"Drawbar 8", -1, -1, 18, NULL, tm_drawbar, 0, 127, 0},  // Idx 75
  {"Drawbar 4", -1, -1, 19, NULL, tm_drawbar, 0, 127, 0},  // Idx 76
  {"Drawbar 2 2/3", -1, -1, 20, NULL, tm_drawbar, 0, 127, 0},  // Idx 77
  {"Drawbar 2", -1, -1, 21, NULL, tm_drawbar, 0, 127, 0},  // Idx 78
  {"Drawbar 1 3/5", -1, -1, 22, NULL, tm_drawbar, 0, 127, 0},  // Idx 79
  {"Drawbar 1 1/3", -1, -1, 23, NULL, tm_drawbar, 0, 127, 0},  // Idx 80
  {"Drawbar 1", -1, -1, 24, NULL, tm_drawbar, 0, 127, 0},  // Idx 81
  {"Drawbar Mix 1", -1, -1, 25, NULL, tm_drawbar, 0, 127, 1},  // Idx 82
  {"Drawbar Mix 2", -1, -1, 26, NULL, tm_drawbar, 0, 127, 1},  // Idx 83
  {"Drawbar Mix 3", -1, -1, 27, NULL, tm_drawbar, 0, 127, 1},  // Idx 84
  {"GM Prg 1", -1, -1, 232, NULL, tm_numeric, 0, 126, 0},  // Idx 85
  {"GM Level 1", -1, -1, 233, NULL, tm_pot, 0, 127, 0},  // Idx 86
  {"GM Harm1", -1, -1, 234, NULL, tm_numeric, 0, 5, 0},  // Idx 87
  {"GM Prg 2", -1, -1, 235, NULL, tm_numeric, 0, 126, 1},  // Idx 88
  {"GM Level 2", -1, -1, 236, NULL, tm_pot, 0, 127, 1},  // Idx 89
  {"GM Harm2", -1, -1, 237, NULL, tm_numeric, 0, 5, 1},  // Idx 90
  {"GM Detune 2", -1, -1, 238, NULL, tm_pot, 0, 15, 1},  // Idx 91
  {"Attack", -1, -1, 56, NULL, tm_pot, 0, 127, 1},  // Idx 92
  {"Decay", -1, -1, 57, NULL, tm_pot, 0, 127, 1},  // Idx 93
  {"Sustain", -1, -1, 58, NULL, tm_pot, 0, 127, 1},  // Idx 94
  {"Release", -1, -1, 59, NULL, tm_pot, 0, 127, 1},  // Idx 95
  {"ADSR Harmonic", -1, -1, 60, NULL, tm_pot, 0, 127, 1},  // Idx 96
  {"ADSR on 16", -1, -1, 176, NULL, tm_tab, 0, 1, 1},  // Idx 97
  {"ADSR on 5 1/3", -1, -1, 177, NULL, tm_tab, 0, 1, 1},  // Idx 98
  {"ADSR on 8", -1, -1, 178, NULL, tm_tab, 0, 1, 1},  // Idx 99
  {"ADSR on 4", -1, -1, 179, NULL, tm_tab, 0, 1, 1},  // Idx 100
  {"ADSR on 2 2/3", -1, -1, 180, NULL, tm_tab, 0, 1, 1},  // Idx 101
  {"ADSR on 2", -1, -1, 181, NULL, tm_tab, 0, 1, 1},  // Idx 102
  {"ADSR on 1 3/5", -1, -1, 182, NULL, tm_tab, 0, 1, 1},  // Idx 103
  {"ADSR on 1 1/3", -1, -1, 183, NULL, tm_tab, 0, 1, 1},  // Idx 104
  {"ADSR on 1", -1, -1, 184, NULL, tm_tab, 0, 1, 1},  // Idx 105
  {"ADSR on Mix 1", -1, -1, 185, NULL, tm_tab, 0, 1, 1},  // Idx 106
  {"ADSR on Mix 2", -1, -1, 186, NULL, tm_tab, 0, 1, 1},  // Idx 107
  {"ADSR on Mix 3", -1, -1, 187, NULL, tm_tab, 0, 1, 1},  // Idx 108
  {"Drawbar 16", -1, -1, 72, NULL, tm_drawbar, 0, 127, 0},  // Idx 109
  {"Drawbar 16H", -1, -1, 73, NULL, tm_drawbar, 0, 127, 0},  // Idx 110
  {"Drawbar 8", -1, -1, 74, NULL, tm_drawbar, 0, 127, 0},  // Idx 111
  {"Drawbar 8H", -1, -1, 75, NULL, tm_drawbar, 0, 127, 0},  // Idx 112
  {"Release", -1, -1, 67, NULL, tm_drawbar, 0, 127, 0},  // Idx 113
  {"GM Prg 1", -1, -1, 240, NULL, tm_numeric, 0, 126, 0},  // Idx 114
  {"GM Level 1", -1, -1, 241, NULL, tm_pot, 0, 127, 0},  // Idx 115
  {"GM Harm1", -1, -1, 242, NULL, tm_numeric, 0, 5, 0},  // Idx 116
  {"GM Prg 2", -1, -1, 243, NULL, tm_numeric, 0, 126, 1},  // Idx 117
  {"GM Level 2", -1, -1, 244, NULL, tm_pot, 0, 127, 1},  // Idx 118
  {"GM Harm2", -1, -1, 245, NULL, tm_numeric, 0, 5, 1},  // Idx 119
  {"GM Detune 2", -1, -1, 246, NULL, tm_pot, 0, 15, 1},  // Idx 120
  {"Attack", -1, -1, 64, NULL, tm_pot, 0, 127, 1},  // Idx 121
  {"Decay", -1, -1, 65, NULL, tm_pot, 0, 127, 1},  // Idx 122
  {"Sustain", -1, -1, 66, NULL, tm_pot, 0, 127, 1},  // Idx 123
  {"Release", -1, -1, 67, NULL, tm_pot, 0, 127, 1},  // Idx 124
  {"ADSR Harmonic", -1, -1, 68, NULL, tm_pot, 0, 127, 1},  // Idx 125
  {"Percussion", -1, -1, 128, NULL, tm_tab, 0, 1, 0},  // Idx 126
  {"Perc Soft", -1, -1, 129, NULL, tm_tab, 0, 1, 0},  // Idx 127
  {"Perc Fast", -1, -1, 130, NULL, tm_tab, 0, 1, 0},  // Idx 128
  {"Perc Third", -1, -1, 131, NULL, tm_tab, 0, 1, 0},  // Idx 129
  {"Vib On Lower", -1, -1, 133, NULL, tm_tab, 0, 1, 0},  // Idx 130
  {"Vib On Upper", -1, -1, 134, NULL, tm_tab, 0, 1, 0},  // Idx 131
  {"Vib Knob", -1, -1, 264, NULL, tm_vibknob, 0, 5, 0},  // Idx 132
  {"H100 2ndVoice", -1, -1, 156, NULL, tm_tab, 0, 1, 0},  // Idx 133
  {"EG DB To Dry ", -1, -1, 158, NULL, tm_tab, 0, 1, 0},  // Idx 134
  {"PHR On Upper", -1, -1, 138, NULL, tm_tab, 0, 1, 1},  // Idx 135
  {"PHR On Lower", -1, -1, 139, NULL, tm_tab, 0, 1, 1},  // Idx 136
  {"PHR Knob", -1, -1, 257, NULL, tm_phrknob, 0, 7, 1},  // Idx 137
  {"Gating Mode", -1, -1, 261, NULL, tm_gating, 0, 4, 1},  // Idx 138
  {"TG WaveSet", -1, -1, 388, NULL, tm_waveset, 0, 7, 0},  // Idx 139
  {"TG Tapering", -1, -1, 392, NULL, tm_tapering, 0, 5, 0},  // Idx 140
  {"TG Flutter", -1, -1, 389, NULL, tm_pot, 0, 15, 0},  // Idx 141
  {"TG Leakage", -1, -1, 390, NULL, tm_pot, 0, 15, 0},  // Idx 142
  {"Cont SpringFlx", -1, -1, 360, NULL, tm_pot, 0, 15, 0},  // Idx 143
  {"Cont SpringDmp", -1, -1, 361, NULL, tm_pot, 0, 15, 0},  // Idx 144
  {"Lubed Contacts", -1, -1, 364, NULL, tm_tab, 0, 1, 0},  // Idx 145
  {"No DB1 @Perc", -1, -1, 357, NULL, tm_tab, 0, 1, 0},  // Idx 146
  {"Perc Norm Level", -1, -1, 480, NULL, tm_pot, 0, 127, 0},  // Idx 147
  {"Perc Soft Level", -1, -1, 481, NULL, tm_pot, 0, 127, 0},  // Idx 148
  {"Perc Long Time", -1, -1, 482, NULL, tm_pot, 0, 127, 0},  // Idx 149
  {"Perc Short Time", -1, -1, 483, NULL, tm_pot, 0, 127, 0},  // Idx 150
  {"Perc Muted Lvl", -1, -1, 484, NULL, tm_pot, 0, 127, 0},  // Idx 151
  {"AO28 Tone Pot", -1, -1, 87, NULL, tm_pot, 0, 127, 0},  // Idx 152
  {"AO28 Gain Cap", -1, -1, 88, NULL, tm_pot, 0, 127, 0},  // Idx 153
  {"AO28 MinSwell", -1, -1, 89, NULL, tm_pot, 0, 127, 0},  // Idx 154
  {"AO28 Tube Age", -1, -1, 90, NULL, tm_pot, 0, 127, 0},  // Idx 155
  {"Upper Level", -1, -1, 82, NULL, tm_pot, 0, 127, 0},  // Idx 156
  {"Lower Level", -1, -1, 83, NULL, tm_pot, 0, 127, 0},  // Idx 157
  {"Pedal Level", -1, -1, 84, NULL, tm_pot, 0, 127, 0},  // Idx 158
  {"Perc Level", -1, -1, 85, NULL, tm_pot, 0, 127, 0},  // Idx 159
  {"Reverb 1 Level ", -1, -1, 400, NULL, tm_pot, 0, 127, 0},  // Idx 160
  {"Reverb 2 Level ", -1, -1, 401, NULL, tm_pot, 0, 127, 0},  // Idx 161
  {"Reverb 3 Level ", -1, -1, 402, NULL, tm_pot, 0, 127, 0},  // Idx 162
  {"Bypass", -1, -1, 159, NULL, tm_tab, 0, 1, 0},  // Idx 163
  {"Bass Level", -1, -1, 112, NULL, tm_pot, 0, 127, 0},  // Idx 164
  {"Bass Equ Frq", -1, -1, 113, NULL, tm_pot, 0, 127, 0},  // Idx 165
  {"Bass Equ Q", -1, -1, 114, NULL, tm_pot, 0, 127, 0},  // Idx 166
  {"Mid Level", -1, -1, 115, NULL, tm_pot, 0, 127, 0},  // Idx 167
  {"Mid Equ Frq  ", -1, -1, 116, NULL, tm_pot, 0, 127, 0},  // Idx 168
  {"Mid Equ Q", -1, -1, 117, NULL, tm_pot, 0, 127, 0},  // Idx 169
  {"Treble Level", -1, -1, 118, NULL, tm_pot, 0, 127, 0},  // Idx 170
  {"Treb Equ Frq", -1, -1, 119, NULL, tm_pot, 0, 127, 0},  // Idx 171
  {"Treb Equ Q", -1, -1, 120, NULL, tm_pot, 0, 127, 0},  // Idx 172
  {"Parametr B/T", -1, -1, 121, NULL, tm_tab, 0, 1, 0},  // Idx 173
  {"Horn SlowSpeed", -1, -1, 448, NULL, tm_pot, 0, 63, 0},  // Idx 174
  {"Rotr SlowSpeed", -1, -1, 449, NULL, tm_pot, 0, 63, 0},  // Idx 175
  {"Horn FastSpeed", -1, -1, 450, NULL, tm_pot, 0, 127, 0},  // Idx 176
  {"Rotr FastSpeed", -1, -1, 451, NULL, tm_pot, 0, 127, 0},  // Idx 177
  {"Horn RampUp   ", -1, -1, 452, NULL, tm_pot, 0, 31, 0},  // Idx 178
  {"Rotor RampUp  ", -1, -1, 453, NULL, tm_pot, 0, 31, 0},  // Idx 179
  {"Horn RampDown ", -1, -1, 454, NULL, tm_pot, 0, 31, 0},  // Idx 180
  {"Rotor RampDown", -1, -1, 455, NULL, tm_pot, 0, 31, 0},  // Idx 181
  {"Rotary Throb ", -1, -1, 456, NULL, tm_pot, 0, 127, 0},  // Idx 182
  {"Rotary Spread", -1, -1, 457, NULL, tm_pot, 0, 127, 0},  // Idx 183
  {"Rotary Balance", -1, -1, 458, NULL, tm_pot, 0, 127, 0},  // Idx 184
  {"Tube Select A", -1, -1, 460, NULL, tm_numeric, 0, 7, 0},  // Idx 185
  {"Tube Select B", -1, -1, 461, NULL, tm_numeric, 0, 7, 0},  // Idx 186
  {"Transpose", -1, -1, 355, NULL, tm_numeric, 0, 24, 0},  // Idx 187
  {"Velocity Slope", -1, -1, 363, NULL, tm_pot, 1, 30, 0},  // Idx 188
  {"MIDI Channel", -1, -1, 368, NULL, tm_numeric, 1, 12, 0},  // Idx 189
  {"MIDI CC Set  ", -1, -1, 370, NULL, tm_midicc, 0, 10, 0},  // Idx 190
  {"MIDI Swell CC", -1, -1, 371, NULL, tm_numeric, 0, 127, 0},  // Idx 191
  {"MIDI VolumeCC", -1, -1, 372, NULL, tm_numeric, 0, 127, 0},  // Idx 192
  {"MIDI PresetCC", -1, -1, 374, NULL, tm_numeric, 0, 127, 0},  // Idx 193
  {"MIDI Send    ", -1, -1, 373, NULL, tm_numeric, 0, 127, 0},  // Idx 194
  {"Split Keyb   ", -1, -1, 143, NULL, tm_numeric, 0, 127, 0},  // Idx 195
  {"Split Point  ", -1, -1, 353, NULL, tm_numeric, 0, 127, 0},  // Idx 196
  {"Split Mode   ", -1, -1, 354, NULL, tm_numeric, 0, 5, 0},  // Idx 197
  {"No ProgChgRcv", -1, -1, 376, NULL, tm_numeric, 0, 127, 0},  // Idx 198
  {"Preset Name  ", -1, -1, -1, &enterPresetName, tm_button, 0, 1, 0},  // Idx 199
  {"Preset Init  ", -1, -1, -1, NULL, tm_button, 0, 1, 0},  // Idx 200
  {"LED Dimmer   ", -1, -1, 495, NULL, tm_pot, 0, 15, 0},  // Idx 201
  {"Bootld Update", -1, -1, -1, &enterBootloader, tm_button, 0, 1, 0},  // Idx 202
  {"WiFi Mode", -1, -1, 496, &saveWifi, tm_wifimode, 0, 2, 0},  // Idx 203
};


// ------------------------------------------------------------------------------

const String Msg[] = {"FCK TRMP", "FCK AFD"};

uint16_t manualSelects[MAIN_MENU_END + 1] = {0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 0, 0, 0};

// ------------------------------------------------------------------------------


void getMenuEntry(menuEntryType* entry, uint16_t index) {
  // einen Menüpunkt aus PROGMEM lesen und lokal in currentMenuEntry speichern, 
  // damit wir die Werte daraus verwenden können
  if (index >= MENU_ITEMCOUNT) return;
  memcpy_P(entry, &MenuItems[index], sizeof(menuEntryType));
}

void initMainMenuItems() {
  // Hauptmenu aus PROGMEM lesen und lokal in tempMenuArr speichern, 
  // damit wir die Werte daraus verwenden können
  menuEntryType tempMenuEntry;
  memset(subMenuProperties, 0, sizeof(subMenuProperties)); // Submenü-Positionen auf 0 setzen
  for (uint16_t idx = 0; idx <= MAIN_MENU_END; idx++) {
    getMenuEntry(&tempMenuEntry, idx);
    // copy complete temp entry to mainMenuItems array for display and editing
    memcpy(&mainMenuItems[idx], &tempMenuEntry, sizeof(menuEntryType));
    if (tempMenuEntry.submenuStart >= 0 && tempMenuEntry.submenuEnd >= 0) {
      subMenuProperties[idx].startIndex = tempMenuEntry.submenuStart; // Startindex des Submenüs speichern
      subMenuProperties[idx].itemCount =  (tempMenuEntry.submenuEnd - tempMenuEntry.submenuStart + 1); // Anzahl der Submenüeinträge für dieses Hauptmenü berechnen und speichern
    }
  }
}

void getSubMenuItems(uint16_t main_idx) {
  // Untermenü zu Hauptmenu-Eintrag main_idx aus PROGMEM lesen und lokal in subMenuArr speichern, 
  // damit wir die Werte daraus verwenden können
  menuEntryType tempMenuEntry;
  getMenuEntry(&tempMenuEntry, main_idx);
  memset(subMenuItems, 0, sizeof(subMenuItems)); // Clear the subMenuItems array
  int16_t item_count = subMenuProperties[main_idx].itemCount; // Anzahl der Submenüeinträge für dieses Hauptmenü
  if (item_count > 0) {
    DPRINTF("Get sub items for main_idx: ");
    DPRINTLN(main_idx);
    for (uint16_t idx = 0; idx < item_count; idx++) {
      getMenuEntry(&tempMenuEntry, subMenuProperties[main_idx].startIndex + idx);
      memcpy(&subMenuItems[idx], &tempMenuEntry, sizeof(menuEntryType)); // copy menu header to subMenuItems array for display
    }
    // strcpy(subMenuItems[item_count].menuHeader , "CANCEL"); // immer letzter Eintrag
    // subMenuItems[item_count].displayType = tm_cancel; // CANCEL-Action
    // subMenuItems[item_count].editArrayIdx = -1; // kein Wert zu editieren
    // subMenuProperties[main_idx].itemCount = item_count + 1; // Anzahl der Submenüeinträge inklusive CANCEL aktualisieren
  }
}


// #############################################################################
//
//     #     # ####### #     # #     #    #     #    #    #        #####  
//     ##   ## #       ##    # #     #    #     #   # #   #       #     # 
//     # # # # #       # #   # #     #    #     #  #   #  #       #       
//     #  #  # #####   #  #  # #     #    #     # #     # #        #####  
//     #     # #       #   # # #     #     #   #  ####### #             # 
//     #     # #       #    ## #     #      # #   #     # #       #     # 
//     #     # ####### #     #  #####        #    #     # #######  #####  
//                                                                
// #############################################################################

// -----------------------------------------------------------------------------

void selectSubMenu(int16_t delta) {
  // Hilfsfunktion, um einen Untermenüpunkt auszuwählen, z.B. nach einem UP/DOWN-Click
  drawSubmenuSelect(mainMenuItem, delta); // Submenü-Auswahl aktualisieren
  activeMenuItem = mainMenuItems[mainMenuItem].submenuStart + subMenuProperties[mainMenuItem].itemIndex;
  getMenuEntry(&currentMenuEntry, activeMenuItem); // Aktuelle Menü-Entry-Daten in globalen Variablen aktualisieren
}


// Change the value of a menu entry based on the delta from the rotary encoder and call the editAction if defined
void changeValue(menuEntryType *entry, int16_t delta) {
  if (entry->editArrayIdx >= 0) {
    int16_t newValue = delta + hx3EditArray[entry->editArrayIdx];
    if (newValue < entry->menuValueMin) newValue = entry->menuValueMin;
    if (newValue > entry->menuValueMax) newValue = entry->menuValueMax;
    hx3EditArray[entry->editArrayIdx] = newValue; // Beispiel: Rückgabe eines Werts aus einem Array
    // Bei Bedarf neue Werte vom FPGA anfordern oder senden
    // Standard-Aktion: Wert an HX3 senden
    spi_xc_binarycmd(entry->editArrayIdx + 1000, newValue);
    // Bei Presets, Voices, Orgel- und Speaker-Modellen zusätzlich Array neu anfordern,
    // damit die Anzeige aktualisiert wird
    switch (entry->editArrayIdx) {
      case 0:  // Preset-Nummer
      case 1:  // Voice-Nummer Upper
      case 4:  // Voice-Nummer Lower
      case 5:  // Voice-Nummer Lower
      case 16: // Orgelmodell
      case 17: // Speakermodell
        spi_xc_request_editArray(); // Aktualisiert den Wert im Edit-Array, damit die Anzeige den neuen Wert zeigt
        break;
    }
  }
}

// -----------------------------------------------------------------------------

void refreshMainPage(bool dimmed = false) {
  uint16_t text_color_main = TFT_BLACK;
  uint16_t text_color_sub = TFT_BLACK;
  uint16_t line_color = TFT_WHITE;
  setMainDimmedState(dimmed); // Hauptfenster wieder auf normale Helligkeit setzen
  static bool last_dimmed_state = !dimmedMainWindow;

  getMenuEntry(&currentMenuEntry, activeMenuItem); // Aktuelle Menü-Entry-Daten in globalen Variablen aktualisieren
  if (last_dimmed_state != dimmed) {
    drawSubMenuBox();
    last_dimmed_state = dimmed;
  }
  drawMainMenu(mainMenuItem);
  drawSubmenuSelect(mainMenuItem, 0);
  drawValue(&currentMenuEntry);
  drawOrgan(manualSelects[mainMenuItem]); // Zeichnet die Orgelgrafik, z.B. für die Anzeige der Registerbelegung oder ähnliches
}

void displayMainPage() {
  // nach Message oder beim Start aufzurufen
  setMainDimmedState(false);
  currentMenuState = s_inmainmenu; // Zurück zum Hauptmenü, um die Anzeige zu aktualisieren
  drawBMP("/background.bmp", 0, 0);
  getSubMenuItems(mainMenuItem); // Submenü-Strings basierend auf der Auswahl im Hauptmenü aktualisieren
  refreshMainPage(false);
}


// #############################################################################
//
//        #     #####  ####### ### ####### #     # 
//       # #   #     #    #     #  #     # ##    # 
//      #   #  #          #     #  #     # # #   # 
//     #     # #          #     #  #     # #  #  # 
//     ####### #          #     #  #     # #   # # 
//     #     # #     #    #     #  #     # #    ## 
//     #     #  #####     #    ### ####### #     # 
//                                                 
// #############################################################################


// Action for special menu items that require custom handling, 
// e.g. to display the value in a specific way or to perform additional operations  when the value changes. 
// The action function takes two parameters: 
// line_color can be used to set the color of the text when displaying the value
// delta represents the change from the rotary encoder (e.g. +1 or -1) that can be used to calculate the new value.

void savePreset() {
  int16_t presetNumber = hx3EditArray[currentMenuEntry.editArrayIdx]; // Aktuelle Preset-Nummer aus dem Edit-Array lesen
  drawEnterNumber("Save Preset to", &presetNumber, 0, 99);
  hx3EditArray[currentMenuEntry.editArrayIdx] = presetNumber; // Aktualisierte Preset-Nummer zurück ins Edit-Array schreiben
  DPRINTLNF("Save Preset action");
  displayMainPage();
}

void enterPresetName(){ 
  char presetName[14]; // Buffer für den Preset-Namen (16 Zeichen + Nullterminator)
  memset(presetName, 32, sizeof(presetName)); // Buffer initialisieren
  presetName[13] = '\0'; // Sicherstellen, dass der String nullterminiert ist
  strcpy(presetName, (char*)hx3EditArray + 192); // Aktuellen Preset-Namen aus dem Edit-Array lesen
  drawEnterText("Enter Preset Name", presetName); // Funktion zum Eingeben von Text aufrufen
  strcpy((char*)hx3EditArray + 192, presetName); // Aktualisierten Preset-Namen zurück ins Edit-Array schreiben
  DPRINTLNF("Enter Preset Name action");
  displayMainPage();
}

void saveWifi() {
  settings.wifiMode = hx3EditArray[currentMenuEntry.editArrayIdx]; // Aktuellen WiFi-Modus aus dem Edit-Array lesen
  DPRINTLNF("Save WiFi Mode");
  drawMsgTimeout("WIFI mode saved", "Restart to apply", MSG_DISPLAY_TIME, DB_INFO_OK);
  saveCredentials(); // WiFi-Einstellungen speichern, damit sie nach einem Neustart erhalten bleiben
  displayMainPage();
}

void saveUpper() {
  int16_t voiceNumber = hx3EditArray[currentMenuEntry.editArrayIdx]; // Aktuelle Voice-Nummer aus dem Edit-Array lesen
  drawEnterNumber("Save Upper to", &voiceNumber, 0, 15);
  hx3EditArray[currentMenuEntry.editArrayIdx] = voiceNumber; // Aktualisierte Voice-Nummer zurück ins Edit-Array schreiben
  DPRINTLNF("Save Upper Voice action");
  displayMainPage();
}

void saveLower(){ 
  int16_t voiceNumber = hx3EditArray[currentMenuEntry.editArrayIdx]; // Aktuelle Voice-Nummer aus dem Edit-Array lesen
  drawEnterNumber("Save Lower to", &voiceNumber, 0, 15);
  hx3EditArray[currentMenuEntry.editArrayIdx] = voiceNumber; // Aktualisierte Voice-Nummer zurück ins Edit-Array schreiben
  DPRINTLNF("Save Lower Voice action");
  displayMainPage();
}

void savePedal(){ 
  int16_t voiceNumber = hx3EditArray[currentMenuEntry.editArrayIdx]; // Aktuelle Voice-Nummer aus dem Edit-Array lesen
  drawEnterNumber("Save Pedal to", &voiceNumber, 0, 15);
  hx3EditArray[currentMenuEntry.editArrayIdx] = voiceNumber; // Aktualisierte Voice-Nummer zurück ins Edit-Array schreiben
  DPRINTLNF("Save Pedal Voice action");
  displayMainPage();
}

void saveOrgan(){ 
  int16_t voiceNumber = hx3EditArray[currentMenuEntry.editArrayIdx]; // Aktuelle Voice-Nummer aus dem Edit-Array lesen
  drawEnterNumber("Save Organ to", &voiceNumber, 0, 15);
  hx3EditArray[currentMenuEntry.editArrayIdx] = voiceNumber; // Aktualisierte Voice-Nummer zurück ins Edit-Array schreiben
  DPRINTLNF("Save Organ action");
  displayMainPage();
}

void saveSpeaker(){ 
  int16_t voiceNumber = hx3EditArray[currentMenuEntry.editArrayIdx]; // Aktuelle Voice-Nummer aus dem Edit-Array lesen
  drawEnterNumber("Save Speaker to", &voiceNumber, 0, 15);
  hx3EditArray[currentMenuEntry.editArrayIdx] = voiceNumber; // Aktualisierte Voice-Nummer zurück ins Edit-Array schreiben
  DPRINTLNF("Save Speaker action");
  displayMainPage();
}

void saveDefaults(){ 
  int16_t voiceNumber = hx3EditArray[currentMenuEntry.editArrayIdx]; // Aktuelle Voice-Nummer aus dem Edit-Array lesen
  drawMsgTimeout("Defaults saved", "", MSG_DISPLAY_TIME, DB_INFO_OK);
  DPRINTLNF("Save Defaults action");
  displayMainPage();
}

void enterBootloader(){ 
  int16_t voiceNumber = hx3EditArray[currentMenuEntry.editArrayIdx]; // Aktuelle Voice-Nummer aus dem Edit-Array lesen
  drawMsgTimeout("Entered Bootloader", "Restart to exit", MSG_DISPLAY_TIME, DB_INFO_OK);
  DPRINTLNF("Enter Bootloader action");
  displayMainPage();
}

#endif