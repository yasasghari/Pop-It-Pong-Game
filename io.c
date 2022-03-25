//IO stuff
//We're using switches for controlling the game
// and button for controlling menus
#include <stdbool.h>
#include <pic32mx.h>
#include "io.h"
#include <stdint.h>

uint8_t button = 0;
uint8_t consumedButton = 0;

//button stuff by Pontus Ask
//switches by EF & YA

bool getSwitch(short i){ //gets data from a switch
    return (PORTD>>(i+8)&0x1);
}
int getbtns(void){
	return (PORTD & 0xE0) >> 5;
}
int consume(uint8_t btn) {
  if((button & btn) & ~(consumedButton & btn)) {
    consumedButton |= btn;
    button &= ~btn;
    return 1;
  }
  return 0;
}
void init_buttons(){
  // for some reason btn1 is dealt with another way, so we can only use 2-4
  TRISD |= 0xFE0;
}
void updateButtons(){

  //Inspired from EPI
  // with permission from him, Pontus Ask
  // We originally used our own functions from lab 3, but this didn't work properly, so we asked him for help. 
  if((getbtns() & BUTTON1) & ~(consumedButton & BUTTON1)){
    button = button | BUTTON1;
  } else consumedButton = (consumedButton & ~BUTTON1) | (BUTTON1 & getbtns());

  if((getbtns() & BUTTON2) & ~(consumedButton & BUTTON2)){
    button = button | BUTTON2;
  } else consumedButton = (consumedButton & ~BUTTON2) | (BUTTON2 & getbtns());

  if((getbtns() & BUTTON3) & ~(consumedButton & BUTTON3)){
    button = button | BUTTON3;
  } else consumedButton = (consumedButton & ~BUTTON3) | (BUTTON3 & getbtns());
}