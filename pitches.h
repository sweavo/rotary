/*************************************************
 * Public Constants
 *************************************************/

#define FREQ_B0  31
#define FREQ_C1  33
#define FREQ_CS1 35
#define FREQ_D1  37
#define FREQ_DS1 39
#define FREQ_E1  41
#define FREQ_F1  44
#define FREQ_FS1 46
#define FREQ_G1  49
#define FREQ_GS1 52
#define FREQ_A1  55
#define FREQ_AS1 58
#define FREQ_B1  62
#define FREQ_C2  65
#define FREQ_CS2 69
#define FREQ_D2  73
#define FREQ_DS2 78
#define FREQ_E2  82
#define FREQ_F2  87
#define FREQ_FS2 93
#define FREQ_G2  98
#define FREQ_GS2 104
#define FREQ_A2  110
#define FREQ_AS2 117
#define FREQ_B2  123
#define FREQ_C3  131
#define FREQ_CS3 139
#define FREQ_D3  147
#define FREQ_DS3 156
#define FREQ_E3  165
#define FREQ_F3  175
#define FREQ_FS3 185
#define FREQ_G3  196
#define FREQ_GS3 208
#define FREQ_A3  220
#define FREQ_AS3 233
#define FREQ_B3  247
#define FREQ_C4  262
#define FREQ_CS4 277
#define FREQ_D4  294
#define FREQ_DS4 311
#define FREQ_E4  330
#define FREQ_F4  349
#define FREQ_FS4 370
#define FREQ_G4  392
#define FREQ_GS4 415
#define FREQ_A4  440
#define FREQ_AS4 466
#define FREQ_B4  494
#define FREQ_C5  523
#define FREQ_CS5 554
#define FREQ_D5  587
#define FREQ_DS5 622
#define FREQ_E5  659
#define FREQ_F5  698
#define FREQ_FS5 740
#define FREQ_G5  784
#define FREQ_GS5 831
#define FREQ_A5  880
#define FREQ_AS5 932
#define FREQ_B5  988
#define FREQ_C6  1047
#define FREQ_CS6 1109
#define FREQ_D6  1175
#define FREQ_DS6 1245
#define FREQ_E6  1319
#define FREQ_F6  1397
#define FREQ_FS6 1480
#define FREQ_G6  1568
#define FREQ_GS6 1661
#define FREQ_A6  1760
#define FREQ_AS6 1865
#define FREQ_B6  1976
#define FREQ_C7  2093
#define FREQ_CS7 2217
#define FREQ_D7  2349
#define FREQ_DS7 2489
#define FREQ_E7  2637
#define FREQ_F7  2794
#define FREQ_FS7 2960
#define FREQ_G7  3136
#define FREQ_GS7 3322
#define FREQ_A7  3520
#define FREQ_AS7 3729
#define FREQ_B7  3951
#define FREQ_C8  4186
#define FREQ_CS8 4435
#define FREQ_D8  4699
#define FREQ_DS8 4978

