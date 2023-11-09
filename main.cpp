#include <math.h>
#include <SDL2/SDL.h>
#include <thread>
#include <vector>
#include <string>

using std::string;
using std::to_string;

// Utility macros
#define CHECK_ERROR(test, message) \
    do { \
        if((test)) { \
            fprintf(stderr, "%s\n", (message)); \
            exit(1); \
        } \
    } while(0)

int WINDOW_WIDTH 		= 640;
int WINDOW_HEIGHT 		= 480;
int WINDOW_WIDTH_HALF 	= WINDOW_WIDTH/2;
int WINDOW_HEIGHT_HALF 	= WINDOW_HEIGHT/2;
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
	float lineCoordinate;
	struct Texture *hitTexture;
	struct Line *hitLine;
};
typedef struct ScreenColumn ScreenColumn;

/* ---- Debug Variables */
bool renderMode = false;
Color skyLight;
Color floorColor;
Point defaultPosition;
bool noclip = true;

/* --- Default Color ---- */
Color white;
Color black;

Color red;
Color green;
Color blue;

Color yellow;
Color magenta;
Color cyan;

/* ---- CLASSES ---- */
// A primitive PointLight
class PointLight {
	public:
		Point position;
		Color color;
		float brightness;
		float distance;
		bool dynamic = false;
		
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
		Texture* texturePointer;
		float textureScale = 1.0f;
		float textureOffsetHorizontal = 0.0f;
		float textureOffsetVertical = 0.0f;
		// If > 0, it's a portal
		int portalIndex = 0;
		
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

// A 2D Image to be projected onto a line, floor or ceiling
class Texture {
	public:
		int width, height;
		Color *TextureData;
		Texture(int _width, int _height) {
			width = _width;
			height = _height;
			TextureData = (struct Color *)calloc(width*height, sizeof(struct Color));
		}
		
		void setTexturePixel(int xPixel, int yPixel, float r, float g, float b) {
			Color color;
			color.r = r;
			color.g = g;
			color.b = b;
			TextureData[xPixel + yPixel*width] = color;
		}
		
		void setTexturePixel(int xPixel, int yPixel, Color _color) {
			TextureData[xPixel + yPixel*width] = _color;
		}
		
		Color getRangedTexturePixel(float xRange, float yRange, Line* line) {
			float f_Width = (float)width;
			float f_Height = (float)height;
			// xRange and yRange come in from 0.0f to 1.0f
			if (line && width && height) {
				float xModifiedPos = f_Width  * (xRange * line->textureScale);
				float yModifiedPos = f_Height * (yRange * line->textureScale);
				
				int xPixel = abs(((int)(xModifiedPos + line->textureOffsetHorizontal)))%width;
				int yPixel = abs(((int)(yModifiedPos + line->textureOffsetVertical  )))%height;
				return TextureData[xPixel + yPixel*width];
			} else {
				return magenta;
			}
		}
};

class Segment {
	
};

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
// Contains the pre-calculated Lighting of the floor and ceiling
struct Color *PreCalculatedLighting;
// Contains the pre-calculated Lighting of the floor and ceiling
struct Texture *TextureArray;

/* -- SDL Pointers --*/
SDL_Event event;
SDL_Renderer *renderer;
SDL_Window *window;
SDL_Texture *texture;
void *texture_pixels;
int texture_pitch;

ScreenColumn *ScreenColumnArray;

// Camera Variables

/* ---- Runtime Variables ---- */
bool frameDone = false;
bool running = true;
int lastFrameTime = 0;

/* ---- Object Limits ---- */
int numberOfLines = 20;
int numberOfLights = 10;
int numberOfBounces = 0;
int numberOfTextures = 10;
int numberOfRenderSectors = 1;
int numberOfRays = numberOfRenderSectors;
// Max Room size;
int maximumWidth;
int maximumHeight;

/* ---- Camera Variables ---- */
Point cameraPosition;
float cameraRotation = 0.0f;
float fieldOfView = 180.0f;
float horizonDistance = 16.0f;
float initialRayStepSize = 0.1f;


/* ---- Player Variables ---- */
float maxSpeed = 10.0f;
float cameraHeight = 0.0f;
float cameraSpeedHorziontal = 0.0f;
float cameraSpeedVertical = 0.0f;
float cameraSpeedRotational = 0.0f;

// We need missing texture here :v
Texture* missingTexture = nullptr;

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
	float a1 = Line1.p2.y - Line1.p1.y;
	float b1 = Line1.p1.x - Line1.p2.x;
	float c1 = a1 * Line1.p1.x + b1 * Line1.p1.y;

