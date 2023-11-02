#include <iostream>  
#include <math.h>
#include <SDL2/SDL.h>
#include <time.h>
#include <stdlib.h>
#include <thread>
#include <vector>

using namespace std;  

// Utility macros
#define CHECK_ERROR(test, message) \
    do { \
        if((test)) { \
            fprintf(stderr, "%s\n", (message)); \
            exit(1); \
        } \
    } while(0)

#define WINDOW_WIDTH 640
#define WINDOW_WIDTH_HALF WINDOW_WIDTH/2
#define WINDOW_HEIGHT 480
#define WINDOW_HEIGHT_HALF WINDOW_HEIGHT/2
#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062
#define TICKS_FOR_NEXT_FRAME (1000 / 60)

// Font Variables
#define renderFontWidth 8
#define renderFontHeight 8

// Font Data
unsigned char renderFontArray[256*256] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x10,0x10,0x10,0x00,0x10,0x00,0x00,
	0x00,0x28,0x28,0x00,0x00,0x00,0x00,0x00,
	0x00,0x14,0x54,0x28,0x54,0x50,0x00,0x00,
	0x10,0x3C,0x50,0x38,0x14,0x78,0x10,0x00,
	0x00,0x64,0x68,0x10,0x2C,0x4C,0x00,0x00,
	0x00,0x20,0x50,0x24,0x58,0x34,0x00,0x00,
	0x00,0x10,0x10,0x00,0x00,0x00,0x00,0x00,
	0x00,0x10,0x20,0x20,0x20,0x10,0x00,0x00,
	0x00,0x10,0x08,0x08,0x08,0x10,0x00,0x00,
	0x00,0x28,0x10,0x28,0x00,0x00,0x00,0x00,
	0x00,0x10,0x10,0x7C,0x10,0x10,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x10,0x20,0x00,
	0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,
	0x00,0x08,0x08,0x10,0x20,0x20,0x00,0x00,
	0x00,0x38,0x4C,0x54,0x64,0x38,0x00,0x00,
	0x00,0x10,0x30,0x10,0x10,0x38,0x00,0x00,
	0x00,0x38,0x44,0x18,0x20,0x7C,0x00,0x00,
	0x00,0x38,0x44,0x18,0x44,0x38,0x00,0x00,
	0x00,0x44,0x44,0x3C,0x04,0x04,0x00,0x00,
	0x00,0x7C,0x40,0x78,0x04,0x78,0x00,0x00,
	0x00,0x38,0x40,0x78,0x44,0x38,0x00,0x00,
	0x00,0x7C,0x04,0x08,0x10,0x10,0x00,0x00,
	0x00,0x38,0x44,0x38,0x44,0x38,0x00,0x00,
	0x00,0x38,0x44,0x3C,0x04,0x38,0x00,0x00,
	0x00,0x00,0x10,0x00,0x00,0x10,0x00,0x00,
	0x00,0x00,0x10,0x00,0x00,0x10,0x20,0x00,
	0x00,0x08,0x10,0x20,0x10,0x08,0x00,0x00,
	0x00,0x00,0x7C,0x00,0x7C,0x00,0x00,0x00,
	0x00,0x20,0x10,0x08,0x10,0x20,0x00,0x00,
	0x00,0x38,0x08,0x10,0x00,0x10,0x00,0x00,
	0x00,0x38,0x48,0x50,0x44,0x38,0x00,0x00,
	0x00,0x38,0x44,0x7C,0x44,0x44,0x00,0x00,
	0x00,0x70,0x48,0x70,0x48,0x78,0x00,0x00,
	0x00,0x38,0x44,0x40,0x44,0x38,0x00,0x00,
	0x00,0x78,0x44,0x44,0x44,0x78,0x00,0x00,
	0x00,0x7C,0x40,0x70,0x40,0x7C,0x00,0x00,
	0x00,0x7C,0x40,0x70,0x40,0x40,0x00,0x00,
	0x00,0x38,0x40,0x4C,0x44,0x38,0x00,0x00,
	0x00,0x44,0x44,0x7C,0x44,0x44,0x00,0x00,
	0x00,0x38,0x10,0x10,0x10,0x38,0x00,0x00,
	0x00,0x7C,0x04,0x04,0x44,0x38,0x00,0x00,
	0x00,0x44,0x48,0x70,0x48,0x44,0x00,0x00,
	0x00,0x40,0x40,0x40,0x40,0x7C,0x00,0x00,
	0x00,0x44,0x6C,0x54,0x44,0x44,0x00,0x00,
	0x00,0x44,0x64,0x54,0x4C,0x44,0x00,0x00,
	0x00,0x38,0x44,0x44,0x44,0x38,0x00,0x00,
	0x00,0x78,0x44,0x78,0x40,0x40,0x00,0x00,
	0x00,0x38,0x44,0x44,0x4C,0x3C,0x00,0x00,
	0x00,0x78,0x44,0x78,0x44,0x44,0x00,0x00,
	0x00,0x3C,0x40,0x38,0x04,0x78,0x00,0x00,
	0x00,0x7C,0x10,0x10,0x10,0x10,0x00,0x00,
	0x00,0x44,0x44,0x44,0x44,0x38,0x00,0x00,
	0x00,0x44,0x44,0x44,0x28,0x10,0x00,0x00,
	0x00,0x44,0x44,0x54,0x6C,0x44,0x00,0x00,
	0x00,0x44,0x28,0x10,0x28,0x44,0x00,0x00,
	0x00,0x44,0x28,0x10,0x10,0x10,0x00,0x00,
	0x00,0x7C,0x08,0x10,0x20,0x7C,0x00,0x00,
	0x00,0x30,0x20,0x20,0x20,0x30,0x00,0x00,
	0x00,0x20,0x20,0x10,0x08,0x08,0x00,0x00,
	0x00,0x18,0x08,0x08,0x08,0x18,0x00,0x00,
	0x00,0x10,0x28,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x7C,0x00,0x00,
	0x20,0x10,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x38,0x48,0x48,0x3C,0x00,0x00,
	0x00,0x40,0x40,0x78,0x44,0x78,0x00,0x00,
	0x00,0x00,0x38,0x40,0x40,0x38,0x00,0x00,
	0x00,0x04,0x04,0x3C,0x44,0x3C,0x00,0x00,
	0x00,0x00,0x38,0x78,0x40,0x38,0x00,0x00,
	0x00,0x08,0x10,0x38,0x10,0x10,0x00,0x00,
	0x00,0x00,0x38,0x48,0x38,0x08,0x30,0x00,
	0x00,0x40,0x40,0x78,0x44,0x44,0x00,0x00,
	0x00,0x10,0x00,0x10,0x10,0x10,0x00,0x00,
	0x00,0x10,0x00,0x10,0x10,0x10,0x20,0x00,
	0x00,0x40,0x48,0x70,0x48,0x48,0x00,0x00,
	0x00,0x20,0x20,0x20,0x20,0x10,0x00,0x00,
	0x00,0x00,0x68,0x54,0x54,0x54,0x00,0x00,
	0x00,0x00,0x78,0x44,0x44,0x44,0x00,0x00,
	0x00,0x00,0x38,0x44,0x44,0x38,0x00,0x00,
	0x00,0x00,0x78,0x44,0x7C,0x40,0x40,0x00,
	0x00,0x00,0x3C,0x44,0x7C,0x04,0x04,0x00,
	0x00,0x00,0x5C,0x60,0x40,0x40,0x00,0x00,
	0x00,0x00,0x3C,0x70,0x0C,0x78,0x00,0x00,
	0x00,0x10,0x38,0x10,0x10,0x08,0x00,0x00,
	0x00,0x00,0x44,0x44,0x44,0x3C,0x00,0x00,
	0x00,0x00,0x44,0x44,0x28,0x10,0x00,0x00,
	0x00,0x00,0x44,0x54,0x54,0x28,0x00,0x00,
	0x00,0x00,0x00,0x28,0x10,0x28,0x00,0x00,
	0x00,0x00,0x28,0x28,0x18,0x08,0x10,0x00,
	0x00,0x00,0x7C,0x08,0x30,0x7C,0x00,0x00,
	0x00,0x10,0x20,0x60,0x20,0x10,0x00,0x00,
	0x00,0x10,0x10,0x10,0x10,0x10,0x00,0x00,
	0x00,0x10,0x08,0x0C,0x08,0x10,0x00,0x00,
	0x00,0x00,0x34,0x48,0x00,0x00,0x00,0x00,
	0x54,0xAA,0x54,0xAA,0x54,0xAA,0x54,0x00,
	0x00,0x00,0x00,0x1F,0x10,0x10,0x10,0x10,
	0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0xF0,0x10,0x10,0x10,0x10,
	0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
	0x10,0x10,0x10,0x1F,0x00,0x00,0x00,0x00,
	0x10,0x10,0x10,0xF0,0x00,0x00,0x00,0x00,
	0x10,0x10,0x10,0xFF,0x10,0x10,0x10,0x10,
	0x10,0x10,0x10,0xFF,0x00,0x00,0x00,0x00,
	0x10,0x10,0x10,0x1F,0x10,0x10,0x10,0x10,
	0x00,0x00,0x00,0xFF,0x10,0x10,0x10,0x10,
	0x10,0x10,0x10,0xF0,0x10,0x10,0x10,0x10,
	0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0x00,
	0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0x00,
	0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0x00,
	0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0x00,
	0xFE,0xAA,0xFE,0xAA,0xFE,0xAA,0xFE,0x00,
	0xFF,0xFF,0xFF,0xE0,0xEF,0xEF,0xEF,0xEF,
	0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0x0F,0xEF,0xEF,0xEF,0xEF,
	0xEF,0xEF,0xEF,0xEF,0xEF,0xEF,0xEF,0xEF,
	0xEF,0xEF,0xEF,0xE0,0xFF,0xFF,0xFF,0xFF,
	0xEF,0xEF,0xEF,0x0F,0xFF,0xFF,0xFF,0xFF,
	0xEF,0xEF,0xEF,0x00,0xEF,0xEF,0xEF,0xEF,
	0xEF,0xEF,0xEF,0x00,0xFF,0xFF,0xFF,0xFF,
	0xEF,0xEF,0xEF,0xE0,0xEF,0xEF,0xEF,0xEF,
	0xFF,0xFF,0xFF,0x00,0xEF,0xEF,0xEF,0xEF,
	0xEF,0xEF,0xEF,0x0F,0xEF,0xEF,0xEF,0xEF,
	0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0x00,
	0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0x00,
	0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0x00,
	0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0x00,
	0x00,0x54,0x00,0x54,0x00,0x54,0x00,0x00,
	0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0x00,
	0xFE,0xEE,0xEE,0xEE,0xFE,0xEE,0xFE,0x00,
	0xFE,0xD6,0xD6,0xFE,0xFE,0xFE,0xFE,0x00,
	0xFE,0xEA,0xAA,0xD6,0xAA,0xAE,0xFE,0x00,
	0xEE,0xC2,0xAE,0xC6,0xEA,0x86,0xEE,0x00,
	0xFE,0x9A,0x96,0xEE,0xD2,0xB2,0xFE,0x00,
	0xFE,0xDE,0xAE,0xDA,0xA6,0xCA,0xFE,0x00,
	0xFE,0xEE,0xEE,0xFE,0xFE,0xFE,0xFE,0x00,
	0xFE,0xEE,0xDE,0xDE,0xDE,0xEE,0xFE,0x00,
	0xFE,0xEE,0xF6,0xF6,0xF6,0xEE,0xFE,0x00,
	0xFE,0xD6,0xEE,0xD6,0xFE,0xFE,0xFE,0x00,
	0xFE,0xEE,0xEE,0x82,0xEE,0xEE,0xFE,0x00,
	0xFE,0xFE,0xFE,0xFE,0xFE,0xEE,0xDE,0x00,
	0xFE,0xFE,0xFE,0x82,0xFE,0xFE,0xFE,0x00,
	0xFE,0xFE,0xFE,0xFE,0xFE,0xEE,0xFE,0x00,
	0xFE,0xF6,0xF6,0xEE,0xDE,0xDE,0xFE,0x00,
	0xFE,0xC6,0xB2,0xAA,0x9A,0xC6,0xFE,0x00,
	0xFE,0xEE,0xCE,0xEE,0xEE,0xC6,0xFE,0x00,
	0xFE,0xC6,0xBA,0xE6,0xDE,0x82,0xFE,0x00,
	0xFE,0xC6,0xBA,0xE6,0xBA,0xC6,0xFE,0x00,
	0xFE,0xBA,0xBA,0xC2,0xFA,0xFA,0xFE,0x00,
	0xFE,0x82,0xBE,0x86,0xFA,0x86,0xFE,0x00,
	0xFE,0xC6,0xBE,0x86,0xBA,0xC6,0xFE,0x00,
	0xFE,0x82,0xFA,0xF6,0xEE,0xEE,0xFE,0x00,
	0xFE,0xC6,0xBA,0xC6,0xBA,0xC6,0xFE,0x00,
	0xFE,0xC6,0xBA,0xC2,0xFA,0xC6,0xFE,0x00,
	0xFE,0xFE,0xEE,0xFE,0xFE,0xEE,0xFE,0x00,
	0xFE,0xFE,0xEE,0xFE,0xFE,0xEE,0xDE,0x00,
	0xFE,0xF6,0xEE,0xDE,0xEE,0xF6,0xFE,0x00,
	0xFE,0xFE,0x82,0xFE,0x82,0xFE,0xFE,0x00,
	0xFE,0xDE,0xEE,0xF6,0xEE,0xDE,0xFE,0x00,
	0xFE,0xC6,0xF6,0xEE,0xFE,0xEE,0xFE,0x00,
	0xFE,0xC6,0xB6,0xAE,0xBA,0xC6,0xFE,0x00,
	0xFE,0xC6,0xBA,0x82,0xBA,0xBA,0xFE,0x00,
	0xFE,0x8E,0xB6,0x8E,0xB6,0x86,0xFE,0x00,
	0xFE,0xC6,0xBA,0xBE,0xBA,0xC6,0xFE,0x00,
	0xFE,0x86,0xBA,0xBA,0xBA,0x86,0xFE,0x00,
	0xFE,0x82,0xBE,0x8E,0xBE,0x82,0xFE,0x00,
	0xFE,0x82,0xBE,0x8E,0xBE,0xBE,0xFE,0x00,
	0xFE,0xC6,0xBE,0xB2,0xBA,0xC6,0xFE,0x00,
	0xFE,0xBA,0xBA,0x82,0xBA,0xBA,0xFE,0x00,
	0xFE,0xC6,0xEE,0xEE,0xEE,0xC6,0xFE,0x00,
	0xFE,0x82,0xFA,0xFA,0xBA,0xC6,0xFE,0x00,
	0xFE,0xBA,0xB6,0x8E,0xB6,0xBA,0xFE,0x00,
	0xFE,0xBE,0xBE,0xBE,0xBE,0x82,0xFE,0x00,
	0xFE,0xBA,0x92,0xAA,0xBA,0xBA,0xFE,0x00,
	0xFE,0xBA,0x9A,0xAA,0xB2,0xBA,0xFE,0x00,
	0xFE,0xC6,0xBA,0xBA,0xBA,0xC6,0xFE,0x00,
	0xFE,0x86,0xBA,0x86,0xBE,0xBE,0xFE,0x00,
	0xFE,0xC6,0xBA,0xBA,0xB2,0xC2,0xFE,0x00,
	0xFE,0x86,0xBA,0x86,0xBA,0xBA,0xFE,0x00,
	0xFE,0xC2,0xBE,0xC6,0xFA,0x86,0xFE,0x00,
	0xFE,0x82,0xEE,0xEE,0xEE,0xEE,0xFE,0x00,
	0xFE,0xBA,0xBA,0xBA,0xBA,0xC6,0xFE,0x00,
	0xFE,0xBA,0xBA,0xBA,0xD6,0xEE,0xFE,0x00,
	0xFE,0xBA,0xBA,0xAA,0x92,0xBA,0xFE,0x00,
	0xFE,0xBA,0xD6,0xEE,0xD6,0xBA,0xFE,0x00,
	0xFE,0xBA,0xD6,0xEE,0xEE,0xEE,0xFE,0x00,
	0xFE,0x82,0xF6,0xEE,0xDE,0x82,0xFE,0x00,
	0xFE,0xCE,0xDE,0xDE,0xDE,0xCE,0xFE,0x00,
	0xFE,0xDE,0xDE,0xEE,0xF6,0xF6,0xFE,0x00,
	0xFE,0xE6,0xF6,0xF6,0xF6,0xE6,0xFE,0x00,
	0xFE,0xEE,0xD6,0xFE,0xFE,0xFE,0xFE,0x00,
	0xFE,0xFE,0xFE,0xFE,0xFE,0x82,0xFE,0x00,
	0xDE,0xEE,0xFE,0xFE,0xFE,0xFE,0xFE,0x00,
	0xFE,0xFE,0xC6,0xB6,0xB6,0xC2,0xFE,0x00,
	0xFE,0xBE,0xBE,0x86,0xBA,0x86,0xFE,0x00,
	0xFE,0xFE,0xC6,0xBE,0xBE,0xC6,0xFE,0x00,
	0xFE,0xFA,0xFA,0xC2,0xBA,0xC2,0xFE,0x00,
	0xFE,0xFE,0xC6,0x86,0xBE,0xC6,0xFE,0x00,
	0xFE,0xF6,0xEE,0xC6,0xEE,0xEE,0xFE,0x00,
	0xFE,0xFE,0xC6,0xB6,0xC6,0xF6,0xCE,0x00,
	0xFE,0xBE,0xBE,0x86,0xBA,0xBA,0xFE,0x00,
	0xFE,0xEE,0xFE,0xEE,0xEE,0xEE,0xFE,0x00,
	0xFE,0xEE,0xFE,0xEE,0xEE,0xEE,0xDE,0x00,
	0xFE,0xBE,0xB6,0x8E,0xB6,0xB6,0xFE,0x00,
	0xFE,0xDE,0xDE,0xDE,0xDE,0xEE,0xFE,0x00,
	0xFE,0xFE,0x96,0xAA,0xAA,0xAA,0xFE,0x00,
	0xFE,0xFE,0x86,0xBA,0xBA,0xBA,0xFE,0x00,
	0xFE,0xFE,0xC6,0xBA,0xBA,0xC6,0xFE,0x00,
	0xFE,0xFE,0x86,0xBA,0x82,0xBE,0xBE,0x00,
	0xFE,0xFE,0xC2,0xBA,0x82,0xFA,0xFA,0x00,
	0xFE,0xFE,0xA2,0x9E,0xBE,0xBE,0xFE,0x00,
	0xFE,0xFE,0xC2,0x8E,0xF2,0x86,0xFE,0x00,
	0xFE,0xEE,0xC6,0xEE,0xEE,0xF6,0xFE,0x00,
	0xFE,0xFE,0xBA,0xBA,0xBA,0xC2,0xFE,0x00,
	0xFE,0xFE,0xBA,0xBA,0xD6,0xEE,0xFE,0x00,
	0xFE,0xFE,0xBA,0xAA,0xAA,0xD6,0xFE,0x00,
	0xFE,0xFE,0xFE,0xD6,0xEE,0xD6,0xFE,0x00,
	0xFE,0xFE,0xD6,0xD6,0xE6,0xF6,0xEE,0x00,
	0xFE,0xFE,0x82,0xF6,0xCE,0x82,0xFE,0x00,
	0xFE,0xEE,0xDE,0x9E,0xDE,0xEE,0xFE,0x00,
	0xFE,0xEE,0xEE,0xEE,0xEE,0xEE,0xFE,0x00,
	0xFE,0xEE,0xF6,0xF2,0xF6,0xEE,0xFE,0x00,
	0xFE,0xFE,0xCA,0xB6,0xFE,0xFE,0xFE,0x00,
	0xAA,0x00,0xAA,0x00,0xAA,0x00,0xAA,0x00
};

