#include <stdint.h>
#include <pic32mx.h>
#include "gameheader.h"
#include "sprites.h"

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

uint8_t spi_send_recv(uint8_t data)
{
    // Wait for buffer
    while (!(SPI2STAT & 0x08))
        ;
    // Hand over data
    SPI2BUF = data;
    // Wait for buffer response
    while (!(SPI2STAT & 1))
        ;
    return SPI2BUF;
}

void display_init(void)
{ // this function is taken directly from mipslabfunc. It is used to initialize screen, has to be used after other init
    DISPLAY_CHANGE_TO_COMMAND_MODE;
    quicksleep(10);
    DISPLAY_ACTIVATE_VDD;
    quicksleep(10000);

    spi_send_recv(0xAE);
    DISPLAY_ACTIVATE_RESET;
    quicksleep(10);
    DISPLAY_DO_NOT_RESET;
    quicksleep(10);

    spi_send_recv(0x8D);
    spi_send_recv(0x14);

    spi_send_recv(0xD9);
    spi_send_recv(0xF1);

    DISPLAY_ACTIVATE_VBAT;
    quicksleep(10000000);

    spi_send_recv(0xA1);
    spi_send_recv(0xC8);

    spi_send_recv(0xDA);
    spi_send_recv(0x20);

    spi_send_recv(0xAF);
}
void setPixel(int x, int y, const uint8_t state) {
	if((state & 0x8) || (y < 128 && y >= 0 && x < 32 && x >= 0)) {
		while(y < 0) y += 128; 
		while(x < 0) x += 32; 
		uint8_t bit = 0x1 << (x - (x/8 * 8));
		if(state & 0x1) {
			display_full[(y % 128 + (x/8) * 128) % 512] |= bit;
		} else {
			display_full[(y % 128 + (x/8) * 128) % 512] &= ~bit;
		}
	}
}
void memcpy() // this was never used lol -- Edvin
{
}
void drawBitmap(const uint8_t sprite[], int x, int y, short longi, short lati, uint8_t destructive) {
    //written by EF, YA
    // we asked advice on proper implementation from Pontus
short temp = x;
x = y;
y = temp;
  int n, b, i;
  //it goes through each section of 8x8
  // if we were using for example a larger texture this would be taken into account by lati(which is width / 8, amount of sections)
  for(n = 0; n < lati; n++) {
    int bit = 1;
    //for each bit in our current section, we write that using setPixel to the displayFull
    for(b = 0; b < 8; b++) {
      for(i = longi; i > 0; i--) {
        // if our current pixel is supposed to be 1, we try and write a 1.
        if (sprite[longi-i + n * longi] & bit) {
          setPixel(y + i, x + n * 8 + b, 1 | (destructive & 8));
        }
        else if(destructive){ // if its trying to write 0
          setPixel(y + i, x + n * 8 + b, destructive & 8);
        }
      }
      // we also check if its supposed to overwrite the current screen using destructive. we never really used this feature, always to setting it to 0
      bit = bit << 1;
    }
  }
}


void drawTexture(texture *in, int x, int y) // drawtexture writes a texture to display_full
{
    drawBitmap(in->texture, x, y, in->width, in->height/8, 0 ); // does not work structs are weird uwu //edvin
    // i think if we had more time we could get this struct to work -- ef
}


void write2oled()
{ // updates screen from display_full
    int i, j;

    for (i = 0; i < 4; i++)
    {
        DISPLAY_CHANGE_TO_COMMAND_MODE;

        /*
        Sets the screen to take in one section at a time. A section is a 8x1. SPI-send-recv on mode 34 is something we got from lab3
        After that we give it what section. 
        */
        spi_send_recv(0x22);
        spi_send_recv(i);

        spi_send_recv(0);
        spi_send_recv(0x10);

        DISPLAY_CHANGE_TO_DATA_MODE;

        // 128 y coordinates, 4 sections of 8 coordinates. 4*128 = 512 512 is our amount of sections.
        // We just copy the data from displayfull one a section at a time. 

        for (j = 0; j < 128; j++)
        {

            spi_send_recv(display_full[i * 128 + j]);
        }
    }
}

