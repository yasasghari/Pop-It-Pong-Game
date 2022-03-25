#ifndef HEADER
#define HEADER
typedef struct {
    uint8_t texture[8]; //texture consists of a long string of characters, where each character represents the grey channel value of the pixel
    int width; //width is the width of the texture, used to determine other stuff
    int height;
} texture;

typedef struct{ //we're declaring all our structs as typedef in a header file because we want to use them in more than one file. 
                //typeDef tries to emulate an object in an object-oriented language by allowing us call it by just its name, not "struct gameObj"
    float xPos;
    float yPos;
    float vX;
    float vY;
    uint8_t alive;

} gameObj;

typedef struct{ //struct containing 3 names and 3 top scores.
    char firstName[3];
    short firstNumber;
    char secondName[3];
    short secondNumber;
    char thirdName[3];
    short thirdNumber;
} scoreBoard;




#define GAMERUNNING 1
#define MENU 0
#define HIGHSCORE 2
#define DIFF 3
#define GAMEOVER 4
#define ENTERNAME 5

texture makeTexture(const uint8_t *data, int w, int h); //make a new texture
gameObj makeObj(float _xPos, float _yPos, float _vX, float _vY);


/*Declarations for all things graphics, setPixel, setSection etc. */
extern uint8_t display_full[512];
extern short _SCORE_;
extern gameObj _bricks[48];
void setPixel(int x, int y, const uint8_t state);
void setSection(int x, int y, uint8_t section);
char * itoaconv(int num);

void display_init();
void write2oled();


char* stringCopy(char* destination, const char* source, uint8_t num);

//BALL FUNCTIONS FOR MOVING BALL
void updateBall(gameObj* ball, gameObj _bricks[48], float modifier,gameObj*  plyr1,gameObj*  plyr2);
void updatePaddle(gameObj*  player1,gameObj*  player2,gameObj* ball1, gameObj* ball2, short number, float modifier);




#endif