#include "ST7565.h"
 
int ledPin = 13; // LED connected to digital pin 13
 
// the LCD backlight is connected up to a pin so you can turn it on & off
#define BACKLIGHT_LED 4
// Below are pin values
// CHANGE PIN VALUES ACCORDING TO WHERE YOU PLUG
#define P1_UP 11
#define P1_DN 12
#define P2_UP 3
#define P2_DN 2
#define BTN_BLK 10
#define BTN_WHT 4
 
// pin 9 - Serial data out (SID)
// pin 8 - Serial clock out (SCLK)
// pin 7 - Data/Command select (RS or A0)
// pin 6 - LCD reset (RST)
// pin 5 - LCD chip select (CS)
ST7565 glcd(9, 8, 7, 6, 5);
 
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH 16
#define LCD_WIDTH 128
#define LCD_HEIGHT 64
 
// GAME CONSTANTS
#define BALL_RADIUS LCD_WIDTH/60
#define BALL_CIRC LCD_WIDTH/10
#define PADDLE_WIDTH LCD_WIDTH/60
#define PADDLE_HEIGHT LCD_HEIGHT/4
#define BALL_SPEED 2
#define PADDLE_SPEED 4
 
struct posn {
    int x;
    int y;
};
 
struct velo_vector {
    int  vertical;
    int horizontal;
};
 
struct ball {
    struct posn b_pos;
    struct velo_vector velo;
};
 
struct paddle {
    struct posn p_pos;
};
 
struct ball game_ball;
struct paddle player_1;
struct paddle player_2;
 
/*const static unsigned char __attribute__ ((progmem)) logo16_glcd_bmp[]={
0x30, 0xf0, 0xf0, 0xf0, 0xf0, 0x30, 0xf8, 0xbe, 0x9f, 0xff, 0xf8, 0xc0, 0xc0, 0xc0, 0x80, 0x00,
0x20, 0x3c, 0x3f, 0x3f, 0x1f, 0x19, 0x1f, 0x7b, 0xfb, 0xfe, 0xfe, 0x07, 0x07, 0x07, 0x03, 0x00, };*/
 
// The setup() method runs once, when the sketch starts
 
void setup() {
    // assign input and output pins for joysicks 
    // set default (unpressed) state of inputs to HIGH 
    pinMode(P1_UP, INPUT);
    digitalWrite(P1_UP, HIGH);
    pinMode(P1_DN, INPUT);
    digitalWrite(P1_DN, HIGH);
    pinMode(P2_UP, INPUT);
    digitalWrite(P2_UP, HIGH);
    pinMode(P2_DN, INPUT);
    digitalWrite(P2_DN, HIGH);
    // assign buttons 
    pinMode(BTN_BLK, INPUT);
    pinMode(BTN_WHT, INPUT);



  
    Serial.begin(9600);
 
    #ifdef __AVR__
    Serial.print(freeRam());
    #endif
 
    // turn on backlight
    pinMode(BACKLIGHT_LED, OUTPUT);
    digitalWrite(BACKLIGHT_LED, LOW);
 
    // initialize and set the contrast to 0x18
    glcd.begin(0x18);
 
    glcd.display(); // show splashscreen
    delay(2000);
    glcd.clear();
   
    reset_game();
   
//    // draw a single pixel
//    glcd.setpixel(10, 10, BLACK);
//    glcd.display(); // show the changes to the buffer
//    delay(2000);
//    glcd.clear();
}
 
 
void loop() {
    
    //READ JOYSTICK INPUT
    int p1_joypos = digitalRead(P1_UP);
    int p2_joypos = digitalRead(P1_DN);
    int p3_joypos = digitalRead(P2_UP);
    int p4_joypos = digitalRead(P2_DN);
    int BlackButtonValue = digitalRead(BTN_BLK);
    int WhiteButtonValue = digitalRead(BTN_WHT);

    if(BlackButtonValue==HIGH){
        Serial.println("Black button is pressed");
      }
   if(WhiteButtonValue==HIGH){
        Serial.println("White button is pressed");
      }
          
    

    //Read and store positions of paddles
    int pos_1 = player_1.p_pos.y;
    int pos_2 = player_2.p_pos.y;
    
    
    if (p1_joypos==HIGH && p2_joypos==HIGH){
      player_1.p_pos.y = pos_1;
      }
    else if(p1_joypos==HIGH){ //P1_UP pin
      player_1.p_pos.y -= PADDLE_SPEED; //This moves it up
      //p2_joypos = LOW;
      }
    else if(p2_joypos == HIGH){ //p1 down pin
      player_1.p_pos.y += PADDLE_SPEED; //This moves the paddle down
      //p1_joypos = LOW;
      }

    if (p3_joypos==HIGH && p4_joypos==HIGH){
      player_2.p_pos.y = pos_2;
      }
    else if(p3_joypos==HIGH){ //P2_UP pin
      player_2.p_pos.y -= PADDLE_SPEED; //This moves it up
      //p2_joypos = LOW;
      }
    else if(p4_joypos == HIGH){ //p1 down pin
      player_2.p_pos.y += PADDLE_SPEED; //This moves the paddle down
      //p1_joypos = LOW;
      }
     
   
    if((game_ball.b_pos.x-BALL_RADIUS <= 0) || (game_ball.b_pos.x+BALL_RADIUS >= LCD_WIDTH))
    {
        glcd.clear();

        if (game_ball.b_pos.x-BALL_RADIUS <= 0){
            glcd.drawstring(2, 2, "PLAYER 2 WON");
          }
        if (game_ball.b_pos.x+BALL_RADIUS >= LCD_WIDTH){
            glcd.drawstring(2, 2, "PLAYER 1 WON");
          }        
        
        glcd.display();

        while(BlackButtonValue!=HIGH){
           BlackButtonValue = digitalRead(BTN_BLK);
           if(BlackButtonValue==HIGH){
            glcd.clear();
            reset_game();
          }
        }
    }
    else if(player_1.p_pos.y<=0){
        player_1.p_pos.y=pos_1;
      }
    else if(player_2.p_pos.y<=0){
        player_2.p_pos.y=pos_2;
      }
    else if(game_ball.b_pos.y>=63){
        game_ball.velo.vertical *= -1;
      }
    else if(game_ball.b_pos.y<=2){
        game_ball.velo.vertical *= -1;
      }
      //player 2, add 1 later
    else if(game_ball.b_pos.x == 124 && game_ball.b_pos.y >= player_2.p_pos.y && game_ball.b_pos.y <= (player_2.p_pos.y+16)){
        game_ball.velo.horizontal *= -2;
        
      }
    else if(game_ball.b_pos.x <= 4 && game_ball.b_pos.y >= player_1.p_pos.y && game_ball.b_pos.y <= (player_1.p_pos.y+16)){
        game_ball.velo.horizontal *= -2;
        
      }
   
    game_ball.b_pos.y += game_ball.velo.vertical;
    game_ball.b_pos.x += game_ball.velo.horizontal;
   
    draw_board();
}
 
