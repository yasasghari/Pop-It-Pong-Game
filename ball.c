#include <pic32mx.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "gameheader.h"
#include "sprites.h"

#define MINIMUM 3
#define MAXIMUM 10

//Like everything in this project our files are a collaberative effort
// Edvin Frosterud
// Yas Asghari 


void collisionCheck(gameObj *ball, gameObj _bricks[48], float modifier,gameObj *plyr1,gameObj *plyr2);
void paddleCheck(gameObj *ball, gameObj *player1, float modifier);
void boundaryCheck(gameObj *ball, float modifier, gameObj *player1, gameObj *player2);

void updateBall(gameObj *ball, gameObj _bricks[48], float modifier, gameObj *_PLAYER1, gameObj *_PLAYER2)
{

    //update ball handles movement, collision between brick and ball, ball and paddle and ball and boundaries. 
    //collision check checks if the ball is currently goign to hit a brick inside the 48-list of bricks. 
    //paddleCheck checks if the ball we test for(we run this function twice) is going to hit any of the two paddles. 

    collisionCheck(ball, _bricks, modifier, _PLAYER1, _PLAYER2);
    paddleCheck(ball, _PLAYER1, modifier);
    paddleCheck(ball, _PLAYER2, modifier);


    //We set a minimum and maximum speed to minimize glitches happening while the ball is faster than 1-brick/tick or slower than 1
    // we set these as defines for easy modfiication. 
    if(ball->vX > MAXIMUM){// max speed
        ball->vX = MAXIMUM;
    }
    if(ball->vX < -MAXIMUM){// max speed
        ball->vX = -MAXIMUM;
    }    
    if(ball->vY > MAXIMUM){// max speed
        ball->vY = MAXIMUM;
    }    
    if(ball->vY < -MAXIMUM){// max speed
        ball->vY = -MAXIMUM;
    }
    
    if(ball->vX < MINIMUM && ball->vX >= 0){// min speed
        ball->vX = MINIMUM;
    }
    if(ball->vX > -MINIMUM && ball->vX < 0){// min speed
        ball->vX = -MINIMUM;
    }    
    if(ball->vY < MINIMUM && ball->vY >= 0){// min speed
        ball->vY = MINIMUM;
    }    
    if(ball->vY > -MINIMUM && ball->vY < 0){// min speed
        ball->vY = -MINIMUM;
    }

    //boundarycheck checks if the balls are going to go out of bounds.
    //if they do we modifiy their velocity accordingly. 

    boundaryCheck(ball, modifier, _PLAYER1, _PLAYER2);

    //add modified velocities to the current position of the ball. 
    ball->xPos += (ball->vX) * modifier;
    ball->yPos += (ball->vY) * modifier;
}

void collisionCheck(gameObj *ball, gameObj _bricks[48], float modifier,gameObj*PLYR1,gameObj*PLYR2)