// 2D Position
struct Point {
	float x;
	float y;
};
typedef struct Point Point;
	
struct Vec3 {
	float x;
	float y;
	float z;
};
typedef struct Vec3 Vec3;

// RGB Color Struct going from 0.0f - 1.0f
struct Color {
	float r;
	float g;
	float b;
};
typedef struct Color Color;

// Screen Column Struct, containing an RGB Pixel color, and the distance it has
struct ScreenColumn {
	Color color;
	int amountOfRaySteps;
};
typedef struct ScreenColumn ScreenColumn;

/* ---- Pointers ---- */
// Contains the Array Objects used for calculating the screen
struct Ray *RayArray;
// Contains the Lines making up the level
struct Line *LineArray;
// Contains the Point Lights of the Level
struct PointLight *LightArray;
// Contains the calculated Distances and Colors of the Floorlight
struct Color *FloorLightArray;
// Contains the calculated Distances and Colors of the Ceilinglight
struct Color *CeilingLightArray;
// Contains the pre-calculated Distances of each Step
float *StepSizeDistanceArray;

/* -- SDL Pointers --*/
SDL_Event event;
SDL_Renderer *renderer;
SDL_Window *window;
SDL_Texture *texture;
void *texture_pixels;
int texture_pitch;

ScreenColumn screen[WINDOW_WIDTH];

