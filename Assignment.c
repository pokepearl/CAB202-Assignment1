#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/* Global Variables
    These include the arrays responsible for storing settings and statistics for the game as well as the sprites for objects
*/
int statcounter[] = {10,00,00,0000,0,198}; //[0] - Lives, [1] - Minutes, [2] - Seconds, [3] - Time Counter, [4] - Score, [5] - Total number of blocks
bool boolcounter[] = {false,false,true,false,false}; //[0] - Whether to allow point collection, [1] - Game Over Status, [2] - Screen Refresh, [3] - Treasure Pause, [4] - Treasure Direction
sprite_id blocks[198]; //Array of all blocks spawned
char * blockSafeSpr = "======="
                      "=======";
char * blockUnsafeSpr="xxxxxxx"
                      "xxxxxxx";
char * blockBlankSpr =" ";
char * treasurespr = "@@"
                     "[]";
char * treasurespr2 = "[]"
                     "@@";
char * charsprite = " O "
                    "/|\\"
                    " | "
                    "/ \\"; //Default Character Sprite
char * charspawn =  "\\O/"
                    " | "
                    " | "
                    "/ \\";
char * charleft =   " O "
                    "/| "
                    " | "
                    "/ \\";
char * charright =   " O "
                     " |\\"
                     " | "
                     "/ \\";  
/* End Game Function
    Responsible for clearing the screen and outputting the appropriate text and statistics for a game-over situation.
*/
void endgame() {
    *&boolcounter[1] = true;
    char* mesg[] = {"You have lost the game!","Your Score: ","Your Time:   :  "};
    clear_screen();
    int msw = strlen(mesg[0]) / 2;
    int msh = 1;
    int msx = (screen_width() - msw) / 2;
    int msy = (screen_height() - msw) / 2;
    sprite_id losemessage = sprite_create(msx,msy,msw + msw+1,msh,mesg[0]);
    sprite_draw(losemessage);
    sprite_id scoremessage = sprite_create(msx,msy+1,strlen(mesg[1]),1,mesg[1]);
    sprite_draw(scoremessage);
    draw_int(msx+(strlen(mesg[1])),msy+1,statcounter[4]);
    sprite_id timemessage = sprite_create(msx,msy+2,strlen(mesg[2]),1,mesg[2]);
    draw_int(msx+(strlen(mesg[2]))-5,msy+2,statcounter[1]);
    draw_int(msx+(strlen(mesg[2]))-2,msy+2,statcounter[2]);
    sprite_draw(timemessage);
    show_screen();
    while (get_char() >= 0){}
    wait_char();
}
/* Change Sprite Animation
    Changes the character variable used for a sprite.
    Input: Sprite name (sprite_id), New Sprite Char (char*)
*/
void sprite_animation_change(sprite_id sprite, char* anim) {
    sprite->bitmap = anim;
}


