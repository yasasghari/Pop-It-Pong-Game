#include <stdint.h>
#include "gameheader.h"

void drawTexture(texture texture, int x, int y);
void setPixel(int x, int y, const uint8_t state);
void drawBitmap(const uint8_t map[], short x, short y, short width, short height, uint8_t destructive);
void clearScreen(void);
void printNumber(int number, int x, int );
void drawString( char string[], int stringLength, int x, int y);