// Camera Variables

/* ---- Runtime Variables ---- */
bool frameDone = false;
bool running = true;
int lastFrameTime = 0;

/* ---- Object Limits ---- */
int numberOfLines = 20;
int numberOfLights = 10;
int numberOfBounces = 0;
int numberOfRenderSectors = 4;
int numberOfRays = numberOfRenderSectors;

/* ---- Camera Variables ---- */
Point cameraPosition;
float cameraRotation = 0.0f;
float fieldOfView = 180.0f;
float horizonDistance = 16.0f;
float initialRayStepSize = 0.01f;


/* ---- Player Variables ---- */
float maxSpeed = 6.0f;
float cameraSpeedHorziontal = 0.0f;
float cameraSpeedVertical = 0.0f;
float cameraSpeedRotational = 0.0f;

/* ---- Debug Variables */
bool topDown = false;
bool renderMode = false;
Color skyLight;
Color floorColor;

/* --- Default Color ---- */
Color white;
Color black;
Color red;
Color green;
Color blue;

/* ---- CLASSES ---- */

/*
class Material {
	public 
}

class Texture {
	
}
*/

// A primitive PointLight
class PointLight {
	public:
		Point position;
		Color color;
		float brightness;
		float distance;
		
	PointLight(Point _p, Color _color, float _brightness, float _distance) {
		position = _p;
		color = _color;
		brightness = _brightness;
		distance = _distance;
	}
	
