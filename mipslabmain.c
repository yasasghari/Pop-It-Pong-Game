/* mipslabmain.c

   This file written 2015 by Axel Isaksson,
   modified 2015, 2017 by F Lundevall

   Latest update 2017-04-21 by F Lundevall

   For copyright and licensing, see file COPYING */

#include <stdint.h>	 /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "gameheader.h"
#include <stdlib.h>
#include "sprites.h"
#include "graphics.h"

#include <stdbool.h>
#include "io.h"

short _PLAYERS_ = 2; // amount of players default, if for some reason it doesnt get set(for example not going through menu.)
short _SCORE_ = 0; // score is set to 0, done later too though

scoreBoard highScores; // scoreboard is our way of tracking highscores. It's a struct containing 3 numbers and 3 names corresponding to each

char name[3] = "AAA"; //this is the default name when entering a name into the state ENTERNAME
short target = 0; // this is the current letter we're looking at. by pressing btn 3 we increase target to choose a letter

gameObj ball1, ball2; // balls are our structs gameObj that contains an xPos, yPos, and velocities in both directions. 
//they also contain if the gameObj is alive or not, if it's not alive, we do not render nor deal with it in updates
gameObj plyr1, plyr2;
//same thing here. we originally wanted these to be  nestled struct but we couldnt make it work in time. the reason we wanted a nestled struct
// is to have individual scores for each player. 
gameObj _bricks[48];
// a list of bricks in the middle of the screen, 6*8 = 48 

int gameState = MENU;

bool running = true; // if the game is running. this just makes an infinite while loop
double gameSpeed = 0.02; // this is our multiplier for all movement, but not graphical updates. 

void update_Objects(float gameSpeed);
void draw_Screen();

// Menu Definitions
void updateMenu();
void drawMenu();

//difficulty settings
void updateDiff();
void drawDiff();

//Menu for Game Over. It just draws game over and a winner. 
void draw_GO();
void update_GO();

//highscore stuff
void update_HIGH();
void draw_HIGH();

// general help functions and boot
void setupGame();
void quicksleep();

// enter name 
void update_EN();
void draw_EN();

int main(void)
{
	/*
Main Function.
Includes a while loop with game logic and updates to all objects and draws to screen every cycle.
When main is called we do a bunch of pre-game setup including initializing buttons, screen, and making sure to clear the screen
one time before we run any other code. 

*/
	gameState = MENU;
	display_init();
	init_buttons();
	setupGame();
	clearScreen();
	write2oled();


//We also set the default highscores, because we're storing our highscores in volatile memory, we do not save them between restarts.
// this is just to have as a comparison for showing the game

	highScores.firstNumber = 4;
	highScores.secondNumber = 2;
	highScores.thirdNumber = 0;

	stringCopy(highScores.firstName, "EMP", 3);
	stringCopy(highScores.secondName, "EMP", 3);
	stringCopy(highScores.thirdName, "EMP", 3);

	while (running) //we're just looping through very similar methods on every gamestate. 
	// an update then a draw, where we just draw what has been put on the screen.
	// a lot of update functions do not do much, because they're practically just detecting buttons for other menus
	{
		clearScreen(); // clears memory of screen
		updateButtons(); // resets our buttons to be 0 for new detection. this also means that we dont have any way of checking i you hold the buttons

		switch(gameState){ //depending on our gameState the game draws and updates different screens
			case MENU: // menu is the screen where we choose gameplay mode. the update looks for buttons and changes gamestate
			updateMenu();
			drawMenu();
			break;

			case GAMERUNNING:
			update_Objects(gameSpeed);
			draw_Screen();
			break;

			case HIGHSCORE:
			update_HIGH();
			draw_HIGH();
			break;

			case DIFF:
			updateDiff();
			drawDiff();
			break;

			case GAMEOVER:
			update_GO();
			draw_GO();
			break;
			
			case ENTERNAME:
			update_EN();
			draw_EN();
			break;
		}
	}
	return 0;
}