	float a2 = Line2.p2.y - Line2.p1.y;
	float b2 = Line2.p1.x - Line2.p2.x;
	float c2 = a2 * Line2.p1.x + b2 * Line2.p1.y;

	float delta = a1 * b2 - a2 * b1;

	if (delta == 0) {
		return defaultPosition;
	}

	Point result;
	result.x = (b2 * c1 - b1 * c2) / delta;
	result.y = (a1 * c2 - a2 * c1) / delta;
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

// Mulitply two colors together
Color colorMultiply(Color& color1, Color& color2) {
	Color resultColor;
	resultColor.r = color1.r * color2.r;
	resultColor.g = color1.g * color2.g;
	resultColor.b = color1.b * color2.b;
	return resultColor;
}

// Add two colors together
Color colorAdd(Color& color1, Color& color2) {
	Color resultColor;
	resultColor.r = color1.r + color2.r;
	resultColor.g = color1.g + color2.g;
	resultColor.b = color1.b + color2.b;
	return resultColor;
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
		
		/*
		// Up
        if (keys[SDL_SCANCODE_SPACE]) {
			cameraHeight += 1.0f;
		}
		
		// Down
        if (keys[SDL_SCANCODE_LSHIFT]) {
			cameraHeight -= 1.0f;
		}
		*/
		
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
	Point previousPosition = cameraPosition;
	while(running) {		
		processControls();
		
		// Update Origin
		cameraPosition.x += cameraSpeedHorziontal;
		cameraPosition.y += cameraSpeedVertical;
		cameraRotation += cameraSpeedRotational;
		limitDegreeAngle(cameraRotation);
		if ((checkIfAnyLinesIntersect(previousPosition,cameraPosition)) && !noclip) {
			cameraPosition = previousPosition;
		}
		
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
		
		LineArray[2].textureOffsetVertical-= 0.50f;
		LineArray[2].textureOffsetHorizontal+= 0.05f;
		
		
		
		previousPosition = cameraPosition;
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


// Used to calculate the lighting of the specified position
Color updateColorBasedOnLocation(Point position, Color baseColor) {
	Color resultingColor = black;
	// Check if we hit anything on our way to a lightsource
	// TODO: Could theoretically be threaded?
	for (int currentLightIndex = 1; currentLightIndex <= numberOfLights; currentLightIndex++) {
		// Get current Light
		PointLight currentLightObject = LightArray[currentLightIndex];
		// TODO: Currently this disables all dynamic lights!!!!
		if (!currentLightObject.dynamic) {
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
	}
	
	// Distance Based Shade	
	/*
	float normalizedShade = 1.0f-((float)stepCount/(float)(WINDOW_HEIGHT_HALF));
	resultingColor.r *= normalizedShade;
	resultingColor.g *= normalizedShade;
	resultingColor.b *= normalizedShade;
	*/
	
	// Clamp results from 0.0f to 1.0f
	resultingColor.r = clamp(resultingColor.r, 0.0f, 1.0f);
	resultingColor.g = clamp(resultingColor.g, 0.0f, 1.0f); 
	resultingColor.b = clamp(resultingColor.b, 0.0f, 1.0f);
	return resultingColor;
}

// Used for reprojecting portal coordinates, and getting texture positions
// Returns scale of 0.0f to 1.0f, where 0.0f = p1, and 1.0f = p2
float mapWorldPointToLineCoordinate(Line& line, Point& intersectionPoint) {
	// Get length of line
	float lineLength = getDistance(line.p1, line.p2);
	// Get distance between p1 and intersectionPoint
	float distanceFromOrigin = getDistance(line.p1, intersectionPoint);
	//printf("%f:%f - %f:%f\n", line.p1.x, line.p1.y, intersectionPoint.x, intersectionPoint.y);
	return distanceFromOrigin/lineLength;
}

Point mapLineCoordinateToWorldPoint(Line& line, float lineCoordinate) {
  Point worldSpacePoint;
  worldSpacePoint.x = line.p1.x + lineCoordinate * (line.p2.x - line.p1.x);
  worldSpacePoint.y = line.p1.y + lineCoordinate * (line.p2.y - line.p1.y);
  return worldSpacePoint;
}


// Figure out which Portal to look out of
Point getPortalPosition(int originPortalLineIndex, int destinationPortalLineIndex, Point& intersectionPoint) {
	//printf("%f\n", contactPointToLineCoordinate(*intersectedLineObject,intersectionPoint));
	if (LineArray[destinationPortalLineIndex].portalIndex) {
		// Figure out where we hit the line locally
		float lineCoordinate = mapWorldPointToLineCoordinate(LineArray[originPortalLineIndex],intersectionPoint);
		Point worldCoorindate = mapLineCoordinateToWorldPoint(LineArray[destinationPortalLineIndex], lineCoordinate);
		//printf("%f -> %f:%f\n", lineCoordinate, worldCoorindate.x, worldCoorindate.y);
		return worldCoorindate;
	}
	return defaultPosition;
}

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
			RayStepSize = StepSizeDistanceArray[stepCount + WINDOW_HEIGHT_HALF*currentColumn];//pow((float)stepCount/(float)(WINDOW_HEIGHT_HALF),2) * horizonDistance;
			//printf("%f\n",RayStepSize);
			Point previousPosition = position;
			float rad = degreeToRadian(direction);
			position.x += (RayStepSize*sin(rad));
			position.y += (RayStepSize*cos(rad));
			//(getDistance(position,cameraPosition)/horizonDistance) * WINDOW_HEIGHT/6;
			//printf("%f\n",RayStepSize);
			
			// Draws those cool light columns
			// Iterate through all the lights in the scene
			/*
			for (int currentLightIndex = 1; currentLightIndex <= numberOfLights; currentLightIndex++) {
				PointLight currentLightObject = LightArray[currentLightIndex];
				// Draw a Cylinder with a 5 Unit radius
				if (getDistance(position,currentLightObject.position) <= 5.0f){
					float distance = getDistance(position,cameraPosition);
					// Set the Color of that column to the color of the lamp
					ScreenColumnArray[currentColumn].color.r = (currentLightObject.color.r);
					ScreenColumnArray[currentColumn].color.g = (currentLightObject.color.g);
					ScreenColumnArray[currentColumn].color.b = (currentLightObject.color.b);
					ScreenColumnArray[currentColumn].amountOfRaySteps = stepCount;        
				    // idk why this crashes it :p
					// probably because it's missing a line to map the texture to!
					//ScreenColumnArray[currentColumn].hitTexture = &missingTexture;
					return 1;
				}
			}
			*/			
			
			/* ---- Check if a Line has been hit ---- */
			int intersectedLineIndex = checkIfAnyLinesIntersect(previousPosition,position);
			Line* intersectedLineObject = &LineArray[intersectedLineIndex];
			// If a Line has been hit, draw it
			
			if (intersectedLineIndex) {
				// Get Intersection Point of Ray and Line
				Line RayLine(cameraPosition,position);
				Point intersectionPoint = getIntersectionPoint(RayLine,*intersectedLineObject);
				//printf("%f,%f\n", intersectionPoint.x, intersectionPoint.y);
				//running = false;
				
				// Portal check
				if (intersectedLineObject->portalIndex) {
					position = getPortalPosition(
						intersectedLineIndex,
						intersectedLineObject->portalIndex,
						intersectionPoint
					);
					return 0;
				} else {
					// Reset Position of the Ray to it's previous position
					// (to avoid intersecting with the same line twice when calculating the lighting)
					position = previousPosition;
				}
				
				// Get Line Pointer
				ScreenColumnArray[currentColumn].hitLine = intersectedLineObject;				
				// Get distance to camera
				ScreenColumnArray[currentColumn].amountOfRaySteps = stepCount;
				// Get texture of hit line
                // Check if a texture is present
                if (intersectedLineObject->texturePointer) {
                    // If you find one, good on you
				    ScreenColumnArray[currentColumn].hitTexture = intersectedLineObject->texturePointer;
                } else {
                    // If you don't, that sucks!                    
				    ScreenColumnArray[currentColumn].hitTexture = missingTexture;
				    intersectedLineObject->emissive = true;
				    intersectedLineObject->color = white;
                }
				
				// Light Distance Shading
				if (intersectedLineObject->emissive) {
					ScreenColumnArray[currentColumn].color.r = intersectedLineObject->color.r;
					ScreenColumnArray[currentColumn].color.g = intersectedLineObject->color.g;
					ScreenColumnArray[currentColumn].color.b = intersectedLineObject->color.b;
				} else {
					// If the line has a texture, ignore color
					if (intersectedLineObject->texturePointer) {
						ScreenColumnArray[currentColumn].color =
							updateColorBasedOnLocation(position, white);
					} else {
						// If the line has no texture, use it's color
						ScreenColumnArray[currentColumn].color =
							updateColorBasedOnLocation(position, intersectedLineObject->color);
					}
				}
				ScreenColumnArray[currentColumn].lineCoordinate =
					mapWorldPointToLineCoordinate(*intersectedLineObject, intersectionPoint);
				return 1;
			} else {
				// Maybe bake the lighting, except "dynamic" lights-?
				// Insert Floor and ceiling lighting here
				// Calculate the Floorlight and add it to the FloorLightArray
				int xPos = abs(((int)position.x)%maximumWidth );
				int yPos = abs(((int)position.y)%maximumHeight);
				FloorLightArray[stepCount + currentColumn*(WINDOW_HEIGHT_HALF)] =
					PreCalculatedLighting[xPos + yPos*maximumWidth];
				//CeilingLightArray[stepCount + currentColumn*(WINDOW_HEIGHT_HALF)] = skyLight; //updateColorBasedOnLocation(position, floorColor);
				return 0;
			}
			
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
    // TODO: Get this from the precalculated ray step positions
	currentRay.direction = cameraRotation+(((((float)currentRay.currentColumn)/((float)WINDOW_WIDTH))-0.5f)*fieldOfView); // (cameraRotation + ((fieldOfView/2)*-1)) + (fieldOfView/WINDOW_WIDTH*currentRay.currentColumn);

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
				ScreenColumnArray[currentRay.currentColumn].amountOfRaySteps = WINDOW_HEIGHT_HALF;// horizonDistance;
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
		ScreenColumnArray[currentRay.currentColumn].color.r = 0.0f;
		ScreenColumnArray[currentRay.currentColumn].color.g = 0.0f;
		ScreenColumnArray[currentRay.currentColumn].color.b = 0.0f;
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
	x = x % WINDOW_WIDTH;
	y = y % WINDOW_HEIGHT;
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
	Color pixelColor;
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
				ScreenColumn* currentScreenColumn = &ScreenColumnArray[currentColumn];
				
				// Calculate Size of column
				sliceSize = currentScreenColumn->amountOfRaySteps;
				
				// Render Column
				for (int currentRow = 0; currentRow <= WINDOW_HEIGHT; currentRow++) {
					if (currentRow < sliceSize) {
						// Draw Ceiling
						pixelColor = FloorLightArray[currentRow + currentColumn*(WINDOW_HEIGHT_HALF)];
						renderPixel(pixel, WINDOW_WIDTH-currentColumn, currentRow, pixelColor);
					} else {
						// Render Wall
						if (currentRow <= WINDOW_HEIGHT-sliceSize) {
							// If there is a texture, draw it
							if (currentScreenColumn->hitTexture) {
								int wallBeginning = sliceSize;
								int wallEnd = WINDOW_HEIGHT-sliceSize;
								int wallTotal = wallEnd - wallBeginning;
								int adjustedCurrentRow = currentRow - wallBeginning;
								
								float rowCoorinate = ((float)adjustedCurrentRow/(float)wallTotal);
								Color textureColor = currentScreenColumn->hitTexture->getRangedTexturePixel(
									currentScreenColumn->lineCoordinate,
									rowCoorinate,
									currentScreenColumn->hitLine
								);
								pixelColor = colorMultiply(textureColor, currentScreenColumn->color); //colorAdd(textureColor, renderColor);
							} else {
								pixelColor = currentScreenColumn->color;
							}
							renderPixel(pixel, WINDOW_WIDTH-currentColumn, currentRow, pixelColor);
						} else {
							// Render Floor
							pixelColor = FloorLightArray[(WINDOW_HEIGHT-currentRow) + currentColumn*(WINDOW_HEIGHT_HALF)];
							renderPixel(pixel, WINDOW_WIDTH-currentColumn, currentRow, pixelColor);
						}
					}
				}
			}
			// Print fps text 
			string dateTimeString = "Built on " + (string)__TIMESTAMP__;
			renderText(pixel,renderFontWidth,renderFontHeight, white, black, dateTimeString);
			
			int msCalc = newFrameTime-lastFrameTime;
			
			// Print fps text 
			string fpsString = to_string(1000/(msCalc)) + "/60fps";
			renderText(pixel,renderFontWidth,renderFontHeight*2, white, black, fpsString);
			
			// Print ms text
			string millisecondString = to_string(msCalc) + "/16ms";
			if (msCalc <= 16) {
				renderText(pixel,renderFontWidth,renderFontHeight*3, green, black, millisecondString);
			} else if ((msCalc > 16) && (msCalc <= 33)) {
				renderText(pixel,renderFontWidth,renderFontHeight*3, yellow, black, millisecondString);
			} else {
				renderText(pixel,renderFontWidth,renderFontHeight*3, red, black, millisecondString);
			}
			
			// Finish frame
			SDL_UnlockTexture(texture);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
			
			// Prep next frame
			lastFrameTime = SDL_GetTicks();
			frameDone = false;
		} else {
			SDL_Delay(1);
		}
	}
}