	PointLight(float x, float y, int r, int g, int b, float _brightness, float _distance) {
		position.x = x;
		position.y = y;
		color.r = r;
		color.g = g;
		color.b = b;
		brightness = _brightness;
		distance = _distance;
	}
};


// A simple line or wall
class Line {
	public:
		Point p1;
		Point p2;
		Color color;
		bool emissive = false;
		
		// Without color
		Line(float x1, float y1, float x2, float y2) {
			p1.x = x1;
			p1.y = y1;
			p2.x = x2;
			p2.y = y2;
			color = white;
		}
		
		Line(Point _p1, Point _p2) {
			p1 = _p1;
			p2 = _p2;
			color = white;
		}
		
		// With color
		Line(float x1, float y1, float x2, float y2, int r, int g, int b) {
			p1.x = x1;
			p1.y = y1;
			p2.x = x2;
			p2.y = y2;
			color.r = r;
			color.g = g;
			color.b = b;
		}
		
		Line(Point _p1, Point _p2, Color _color) {
			p1 = _p1;
			p2 = _p2;
			color = _color;
		}
};

/* ---- HELPER FUNCTIONS ---- */
// If returns 0, no intersection has been found
// If returns > 0, index of Line
bool ccw(const Point& p1, const Point& p2, const Point& p3) {
  return (p3.y - p1.y) * (p2.x - p1.x) > (p2.y - p1.y) * (p3.x - p1.x);
}

bool intersect(const Point& p1, const Point& p2, const Point& p3,
               const Point& p4) {
  return ccw(p1, p3, p4) != ccw(p2, p3, p4) &&
         ccw(p1, p2, p3) != ccw(p1, p2, p4);
}

int checkIfAnyLinesIntersect(const Point& p1, const Point& p2) {
  for (int i = 1; i <= numberOfLines; i++) {
    if (intersect(p1, p2, LineArray[i].p1, LineArray[i].p2)) {
      return i;
    }
  }
  return 0;
}

/* Input two lines, returns their Intersection Point */
Point getIntersectionPoint(Line Line1, Line Line2) {
	Point result;
  // Check if Line1 is vertical
  if (Line1.p1.x == Line1.p2.x) {
    if (Line2.p1.x == Line2.p2.x) {
      // Both Lines are vertical, so they are either coincident or parallel.
      if (Line1.p1.x == Line2.p1.x && ((Line1.p1.y <= Line2.p2.y && Line1.p2.y <= Line2.p1.y) ||
                                      (Line2.p1.y <= Line1.p2.y && Line2.p2.y <= Line1.p1.y))) {
        return Line1.p1; // Return a non-zero value to indicate an intersection
      }
    } else {
      // Calculate the equation of the Line representing Line2
      float m2 = (Line2.p2.y - Line2.p1.y) / (Line2.p2.x - Line2.p1.x);
      float b2 = Line2.p1.y - m2 * Line2.p1.x;

      // Calculate the x-coordinate of the intersection point
      float x = Line1.p1.x;
      float y = m2 * x + b2;

      // Check if the intersection point is within the Line segments
      if ((Line1.p1.y <= y <= Line1.p2.y) && (Line2.p1.y <= y <= Line2.p2.y)) {
        Line1.p1.x = x;
        Line1.p1.y = y;
        return Line1.p1; // Return a non-zero value to indicate an intersection
      }
    }
  } else {
    float m1 = (Line1.p2.y - Line1.p1.y) / (Line1.p2.x - Line1.p1.x);
    float m2 = (Line2.p2.y - Line2.p1.y) / (Line2.p2.x - Line2.p1.x);

    if (m1 == m2) {
      // Lines are parallel, no intersection
      return result;
    }

    // Calculate the x-coordinate of the intersection point
    float x = (m1 * Line1.p1.x - Line1.p1.y - m2 * Line2.p1.x + Line2.p1.y) / (m1 - m2);
    float y = m1 * (x - Line1.p1.x) + Line1.p1.y;

    // Check if the intersection point is within the Line segments
    if ((Line1.p1.x <= x <= Line1.p2.x) && (Line2.p1.x <= x <= Line2.p2.x)) {
      Line1.p1.x = x;
      Line1.p1.y = y;
      return Line1.p1; // Return a non-zero value to indicate an intersection
    }
  }

  // No intersection found
  return result;
}


// Convert Radian to Degree
float radianToDegree(float radian) {
	return radian * 180.0f/PI;
}

// Convert Degree to Radian
float degreeToRadian(float degree) {
	return degree * PI/180.0f;
}

// Get the Distance between two Points
float getDistance(Point p1, Point p2) {
	return sqrt((p2.x-p1.x)*(p2.x-p1.x) + (p2.y - p1.y)*(p2.y - p1.y));
}

// Calculate the normal angle of a Line
float calculateNormalAngle(Line& line) {
  // Calculate the direction vector of the line.
  float dx = line.p2.x - line.p1.x;
  float dy = line.p2.y - line.p1.y;

  // Calculate the magnitude of the direction vector.
  double magnitude = sqrt(dx * dx + dy * dy);

  // Calculate the normal vector of the line.
  double nx = -dy / magnitude;
  double ny = dx / magnitude;

  // Calculate the angle between the normal vector and the positive x-axis.
  double angle = atan2(ny, nx);

  // Return the angle in radians.
  return angle;
}

// Clamp a float between min and max
float clamp(float in, float min, float max) {
	if (in > max) {
		in = max;
	}
	
	if (in < min) {
		in = min;
	}
	
	return in;
}

// Clamp an int between min and max
int intClamp(int in, int min, int max) {
	if (in < min) {
		in = min;
	}
	if (in > max) {
		in = max;
	}
	return in;
}

// Prevent overflow of angular rotation
int limitDegreeAngle(float& degreeAngle) {
	degreeAngle = fmod(degreeAngle,360.0f);
	return 0;
}

/* ---- TEXTURE FUNCTIONS ---- */

// Get point along line
float getTextureColumnAlongLine(Point hitPosition, Line line) {
	float columnOnLine = 0.0f;
	return columnOnLine;
}

/* ---- INPUT HANDLING ---- */
void PrintKeyInfo( SDL_KeyboardEvent *key ){
	/* Is it a release or a press? */
	if( key->type == SDL_KEYUP )
		printf( "Release:- " );
	else
		printf( "Press:- " );

	/* Print the hardware scancode first */
	printf( "Scancode: 0x%02X", key->keysym.scancode );
	/* Print the name of the key */
	printf( ", Name: %s", SDL_GetKeyName( key->keysym.sym ) );
	printf( "\n" );
}