// clears screen
void clearScreen()
{
    int i;
    for (i = 0; i < 512; i++)
    {
        display_full[i] = 0;
    }
}

// ascii numbers and letters taken from lab files
uint8_t numbers[10][8] = {
    {0, 62, 65, 65, 65, 62, 0, 0},
    {0, 0, 66, 127, 64, 0, 0, 0},
    {0, 0, 98, 81, 73, 70, 0, 0},
    {0, 0, 34, 73, 73, 54, 0, 0},
    {0, 0, 14, 8, 127, 8, 0, 0},
    {0, 0, 35, 69, 69, 57, 0, 0},
    {0, 0, 62, 73, 73, 50, 0, 0},
    {0, 0, 1, 97, 25, 7, 0, 0},
    {0, 0, 54, 73, 73, 54, 0, 0},
    {0, 0, 6, 9, 9, 126, 0, 0}};

uint8_t letters[26][8] = {
    {0, 0, 126, 9, 9, 126, 0, 0},
    {0, 0, 127, 73, 73, 54, 0, 0},
    {0, 0, 62, 65, 65, 34, 0, 0},
    {0, 0, 127, 65, 65, 62, 0, 0},
    {0, 0, 127, 73, 73, 65, 0, 0},
    {0, 0, 127, 9, 9, 1, 0, 0},
    {0, 0, 62, 65, 81, 50, 0, 0},
    {0, 0, 127, 8, 8, 127, 0, 0},
    {0, 0, 65, 127, 65, 0, 0, 0},
    {0, 0, 32, 64, 64, 63, 0, 0},
    {0, 0, 127, 8, 20, 99, 0, 0},
    {0, 0, 127, 64, 64, 64, 0, 0},
    {0, 127, 2, 4, 2, 127, 0, 0},
    {0, 127, 6, 8, 48, 127, 0, 0},
    {0, 0, 62, 65, 65, 62, 0, 0},
    {0, 0, 127, 9, 9, 6, 0, 0},
    {0, 0, 62, 65, 97, 126, 64, 0},
    {0, 0, 127, 9, 9, 118, 0, 0},
    {0, 0, 38, 73, 73, 50, 0, 0},
    {0, 1, 1, 127, 1, 1, 0, 0},
    {0, 0, 63, 64, 64, 63, 0, 0},
    {0, 31, 32, 64, 32, 31, 0, 0},
    {0, 63, 64, 48, 64, 63, 0, 0},
    {0, 0, 119, 8, 8, 119, 0, 0},
    {0, 3, 4, 120, 4, 3, 0, 0},
    {0, 0, 113, 73, 73, 71, 0, 0}};
/*Draw String: 
Function for drawing a string, used in menu and highscore*/
/**
 draws a texture onto display
 immensily important that this works
 Should add pixels to screen to then be updated

 */

void reverse(char arr[], int n) //snodd från https://www.techiedelight.com/reverse-array-c/ med modifieringar
{
    int low, high;
    for (low = 0, high = n - 1; low < high; low++, high--)
    {
        char temp = arr[low];
        arr[low] = arr[high];
        arr[high] = temp;
    }
}
void drawString( char string[], int stringLength, int x, int y){

    int i, currentX;

    currentX = x;
    // this is based off of what we could interept in lab 3, and an array of bitmaps. we just check for what letter we're 
    // supposed to use using ascii code 

    for(i = stringLength-1; i >= 0; i--){
        if(string[i] > 64 && string[i] < 91){
            drawBitmap(letters[(int)string[i]-65], currentX, y, 8, 1, 0);   
        }
        else if (string[i] > 47 && string[i] < 58)
        {
            drawBitmap(numbers[(int)string[i]-48], currentX, y, 8, 1, 0);
        }
        else{

        }
        currentX += 6; // 6 felts like right amount of average pixels between the start of each letter(they're usually 4-5 wide)
    }
}