void update_EN(){
	// as there is no point in entering name when you haven't placed on leaderboards we just skip to menu;
	if(highScores.thirdNumber> _SCORE_){
		gameState = MENU; // skip to menu if theres no point in entering name
	}
	int i , spot; 
	spot = 4;
		if(_SCORE_ > highScores.firstNumber){ //we check if ur score is higher than 1st place. if so we move everything else down a space
		// and copy ur values into 1stNMR 
			volatile int temp = highScores.secondNumber;
			highScores.thirdNumber = temp;
			highScores.secondNumber = highScores.firstNumber;
			stringCopy(highScores.thirdName, highScores.secondName, 3); //we took our own version of strnCPY because the string.h libaray is bugged
			stringCopy(highScores.secondName, highScores.firstName, 3);

			spot = 0;

		}else{
			if(_SCORE_ > highScores.secondNumber){ // else if did not work. it's doing all of them. fucking c doing nested if else instead
			spot = 1;
			highScores.thirdNumber = highScores.secondNumber;
			stringCopy(highScores.thirdName, highScores.secondName, 3);
			} 
			else{
				if(_SCORE_ > highScores.thirdNumber){
				spot = 2;
				}

			}
		}
	
	//let user input name

	//button 2 (BTN3) enters in the current letter by changing our target letter
	if(consume(BUTTON2)){
		target++;
	}
	if(consume(BUTTON1)){ // this just cycles through the alphabet as if they were numbers(chars are numbers ;) )
		name[target] ++;
		if(name[target] > 'Z'){
			name[target] = 'A';
		}
	}
	if(consume(BUTTON3)){ // same thing, other direction here.
		name[target] --;
		if(name[target] < 'A'){
			name[target] = 'Z';
		}
	}

	if(target == 3){ // if we're done writing( index 4 on a 3 letter name)
		switch (spot)
		{
		case 0:
			stringCopy(highScores.firstName, name, 3); //we just copy it depending on what spot on the leaderboard ur supposed ot have
			highScores.firstNumber = _SCORE_;
			break;
		case 1:
			stringCopy(highScores.secondName, name, 3);
			highScores.secondNumber = _SCORE_;
			break;
		case 2:
			stringCopy(highScores.thirdName, name, 3);
			highScores.thirdNumber = _SCORE_;
			break;
		default:
			break;
		}



		target = 0;
		gameState = HIGHSCORE;
	}
	drawString(itoaconv(spot),2,16, 64);

	

}
void draw_EN(){
	drawString("INPUT", 5, 0, 12); // tell user what to do 
	drawString("NAME", 4, 3, 22);

	drawString(name, 3, 6, 42); //name updates in entername update so we just write it repeadelty and all changes carry through

	write2oled();
}
void update_HIGH(){
	if(consume(BUTTON1) || consume(BUTTON2) || consume(BUTTON3)){ // goes back to menu if any buttons are pressed
		gameState = MENU;
	}
}
void draw_HIGH(){
	drawString("HIGHS", 5, 32, 0);
	int i, y;
	y = 1;
	char *temp; // draws top two players --ef was supposed to be 3 
	for(i = 0; i < 3; i++){ // i is player number. 0 is the best player, and so forth
		switch (i)
		{
		case 0:
			temp = highScores.firstName;
			drawString(temp, 3, 0, y*10+2);
			drawString(itoaconv(highScores.firstNumber), 3, 0, y*10 + 12);

			break;
		
		case 1:
			temp = highScores.secondName;
			drawString(temp, 3, 0, y*10+2);
			drawString(itoaconv(highScores.secondNumber), 3, 0, y*10 + 12);
			break;
		/*case 2:
		Because C is literally unusable whenever we use a third place on the leaderboards it just decides to copy any value from 2???
		We don't have time to bugfix this more, we've been at it for 2 hours. 
			temp = highScores.thirdName;
			drawString(temp, 3, 0, y*10+2);
			drawString(itoaconv(highScores.thirdNumber), 3, 0, y*10 + 12);
		break;*/
		default:
			break;
		}
		y += 2 ; //goes down 2 "lines" each line is 10 pixels
	}
	write2oled();
}
int counter;
void update_GO(){

	counter++;
	if(counter > 500){ // we didnt finish any timer so we're just counting a number til it hits 500 which is about half a second
		gameState = ENTERNAME; // we just go to entername, entername then handles if they're supposed to be on leaderboards or not
		counter = 0;
	}
}
void draw_GO(){
	drawString("GAME", 4, 3, 22); // we draw a game over screen
	drawString("OVER", 4, 3, 32);
	if(plyr1.alive){ // depending on who wins, we say a player wins.
		drawString("PLYR2", 5, 0, 42);
	}
	else
	{
		drawString("PLYR1",5,0,42);
	} 

	drawString("WINS",4,4,52);

	write2oled;
	
}