int processControls() {
		const Uint8 *keys = SDL_GetKeyboardState(NULL);
		cameraSpeedHorziontal /= 2.0f;
		cameraSpeedVertical /= 2.0f;
		cameraSpeedRotational /= 2.0f;
		
		// Forward
        if (keys[SDL_SCANCODE_W]) {
			cameraSpeedHorziontal += 1.0f*sin(degreeToRadian(cameraRotation));
			cameraSpeedVertical += 1.0f*cos(degreeToRadian(cameraRotation));
		}
		
		// Backward
        if (keys[SDL_SCANCODE_S]) {
			cameraSpeedHorziontal -= 1.0f*sin(degreeToRadian(cameraRotation));
			cameraSpeedVertical -= 1.0f*cos(degreeToRadian(cameraRotation));
		}
		
		// Left
        if (keys[SDL_SCANCODE_A]) {
			cameraSpeedHorziontal += 1.0f*sin(degreeToRadian(cameraRotation+90));
			cameraSpeedVertical += 1.0f*cos(degreeToRadian(cameraRotation+90));
		}
		
		// Right
        if (keys[SDL_SCANCODE_D]) {
			cameraSpeedHorziontal += 1.0f*sin(degreeToRadian(cameraRotation-90));
			cameraSpeedVertical += 1.0f*cos(degreeToRadian(cameraRotation-90));
		}
		
		// Look-Left
        if (keys[SDL_SCANCODE_Q]) {
			cameraSpeedRotational += 1.0f;
		}
		
		// Look-Right
        if (keys[SDL_SCANCODE_E]) {
			cameraSpeedRotational -= 1.0f;
		}
		
		// Quit
        if (keys[SDL_SCANCODE_ESCAPE]) {
			running = false;
		}
		return 0;
}

void updateInputs() {
	float lightMotion = -2.0;
	while(running) {		
		processControls();
		
		// Update Origin
		cameraPosition.x += cameraSpeedHorziontal;
		cameraPosition.y += cameraSpeedVertical;
		cameraRotation += cameraSpeedRotational;
		limitDegreeAngle(cameraRotation);
		
		// Limit to maxSpeed
		if (cameraSpeedHorziontal >= maxSpeed) {
			cameraSpeedHorziontal = maxSpeed;
		}
		if (cameraSpeedVertical >= maxSpeed) {
			cameraSpeedVertical = maxSpeed;
		}
		if (cameraSpeedRotational >= maxSpeed) {
			cameraSpeedRotational = maxSpeed;
		}
		SDL_Delay(16);
		
		// Object movement
		
		// LightAnimation
		if (LightArray[2].position.x < (640*2)/32) {
			lightMotion*=-1;
		}
		
		if (LightArray[2].position.x > (640*2)/32*31) {
			lightMotion*=-1;
		}
	
		LightArray[2].position.x+=lightMotion;
	}
}

// Get the bounce angle based on the Angle of incident and the normal angle
float getBounceReflection(float incidentAngle, float normalAngle) {
	// Convert angles from degrees to radians
	float incidentDirectionRad = degreeToRadian(incidentAngle);
	float normalAngleRad = degreeToRadian(normalAngle);
	
	// Calculate the incident direction vector
	float incidentX = cos(incidentDirectionRad);
	float incidentY = sin(incidentDirectionRad);

	// Calculate the normal vector
	float normalX = cos(normalAngleRad);
	float normalY = sin(normalAngleRad);

	// Calculate the dot product of incident and normal vectors
	float dotProduct = incidentX * normalX + incidentY * normalY;

	// Calculate the reflection vector
	float reflectionX = incidentX - 2 * dotProduct * normalX;
	float reflectionY = incidentY - 2 * dotProduct * normalY;

	// Calculate the angle of the reflection vector in degrees
	return radianToDegree(atan2(reflectionY, reflectionX));
}

/* ---- RAYTRACING ---- */
// The Ray class, used for raytracing
class Ray {
	public:
		Point position;
		float direction;
		int currentColumn;
		float RayStepSize;
		int stepCount = 0;
		
		Ray(float x, float y, float _direction) {
			position.x = x;
			position.y = y;
			direction = _direction;
			RayStepSize = initialRayStepSize;
		}
		
		Ray() {
			position.x = cameraPosition.x;
			position.y = cameraPosition.y;
			direction = cameraRotation;
			RayStepSize = initialRayStepSize;
		}
		
		void ResetRay() {
			position.x = cameraPosition.x;
			position.y = cameraPosition.y;
			direction = cameraRotation;
			RayStepSize = initialRayStepSize;
		}
		
		// Because for some reason just defining it isn't enough-?
		void setInitialRayStepSize(float _RayStepSize) {
			RayStepSize	= _RayStepSize;
		}
		
		// Used to step through the Ray until a wall is hit
		int step() {
			//printf("%f\n",RayStepSize);
			//RayStepSize = pow((float)stepCount/(float)(WINDOW_HEIGHT_HALF),3)*3;
			RayStepSize = StepSizeDistanceArray[stepCount];//pow((float)stepCount/(float)(WINDOW_HEIGHT_HALF),2) * horizonDistance;
			//printf("%f\n",RayStepSize);
			Point previousPosition = position;
			float rad = degreeToRadian(direction);
			position.x += (RayStepSize*sin(rad));
			position.y += (RayStepSize*cos(rad));
			//(getDistance(position,cameraPosition)/horizonDistance) * WINDOW_HEIGHT/6;
			//printf("%f\n",RayStepSize);
			
			// Draws those cool light columns
			// Iterate through all the lights in the scene
			for (int currentLightIndex = 1; currentLightIndex <= numberOfLights; currentLightIndex++) {
				PointLight currentLightObject = LightArray[currentLightIndex];
				// Draw a Cylinder with a 5 Unit radius
				if (getDistance(position,currentLightObject.position) <= 5.0f){
					float distance = getDistance(position,cameraPosition);
					// Set the Color of that column to the color of the lamp
					screen[currentColumn].color.r = (currentLightObject.color.r);
					screen[currentColumn].color.g = (currentLightObject.color.g);
					screen[currentColumn].color.b = (currentLightObject.color.b);
					screen[currentColumn].amountOfRaySteps = stepCount;
					return 1;
				}
			}
			
			/* ---- Check if a Line has been hit ---- */
			int intersectedLineIndex = checkIfAnyLinesIntersect(previousPosition,position);
			Line* intersectedLineObject = &LineArray[intersectedLineIndex];
			// If a Line has been hit, draw it
			
			if (intersectedLineIndex) {
				// Get Intersection Point of Ray and Line
				Line RayLine(cameraPosition,position);
				Point intersectionPoint = getIntersectionPoint(RayLine,*intersectedLineObject);
				
				// Reset Position of the Ray to it's previous position
				// (to avoid intersecting with the same line twice when calculating the lighting)
				position = previousPosition;
				
				// Get distance to camera
				screen[currentColumn].amountOfRaySteps = stepCount;//getDistance(position, cameraPosition);
				
				// Light Distance Shading
				if (intersectedLineObject->emissive) {
					screen[currentColumn].color.r = intersectedLineObject->color.r;
					screen[currentColumn].color.g = intersectedLineObject->color.g;
					screen[currentColumn].color.b = intersectedLineObject->color.b;
				} else {
					screen[currentColumn].color = updateColorBasedOnLocation(position, intersectedLineObject->color);
				}
				return 1;
			} else {
				// Insert Floor and ceiling lighting here
				// Calculate the Floorlight and add it to the FloorLightArray
				FloorLightArray[stepCount + currentColumn*(WINDOW_HEIGHT_HALF)] = updateColorBasedOnLocation(position, floorColor);
				//CeilingLightArray[stepCount + currentColumn*(WINDOW_HEIGHT_HALF)] = skyLight; //updateColorBasedOnLocation(position, floorColor);
				return 0;
			}
			
		}
		
