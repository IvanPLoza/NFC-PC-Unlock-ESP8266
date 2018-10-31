/****************************************************************************
 *  @project:     DUMP ESP8266 NFC Office implementation
 *  @file_name:   keyboard.ino
 *  @brief:       Code for trinket board to unlock and lock pc
 *  @note:        Works as USB HID device, emulating keyboard
 *  @author:      Ivan Pavao Lozancic @dump
 *  @date:        19-08-2018
 ****************************************************************************/
#include <TrinketKeyboard.h>

//INPUT PIN
#define INPUT_PIN 2

bool PC_LOCKED = true; //First state when started

/****************************************************************************
 *                            Public definitions
 ***************************************************************************/

//uint8_t Password[10{}

/****************************************************************************
 *                           Public functions
 ***************************************************************************/
/****************************************************************************
 *  @name:        UnlockPC
 *  *************************************************************************
 *  @brief:       Emulate keyboard and unlocks PC
 *  @note:
 *  *************************************************************************
 *  @param[in]:
 *  @param[out]:   
 *  @return:      nothing
 *  *************************************************************************
 *  @author:      Ivan Pavao Lozancic
 *  @date:        19-08-2018
 ***************************************************************************/
 void UnlockPC(){

  TrinketKeyboard.poll();

  TrinketKeyboard.pressKey(0, KEYCODE_BACKSPACE);
  TrinketKeyboard.pressKey(0, 0);

  delay(500);

  TrinketKeyboard.print("IplgamerVoli2/"); // Its / instead of - becasue of keyboard language 
  TrinketKeyboard.pressKey(0, 0);

  delay(100);

  TrinketKeyboard.pressKey(0, KEYCODE_ENTER);
  TrinketKeyboard.pressKey(0, 0);

  delay(2000);

 }

 /***************************************************************************
 *  @name:        LockPC
 *  *************************************************************************
 *  @brief:       Emulates keyboard and locks PC
 *  @note:
 * **************************************************************************
 *  @param[in]:
 *  @param[out]:   
 *  @return:      nothing
 *  *************************************************************************
 *  @author:      Ivan Pavao Lozancic
 *  @date:        21-08-2018
 ***************************************************************************/
 void LockPC(){

  TrinketKeyboard.poll();

  TrinketKeyboard.pressKey(KEYCODE_MOD_LEFT_GUI, KEYCODE_L);
  TrinketKeyboard.pressKey(0, 0);

  delay(2000);

 }
 /****************************************************************************
 *                            Setup function
 ***************************************************************************/
void setup()
{
  
  //Set input "trigger" pin
  pinMode(INPUT_PIN, INPUT);

  //Start USB HID device
  TrinketKeyboard.begin();
}

/****************************************************************************
 *                            Main function
 ***************************************************************************/
void loop()
{
  //Refresh so computer won't recognized it as broken device
  TrinketKeyboard.poll(); 

  if(digitalRead(INPUT_PIN) == 1){ 
    if(PC_LOCKED == true){

      UnlockPC();

      PC_LOCKED = false;
    }

    else{

      LockPC();

      PC_LOCKED = true;
    }

    delay(1000); //Safe wait
  }
} 


