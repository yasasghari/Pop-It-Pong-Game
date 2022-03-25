#include <pic32mx.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "gameheader.h"
#include "sprites.h"
#include "io.h"
gameObj makeObj(float _xPos, float _yPos, float _vX, float _vY){
    gameObj object;
    object.xPos = _xPos;
    object.yPos = _yPos;
    object.vX = _vX;
    object.vY = _vY;
    object.alive = true;

     return object;
}



void updatePaddle(gameObj* player2, gameObj* player1, gameObj* ball1, gameObj* ball2, short nmrOfPlayers, float modifier){
    if(nmrOfPlayers == 2){ // if two-player mode. we just get switches from LAB3 and we change their velocity to match.
    // we cannot however stop the platforms, as the switches only have two modes of input. This was intentional, to make the game
    // challenging. 
        if(getSwitch(3)){
            player2->vX = -6;
        }
        else{
            player2->vX = 6;
        }

        if(getSwitch(0)){
            player1->vX = -6;
        }
        else{
            player1->vX = 6;
        }
    }
    else{ //if ur playing against AI 
        if(getSwitch(0)){ //for player 1, this is the 
            player1->vX = -6;
        }
        else{
            player1->vX = 6;
        }
        if(!(ball2->yPos < ball1->yPos)){ // this is selecting what ball the ai should follow. its follows the closest ball, which
        // is usually the ball spawns with. 
            if(player2->xPos > ball1->xPos){ // if the ball is to the right of the paddle, the ai goes to the right
                player2->vX = -6;
            }
            else{ //else it goes to the left. this however means that if for some reason the xpos of ball1 is empty, we go to the left
             player2->vX = 6; 
            }
            
        }
        else  {
            if(player2->xPos > ball2->xPos){
                player2->vX = -6;
            }
            else{
             player2->vX = 6; 
            }
        }
    }

    //adding the velocities calculated previously to the player position. 

    player2->xPos += player2->vX*modifier;
    player2->yPos += player2->vY*modifier;
    player1->xPos += player1->vX*modifier;
    player1->yPos += player1->vY*modifier;


// after that we check for out of bounds and teleport back inside before any drawing is done. 
    if(player2->xPos > 24){
        player2->xPos = 24;
    }
    if(player2->xPos < 1){
        player2->xPos = 1;
    }
    if(player1->xPos > 24){
        player1->xPos = 24;
    }
    if(player1->xPos < 1){
        player1->xPos = 1;
    }
    

}