// Used to precalculate the distance between ray steps
void preCalculateStepArray() {
	printf("PreCalculating Steps...\n");
	for (int initStepColumn = 0 ; initStepColumn < WINDOW_WIDTH; initStepColumn++) {
		//printf("Column: %d/%d\n", initStepColumn, WINDOW_WIDTH);
		for (int initStepCount = 0; initStepCount < WINDOW_HEIGHT_HALF; initStepCount++) {		
			StepSizeDistanceArray[initStepCount + initStepColumn*WINDOW_HEIGHT_HALF] = 
				(int)(((float)initStepCount/(float)(WINDOW_HEIGHT_HALF)) * horizonDistance);
		}
	}
	/*
	for (int initStepColumn = 0 ; initStepColumn < WINDOW_WIDTH; initStepColumn++) {
		// Keep step location consistent.
		for (int initStepCount = 0; initStepCount < WINDOW_HEIGHT_HALF; initStepCount++) {		
			// Goes from 0.0f - 1.0f, aka closest to furthest pixel
			float stepCountScale = ((float)initStepCount/(float)(WINDOW_HEIGHT_HALF));
			// Far clip plane
			float farClipPlaneDistance = pow(stepCountScale,2) * horizonDistance;
			float farClipPlaneWidth = fieldOfView/2;
			float centeredRay = (((float)initStepColumn/(float)WINDOW_WIDTH)-0.5f) * farClipPlaneWidth;
			float farClipPlaneHypotenuse = sqrt(pow(farClipPlaneDistance,2) + pow(centeredRay,2));	
			StepSizeDistanceArray[initStepCount + initStepColumn*WINDOW_HEIGHT_HALF] = farClipPlaneHypotenuse;
		}
	}
	*/
}

