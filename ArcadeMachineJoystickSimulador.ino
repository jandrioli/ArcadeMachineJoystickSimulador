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

20171205 Now changing the whole design to remove charlieplexing buttons and implement direct
connection to parallel-load shift registers (parallel-to-serial). Using HC165 as chip.

check shiftReader.h for more info 
*/


#include "Joystick.h"
#include "shiftReader.h"

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
  
  initShiftReader();
    
  Serial.println("ArcadeMachine Joystick Simulator");
  Joystick.begin();
}

void loop() 
{ 
  // de-boucing techniques are getting quite advanced nowadays... 
  delay(50);
  
  {
    /* Read the state of all zones.
    */
    pinValues = read_shift_regs();

    /* If there was a chage in state, display which ones changed.
    */
    if(pinValues != oldPinValues)
    {
        Serial.print("*Pin value change detected*\r\n");
        display_pin_values();
        oldPinValues = pinValues;
    }

    delay(POLL_DELAY_MSEC);
  }

  Serial.println("===================== WARNING: TODO validate change in reading button presses! ==================");
  for (int k=0; k<=31; k++)
  {
    bool bOld = bitRead(oldPinValues, k), 
      bNew = bitRead(pinValues, k);

    if (bOld != bNew)
    {
      if ( bNew == LOW ) //  pressed
      {
        {
          Joystick.pressButton(k);
          Serial.print("Button ");
          Serial.print(" state ");
          Serial.print(" Pin ");
          Serial.println(k);
        }
      }
      else  // button is not pressed
      {
        {
          Serial.print("Button ");
          Serial.print(" state ");
          Serial.print(" Pin ");
          Serial.println(k);
          Joystick.releaseButton(k);
        }
      }
    }
  }  
}