/* Draw Top Display
    Responsible for drawing the top display and filling in the appropriate variables
*/
void draw_banner (void) {
    draw_line(0,0,screen_width()-1,0,'~'); //Top Line (1e)
    draw_line(0,4,screen_width()-1,4,'~'); //Bottom Line (1e)
    draw_string(1,1,"Student Number: REDACTED"); //Student Number (1a)
    draw_string(screen_width()/2,1,"Lives Remaining: "); //Lives Remaining (1b)
    draw_int((screen_width()/2)+18,1,statcounter[0]); //Lives Remaining (Remain Value) (1b)
    draw_string(1,2,"Play Time:   :  "); //Play Time (1c)
    draw_int(12,2,statcounter[1]); //Play Time (Minute Value) (1c)
    draw_int(15,2,statcounter[2]); //Play Time (Second Value) (1c)
    draw_string(1,3,"Score: "); //Score (1d)
    draw_int(8,3,statcounter[4]); //Score (Value) (1d)
}
/* Draw Character Sprite
    Initialises and draws the Character's sprite at a given position
    Input: Desired X Position (int), Desired Y Position (int)
*/
sprite_id character; //Init Sprite
void char_draw(int spritex, int spritey) {
    character = sprite_create(spritex,spritey,3,4,&*charsprite); //Position Sprite to passed coords
    sprite_draw(character); //Draw Sprite (2a)
}
/* Respawn Character
    Changes the character sprite to the respawn animation and moves them to the default position
*/
void respawnChar() {
    sprite_animation_change(character, charspawn);
    char_draw(3,6);
}
/* Resets the game
    Clears all time, life and score related variables and runs the function to respawn the player
*/
void resetgame() {
    *&statcounter[0] = 10;
    *&statcounter[1] = 0;
    *&statcounter[2] = 0;
    *&statcounter[3] = 0;
    *&statcounter[4] = 0;
    respawnChar();
}
/* Generate Block Sprites
    Generates the sprites for blocks with their given positions and determines the appropriate sprite image to use
    Input: Sprite Column (int), Sprite Row (int)
*/
sprite_id setup_block(int col, int row) {
    int bx = (col * 10)-10;
    int by;
    if (row == 1) {by = (row * 6) + 6;}
    else {by = (row * 6) + 7;}
    int random = rand() % 3;
    if (bx == 0) {random = 0;}
    if (bx == 0) {
        sprite_id newblock = sprite_create(0,by,7,2,blockSafeSpr);
        return newblock;
    }
    else if (bx > 7) {
        if (random == 2) {
            sprite_id newblock = sprite_create(bx,by,1,1,blockBlankSpr);
            newblock->is_visible = false;
            return newblock;
        }
        else if (random == 1) {
            sprite_id newblock = sprite_create(bx,by,7,2,blockUnsafeSpr);
            return newblock;
        }
        else {
            sprite_id newblock = sprite_create(bx,by,7,2,blockSafeSpr);
            return newblock;
        }
    }
    else {
        sprite_id newblock = sprite_create(bx,by,1,1,blockBlankSpr);
        newblock->is_visible = false;
            return newblock;
    }
    
}
/* Prepare to Create Blocks
    Iterates over the array used to hold the block sprites and call the block drawing function
*/
void setup_blocks() {
    int colC = 1; //Default Column
    int rowC = 1; //Default Row
    for (int i = 0; i < *&statcounter[5]; i++) {
        blocks[i] = setup_block(colC, rowC); //Run SetupBlock commands for i at row and col
        if (colC >= 33) { //If Column is equal to 10, reset to 1 and start new row. HARDCODED
            colC = 1;
            rowC++;
        }
        else { //Otherwise just increment column.
            colC++;
        }
    }
}
/* Draw Blocks Initial
    Redraws blocks for the first time, iterates through the array and calls the ZDK function to draw a particular sprite
    Input: Array of Sprites (sprite_id), Total number of sprites (int)
*/
void blockdraw(sprite_id bid[], int i) {
    for (int c = 0; c < i; c++) {
        sprite_draw(bid[c]); //Loop through maximum elements in Block array and call draw function
    }
}
/* Redraw Blocks after wipe
    Redraws blocks after a screen wipe, iterates through the array and calls the ZDK function to draw sprites.
    Also responsible for moving and teleporting block sprites around the screen.
    Input: Array of Sprites (sprite_id), Total number of sprites (int)
*/
void blockdrawloop(sprite_id bid[], int i) {
    for (int c = 0; c < i; c++) {
        if (bid[c]->y > 12 && bid[c]->y < 23) {
            bid[c]->x = bid[c]->x + 1;
        }
        else if (bid[c]->y == 31) {
            bid[c]->x = bid[c]->x -2;
        }
        if (bid[c]->x > screen_width()) {
            bid[c]->x = 0;
        }
        else if (bid[c]->x < 0) {
            bid[c]->x = screen_width();
        }

        sprite_draw(bid[c]); //Loop through maximum elements in Block array and call draw function
    }
}
/* Read Input
    Reads a user's key input and returns the key code
*/
int detectChar() { //Detect key for movement
    int keycode = get_char();
    return keycode;
}
/* Move player and control game functions according to given key.
    Responsible for moving the player as well as pausing the chest, resetting the game and quitting the game.
    Input: Pressed Key (int)
*/
void moveplayer( int key) {
    if (key == 'a') { //Move character left
        *&boolcounter[0] = true;
        sprite_animation_change(character, charleft);
        sprite_move(character, -1, 0);
    }
    else if (key == 'd') { //Move character right
        *&boolcounter[0] = true;
        sprite_animation_change(character, charright);
        sprite_move(character, +1, 0);
    }
    else if (key == 't') { //If 't' is pressed, check value of the global pausetreasure variable and flip it.
        if (!*&boolcounter[3]){
            *&boolcounter[3] = true;
        }
        else {
            *&boolcounter[3] = false;
        }
    }
    else if (key == 'q') {
        endgame();
    }
    else if (key == 'r') {
        resetgame();
    }
}
/* Detecting Sprite Collision
    Detects if a sprite has collided with another and returns an appropriate boolean.
    Input: Sprite 1 (sprite_id), Sprite 2 (sprite_id)
*/
bool spritecollide(sprite_id sp1, sprite_id sp2) {
    int x1 = round(sprite_x(sp1));
    int x2 = round(sprite_x(sp2));
    int y1 = round(sprite_y(sp1));
    int y2 = round(sprite_y(sp2));
    int w1 = x1 + sprite_width(sp1) - 1;
    int w2 = x2 + sprite_width(sp2) - 1;
    int h1 = y1 + sprite_height(sp1) - 1;
    int h2 = y2 + sprite_height(sp2) - 1;  
    if (x1 > w2) {return false;}
    else if (x2 > w1) {return false;}
    else if (y1 > h2) {return false;}
    else if (y2 > h1) {return false;}
    return true;

}
/* Change Life Value
    Increments or Decrements the life value counter
    Input: Increment or Decrement (char*), Amount to use (int)
*/
void lifechange(char* method, int amount) {
    if (strcmp(method,"i") == 0) {
        *&statcounter[0] = *&statcounter[0] + amount;
    }
    else if (strcmp(method,"d") == 0) {
        *&statcounter[0] = *&statcounter[0] - amount;
    }
}
/* Increment Score
    Increments Player Score
    Input: Amount to increment by (int)
*/
void incrementscore(int amount){
    *&statcounter[4] = *&statcounter[4] + amount;
}
/* Check Sprite Collisions with an array
    Iterates over an array of sprites, checks if they collide with another and performs certain actions depending on the image used for the sprite.
    Input: Array of Sprites (sprite_id), Number of Sprites (int), Sprite to check against (sprite_id)
*/
void sprcollideArray(sprite_id bid[], int i, sprite_id chara) { //Sprite Collision detector for Array-based sprite_ids
    for (int c = 0; c < i; c++) {
        if (spritecollide(chara, bid[c]) == true) {
            if (strcmp(bid[c]->bitmap,blockUnsafeSpr) == 0) { //Check if player collided with the unsafe sprite
                if (bid[c]->x == 0 && bid[c]->y == 12) {

                } else {
                    lifechange("d",1);
                    respawnChar();
                }
                
            }
            else if (strcmp(bid[c]->bitmap,blockSafeSpr) == 0) {
                if (boolcounter[0]) {incrementscore(1);}
                sprite_move(chara, 0,-1);
            }
            else {
                
                sprite_move(chara, 0,-1);
            }
            
        }
    }
}
/* Check Sprite Collisions without an array
    Used for checking with the chest
    Input: Sprite to check (sprite_id), Player Sprite (sprite_id)
*/
void sprcollideStd(sprite_id bid, sprite_id chara) { //Sprite Collision detector for Non-Array-based sprite_ids
        if (spritecollide(chara, bid) == true) {
            if (strcmp(bid->bitmap,treasurespr) == 0) {
                lifechange("i",2); //Increment Life balance
                respawnChar(); //Respawn player at default point
            }
            
    }
}
/* Spawn Treasure
    Spawns Treasure sprite at a given location
    Input: Treasure X (int), Treasure Y (int)
*/
sprite_id treasure;
void drawtreasure(int sprx, int spry) {
    treasure = sprite_create(sprx, spry, 2,2,treasurespr);
    sprite_draw(treasure);
}
/* Move Treasure
    Check Treasure location, flip the direction boolean if necessary and move the sprite according to the boolean
*/
void movetreasure() {
    if (sprite_x(treasure) <= 1) { // Read if treasure goes off left of screen and set bool.
        
        *&boolcounter[4] = true;
    }
    else if (sprite_x(treasure) >= screen_width()) { // Read if treasure goes off right of screen and set bool.
        *&boolcounter[4] = false;
        
    }
    if (!*&boolcounter[4]) { // If bool is false move to left
        sprite_move(treasure, -1, 0);
        sprite_animation_change(treasure,treasurespr);
    }
    else if (*&boolcounter[4]) { // If bool is true move to right
        sprite_move(treasure, +1, 0);
        sprite_animation_change(treasure,treasurespr2);
    }
}
/* Increments Time Counter
    Increments value on every loop (100ms) and updates the minute and second values every second
*/
void incrementTimer() { // Increment the timer before redrawing screen
    if (statcounter[3] == 1000) { //Check if timer equals 1000, program runs in 100 ms increments.
        *&statcounter[3] = 0;
        if (statcounter[2] == 59) {
            *&statcounter[2] = 0;
            *&statcounter[1] = *&statcounter[1] + 1;    
        }
        else {
            *&statcounter[2] = *&statcounter[2] + 1;
        }
    }
    *&statcounter[3] = *&statcounter[3] + 100;
}
/* Check Life total
    Check the total remaining lives and run the game-over function if necessary
*/
void playerdeathcheck() {
    if (statcounter[0] < 1) {
        endgame();
    }
}
/* Primary Loop
    Runs every 100ms to update the game and check statistics
*/
void gameloop(void) {
    *&boolcounter[0] = false;
    playerdeathcheck();
    clear_screen();
    incrementTimer();
    draw_banner();
    sprite_animation_change(character, charsprite);
    blockdrawloop(blocks, *&statcounter[5]);
    int keyC;
    keyC = detectChar();
    sprite_move(character,0,+1);
    moveplayer(keyC);
    sprcollideArray(blocks, *&statcounter[5], character);
    sprcollideStd(treasure, character);
    sprite_draw(character);
    sprite_draw(treasure);
    if (!*&boolcounter[3]) {movetreasure();}
    draw_int((screen_width()/2)+18,1,statcounter[0]); //Lives Remaining (Remain Value) (1b)
    if (sprite_y(character) > screen_height()) {
        lifechange("d",1);
        respawnChar();}
}
/* Initial Setup
    Spawns the character in the default location and the treasure in a given location
*/
void initsetup() {
    char_draw(3,7);
    drawtreasure(screen_width()-13, screen_height()-8);
}

/* Main
    Initial Function, responsible for initialising the game and maintaining the loop
*/
int main(void) {
    srand(time(NULL));
    setup_blocks();
    setup_screen();
    initsetup();
    show_screen();
    while (!*&boolcounter[1]) {
        gameloop();
        if (*&boolcounter[2]) {
            show_screen();
        }
        timer_pause(100);
    }
    cleanup_screen();
    return 0;
}