void precalculateLighting() {
	// Determine Bounds
	printf("Determine Level Bounds...\n");
	float minX, minY = 0.0f;
	float maxX, maxY = 0.0f;
	for (int lineIndex = 1; lineIndex <= numberOfLines; lineIndex++) {
		Line* currentLine = &LineArray[lineIndex];
		// P1
		Point currentPoint = currentLine->p1;
			if (currentPoint.x < minX) {
				minX = currentPoint.x;
			}
			if (currentPoint.y < minY) {
				minY = currentPoint.y;
			}
			
			if (currentPoint.x > maxX) {
				maxX = currentPoint.x;
			}
			if (currentPoint.y > maxY) {
				maxY = currentPoint.y;
			}	
		// P2
		currentPoint = currentLine->p2;
			if (currentPoint.x < minX) {
				minX = currentPoint.x;
			}
			if (currentPoint.y < minY) {
				minY = currentPoint.y;
			}
			
			if (currentPoint.x > maxX) {
				maxX = currentPoint.x;
			}
			if (currentPoint.y > maxY) {
				maxY = currentPoint.y;
			}	
		//printf("%d\tX: %f - %f\n\tY: %f - %f\n", lineIndex, minX, maxX, minY, maxY);
	}
	
	maximumWidth  = (int)(ceil(maxX) - floor(minX));
	maximumHeight = (int)(ceil(maxY) - floor(minY));
	printf("Bounds: %d:%d\n", maximumWidth, maximumHeight);
	
	// Allocate Lighting
	PreCalculatedLighting = (struct Color *)calloc(maximumWidth*maximumHeight, sizeof(struct Color));
	
	// Calculate it!
	Point currentPosition;
	printf("Calculating Lighting...\n");
	for (int y = 0; y < maximumHeight; y++) {
		for (int x = 0; x < maximumWidth; x++) {
			currentPosition.x = (float)x;
			currentPosition.y = (float)y;
			PreCalculatedLighting[x + y*maximumWidth] =
				updateColorBasedOnLocation(currentPosition, floorColor);
		}
	}
	printf("Finished!\n");
}

