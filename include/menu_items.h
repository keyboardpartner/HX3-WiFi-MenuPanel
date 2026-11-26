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
#include "server.h"
#ifdef ENABLE_SCREENSAVER
#include "screensaver.h"
#endif

// Forward declaration of menu action routines
void savePreset();
void saveWifi();
void saveUpper();
void saveLower();
void savePedal();
void saveOrgan();
void saveSpeaker();
void saveDefaults();
void enterPresetName();
void enterScreenSaver();
void bootloaderCmd();
void requestBoardInfo();
void enterBootloader();
void exitBootloader();

void resetAll(){
  requestSema.reset = true; // Flag setzen, um einen Reset anzufordern
}

// ------------------------------------------------------------------------------
// Hier Daten aus Excel-Tabelle "MIDI_menuItems.xlsx" einfügen
// ------------------------------------------------------------------------------


// Diese Tabelle enthält die Menüstruktur, die in der Excel-Tabelle MIDI_menuItems.xlsx definiert ist
// Menü-Text, Link zu Untermenüs, Zeiger auf Werte, die bei Änderung geändert werden sollen, 
// Action-Routine bei Änderung, Min- und Maximalwerte für die Editierung
const menuEntryType MenuItems[214] PROGMEM = { 
  {"Preset", 201, 202, 268, NULL, &savePreset, tm_preset, 0, 99, 0},  // Idx 0
  {"Upper Voice", 27, 38, 269, NULL, &saveUpper, tm_numeric, 0, 15, 0},  // Idx 1
  {"Lower Voice", 75, 86, 270, NULL, &saveLower, tm_numeric, 0, 15, 0},  // Idx 2
  {"Pedal Voice", 111, 115, 271, NULL, &savePedal, tm_numeric, 0, 15, 0},  // Idx 3
  {"Tabs", 128, 139, -1, NULL, NULL, tm_none, -1, -1, 0},  // Idx 4
  {"Rotary Speed", 23, 25, 256, NULL, NULL, tm_halfmoon, 0, 2, 0},  // Idx 5
  {"Master Volume", 21, 22, 80, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 6
  {"Amp Gain", 26, 26, 81, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 7
  {"Reverb Prgm", 162, 164, 263, NULL, NULL, tm_numeric, 0, 3, 0},  // Idx 8
  {"Equalizer", 165, 175, -1, NULL, NULL, tm_none, -1, -1, 0},  // Idx 9
  {"Upper ADSR", 46, 74, -1, NULL, NULL, tm_none, -1, -1, 1},  // Idx 10
  {"Upper GM", 39, 45, -1, NULL, NULL, tm_none, -1, -1, 0},  // Idx 11
  {"Lower ADSR", 94, 110, -1, NULL, NULL, tm_none, -1, -1, 1},  // Idx 12
  {"Lower GM", 87, 93, -1, NULL, NULL, tm_none, -1, -1, 0},  // Idx 13
  {"Pedal ADSR", 123, 127, -1, NULL, NULL, tm_none, -1, -1, 0},  // Idx 14
  {"Pedal GM", 116, 122, -1, NULL, NULL, tm_none, -1, -1, 0},  // Idx 15
  {"Organ", 140, 161, 265, NULL, &saveOrgan, tm_organ, 0, 15, 0},  // Idx 16
  {"Speaker", 176, 188, 266, NULL, &saveSpeaker, tm_speaker, 0, 15, 0},  // Idx 17
  {"Keybd Setup", 189, 200, -1, NULL, &saveDefaults, tm_none, -1, -1, 0},  // Idx 18
  {"Settings", 203, 208, -1, NULL, NULL, tm_none, -1, -1, 0},  // Idx 19
  {"Bootloader", 209, 213, -1, NULL, NULL, tm_none, -1, -1, 0},  // Idx 20
  {"AO28 Tone Pot", -1, -1, 87, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 21
  {"AO28 Gain Cap", -1, -1, 88, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 22
  {"Ext FX Insert", -1, -1, 172, NULL, NULL, tm_tab, 0, 1, 0},  // Idx 23
  {"Rotary Bypass", -1, -1, 137, NULL, NULL, tm_tab, 0, 1, 0},  // Idx 24
  {"Separate Pedal", -1, -1, 142, NULL, NULL, tm_tab, 0, 1, 0},  // Idx 25
  {"Amp Bypass", -1, -1, 136, NULL, NULL, tm_tab, 0, 1, 0},  // Idx 26
  {"Drawbar 16", -1, -1, 0, NULL, &saveUpper, tm_drawbar, 0, 127, 0},  // Idx 27
  {"Drawbar 5 1/3", -1, -1, 1, NULL, &saveUpper, tm_drawbar, 0, 127, 0},  // Idx 28
  {"Drawbar 8", -1, -1, 2, NULL, &saveUpper, tm_drawbar, 0, 127, 0},  // Idx 29
  {"Drawbar 4", -1, -1, 3, NULL, &saveUpper, tm_drawbar, 0, 127, 0},  // Idx 30
  {"Drawbar 2 2/3", -1, -1, 4, NULL, &saveUpper, tm_drawbar, 0, 127, 0},  // Idx 31
  {"Drawbar 2", -1, -1, 5, NULL, &saveUpper, tm_drawbar, 0, 127, 0},  // Idx 32
  {"Drawbar 1 3/5", -1, -1, 6, NULL, &saveUpper, tm_drawbar, 0, 127, 0},  // Idx 33
  {"Drawbar 1 1/3", -1, -1, 7, NULL, &saveUpper, tm_drawbar, 0, 127, 0},  // Idx 34
  {"Drawbar 1", -1, -1, 8, NULL, &saveUpper, tm_drawbar, 0, 127, 0},  // Idx 35
  {"Drawbar Mix 1", -1, -1, 9, NULL, &saveUpper, tm_drawbar, 0, 127, 0},  // Idx 36
  {"Drawbar Mix 2", -1, -1, 10, NULL, &saveUpper, tm_drawbar, 0, 127, 0},  // Idx 37
  {"Drawbar Mix 3", -1, -1, 11, NULL, &saveUpper, tm_drawbar, 0, 127, 0},  // Idx 38
  {"GM Prg 1", -1, -1, 224, NULL, NULL, tm_numeric, 0, 126, 0},  // Idx 39
  {"GM Level 1", -1, -1, 225, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 40
  {"GM Hrm 1", -1, -1, 226, NULL, NULL, tm_numeric, 0, 5, 0},  // Idx 41
  {"GM Prg 2", -1, -1, 227, NULL, NULL, tm_numeric, 0, 126, 1},  // Idx 42
  {"GM Level 2", -1, -1, 228, NULL, NULL, tm_pot, 0, 127, 1},  // Idx 43
  {"GM Harm2", -1, -1, 229, NULL, NULL, tm_numeric, 0, 5, 1},  // Idx 44
  {"GM Detune 2", -1, -1, 230, NULL, NULL, tm_pot, 0, 15, 1},  // Idx 45
  {"Env/Perc 16", -1, -1, 96, NULL, NULL, tm_drawbar, 0, 127, 1},  // Idx 46
  {"Env/Perc 5 1/3", -1, -1, 97, NULL, NULL, tm_drawbar, 0, 127, 1},  // Idx 47
  {"Env/Perc 8", -1, -1, 98, NULL, NULL, tm_drawbar, 0, 127, 1},  // Idx 48
  {"Env/Perc 4", -1, -1, 99, NULL, NULL, tm_drawbar, 0, 127, 1},  // Idx 49
  {"Env/Perc 2 2/3", -1, -1, 100, NULL, NULL, tm_drawbar, 0, 127, 1},  // Idx 50
  {"Env/Perc 2", -1, -1, 101, NULL, NULL, tm_drawbar, 0, 127, 1},  // Idx 51
  {"Env/Perc 1 3/5", -1, -1, 102, NULL, NULL, tm_drawbar, 0, 127, 1},  // Idx 52
  {"Env/Perc 1 1/3", -1, -1, 103, NULL, NULL, tm_drawbar, 0, 127, 1},  // Idx 53
  {"Env/Perc 1", -1, -1, 104, NULL, NULL, tm_drawbar, 0, 127, 1},  // Idx 54
  {"Env/Perc Mix 1", -1, -1, 105, NULL, NULL, tm_drawbar, 0, 127, 1},  // Idx 55
  {"Env/Perc Mix 2", -1, -1, 106, NULL, NULL, tm_drawbar, 0, 127, 1},  // Idx 56
  {"Env/Perc Mix 3", -1, -1, 107, NULL, NULL, tm_drawbar, 0, 127, 1},  // Idx 57
  {"Attack", -1, -1, 48, NULL, NULL, tm_pot, 0, 127, 1},  // Idx 58
  {"Decay", -1, -1, 49, NULL, NULL, tm_pot, 0, 127, 1},  // Idx 59
  {"Sustain", -1, -1, 50, NULL, NULL, tm_pot, 0, 127, 1},  // Idx 60
  {"Release", -1, -1, 51, NULL, NULL, tm_pot, 0, 127, 1},  // Idx 61
  {"ADSR Harmonic", -1, -1, 52, NULL, NULL, tm_pot, 0, 127, 1},  // Idx 62
  {"ADSR on 16", -1, -1, 160, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 63
  {"ADSR on 5 1/3", -1, -1, 161, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 64
  {"ADSR on 8", -1, -1, 162, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 65
  {"ADSR on 4", -1, -1, 163, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 66
  {"ADSR on 2 2/3", -1, -1, 164, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 67
  {"ADSR on 2", -1, -1, 165, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 68
  {"ADSR on 1 3/5", -1, -1, 166, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 69
  {"ADSR on 1 1/3", -1, -1, 167, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 70
  {"ADSR on 1", -1, -1, 168, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 71
  {"ADSR on Mix 1", -1, -1, 169, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 72
  {"ADSR on Mix 2", -1, -1, 170, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 73
  {"ADSR on Mix 3", -1, -1, 171, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 74
  {"Drawbar 16", -1, -1, 16, NULL, &saveLower, tm_drawbar, 0, 127, 0},  // Idx 75
  {"Drawbar 5 1/3", -1, -1, 17, NULL, &saveLower, tm_drawbar, 0, 127, 0},  // Idx 76
  {"Drawbar 8", -1, -1, 18, NULL, &saveLower, tm_drawbar, 0, 127, 0},  // Idx 77
  {"Drawbar 4", -1, -1, 19, NULL, &saveLower, tm_drawbar, 0, 127, 0},  // Idx 78
  {"Drawbar 2 2/3", -1, -1, 20, NULL, &saveLower, tm_drawbar, 0, 127, 0},  // Idx 79
  {"Drawbar 2", -1, -1, 21, NULL, &saveLower, tm_drawbar, 0, 127, 0},  // Idx 80
  {"Drawbar 1 3/5", -1, -1, 22, NULL, &saveLower, tm_drawbar, 0, 127, 0},  // Idx 81
  {"Drawbar 1 1/3", -1, -1, 23, NULL, &saveLower, tm_drawbar, 0, 127, 0},  // Idx 82
  {"Drawbar 1", -1, -1, 24, NULL, &saveLower, tm_drawbar, 0, 127, 0},  // Idx 83
  {"Drawbar Mix 1", -1, -1, 25, NULL, &saveLower, tm_drawbar, 0, 127, 1},  // Idx 84
  {"Drawbar Mix 2", -1, -1, 26, NULL, &saveLower, tm_drawbar, 0, 127, 1},  // Idx 85
  {"Drawbar Mix 3", -1, -1, 27, NULL, &saveLower, tm_drawbar, 0, 127, 1},  // Idx 86
  {"GM Prg 1", -1, -1, 232, NULL, NULL, tm_numeric, 0, 126, 0},  // Idx 87
  {"GM Level 1", -1, -1, 233, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 88
  {"GM Harm1", -1, -1, 234, NULL, NULL, tm_numeric, 0, 5, 0},  // Idx 89
  {"GM Prg 2", -1, -1, 235, NULL, NULL, tm_numeric, 0, 126, 1},  // Idx 90
  {"GM Level 2", -1, -1, 236, NULL, NULL, tm_pot, 0, 127, 1},  // Idx 91
  {"GM Harm2", -1, -1, 237, NULL, NULL, tm_numeric, 0, 5, 1},  // Idx 92
  {"GM Detune 2", -1, -1, 238, NULL, NULL, tm_pot, 0, 15, 1},  // Idx 93
  {"Attack", -1, -1, 56, NULL, NULL, tm_pot, 0, 127, 1},  // Idx 94
  {"Decay", -1, -1, 57, NULL, NULL, tm_pot, 0, 127, 1},  // Idx 95
  {"Sustain", -1, -1, 58, NULL, NULL, tm_pot, 0, 127, 1},  // Idx 96
  {"Release", -1, -1, 59, NULL, NULL, tm_pot, 0, 127, 1},  // Idx 97
  {"ADSR Harmonic", -1, -1, 60, NULL, NULL, tm_pot, 0, 127, 1},  // Idx 98
  {"ADSR on 16", -1, -1, 176, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 99
  {"ADSR on 5 1/3", -1, -1, 177, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 100
  {"ADSR on 8", -1, -1, 178, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 101
  {"ADSR on 4", -1, -1, 179, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 102
  {"ADSR on 2 2/3", -1, -1, 180, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 103
  {"ADSR on 2", -1, -1, 181, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 104
  {"ADSR on 1 3/5", -1, -1, 182, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 105
  {"ADSR on 1 1/3", -1, -1, 183, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 106
  {"ADSR on 1", -1, -1, 184, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 107
  {"ADSR on Mix 1", -1, -1, 185, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 108
  {"ADSR on Mix 2", -1, -1, 186, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 109
  {"ADSR on Mix 3", -1, -1, 187, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 110
  {"Drawbar 16", -1, -1, 72, NULL, &savePedal, tm_drawbar, 0, 127, 0},  // Idx 111
  {"Drawbar 16H", -1, -1, 73, NULL, &savePedal, tm_drawbar, 0, 127, 0},  // Idx 112
  {"Drawbar 8", -1, -1, 74, NULL, &savePedal, tm_drawbar, 0, 127, 0},  // Idx 113
  {"Drawbar 8H", -1, -1, 75, NULL, &savePedal, tm_drawbar, 0, 127, 0},  // Idx 114
  {"Release", -1, -1, 67, NULL, &savePedal, tm_drawbar, 0, 127, 0},  // Idx 115
  {"GM Prg 1", -1, -1, 240, NULL, NULL, tm_numeric, 0, 126, 0},  // Idx 116
  {"GM Level 1", -1, -1, 241, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 117
  {"GM Harm1", -1, -1, 242, NULL, NULL, tm_numeric, 0, 5, 0},  // Idx 118
  {"GM Prg 2", -1, -1, 243, NULL, NULL, tm_numeric, 0, 126, 1},  // Idx 119
  {"GM Level 2", -1, -1, 244, NULL, NULL, tm_pot, 0, 127, 1},  // Idx 120
  {"GM Harm2", -1, -1, 245, NULL, NULL, tm_numeric, 0, 5, 1},  // Idx 121
  {"GM Detune 2", -1, -1, 246, NULL, NULL, tm_pot, 0, 15, 1},  // Idx 122
  {"Attack", -1, -1, 64, NULL, NULL, tm_pot, 0, 127, 1},  // Idx 123
  {"Decay", -1, -1, 65, NULL, NULL, tm_pot, 0, 127, 1},  // Idx 124
  {"Sustain", -1, -1, 66, NULL, NULL, tm_pot, 0, 127, 1},  // Idx 125
  {"Release", -1, -1, 67, NULL, NULL, tm_pot, 0, 127, 1},  // Idx 126
  {"ADSR Harmonic", -1, -1, 68, NULL, NULL, tm_pot, 0, 127, 1},  // Idx 127
  {"Percussion", -1, -1, 128, NULL, NULL, tm_tab, 0, 1, 0},  // Idx 128
  {"Perc Soft", -1, -1, 129, NULL, NULL, tm_tab, 0, 1, 0},  // Idx 129
  {"Perc Fast", -1, -1, 130, NULL, NULL, tm_tab, 0, 1, 0},  // Idx 130
  {"Perc Third", -1, -1, 131, NULL, NULL, tm_tab, 0, 1, 0},  // Idx 131
  {"Vib On Upper", -1, -1, 132, NULL, NULL, tm_tab, 0, 1, 0},  // Idx 132
  {"Vib On Lower", -1, -1, 133, NULL, NULL, tm_tab, 0, 1, 0},  // Idx 133
  {"Vib Knob", -1, -1, 264, NULL, NULL, tm_vibknob, 0, 5, 0},  // Idx 134
  {"H100 2ndVoice", -1, -1, 156, NULL, NULL, tm_tab, 0, 1, 0},  // Idx 135
  {"EG DB To Dry ", -1, -1, 158, NULL, NULL, tm_tab, 0, 1, 0},  // Idx 136
  {"PHR On Upper", -1, -1, 138, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 137
  {"PHR On Lower", -1, -1, 139, NULL, NULL, tm_tab, 0, 1, 1},  // Idx 138
  {"PHR Knob", -1, -1, 256, NULL, NULL, tm_phrknob, 0, 7, 1},  // Idx 139
  {"Gating Mode", -1, -1, 260, NULL, NULL, tm_gating, 0, 4, 1},  // Idx 140
  {"TG WaveSet", -1, -1, 388, NULL, NULL, tm_waveset, 0, 7, 0},  // Idx 141
  {"TG Tapering", -1, -1, 392, NULL, NULL, tm_tapering, 0, 5, 0},  // Idx 142
  {"TG Flutter", -1, -1, 389, NULL, NULL, tm_pot, 0, 15, 0},  // Idx 143
  {"TG Leakage", -1, -1, 390, NULL, NULL, tm_pot, 0, 15, 0},  // Idx 144
  {"Cont SpringFlx", -1, -1, 360, NULL, NULL, tm_pot, 0, 15, 0},  // Idx 145
  {"Cont SpringDmp", -1, -1, 361, NULL, NULL, tm_pot, 0, 15, 0},  // Idx 146
  {"Lubed Contacts", -1, -1, 364, NULL, NULL, tm_tab, 0, 1, 0},  // Idx 147
  {"No DB1 @Perc", -1, -1, 357, NULL, NULL, tm_tab, 0, 1, 0},  // Idx 148
  {"Perc Norm Level", -1, -1, 480, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 149
  {"Perc Soft Level", -1, -1, 481, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 150
  {"Perc Long Time", -1, -1, 482, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 151
  {"Perc Short Time", -1, -1, 483, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 152
  {"Perc Muted Lvl", -1, -1, 484, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 153
  {"AO28 Tone Pot", -1, -1, 87, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 154
  {"AO28 Gain Cap", -1, -1, 88, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 155
  {"AO28 MinSwell", -1, -1, 89, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 156
  {"AO28 Tube Age", -1, -1, 90, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 157
  {"Upper Level", -1, -1, 82, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 158
  {"Lower Level", -1, -1, 83, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 159
  {"Pedal Level", -1, -1, 84, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 160
  {"Perc Level", -1, -1, 85, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 161
  {"Reverb 1 Level ", -1, -1, 400, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 162
  {"Reverb 2 Level ", -1, -1, 401, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 163
  {"Reverb 3 Level ", -1, -1, 402, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 164
  {"Bypass", -1, -1, 159, NULL, NULL, tm_tab, 0, 1, 0},  // Idx 165
  {"Bass Level", -1, -1, 112, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 166
  {"Bass Equ Frq", -1, -1, 113, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 167
  {"Bass Equ Q", -1, -1, 114, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 168
  {"Mid Level", -1, -1, 115, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 169
  {"Mid Equ Frq  ", -1, -1, 116, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 170
  {"Mid Equ Q", -1, -1, 117, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 171
  {"Treble Level", -1, -1, 118, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 172
  {"Treb Equ Frq", -1, -1, 119, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 173
  {"Treb Equ Q", -1, -1, 120, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 174
  {"Parametr B/T", -1, -1, 121, NULL, NULL, tm_tab, 0, 1, 0},  // Idx 175
  {"Horn SlowSpeed", -1, -1, 448, NULL, NULL, tm_pot, 0, 63, 0},  // Idx 176
  {"Rotr SlowSpeed", -1, -1, 449, NULL, NULL, tm_pot, 0, 63, 0},  // Idx 177
  {"Horn FastSpeed", -1, -1, 450, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 178
  {"Rotr FastSpeed", -1, -1, 451, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 179
  {"Horn RampUp   ", -1, -1, 452, NULL, NULL, tm_pot, 0, 31, 0},  // Idx 180
  {"Rotor RampUp  ", -1, -1, 453, NULL, NULL, tm_pot, 0, 31, 0},  // Idx 181
  {"Horn RampDown ", -1, -1, 454, NULL, NULL, tm_pot, 0, 31, 0},  // Idx 182
  {"Rotor RampDown", -1, -1, 455, NULL, NULL, tm_pot, 0, 31, 0},  // Idx 183
  {"Rotary Throb ", -1, -1, 456, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 184
  {"Rotary Spread", -1, -1, 457, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 185
  {"Rotary Balance", -1, -1, 458, NULL, NULL, tm_pot, 0, 127, 0},  // Idx 186
  {"Tube Select A", -1, -1, 460, NULL, NULL, tm_numeric, 0, 7, 0},  // Idx 187
  {"Tube Select B", -1, -1, 461, NULL, NULL, tm_numeric, 0, 7, 0},  // Idx 188
  {"Transpose", -1, -1, 355, NULL, NULL, tm_numeric, 0, 24, 0},  // Idx 189
  {"Velocity Slope", -1, -1, 363, NULL, NULL, tm_pot, 1, 30, 0},  // Idx 190
  {"MIDI Channel", -1, -1, 368, NULL, NULL, tm_midich, 0, 11, 0},  // Idx 191
  {"MIDI CC Set  ", -1, -1, 370, NULL, NULL, tm_midicc, 0, 10, 0},  // Idx 192
  {"MIDI Swell CC", -1, -1, 371, NULL, NULL, tm_numeric, 0, 127, 0},  // Idx 193
  {"MIDI VolumeCC", -1, -1, 372, NULL, NULL, tm_numeric, 0, 127, 0},  // Idx 194
  {"MIDI PresetCC", -1, -1, 374, NULL, NULL, tm_numeric, 0, 127, 0},  // Idx 195
  {"MIDI Send    ", -1, -1, 373, NULL, NULL, tm_numeric, 0, 127, 0},  // Idx 196
  {"Split Keyb   ", -1, -1, 143, NULL, NULL, tm_numeric, 0, 127, 0},  // Idx 197
  {"Split Point  ", -1, -1, 353, NULL, NULL, tm_numeric, 0, 127, 0},  // Idx 198
  {"Split Mode   ", -1, -1, 354, NULL, NULL, tm_numeric, 0, 5, 0},  // Idx 199
  {"No ProgChgRcv", -1, -1, 376, NULL, NULL, tm_numeric, 0, 127, 0},  // Idx 200
  {"Preset Name", -1, -1, -1, &enterPresetName, NULL, tm_button, 0, 1, 0},  // Idx 201
  {"Preset Init", -1, -1, -1, NULL, NULL, tm_button, 0, 1, 0},  // Idx 202
  {"LED Dimmer", -1, -1, 495, NULL, NULL, tm_pot, 0, 15, 0},  // Idx 203
  {"Screen Saver", -1, -1, EDIT_IDX_SCREENSAVER, NULL, &enterScreenSaver, tm_numeric, 0, 4, 0},  // Idx 204
  {"WiFi Mode", -1, -1, EDIT_IDX_WIFIMODE, NULL, &saveWifi, tm_wifimode, 0, 2, 0},  // Idx 205
  {"Board Info", -1, -1, -1, &requestBoardInfo, NULL, tm_button, -1, -1, 0},  // Idx 206
  {"Reset/Reboot", -1, -1, -1, NULL, &resetAll, tm_button, -1, -1, 0},  // Idx 207
  {"Enter Bootld", -1, -1, -1, NULL, &enterBootloader, tm_button, -1, -1, 0},  // Idx 208
  {"Start USB DFU", -1, -1, -1, &bootloaderCmd, NULL, tm_button, -1, -1, 0},  // Idx 209
  {"Failsafe FW", -1, -1, -1, &bootloaderCmd, NULL, tm_button, -1, -1, 0},  // Idx 210
  {"Failsafe FPGA", -1, -1, -1, &bootloaderCmd, NULL, tm_button, -1, -1, 0},  // Idx 211
  {"SD Card Upd", -1, -1, -1, &bootloaderCmd, NULL, tm_button, -1, -1, 0},  // Idx 212
  {"Exit Bootload", -1, -1, -1, &exitBootloader, NULL, tm_button, -1, -1, 0},  // Idx 213
};

#define MAIN_MENU_END 20
#define BOOTLOAD_MENU 20
#define BOOTLOAD_MENU_START 209
#define BOOTLOAD_EXIT 213
#define MENU_ITEMCOUNT 214

// ------------------------------------------------------------------------------

uint16_t manualSelects[MAIN_MENU_END + 1] = {0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 0, 0, 0, 0};
const String Msg[] = {"FCK TRMP", "FCK AFD"};

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
    for (uint16_t idx = 0; idx < item_count; idx++) {
      getMenuEntry(&tempMenuEntry, subMenuProperties[main_idx].startIndex + idx);
      memcpy(&subMenuItems[idx], &tempMenuEntry, sizeof(menuEntryType)); // copy menu header to subMenuItems array for display
    }
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

// -----------------------------------------------------------------------------

void redrawOrgan() {
  if (currentMenuEntry.displayType == tm_preset) {
    drawMainOrgan();
    drawOrgan(ORGANBOX_LEFT, ORGANBOX_TOP, ORGANBOX_W, ORGANBOX_H, 1); // Zeichnet die Orgelgrafik, z.B. für die Anzeige der Registerbelegung oder ähnliches
  } else {
    drawSubmenuSelect(mainMenuItem, 0); // Submenü-Auswahl aktualisieren
    drawOrgan(ORGANBOX_LEFT, ORGANBOX_TOP, ORGANBOX_W, ORGANBOX_H, manualSelects[mainMenuItem]); // Zeichnet die Orgelgrafik, z.B. für die Anzeige der Registerbelegung oder ähnliches
  }
  drawValue(&currentMenuEntry, VALUEBOX_LEFT, VALUEBOX_TOP, VALUEBOX_W, VALUEBOX_H); // Zeichnet die Anzeige für den aktuellen Wert des Menüeintrags
}

void refreshMainPage(bool dimmed = false) {
  setMainDimmedState(dimmed); // Hauptfenster wieder auf normale Helligkeit setzen
  static bool last_dimmed_state = !dimmedMainWindow;
  if (last_dimmed_state != dimmed) {
    last_dimmed_state = dimmed;
  }
  drawMainMenu(mainMenuItem);
  redrawOrgan();
}

void displayMainPage() {
  // nach Message oder beim Start aufzurufen
  msgTimeoutActive = false; // Flag zurücksetzen
  setMainDimmedState(false);
  currentMenuState = s_inmainmenu; // Zurück zum Hauptmenü, um die Anzeige zu aktualisieren
  if (DISPLAY_H == 142) {
    drawBMP("/background142.bmp", 0, 0);
  // } else if (DISPLAY_H == 170) {
  //   drawBMP("/background170.bmp", 0, 0);
  } else {  
    drawBMP("/background240.bmp", 0, 0);
  }
  activeMenuItem = mainMenuItem; // Aktuell ausgewähltes Menü-Item speichern, bevor es geändert wird
  getMenuEntry(&currentMenuEntry, activeMenuItem); // Aktuelle Menü-Entry-Daten in globalen Variablen aktualisieren
  getSubMenuItems(mainMenuItem); // Submenü-Strings basierend auf der Auswahl im Hauptmenü aktualisieren
  refreshMainPage(false);
}

// -----------------------------------------------------------------------------

void sendPanelLEDs() {
  // Sende PER und VIB an das 74HC595-Schieberegister, um die LEDs am Panel zu steuern
  // PERC ON: Bit 7, PERC SOFT: Bit 6, PERC FAST: Bit 5, PERC THIRD: Bit 4, 
  // VIB ON UPPER: Bit 3, VIB ON LOWER: Bit 2, Rotary: Bit 1, Bit 0 unbenutzt
  uint8_t led_state = 0;
  if (hx3EditArray[128] != 0) {
    led_state |= 0b10000000; // Bit 7 für PERC ON
  }
  if (hx3EditArray[129] != 0) {
    led_state |= 0b01000000; // Bit 6 für PERC SOFT
  }
  if (hx3EditArray[130] != 0) {
    led_state |= 0b00100000; // Bit 5 für PERC FAST
  }
  if (hx3EditArray[131] != 0) {
    led_state |= 0b00010000; // Bit 4 für PERC THIRD
  }
  if (hx3EditArray[132] != 0) {
    led_state |= 0b00001000; // Bit 3 für VIB ON UPPER
  }
  if (hx3EditArray[133] != 0) {
    led_state |= 0b00000100; // Bit 2 für VIB ON LOWER
  }
  spi_sendLEDs(led_state);
}

// #############################################################################

// Change the value of a menu entry based on the delta from the rotary encoder and call the editAction if defined
void changeValue(menuEntryType *entry, int16_t delta) {
  if (entry->editArrayIdx >= 0) {
    int16_t newValue = delta + hx3EditArray[entry->editArrayIdx];
    if (newValue < entry->menuValueMin) newValue = entry->menuValueMin;
    if (newValue > entry->menuValueMax) newValue = entry->menuValueMax;
    hx3EditArray[entry->editArrayIdx] = newValue; // Beispiel: Rückgabe eines Werts aus einem Array
    editArrayRev++; // Webseiten per Polling informieren, dass sich ein Wert geändert hat
    // Bei Presets, Voices, Orgel- und Speaker-Modellen Array neu anfordern,
    // damit die Anzeige aktualisiert wird
    switch (entry->editArrayIdx) {
      case 256: // Rotary Slow/Stop/Fast
        spi_xc_binarycmd(entry->editArrayIdx + 1000, newValue, 50);
        if (newValue == 0) {
          spi_xc_binarycmd(1134, 1); // RUN/STOP Bit
          spi_xc_binarycmd(1135, 0); // Slow/Fast-Bit
        } else if (newValue == 1) {
          spi_xc_binarycmd(1134, 0); // RUN/STOP Bit
          spi_xc_binarycmd(1135, 0); // Slow/Fast-Bit
        } else if (newValue == 2) {
          spi_xc_binarycmd(1134, 1); // RUN/STOP Bit
          spi_xc_binarycmd(1135, 1); // Slow/Fast-Bit
        }
        break;
      case 268: // Preset/Voice changes
      case 269: // Voice-Nummer Upper
      case 270: // Voice-Nummer Lower
      case 271: // Voice-Nummer Pedal
      case 265: // Orgelmodell
      case 266: // Speakermodell
        spi_xc_binarycmd(entry->editArrayIdx + 1000, newValue, 250);
        break;
      default: // Standard-Aktion: Wert an HX3 senden
        spi_xc_binarycmd(entry->editArrayIdx + 1000, newValue, 50);
        break;
    }
  }
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

void enterBootloader() {
  spi_xc_binarycmd(8300, 0x0A, 150); // Reset to Bootloader Menu an HX3 senden
  BtnMenuStatus = TWI_BLMENU_BUSY;
  refreshMainPage(true); // dimmed
  mainMenuItem = BOOTLOAD_MENU;
  activeMenuItem = BOOTLOAD_MENU_START; // Setze das aktive Menü auf den Start des Bootloader-Menüs
  getMenuEntry(&currentMenuEntry, activeMenuItem); // Aktuelle Menü-Entry-Daten in globalen Variablen aktualisieren
  getSubMenuItems(mainMenuItem); // Submenü-Strings basierend auf der Auswahl im Hauptmenü aktualisieren
  activeMenuItem = mainMenuItems[mainMenuItem].submenuStart + subMenuProperties[mainMenuItem].itemIndex;
  currentMenuState = s_insubmenu;
  // selectSubMenu(0);
  setMainDimmedState(false); // Hauptfenster wieder auf normale Helligkeit setzen
  if (DISPLAY_H == 142) {
    drawBMP("/background142.bmp", 0, 0);
  // } else if (DISPLAY_H == 170) {
  //   drawBMP("/background170.bmp", 0, 0);
  } else {  
    drawBMP("/background240.bmp", 0, 0);
  }
  bootloaderActive = true;
  bootloaderBusy = false;
  drawMainMenu(mainMenuItem);
  drawSubmenuSelect(mainMenuItem, 0);
  drawBootloaderMessagepanel("Bootloader", "started");
  encoder.waitReleased(0);
  BtnMenuStatus = TWI_BLMENU_STARTFDU;
}

void exitBootloader() {
  // spi_xc_setcmd(XCMD_DONE); // Done Command an HX3 senden
  bootloaderActive = false;
  bootloaderBusy = false;
  drawMsg("Bootloader done", "Exit to Firmware", DB_INFO);
  delay(1000);
  encoder.waitReleased(0);
  mainMenuItem = 0;
  activeMenuItem = 0;
  currentMenuState = s_inmainmenu; // Zurück zum Hauptmenü, um die Anzeige zu aktualisieren
  displayMainPage();
  BtnMenuStatus = TWI_BLMENU_NONE; // Setze den Status auf None nach dem Verlassen des Bootloaders
}

void bootloaderCmd() {
  DPRINTF("bootloaderCmd called, activeMenuItem=");
  DPRINTLN(activeMenuItem);
  if (activeMenuItem == BOOTLOAD_EXIT) {
    exitBootloader();
  } else {
    // encoder.waitReleased(0);
  }
}

void savePreset() {
  int16_t presetNumber = hx3EditArray[currentMenuEntry.editArrayIdx]; // Aktuelle Preset-Nummer aus dem Edit-Array lesen
  drawEnterNumber("Save Preset to", &presetNumber, 0, 99);
  hx3EditArray[currentMenuEntry.editArrayIdx] = presetNumber; // Aktualisierte Preset-Nummer zurück ins Edit-Array schreiben
  spi_xc_binarycmd(900, presetNumber, 150); // Save-Befehl an HX3 senden
  displayMainPage();
}

void enterPresetName() { 
  char presetName[14]; // Buffer für den Preset-Namen (16 Zeichen + Nullterminator)
  memset(presetName, 32, sizeof(presetName)); // Buffer initialisieren
  presetName[13] = '\0'; // Sicherstellen, dass der String nullterminiert ist
  strcpy(presetName, (char*)hx3EditArray + 192); // Aktuellen Preset-Namen aus dem Edit-Array lesen
  drawEnterText("Enter Preset Name", presetName); // Funktion zum Eingeben von Text aufrufen
  strcpy(hx3PresetName, presetName); // Aktualisierten Preset-Namen in hx3PresetName speichern
  displayMainPage();
}

void saveWifi() {
  drawMsgTimeout("WIFI mode saved", "Restarting", MSG_DISPLAY_TIME, DB_INFO_OK);
  saveCredentials(); // WiFi-Einstellungen speichern, damit sie nach einem Neustart erhalten bleiben
  requestSema.reconnect = true; // Signalisiert dem Hauptloop, dass eine WiFi-Neuverbindung erforderlich ist
}

void saveUpper() {
  int16_t voiceNumber = hx3EditArray[currentMenuEntry.editArrayIdx]; // Aktuelle Voice-Nummer aus dem Edit-Array lesen
  drawEnterNumber("Save Upper to", &voiceNumber, 0, 15);
  hx3EditArray[currentMenuEntry.editArrayIdx] = voiceNumber; // Aktualisierte Voice-Nummer zurück ins Edit-Array schreiben
  spi_xc_binarycmd(901, voiceNumber, 150); // Save-Befehl an HX3 senden
  displayMainPage();
}

void saveLower() { 
  int16_t voiceNumber = hx3EditArray[currentMenuEntry.editArrayIdx]; // Aktuelle Voice-Nummer aus dem Edit-Array lesen
  drawEnterNumber("Save Lower to", &voiceNumber, 0, 15);
  hx3EditArray[currentMenuEntry.editArrayIdx] = voiceNumber; // Aktualisierte Voice-Nummer zurück ins Edit-Array schreiben
  spi_xc_binarycmd(902, voiceNumber, 150); // Save-Befehl an HX3 senden
  displayMainPage();
}

void savePedal() { 
  int16_t voiceNumber = hx3EditArray[currentMenuEntry.editArrayIdx]; // Aktuelle Voice-Nummer aus dem Edit-Array lesen
  drawEnterNumber("Save Pedal to", &voiceNumber, 0, 15);
  hx3EditArray[currentMenuEntry.editArrayIdx] = voiceNumber; // Aktualisierte Voice-Nummer zurück ins Edit-Array schreiben
  spi_xc_binarycmd(903, voiceNumber, 150); // Save-Befehl an HX3 senden
  displayMainPage();
}

void saveOrgan() { 
  int16_t organNumber = hx3EditArray[currentMenuEntry.editArrayIdx]; // Aktuelle Voice-Nummer aus dem Edit-Array lesen
  drawEnterNumber("Save Organ to", &organNumber, 0, 15);
  hx3EditArray[currentMenuEntry.editArrayIdx] = organNumber; // Aktualisierte Voice-Nummer zurück ins Edit-Array schreiben
  spi_xc_binarycmd(908, organNumber, 150); // Save-Befehl an HX3 senden
  displayMainPage();
}

void saveSpeaker() { 
  int16_t speakerNumber = hx3EditArray[currentMenuEntry.editArrayIdx]; // Aktuelle Voice-Nummer aus dem Edit-Array lesen
  drawEnterNumber("Save Speaker to", &speakerNumber, 0, 15);
  hx3EditArray[currentMenuEntry.editArrayIdx] = speakerNumber; // Aktualisierte Voice-Nummer zurück ins Edit-Array schreiben
  spi_xc_binarycmd(909, speakerNumber, 150); // Save-Befehl an HX3 senden
  displayMainPage();
}

void saveDefaults() { 
  drawMsgTimeout("Defaults saved", "", MSG_DISPLAY_TIME, DB_INFO_OK);
  spi_xc_binarycmd(905, 1, 150); // Save-Befehl an HX3 senden
  displayMainPage();
}

void saveSystemInits() { 
  drawMsgTimeout("Inits saved", "", MSG_DISPLAY_TIME, DB_INFO_OK);
  spi_xc_binarycmd(906, 1, 150); // Save-Befehl an HX3 senden
  displayMainPage();
}

void enterScreenSaver() { 
  settings.screenSaver = hx3EditArray[currentMenuEntry.editArrayIdx]; // Aktuellen Bildschirmschoner-Modus aus dem Edit-Array Dummy (496) lesen
  encoder.waitReleased(0);
  saveCredentials(); // WiFi-Einstellungen speichern, damit sie nach einem Neustart erhalten bleiben
  #ifdef ENABLE_SCREENSAVER
  screenSaverInit();
  switch (settings.screenSaver) {
    case 1:
      starField() ;
      break;
    case 2:
      gameOfLife();
      break;
    case 3:
      mandelbrot();
      break;
    case 4:
      analogClock();
      break;
    default:
      break;
  }
  #endif
  displayMainPage();
}

void requestBoardInfo() { 
  DPRINTLNF("Requesting board info");
  spi_xc_getBoardInfo(); // Fordert die Board-Info vom HX3 an
  drawBoardInfo();
  encoder.waitReleased(0);
  encoder.waitPressed(0);
  drawWifiInfo();
  encoder.waitReleased(0);
  encoder.waitPressed(0);
  displayMainPage();
}

#endif