		// Used to calculate the lighting of the specified position
		Color updateColorBasedOnLocation(Point position, Color baseColor) {
			Color resultingColor = black;
			// Check if we hit anything on our way to a lightsource
			// TODO: Could theoretically be threaded?
			for (int currentLightIndex = 1; currentLightIndex <= numberOfLights; currentLightIndex++) {
				// Get current Light
				PointLight currentLightObject = LightArray[currentLightIndex];
				int lineCoveringLight = checkIfAnyLinesIntersect(position,currentLightObject.position);
				// If no line obstructs the lightsource, add it's value to the screen
				if (!lineCoveringLight) {
					// Scale brightness acording to distance
					float distanceToLight = getDistance(position,currentLightObject.position);
					if (distanceToLight <= currentLightObject.distance) {
						float normalizedLight = (1.0-(distanceToLight/currentLightObject.distance)) * currentLightObject.brightness;
						normalizedLight = normalizedLight*normalizedLight;
						resultingColor.r += (currentLightObject.color.r * baseColor.r * normalizedLight);
						resultingColor.g += (currentLightObject.color.g * baseColor.g * normalizedLight);
						resultingColor.b += (currentLightObject.color.b * baseColor.b * normalizedLight);
					}
				}
			}
			
			// Distance Based Shade	
			float normalizedShade = 1.0f-((float)stepCount/(float)(WINDOW_HEIGHT_HALF));
			resultingColor.r *= normalizedShade;
			resultingColor.g *= normalizedShade;
			resultingColor.b *= normalizedShade;
			
			// Clamp results from 0.0f to 1.0f
			resultingColor.r = clamp(resultingColor.r, 0.0f, 1.0f);
			resultingColor.g = clamp(resultingColor.g, 0.0f, 1.0f); 
			resultingColor.b = clamp(resultingColor.b, 0.0f, 1.0f);
			return resultingColor;
		}
};


// Raytrace Render Column
void traceColumn(Ray& currentRay) {
	// Send ray out from viewport
	// TODO: Really hacky, could probably get a rework, but eh, whatever
	float nearClipPlaneDistance = 0.0f;
	float nearClipPlaneWidth = fieldOfView/2.0f;
	float centeredRay = (((float)currentRay.currentColumn/(float)WINDOW_WIDTH)-0.5f) * nearClipPlaneWidth;
	float nearClipPlaneHypotenuse = sqrt(pow(nearClipPlaneDistance,2) + pow(centeredRay,2));
	if (currentRay.currentColumn <= WINDOW_WIDTH_HALF) {
		nearClipPlaneHypotenuse*=-1;
	}
	
	Point nearClipPlanePosition = cameraPosition;
	// This *kinda* works??
	nearClipPlanePosition.x += nearClipPlaneHypotenuse*cos(degreeToRadian(cameraRotation));
	nearClipPlanePosition.y += nearClipPlaneHypotenuse*sin(degreeToRadian(cameraRotation))*-1;
	
	// Send out a Ray from the camera
	currentRay.position = nearClipPlanePosition;
	// This is where the FoV magically appears!
	currentRay.direction = (cameraRotation + ((fieldOfView/2)*-1)) + (fieldOfView/WINDOW_WIDTH*currentRay.currentColumn);

	currentRay.setInitialRayStepSize(initialRayStepSize);
	
	bool finishedLine = false;
	for (int currentRenderStep = 0; currentRenderStep <= WINDOW_HEIGHT_HALF+1; currentRenderStep++) {
		//while(RayArray[1].bounces < numberOfBounces) {
		Point previousPosition = currentRay.position;
		currentRay.stepCount = currentRenderStep;
		finishedLine = currentRay.step();

		// Stop rendering if we hit a wall
		if (finishedLine) {
			break;
		} else {
			// If we never hit a wall, infinite distance
			if (currentRenderStep == WINDOW_HEIGHT_HALF+1) {
				screen[currentRay.currentColumn].amountOfRaySteps = WINDOW_HEIGHT_HALF;// horizonDistance;
				break;
			}
		}
	}
}

// Trace the assigned render sector
int traceRenderSector(Ray& currentRay) {
	int start = currentRay.currentColumn;
	int finish = currentRay.currentColumn + (WINDOW_WIDTH/numberOfRenderSectors);
	while (currentRay.currentColumn < finish) {
		screen[currentRay.currentColumn].color.r = 0.0f;
		screen[currentRay.currentColumn].color.g = 0.0f;
		screen[currentRay.currentColumn].color.b = 0.0f;
		traceColumn(currentRay);
		currentRay.currentColumn++;
	}
	return 0;
}

/* ---- DISPLAYING ---- */
// Load Color as SDL Render Color
int loadColor(const Color& color) {
	SDL_SetRenderDrawColor(
		renderer,
		(int)(clamp(color.r,0.0f,1.0f)*255),
		(int)(clamp(color.g,0.0f,1.0f)*255),
		(int)(clamp(color.b,0.0f,1.0f)*255),
		255
	);	
	return 0;
}

void renderPixel(uint8_t *pixel, int x, int y, Color &color) {
	pixel = (uint8_t *)texture_pixels + y * texture_pitch + x * 4;
	pixel[0] = (uint8_t)(color.r*255);
	pixel[1] = (uint8_t)(color.g*255);
	pixel[2] = (uint8_t)(color.b*255);
	pixel[3] = 0xFF;
}

// Render Character to Screen Position
void renderCharacter(uint8_t *pixel, int startX, int startY, Color &frontColor, Color &backColor, char c) {
	for (int y = 0; y <= renderFontHeight; y++) {
		unsigned char renderFontRow = renderFontArray[y + c*renderFontHeight];
		
		for (int x = 0; x <= renderFontWidth; x++) {
			renderFontRow = renderFontRow << 1;
			char resultPixel = renderFontRow & 0x80;
			// Render Pixel if 1
			if (resultPixel) {
				renderPixel(pixel,startX+x,startY+y,frontColor);
			} else {
				renderPixel(pixel,startX+x,startY+y,backColor);
			}
		}
	}
	//printf("\n");
}

// Render String to Screen Position
void renderText(uint8_t *pixel, int startX, int startY, Color &frontColor, Color &backColor, string input) {
	for (int c = 0; c < input.length(); c++) {
		renderCharacter(pixel, renderFontWidth*c+startX, startY, frontColor, backColor, input.at(c));
	}		
}

