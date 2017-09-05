/**
JC Andrioli - 13/may/2015
Dual player controller board for arduino leonardo, being used here to simulate a native USB keyboard.
This code maps pins to keystrokes on a single keyboard. This pretending game is useful to make cheap
arcade machines that don't need an analog joystick. Even useful if you DO have a joystick but it's
not analog - like those crappy chinese "red ball joysticks". 
If you want to reuse this for a system WITH analog joysticks, you can do that too, but only one
joystick works reliably (although on Windows it seems to be possible with 2 joysticks too). But who
would build an arcade machine on Windows...

In order to use this with your emulators, all you need to do is configure them with your keyboard!
Remember, there will be no real joystick listed in your computers device list, drivers, etc. Forget
all that complexity. For the software running on the machine, this all really looks just like a kbd.

-- Key maps --
Each player needs at least 12 keys: start,select,up,down,left,right,a,b,x,y,l,r.
Check the 'keys' array variable below, you can see which keys I'm using.
And in some cases you might want those 2 extra keys U and J, for example NeoGeo or "strange"
arcade (MAME) games use them.
You also may want a switch to dis/enable the trigger function (turbo mode).
If you're running a linux-based arcade machine you probably want some other keys too, like
reset, ESC, TAB, SPACE, etc.

Physical connections:
1 - the leo has at most 24 usable pins, and each player uses at least 12 keys, plus the "system"
    keys... we need more pins. A clever way to achieve that is multiplexing, i.e. a matrix of 6:
          pin0|pin1|pin2|pin3|pin4|pin5|pin6|pin7
    pin 0 ####              
    pin 1 2K1  ####            
    pin 2 2K2  2P4  ####          
    pin 3 2K3  2KO  1K4  ####        
    pin 4 2K4  2OK  2RG  1P3  ####      
    pin 5 2P1  2DN  1K1  1P1  1KO  ####    
    pin 6 2P2  1P4  1K2  1P2  1OK  1LF  ####  
    pin 7 2P3  1DN  1UP  2UP  2LF  1K3  1RG  ####

         
    And that's how we get 36 inputs with 12 digital pins. And this is already coded by the
    keypad library!
**/
/*
char keys[6][6] = {                           //check arduinos HID.cpp for more keys
  {2K1,  0,    0,    0,    0,    0,    0,
   2K2,  2P4,  0,    0,    0,    0,    0,
   2K3,  2KO,  1K4,  0,    0,    0,    0,
   2K4,  2OK,  2RG,  1P3,  0,    0,    0,
   2P1,  2DN,  1K1,  1P1,  1KO,  0,    0,
   2P2,  1P4,  1K2,  1P2,  1OK,  1LF,  0,
   2P3,  1DN,  1UP,  2UP,  2LF,  1K3,  1RG}
};
*/
short state[11][11] = {                           /*button states and button indexes for joystick*/
  {0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0},
  {0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0},
  {0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0},
  {0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0},
  {0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0},
  {0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0},
  {0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0},
  {0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0},
  {0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0},
  {0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0},
  {0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0}
};
int bttns[11][11] = {                           /*button states and button indexes for joystick*/
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //0
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //1
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //2
  { 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0 }, //3
  { 0, 0, 0, 13, 0, 0, 0, 0, 0, 0, 0 }, //4
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //5
  { 0, 5, 10, 9, 0, 0, 0, 0, 0, 0, 0 }, //6
  { 0, 0, 1, 0, 0, 0, 4, 0, 0, 0, 0 }, //7
  { 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0 }, //8
  { 0, 0, 8, 7, 0, 0, 3, 14, 11, 0, 0 }, //9
  { 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0 }, //10
};
/*short bttns[11][11] = {                           
  { 1,   2,   3,  4,  5,   6,  7,   8,  9, 10, 11},
  { 11, 12,  13, 14, 15,  16, 17,  18, 19, 10, 11 },
  { 21, 22,  23, 24, 25,  26,  27,  28, 29, 10, 11 },
  { 31, 32,  33, 34, 35,  36,  37,  38, 39, 10, 11 },
  { 41, 42,  43, 44, 45,  46,  47,  48, 49, 10, 11 },
  { 51, 52,  53, 54, 55,  56,  57,  58, 59, 10, 11 },
  { 61, 62,  63, 64, 65,  66,  67,  68, 69, 10, 11 },
  { 71, 72,  73, 74, 75,  76,  77,  78, 79, 10, 11 },
  { 81, 82,  83, 84, 85,  86,  87,  88, 89, 10, 11 },
  { 91, 92,  93, 94,  95,  96,  97,  98, 99, 10, 11 },
  { 101,  2,  3,  4,  5,  6,  7,  8, 9, 10, 11 }
};*/


#include "Joystick.h"

// Create Joystick
Joystick_ Joystick;
boolean blnTurbo = true;                            // By default enable turbo (trigger / quick shot)


void setup() 
{
  Serial.begin(115200);
  for (int x = 0; x<=13; x++)
  {
      pinMode(x, INPUT_PULLUP);
  }  
  Serial.println("ArcadeMachine Joystick Simulator");
  Joystick.begin();
}

void loop() 
{  
  //delay(50);
  //Serial.println("loop...");
  for (int k=0; k<12; k++)
  {
    pinMode(k+2, OUTPUT);
    digitalWrite(k+2, LOW);
    for (int j = 0; j<12; j++)
    {
      if (j==k || j>k) continue;
      if ( digitalRead(j+2) == LOW ) //  pressed
      {
        /*Serial.print("Button ");
        Serial.print(k+j);
        Serial.print(" state ");
        Serial.print(state[k][j]);
        Serial.print(" Pin ");
        Serial.print(k);
        Serial.print(" connected to ");
        Serial.println(j);*/
  
        if (state[k][j] == false)
        {
          state[k][j] = true;
          Joystick.pressButton(bttns[j][k]);
          Serial.print("Button ");
          Serial.print(bttns[k][j]);
          Serial.print(" state ");
          Serial.print(state[k][j]);
          Serial.print(" Pin ");
          Serial.print(k);
          Serial.print(" connected to ");
          Serial.println(j);
        }
      }
      else  // button is not pressed
      {
 
        if (state[k][j] == true)
        {
          state[k][j] = false;
          Serial.print("Button ");
          Serial.print(bttns[k][j]);
          Serial.print(" state ");
          Serial.print(state[k][j]);
          Serial.print(" Pin ");
          Serial.print(k);
          Serial.print(" connected to ");
          Serial.println(j);
          Joystick.releaseButton(bttns[j][k]);
        }
      }
    }
    pinMode(k+2, INPUT_PULLUP);
  }  
}