/*
Collision Logic for detecting when a ball hits a brick. 
All original code written by EF and YA
*/
{
    //we have a couple of helper variables "nextX" & nextY because we don't like writing xPos + vX every time. lol 
    float nextX, nextY;
    int i;
    nextX = ball->xPos + (ball->vX) * modifier;
    nextY = ball->yPos + (ball->vY) * modifier;

    for (i = 0; i < 48; i++)
     // loops through each single brick to find any collisions with any bricks.
     // if we had more time, we would make this process a lot faster by including a "collidable" bool on gameObjs
     // collidable would only be true if the brick touches air. 
    {

       //if a brick is hit from the right  positive vX is moving to the right. 
        //this if statement and digusting, but it works perfectly. We took inspiration from a previous C#
        // project Edvin had, therefore the comments on the right. 
        if (
            ball->vX > 0 && nextX < _bricks[i].xPos + 3             // this.rectangle.Right + velocityX
            && ball->vX + 2 > _bricks[i].xPos + 3                   // this.left < left pos on rectangle
            && _bricks[i].alive && ball->yPos + 2 > _bricks[i].yPos // this.bottom > other.top
            && ball->yPos < _bricks[i].yPos + 3)
        { // collision with brick to the right of the ball
            ball->vX = -ball->vX;
            _bricks[i].alive = false;
            _SCORE_++;
            
        }
        //if a brick is hit from the left. negative vX is moving to the right. 
        //this if statement and digusting, but it works perfectly. We took inspiration from a previous C#
        // project Edvin had, therefore the comments on the right. 
        if (ball->vX < 0 && nextX + 2 > _bricks[i].xPos             // this.rectangle.Right + velocityX
            && ball->vX < _bricks[i].xPos                           // this.left < left pos on rectangle
            && _bricks[i].alive && ball->yPos + 2 > _bricks[i].yPos // this.bottom > other.top
            && ball->yPos < _bricks[i].yPos + 3)
        { // this.top < other.bottom
            // invert velocity in x axis
            ball->vX = -ball->vX;
            _bricks[i].alive = false;
            _SCORE_++;

        }// if the ball is going down, same thing here. 
        if (ball->vY > 0 
        && nextY + 2 > _bricks[i].yPos // big ass if statement for checking ball collision detection.
        && ball->yPos < _bricks[i].yPos 
        && _bricks[i].alive 
        && ball->xPos + 3 > _bricks[i].xPos 
        && ball->xPos < _bricks[i].xPos + 4)
        { // collision with brick under the ball
            // invert velocity in x axis
            ball->vY = -ball->vY;
            _bricks[i].alive = false;
            _SCORE_++;

        
        }// if ball is going up, same thing here. 
        if (ball->vY < 0 && nextY < _bricks[i].yPos + 3 // big ass if statement for checking ball collision detection.
            && ball->yPos > _bricks[i].yPos + 3 
            && _bricks[i].alive 
            && ball->xPos + 3 > _bricks[i].xPos 
            && ball->xPos < _bricks[i].xPos + 4)
        { // collision with brick above the ball
            // invert velocity in x axis
            ball->vY = -ball->vY;
            _bricks[i].alive = false;
            _SCORE_++;

        }
    }
}
void paddleCheck(gameObj *ball, gameObj *player, float modifier)
{

    //very similar to collisionCheck but checks for paddles instead, and has a calculation instead of just inverting
    //vX if it hits in the x-axis. 
    float nextX, nextY, middleOfPaddle;
    middleOfPaddle = player->xPos + 3;

    int i;
    nextX = ball->xPos + (ball->vX) * modifier;
    nextY = ball->yPos + (ball->vY) * modifier;

    if (
        ball->vX > 0 && nextX <= player->xPos + 7         // this.rectangle.Right + velocityX
        && ball->vX + 2 > player->xPos - 7               // this.left < left pos on rectangle
        && player->alive && ball->yPos + 2 > player->yPos // this.bottom > other.top
        && ball->yPos < player->yPos)
    { // collision with brick to the right of the ball
        ball->vX = -ball->vX;
        ball->vY = -ball->vY;
        ball->vX *= middleOfPaddle/(ball->xPos + 1); //this is giving vX a really large speed if it is currently hitting the left of the paddle
        // speed depends on the distance between the middle of the paddle and current position of ball, therefore simulating angle without
        // using trigonometry(because we couldnt get it to work lol )
    }

    if (ball->vX < 0 && nextX + 2 >= player->xPos         // this.rectangle.Right + velocityX
        && ball->vX < player->xPos                       // this.left < left pos on rectangle
        && player->alive 
        && ball->yPos + 3 > player->yPos // this.bottom > other.top
        && ball->yPos < player->yPos)
    { // this.top < other.bottom
        // invert velocity in x axis
        ball->vX = -ball->vX;
        ball->vY = -ball->vY;
        ball->vX *= middleOfPaddle/(ball->xPos + 1); // same as other x axis collision
    }
    if (ball->vY > 0 
        && nextY + 2 >= player->yPos // big ass if statement for checking ball collision detection.
        && ball->yPos < player->yPos 
        && player->alive 
        && ball->xPos < player->xPos + 8
        && ball->xPos + 3 > player->xPos )
    { // collision with brick under the ball
        // invert velocity in y axis
        float multi = (middleOfPaddle/nextX+1);
        if(middleOfPaddle >= nextX && middleOfPaddle <= nextX + 2){
            ball->vY = - ball->vY;
        }
        else if(middleOfPaddle > nextX){ // if it's on the right or the left side of the middle of the paddle, we multiply/divide by the multiplier
        // this is to give it a new angle depending on the current angle of impact, speed and distance between paddle and ball. 
            ball->vY = -(ball->vX+ball->vY)*multi;
            ball->vX = (ball->vX+ball->vY)/multi;


        }else{
            ball->vY = -(ball->vX+ball->vY)*multi;
            ball->vX = -(ball->vX+ball->vY)/multi;
        }

    }
    if(ball->vY < 0 // ball going up 
    && nextY <= player->yPos+6
    && player->alive 
    && ball->yPos> player->yPos + 6
    && ball->xPos < player->xPos + 8
    && ball->xPos + 2 > player->xPos ){
        float multi = (middleOfPaddle/nextX+1);
        if(middleOfPaddle >= nextX && middleOfPaddle <= nextX + 2){
            ball->vY = - ball->vY;
        }
        else if(middleOfPaddle > nextX){
            ball->vY = -(ball->vX+ball->vY)*multi;
            ball->vX = (ball->vX+ball->vY)/multi;


        }else{ //same here as other collision 
            ball->vY = -(ball->vX+ball->vY)*multi;
            ball->vX = -(ball->vX+ball->vY)/multi;
        }
    }
}
void boundaryCheck(gameObj *ball, float modifier, gameObj *player1, gameObj* player2){
    float nextX, nextY;
    //checks for each ball if they're outside the boundaries of the screen. 
    nextX = ball->xPos + (ball->vX) * modifier;
    nextY = ball->yPos + (ball->vY) * modifier;
    if (nextX + 2 >= 30 || nextX <= 2)
    {
        ball->vX = -ball->vX;
    }
    if (nextY + 2 >= 127 || nextY <= 1)
    {
        if(nextY < 5)
        { player1->alive = false;}
        else
        { player2->alive = false;}

    }
}