// Update Display
void updateScreen() {
	int sliceSize;
	uint8_t *pixel;
	int newFrameTime;
	// Could probably also be given to multiple threads
	while(running) {
		// Start new frame
		//SDL_SetRenderDrawColor(renderer, 255, 0, 255, 0);
		//SDL_RenderClear(renderer);
		newFrameTime = SDL_GetTicks();
		if (frameDone && (lastFrameTime+TICKS_FOR_NEXT_FRAME <= newFrameTime)) {
			SDL_LockTexture(texture, NULL, &texture_pixels, &texture_pitch);
			// Render current Column
			// TODO: For some reason there's this stray pixel on the side?? Pls fix!
			for (int currentColumn = 1; currentColumn < WINDOW_WIDTH; currentColumn++) {
				ScreenColumn* currentScreenColumn = &screen[currentColumn];
				Color* renderColor = &currentScreenColumn->color;
				
				// Calculate Size of column
				sliceSize = currentScreenColumn->amountOfRaySteps;
				
				// Render Column
				for (int currentRow = sliceSize; currentRow <= WINDOW_HEIGHT-sliceSize; currentRow++) {
					// Render Wall
					pixel = (uint8_t *)texture_pixels + currentRow * texture_pitch + (WINDOW_WIDTH-currentColumn) * 4;
					pixel[0] = (uint8_t)(renderColor->r*255);
					pixel[1] = (uint8_t)(renderColor->g*255);
					pixel[2] = (uint8_t)(renderColor->b*255);
					pixel[3] = 0xFF;
				}
				
				// Render Floor and Ceiling
				for (int floorRow = 0; floorRow < sliceSize; floorRow++) {
					Color* pointColor = &FloorLightArray[floorRow + currentColumn*(WINDOW_HEIGHT_HALF)];
					
					pixel = (uint8_t *)texture_pixels + (WINDOW_HEIGHT-floorRow) * texture_pitch + (WINDOW_WIDTH-currentColumn) * 4;
					
					// Render Floor
					pixel[0] = (uint8_t)(pointColor->r*255);
					pixel[1] = (uint8_t)(pointColor->g*255);
					pixel[2] = (uint8_t)(pointColor->b*255);
					pixel[3] = 0xFF;
				}
				
				for (int ceilingRow = 0; ceilingRow < sliceSize; ceilingRow++) {
					Color* pointColor = &FloorLightArray[ceilingRow + currentColumn*(WINDOW_HEIGHT_HALF)];
					
					pixel = (uint8_t *)texture_pixels + (WINDOW_HEIGHT-ceilingRow) * texture_pitch + (WINDOW_WIDTH-currentColumn) * 4;
					// Render Ceiling
					pixel = (uint8_t *)texture_pixels + ceilingRow * texture_pitch + (WINDOW_WIDTH-currentColumn) * 4;
					pixel[0] = (uint8_t)(pointColor->r*255);
					pixel[1] = (uint8_t)(pointColor->g*255);
					pixel[2] = (uint8_t)(pointColor->b*255);
					pixel[3] = 0xFF;
			}
			string millisecondString = to_string(newFrameTime-lastFrameTime) + "ms";
			renderText(pixel,renderFontWidth,renderFontHeight, white, black, millisecondString);
			//renderCharacter(pixel,0,0,white,'A');
			SDL_UnlockTexture(texture);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			
			// Weirdly bent for some reason,
				}
			// but I guess that's just down to how I shoot my rays
			/*
			for (int currentColumn = 0; currentColumn < WINDOW_WIDTH ; currentColumn++) {
				sliceSize = (WINDOW_HEIGHT/6) * (log((screen[currentColumn].distance / horizonDistance)));
				Color renderColor = screen[currentColumn].color;
				loadColor(renderColor);
				SDL_RenderDrawLine(
					renderer,
					WINDOW_WIDTH-currentColumn,
					WINDOW_HEIGHT_HALF-sliceSize,
					WINDOW_WIDTH-currentColumn,
					WINDOW_HEIGHT_HALF+sliceSize
				);
			}
			*/
			SDL_RenderPresent(renderer);
			//DrawFPS();
			lastFrameTime = SDL_GetTicks();
			//SDL_RenderPresent(renderer);
			//clearScreenBuffer();
			//SDL_RenderPresent(renderer);
			frameDone = false;
		} else {
			SDL_Delay(1);
		}
	}
}