Texture importNetpbm(string path) {
	FILE *filePointer;
	int width, heigth;
	Color color;
	filePointer = fopen(path.c_str(),"rb");
	if (filePointer == NULL) {
		printf("Missing Texture! No file at %s\n", path.c_str());
		return *missingTexture;
	} else {
		// Read in image info
		Texture *newTexture;
		unsigned char currentByte;
		short int stateCounter = 0;
		int width, height, bitdepth;
		printf("Loading Texture from File: %s\n", path.c_str());
		/*
			0: Reading Filetype
			1: Reading Width
			2: Reading Height
			3: Reading Bit-depth
			4: Create Texture
			5: Read Data
			6: Finished
		*/
		while (stateCounter < 6) {
			printf("%d: %c\n", stateCounter, currentByte);
			switch(stateCounter) {
				case 0: // 0: Reading Filetype
					currentByte = fgetc(filePointer);
					if (currentByte==0x0A) {
						stateCounter++;
					}
					break;
				case 1: // 1: Reading Width
					if (currentByte==0x20) {
						stateCounter++;
					} else {
						fscanf(filePointer, "%d", &width);
						currentByte = fgetc(filePointer);
					}
					break;
				case 2: // 1: Reading Height
					if (currentByte==0x0A) {
						stateCounter++;
						currentByte = fgetc(filePointer);
					} else {
						fscanf(filePointer, "%d", &height);
						currentByte = fgetc(filePointer);
					}
					break;
				case 3: // 0: Reading Bitdepth
					if (currentByte==0x0A) {
						stateCounter++;
					} else {
						// A bit of a hack to avoid losing the first character
						fseek(filePointer, -1, SEEK_CUR);
						fscanf(filePointer, "%d", &bitdepth);
						currentByte = fgetc(filePointer);
					}
					break;
				case 4:
					newTexture = new Texture(width,height);
					stateCounter++;
					break;
				case 5:
					for (int y = 0; y < height; y++) {
						for (int x = 0; x < width; x++) {
							currentByte = fgetc(filePointer);
							color.r = ((float)currentByte/(float)bitdepth);
							currentByte = fgetc(filePointer);
							color.g = ((float)currentByte/(float)bitdepth);
							currentByte = fgetc(filePointer);
							color.b = ((float)currentByte/(float)bitdepth);
							newTexture->setTexturePixel(x, y, color);
						}
					}
					stateCounter++;
					break;
			}
		}
		printf("New Texture: %d:%d@%d\n", width, height, bitdepth);
		fclose(filePointer);
		return *newTexture;
	}
}