void update_Objects(float gameSpeed)
{

	//This updates our paddles trajectories, does the "AI" for the opponent in 1-player mode and checks for 
	//out of bounds paddles. We do this both paddles at a time so we know what paddle to give AI
	updatePaddle(&plyr1, &plyr2, &ball1, &ball2, _PLAYERS_, gameSpeed);

	//We update each ball, giving them the information about all bricks, players and making sure they don't go out of bounds
	//their vX/vY is multipled by gamespeed and added ontop of their current coordinates. 
	updateBall(&ball1, _bricks, gameSpeed, &plyr1, &plyr2);
	updateBall(&ball2, _bricks, gameSpeed, &plyr1, &plyr2);



}

void draw_Screen()
{ // EF & YA
/*
*/
	int i;
	for (i = 0; i < 48; i++) // first layer is bricks 48 of them
	{
		if (_bricks[i].alive)
		{
			drawBitmap(brick_texture, _bricks[i].xPos, _bricks[i].yPos, 8, 1, 0);
		}
	}
	drawBitmap(ball_texture, (int)(ball1.xPos + 0.5), (int)(ball1.yPos + 0.5), 8, 1, 0); // draw ball
	drawBitmap(ball_texture, (int)(ball2.xPos + 0.5), (int)(ball2.yPos + 0.5), 8, 1, 0); // draw ball
	if (plyr1.alive)
	{
		drawBitmap(plyr1_texture, (int)(plyr1.xPos + 0.5), (int)(plyr1.yPos + 0.5), 8, 1, 0); // draw player 1
	}
	if (plyr2.alive)
	{
		drawBitmap(plyr1_texture, (int)(plyr2.xPos + 0.5), (int)(plyr2.yPos + 0.5), 8, 1, 0); // draw player 2
	}
	/*if(gameSpeed == 0.04){
		drawString("NRML",4,4,4);
	}
	else if(gameSpeed == 0.06){  //bugfix stuff
		drawString("HARD",4,4,4);
	}*/
	//we do this in draw because it looked ugly within the switch case lol
	if(plyr1.alive == false || plyr2.alive == false){
		gameState = GAMEOVER;
		clearScreen();
		quicksleep(100);
		draw_GO();
	}
	drawString(itoaconv(_SCORE_), 2, 0, 0 );
	write2oled();
}
void updateMenu()
{
	// check for buttons to change gamestate

	if (consume(BUTTON3))
	{
		gameState = HIGHSCORE;
	}

	if (consume(BUTTON2))
	{
		//two-player mode. we do not have a difficulty so we simply go directly to run, but reset all assets using setupgame
		_PLAYERS_ = 2;
		gameState = GAMERUNNING;
		setupGame();

		_SCORE_ = 0;
	}
	if (consume(BUTTON1))
	{
		// single player mode. 
		// ask for difficulty, then setupGame(sets up assets and resets stage)
		_PLAYERS_ = 1;
		gameState = DIFF;
		setupGame();

		_SCORE_ = 0;
	}

}
void drawMenu()
{

	drawString("POPIT", 5, 0, 12);
	drawString("PLYR1", 5, 0, 24);
	drawString("PLYR2", 5, 0, 36);
	drawString("HIGHS", 5, 0, 48);

	write2oled(); //updates display from information found in Display_FULL
}
void updateDiff(){
	//choosing gamespeed practically. does not do much but was required
	if (consume(BUTTON3))
	{// easy
		gameState = GAMERUNNING;
	}

	if (consume(BUTTON2))
	//medium
	{
		gameSpeed *=2;
		gameState = GAMERUNNING;
	}
	if (consume(BUTTON1))
	//hard
	{
		gameSpeed *=3;
		gameState = GAMERUNNING;
	}
}
void drawDiff()
{
	drawString("DIFF", 4, 0, 12);
	drawString("HARD", 4, 0, 24);
	drawString("NORML", 5, 0, 36);
	drawString("EASY", 4, 0, 48);
	write2oled();
}

void setupGame()
{
	gameSpeed = 0.02;

	// All the bricks.
	int i, j;
	for (i = 0; i < 48; i++)
	//for loop for making 48 bricks in the middle 
	{
		gameObj temp;
		temp = makeObj(4 * (i % 8), 48 + 4 * (i / 8), 0, 0);
		_bricks[i] = temp;
	}

	// Initializing the two balls that bounce
	ball1 = makeObj(9, 16, 0, 8);
	ball2 = makeObj(16, 108, 0, -8);

	_SCORE_ = 0;

	// initializing players as new gameobjects to then make playerobjects
	plyr1 = makeObj(12, 5, 0, 0);
	plyr2 = makeObj(12, 117, 0, 0);

}


void quicksleep(int ms)
{
	int i = 0;
	for (i = 0; i < ms; i++)
	{
	}
}