/* --- MAIN ---- */
// Ye olden Main function
int WinMain(int argc, char **argv) {
	printf("Hello, World!\n");
	//srand(time(NULL)); 
	
    //SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow(
		"Ray",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE // | SDL_WINDOW_FULLSCREEN_DESKTOP
	);
    CHECK_ERROR(window == NULL, SDL_GetError());
	
    renderer = SDL_CreateRenderer(
		window,
		-1,
		SDL_RENDERER_ACCELERATED
	); 
	CHECK_ERROR(renderer == NULL, SDL_GetError());
	
	// Clearing of Screen and creating of Texture
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
	texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING,
		WINDOW_WIDTH,
		WINDOW_HEIGHT
	);
			
	// Color Setting
	skyLight.r = 0.1f;
	skyLight.g = 0.3f;
	skyLight.b = 0.5f;
	
	floorColor.r = 1.0f;
	floorColor.g = 1.0f;
	floorColor.b = 1.0f;
	
	white.r = 1.0f;
	white.g = 1.0f;
	white.b = 1.0f;
	black.r = 0.0f;
	black.g = 0.0f;
	black.b = 0.0f;
	red.r = 1.0f;
	red.g = 0.0f;
	red.b = 0.0f;
	green.r = 0.0f;
	green.g = 1.0f;
	green.b = 0.0f;
	blue.r = 0.0f;
	blue.g = 0.0f;
	blue.b = 1.0f;
	
	// Array Init
	RayArray = (struct Ray *)calloc(numberOfRays+1, sizeof(struct Ray));
	LineArray = (struct Line *)calloc(numberOfLines+1, sizeof(struct Line));
	LightArray = (struct PointLight *)calloc(numberOfLights+1, sizeof(struct PointLight));
	FloorLightArray = (struct Color *)calloc(WINDOW_WIDTH*(WINDOW_HEIGHT_HALF)+1, sizeof(struct Color));
	CeilingLightArray = (struct Color *)calloc(WINDOW_WIDTH*(WINDOW_HEIGHT_HALF)+1, sizeof(struct Color));
	StepSizeDistanceArray = (float *)calloc(WINDOW_HEIGHT_HALF, sizeof(float));
	
	// Precalc of Steps
	for (int initStepCount = 0; initStepCount < WINDOW_HEIGHT_HALF; initStepCount++) {
		StepSizeDistanceArray[initStepCount] = (int)(pow((float)initStepCount/(float)(WINDOW_HEIGHT_HALF),2) * horizonDistance);
	}
	
	// Camera Origin
	cameraPosition.x = 640/2;
	cameraPosition.y = 480/3*2-5;
	cameraRotation = 180.0f;
	for (int i = 1; i <= numberOfRays; i++) {
		RayArray[i] = *new Ray();
		RayArray[i].currentColumn = i-1;
	}

	
	// Scene is loaded here
	LineArray[1] = *new Line(0	,0	,640*2,0	,1.0, 0.0, 0.0);
	LineArray[2] = *new Line(640*2,480,640*2,0	,0.0, 1.0, 0.0);
	LineArray[2].emissive = true;
	LineArray[3] = *new Line(0	,480,640*2,480,0.0, 0.0, 1.0);
	LineArray[4] = *new Line(0 	,480,0	,0	,1.0, 1.0, 1.0);
	
	// House
	LineArray[5] = *new Line(100,180,400,180,1.0, 1.0, 1.0);
	LineArray[6] = *new Line(200,180,200,280,1.0, 1.0, 1.0);
	LineArray[7] = *new Line(200,280,300,280,1.0, 1.0, 1.0);
	LineArray[8] = *new Line(400,180,400,480,1.0, 1.0, 1.0);
	
	// House
	LineArray[9] =  *new Line(660,200,680,210,1.0, 1.0, 1.0);
	LineArray[10] = *new Line(680,210,690,230,1.0, 1.0, 1.0);
	LineArray[11] = *new Line(690,230,680,250,1.0, 1.0, 1.0);
	LineArray[12] = *new Line(680,250,660,260,1.0, 1.0, 1.0);
	LineArray[13] = *new Line(660,260,640,250,1.0, 1.0, 1.0);
	LineArray[14] = *new Line(640,250,630,230,1.0, 1.0, 1.0);
	LineArray[15] = *new Line(630,230,640,210,1.0, 1.0, 1.0);
	LineArray[16] = *new Line(640,210,660,200,1.0, 1.0, 1.0);
	
	// Lines
	/*LineArray[1] = *new Line(0					,0					,WINDOW_WIDTH		,0					,0.5, 0.5, 0.5	);
	LineArray[2] = *new Line(WINDOW_WIDTH		,WINDOW_HEIGHT		,WINDOW_WIDTH		,0					,0.5, 0.5, 0.5	);
	LineArray[3] = *new Line(WINDOW_WIDTH		,WINDOW_HEIGHT		,0					,WINDOW_HEIGHT		,0.5, 0.5, 0.5	);
	LineArray[4] = *new Line(0 					,WINDOW_HEIGHT		,0					,0					,0.5, 0.5, 0.5	);
	
	// Smaller Room
	LineArray[5] = *new Line(WINDOW_WIDTH/3		,WINDOW_HEIGHT/3	,WINDOW_WIDTH/9*4	,WINDOW_HEIGHT/3	,1.0, 0.0, 0.0	);
	LineArray[6] = *new Line(WINDOW_WIDTH/9*5	,WINDOW_HEIGHT/3	,WINDOW_WIDTH/3*2	,WINDOW_HEIGHT/3	,1.0, 1.0, 0.0	);
	
	LineArray[7] = *new Line(WINDOW_WIDTH/3*2	,WINDOW_HEIGHT/3*2	,WINDOW_WIDTH/3*2	,WINDOW_HEIGHT/3	,0.0, 1.0, 0.0	);
	LineArray[8] = *new Line(WINDOW_WIDTH/3*2	,WINDOW_HEIGHT/3*2	,WINDOW_WIDTH/3		,WINDOW_HEIGHT/3*2	,0.0, 0.0, 1.0	);
	LineArray[9] = *new Line(WINDOW_WIDTH/3		,WINDOW_HEIGHT/3*2	,WINDOW_WIDTH/3		,WINDOW_HEIGHT/3	,1.0, 1.0, 1.0	);
	*/
	// Lights
	//LightArray[1] = *new PointLight(WINDOW_WIDTH-5	, 300,1.0	,1.0	,1.0	,1.0f	,128.0f	);
	
	LightArray[1] = *new PointLight(640/2		, 480/2	,1.0	,1.0	,1.0	,1.0f	,512.0f	);
	LightArray[2] = *new PointLight(640/5		, 480/5	,1.0	,1.0	,1.0	,1.0f	,512.0f	);
	LightArray[3] = *new PointLight(770	, 110	,1.0	,0.0	,0.0	,1.0f	,512.0f	);
	LightArray[4] = *new PointLight(500	, 260	,0.0	,1.0	,0.0	,1.0f	,512.0f	);
	LightArray[5] = *new PointLight(760	, 350	,0.0	,0.0	,1.0	,1.0f	,512.0f	);
	LightArray[6] = *new PointLight(10		, 10	,1.0	,1.0	,1.0	,1.0f	,512.0f	);
	LightArray[7] = *new PointLight(640*2+32,480/2,1.0	,1.0	,1.0	,1.0f	,512.0f	);
	//LightArray[3] = *new PointLight(WINDOW_WIDTH/5*4	, WINDOW_HEIGHT/3*2	,1.0	,1.0	,1.0	,1.0f	,512.0f	);
	/*LightArray[2] = *new PointLight(WINDOW_WIDTH/4*4	, WINDOW_HEIGHT/4,1.0	,1.0	,1.0	,1.0f	,512.0f	);
	LightArray[3] = *new PointLight(WINDOW_WIDTH/4	, WINDOW_HEIGHT/4*4,1.0	,1.0	,1.0	,1.0f	,512.0f	);
	LightArray[4] = *new PointLight(WINDOW_WIDTH/4*4	, WINDOW_HEIGHT/4*4,1.0	,1.0	,1.0	,1.0f	,512.0f	);
	*/
	/*
	// Light Position
	LightArray[1] = *new PointLight(WINDOW_WIDTH/3	,WINDOW_HEIGHT_HALF,1.0	,1.0	,1.0	,1.0f	,128.0f	);
	LightArray[2] = *new PointLight(WINDOW_WIDTH/3*2,20.0f			,1.0	,1.0	,1.0	,2.0f	,512.0f	);
	
	//RayArray[1] = *new Ray(WINDOW_WIDTH/10,0,45);
	LineArray[1] = *new Line(0				,WINDOW_HEIGHT-200	,80				, WINDOW_HEIGHT_HALF	,1.0, 0, 0	);
	LineArray[2] = *new Line(80				,WINDOW_HEIGHT_HALF	,150			, WINDOW_HEIGHT-30	,1.0,1.0,0	);
	LineArray[3] = *new Line(150			,WINDOW_HEIGHT-30	,WINDOW_WIDTH_HALF	, WINDOW_HEIGHT-100	,0	,1.0,0	);
	LineArray[4] = *new Line(WINDOW_WIDTH_HALF	,WINDOW_HEIGHT-100	,300			, WINDOW_HEIGHT-200	,0	,1.0,1.0);
	LineArray[5] = *new Line(300			,WINDOW_HEIGHT-200	,WINDOW_WIDTH	, WINDOW_HEIGHT		,0	,0	,1.0);
	
	Point corner1;
	corner1.x = WINDOW_WIDTH/3;
	corner1.y = 160;
	Point corner2;
	corner2.x = WINDOW_WIDTH/3+80;
	corner2.y = 180;
	Point corner3;
	corner3.x = WINDOW_WIDTH/4;
	corner3.y = 300;
	Point corner4;
	corner4.x = WINDOW_WIDTH/3+70;
	corner4.y = 100;
	
	Color segment1;
	segment1.r = 1.0;
	segment1.g = 0;
	segment1.b = 1.0;
	
	LineArray[6] = *new Line(corner1,corner3,segment1);
	LineArray[7] = *new Line(corner2,corner3,segment1);
	LineArray[8] = *new Line(corner2,corner4,segment1);
	*/
	
	// Threadpools
	std::vector<std::thread> raytraceThreadPool;
	std::vector<std::thread> renderThreadPool;
	std::thread gameplayThread(updateInputs);
	
	renderThreadPool.emplace_back(updateScreen);
	
    while (running) {		
		if (topDown) {
			// Render Lights
			for (int i = 1; i <= numberOfLights; i++) {
				SDL_SetRenderDrawColor(renderer, LightArray[i].color.r*255, LightArray[i].color.g*255, LightArray[i].color.b*255, 255);
				SDL_RenderDrawPoint(renderer, (int)LightArray[i].position.x, (int)LightArray[i].position.y);
			}
			
			
			// Render Lines
			for (int i = 1; i <= numberOfLines; i++) {
				SDL_SetRenderDrawColor(renderer, LineArray[i].color.r*255, LineArray[i].color.g*255, LineArray[i].color.b*255, 255);
				SDL_RenderDrawLine(renderer, (int)LineArray[i].p1.x, (int)LineArray[i].p1.y, (int)LineArray[i].p2.x, (int)LineArray[i].p2.y);
			}
		}
		
		// Raytrace
		// Only trace new frames if last frame is done drawing
		if (!frameDone) {
			for (int currentRenderSector = 0; currentRenderSector < numberOfRenderSectors; currentRenderSector++) {
				//printf("%d,", currentColumn);
				Ray& currentRay = RayArray[currentRenderSector];
				currentRay.currentColumn = (WINDOW_WIDTH/numberOfRenderSectors)*currentRenderSector;
				raytraceThreadPool.emplace_back([&currentRay]() { traceRenderSector(currentRay); });
				//traceColumn(currentRay);
			}
			
			// The magic of threading
			for (auto& t : raytraceThreadPool) {
				if (t.joinable()) {
					t.join();
				}
			}
			frameDone = true;
		}
		
		// Update Screen handled by Render Thread
		//updateScreen();
	
        /*! updates the array of keystates */
		//running = false;
		while ((SDL_PollEvent(&event)) != 0)
		{
			/*! request quit */
			if (event.type == SDL_QUIT) 
			{ 
				running = false;
			}
		}
		
		// Keystrokes are processed in keystroke thread
		//processControls();
    }
	end:
	free(RayArray);
	free(LineArray);
	free(LightArray);
	free(FloorLightArray);
	free(CeilingLightArray);
	free(StepSizeDistanceArray);
	
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
	return 0;
}