/* --- MAIN ---- */
// Ye olden Main function
#ifdef __linux__ // Check for Linux
// Code specific to Linux
int main(int argc, char **argv) {
#elif defined(_WIN32) || defined(_WIN64) // Check for Windows
// Code specific to Windows
int WinMain(int argc, char **argv) {
#endif
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
	
	// Set icon
	//SDL_Surface* icon = IMG_Load("icon.png");
	//SDL_SetWindowIcon(window, icon);
	
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
	
	floorColor.r = 0.5f;
	floorColor.g = 0.5f;
	floorColor.b = 0.5f;
	
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
	
	yellow.r = 1.0f;
	yellow.g = 1.0f;
	yellow.b = 0.0f;
	magenta.r = 1.0f;
	magenta.g = 0.0f;
	magenta.b = 1.0f;
	cyan.r = 0.0f;
	cyan.g = 1.0f;
	cyan.b = 1.0f;
	
	// Default Position
	defaultPosition.x = 0.0f;
	defaultPosition.y = 0.0f;
	
	// Array Init
	ScreenColumnArray = (struct ScreenColumn *)calloc(WINDOW_WIDTH, sizeof(struct ScreenColumn));
	RayArray = (struct Ray *)calloc(numberOfRays+1, sizeof(struct Ray));
	LineArray = (struct Line *)calloc(numberOfLines+1, sizeof(struct Line));
	LightArray = (struct PointLight *)calloc(numberOfLights+1, sizeof(struct PointLight));
	FloorLightArray = (struct Color *)calloc(WINDOW_WIDTH*(WINDOW_HEIGHT_HALF)+1, sizeof(struct Color));
	CeilingLightArray = (struct Color *)calloc(WINDOW_WIDTH*(WINDOW_HEIGHT_HALF)+1, sizeof(struct Color));
	StepSizeDistanceArray = (float *)calloc(WINDOW_HEIGHT_HALF*WINDOW_WIDTH, sizeof(float));
	TextureArray = (struct Texture *)calloc(numberOfTextures, sizeof(struct Texture));
	
	// Precalc of Steps
	preCalculateStepArray();
	
	// Camera Origin
	cameraPosition.x = 640/2;
	cameraPosition.y = 480/3*2-5;
	cameraRotation = 180.0f;
	for (int i = 1; i <= numberOfRays; i++) {
		RayArray[i] = *new Ray();
		RayArray[i].currentColumn = i-1;
	}

	
	/*	Textures */
	// Create Missing Texture
	missingTexture = new Texture(2,2);
	missingTexture->setTexturePixel(0,0,black);
	missingTexture->setTexturePixel(0,1,magenta);
	missingTexture->setTexturePixel(1,0,magenta);
	missingTexture->setTexturePixel(1,1,black);
	TextureArray[0] = *missingTexture;
	TextureArray[1] = importNetpbm("./textures/brick.ppm");
	TextureArray[2] = importNetpbm("./textures/mossy_brick.ppm");
	TextureArray[3] = importNetpbm("./textures/water.ppm");
	
	// Scene is loaded here
	
	/* Geometry */
	// Surrounding Walls
	LineArray[1] = *new Line(0	,0	,640*2,0	,1.0, 0.0, 0.0);
	LineArray[2] = *new Line(640*2,480,640*2,0	,1.0, 1.0, 1.0);
	LineArray[2].emissive = true;
	LineArray[2].texturePointer = &TextureArray[3];
	LineArray[2].textureScale = 4.0f;
	//LineArray[2].portalIndex = 4;
	LineArray[3] = *new Line(0	,480,640*2,480,0.0, 0.0, 1.0);
	LineArray[4] = *new Line(0 	,0,0	,480	,1.0, 1.0, 1.0);
	//LineArray[4].portalIndex = 2;
	
	// House
	LineArray[5] = *new Line(100,180,400,180,1.0, 1.0, 1.0);
	LineArray[5].texturePointer = &TextureArray[1];
	LineArray[5].textureScale = 2.0f;
	
	LineArray[6] = *new Line(200,180,200,280,1.0, 1.0, 1.0);
	LineArray[6].texturePointer = &TextureArray[2];
	LineArray[6].textureScale = 2.0f;
	
	LineArray[7] = *new Line(200,280,300,280,1.0, 1.0, 1.0);
	LineArray[7].texturePointer = &TextureArray[1];
	LineArray[7].textureScale = 2.0f;
	
	LineArray[8] = *new Line(400,180,400,480,1.0, 1.0, 1.0);
	LineArray[8].texturePointer = &TextureArray[1];
	LineArray[8].textureScale = 2.0f;
	
	// Column
	/*
	LineArray[9] =  *new Line(660,200,680,210,1.0, 1.0, 1.0);
	LineArray[10] = *new Line(680,210,690,230,1.0, 1.0, 1.0);
	LineArray[11] = *new Line(690,230,680,250,1.0, 1.0, 1.0);
	LineArray[12] = *new Line(680,250,660,260,1.0, 1.0, 1.0);
	LineArray[13] = *new Line(660,260,640,250,1.0, 1.0, 1.0);
	LineArray[14] = *new Line(640,250,630,230,1.0, 1.0, 1.0);
	LineArray[15] = *new Line(630,230,640,210,1.0, 1.0, 1.0);
	LineArray[16] = *new Line(640,210,660,200,1.0, 1.0, 1.0);
	*/
	
	// Lights
	LightArray[1] = *new PointLight(640/2		, 480/2	,1.0	,1.0	,1.0	,1.0f	,512.0f	);
	LightArray[2] = *new PointLight(640/5		, 480/5	,1.0	,1.0	,1.0	,1.0f	,512.0f	);
	LightArray[2].dynamic = true;
	LightArray[3] = *new PointLight(770	, 110	,1.0	,0.0	,0.0	,1.0f	,512.0f	);
	LightArray[4] = *new PointLight(500	, 260	,0.0	,1.0	,0.0	,1.0f	,512.0f	);
	LightArray[5] = *new PointLight(760	, 350	,0.0	,0.0	,1.0	,1.0f	,512.0f	);
	LightArray[6] = *new PointLight(10		, 10	,1.0	,1.0	,1.0	,1.0f	,512.0f	);
	LightArray[7] = *new PointLight(640*2+32,480/2,1.0	,1.0	,1.0	,1.0f	,512.0f	);
	
	// Precalculate Level Lighting
	precalculateLighting();
	
	// Threadpools
	std::vector<std::thread> raytraceThreadPool;
	std::vector<std::thread> renderThreadPool;
	std::thread gameplayThread(updateInputs);
	
	renderThreadPool.emplace_back(updateScreen);
	
    while (running) {		
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
			if (event.type == SDL_WINDOWEVENT) {
				switch(event.window.event) {
					case SDL_WINDOWEVENT_RESIZED:
						SDL_Log("Window %d resized to %dx%d",
						event.window.windowID, event.window.data1,
						event.window.data2);
						break;
				}
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
	
	SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
	return 0;
}