#ifdef __AVR__
// this handy function will return the number of bytes currently free in RAM, great for debugging!  
int freeRam(void) {
    extern int __bss_end;
    extern int * __brkval;
    int free_memory;
    if ((int) __brkval == 0) {
        free_memory = ((int) & free_memory) - ((int) & __bss_end);
    } else {
        free_memory = ((int) & free_memory) - ((int) __brkval);
    }
    return free_memory;
}
#endif

//void start_game(){
//    game_ball.velo.vertical = BALL_SPEED; //CHANGE THIS TO BALL SPEED OR SOMETHING
//    game_ball.velo.horizontal = 2; //SUBJECT TO CHANGE
//  }
 
void reset_game(){
    // BALL SETUP
    game_ball.b_pos.x = LCD_WIDTH/2;
    game_ball.b_pos.y = LCD_HEIGHT/2;
    game_ball.velo.vertical = BALL_SPEED; //CHANGE THIS TO BALL SPEED OR SOMETHING
    game_ball.velo.horizontal = 2; //SUBJECT TO CHANGE
   
    // PADDLE_1 SETUP
    player_1.p_pos.x = 0 + PADDLE_WIDTH;
    player_1.p_pos.y = LCD_HEIGHT/2 - PADDLE_HEIGHT/2;
   
    // PADDLE_2 SETUP
    player_2.p_pos.x = LCD_WIDTH - PADDLE_WIDTH*2;
    player_2.p_pos.y = LCD_HEIGHT/2 - PADDLE_HEIGHT/2;
    draw_board();
}
 
void draw_board(){
    glcd.clear();
        // draw a black circle, 10 pixel radius, at location (32,32)
    //glcd.fillcircle(32, 32, 10, BLACK);
    
    glcd.fillcircle(game_ball.b_pos.x, game_ball.b_pos.y, BALL_RADIUS, BLACK);
    glcd.fillrect(player_1.p_pos.x, player_1.p_pos.y,
                PADDLE_WIDTH, PADDLE_HEIGHT, BLACK);
    glcd.fillrect(player_2.p_pos.x, player_2.p_pos.y,
                PADDLE_WIDTH, PADDLE_HEIGHT, BLACK);
    glcd.display();    
    delay(100);
}
