#include <stdbool.h>
#include <stdint.h>
bool getSwitch(short i);

extern uint8_t button;

void init_buttons(void);
void updateButtons(void);
int consume(uint8_t btn);
#define BUTTON1 0x4
#define BUTTON2 0x2
#define BUTTON3 0x1