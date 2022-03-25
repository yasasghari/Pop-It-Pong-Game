#include <pic32mx.h>
#include <stdint.h>
#include "gameheader.h"
#include "sprites.h"

texture makeTexture(const uint8_t *data, int w, int h){ //create a new texture
        // ... x = width, y = height, n = # 8-bit components per pixel ...
        // we call these components sections. 
        // ... replace '0' with '1'..'4' to force that many components per pixel
        // ... but 'n' will always be the number that it would have been if you said 0

        //this doesnt work idk why wtf 

        texture temp;
        int i = 0;
        for(i = 0; i < sizeof(data)/sizeof(uint8_t); i++){ //loopar igenom alla saker 
            temp.texture[i] = data[i];
        }
        temp.width = w;
        temp.height = h;
        
        return temp;

}