const int PITCHES[] = {
    0,
    FREQ_C1, 
    FREQ_CS1, 
    FREQ_D1, 
    FREQ_DS1, 
    FREQ_E1, 
    FREQ_F1, 
    FREQ_FS1, 
    FREQ_G1, 
    FREQ_GS1, 
    FREQ_A1, 
    FREQ_AS1, 
    FREQ_B1, 
    FREQ_C2, 
    FREQ_CS2, 
    FREQ_D2, 
    FREQ_DS2, 
    FREQ_E2, 
    FREQ_F2, 
    FREQ_FS2, 
    FREQ_G2, 
    FREQ_GS2, 
    FREQ_A2, 
    FREQ_AS2, 
    FREQ_B2, 
    FREQ_C3, 
    FREQ_CS3, 
    FREQ_D3, 
    FREQ_DS3, 
    FREQ_E3, 
    FREQ_F3, 
    FREQ_FS3, 
    FREQ_G3, 
    FREQ_GS3, 
    FREQ_A3, 
    FREQ_AS3, 
    FREQ_B3, 
    FREQ_C4, 
    FREQ_CS4, 
    FREQ_D4, 
    FREQ_DS4, 
    FREQ_E4, 
    FREQ_F4, 
    FREQ_FS4, 
    FREQ_G4, 
    FREQ_GS4, 
    FREQ_A4, 
    FREQ_AS4, 
    FREQ_B4, 
    FREQ_C5, 
    FREQ_CS5, 
    FREQ_D5, 
    FREQ_DS5, 
    FREQ_E5, 
    FREQ_F5, 
    FREQ_FS5, 
    FREQ_G5, 
    FREQ_GS5, 
    FREQ_A5, 
    FREQ_AS5, 
    FREQ_B5, 
    FREQ_C6, 
    FREQ_CS6, 
    FREQ_D6, 
    FREQ_DS6, 
    FREQ_E6, 
    FREQ_F6, 
    FREQ_FS6, 
    FREQ_G6, 
    FREQ_GS6, 
    FREQ_A6, 
    FREQ_AS6, 
    FREQ_B6, 
    FREQ_C7, 
    FREQ_CS7, 
    FREQ_D7, 
    FREQ_DS7, 
    FREQ_E7, 
    FREQ_F7, 
    FREQ_FS7, 
    FREQ_G7, 
    FREQ_GS7, 
    FREQ_A7, 
    FREQ_AS7, 
    FREQ_B7, 
    FREQ_C8, 
    FREQ_CS8, 
    FREQ_D8, 
    FREQ_DS8
};


enum noteIndexes {
    NOTE_OFF,
    NOTE_C1, 
    NOTE_CS1, 
    NOTE_D1, 
    NOTE_DS1, 
    NOTE_E1, 
    NOTE_F1, 
    NOTE_FS1, 
    NOTE_G1, 
    NOTE_GS1, 
    NOTE_A1, 
    NOTE_AS1, 
    NOTE_B1, 
    NOTE_C2, 
    NOTE_CS2, 
    NOTE_D2, 
    NOTE_DS2, 
    NOTE_E2, 
    NOTE_F2, 
    NOTE_FS2, 
    NOTE_G2, 
    NOTE_GS2, 
    NOTE_A2, 
    NOTE_AS2, 
    NOTE_B2, 
    NOTE_C3, 
    NOTE_CS3, 
    NOTE_D3, 
    NOTE_DS3, 
    NOTE_E3, 
    NOTE_F3, 
    NOTE_FS3, 
    NOTE_G3, 
    NOTE_GS3, 
    NOTE_A3, 
    NOTE_AS3, 
    NOTE_B3, 
    NOTE_C4, 
    NOTE_CS4, 
    NOTE_D4, 
    NOTE_DS4, 
    NOTE_E4, 
    NOTE_F4, 
    NOTE_FS4, 
    NOTE_G4, 
    NOTE_GS4, 
    NOTE_A4, 
    NOTE_AS4, 
    NOTE_B4, 
    NOTE_C5, 
    NOTE_CS5, 
    NOTE_D5, 
    NOTE_DS5, 
    NOTE_E5, 
    NOTE_F5, 
    NOTE_FS5, 
    NOTE_G5, 
    NOTE_GS5, 
    NOTE_A5, 
    NOTE_AS5, 
    NOTE_B5, 
    NOTE_C6, 
    NOTE_CS6, 
    NOTE_D6, 
    NOTE_DS6, 
    NOTE_E6, 
    NOTE_F6, 
    NOTE_FS6, 
    NOTE_G6, 
    NOTE_GS6, 
    NOTE_A6, 
    NOTE_AS6, 
    NOTE_B6, 
    NOTE_C7, 
    NOTE_CS7, 
    NOTE_D7, 
    NOTE_DS7, 
    NOTE_E7, 
    NOTE_F7, 
    NOTE_FS7, 
    NOTE_G7, 
    NOTE_GS7, 
    NOTE_A7, 
    NOTE_AS7, 
    NOTE_B7, 
    NOTE_C8, 
    NOTE_CS8, 
    NOTE_D8, 
    NOTE_DS8
};

struct note_t {
  noteIndexes index;
  int value;
};
