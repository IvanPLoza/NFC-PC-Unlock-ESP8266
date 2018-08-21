/****************************************************************************
 *  @project:     DUMP ESP8266 NFC Office implementation
 *  @file_name:   keyboard.ino
 *  @brief:       Code for trinket board to unlock and lock pc
 *  @note:        Works as USB HID device, emulating keyboard
 *  @author:      Ivan Pavao Lozancic @dump
 *  @date:        19-08-2018
 ****************************************************************************/
 #include <TrinketKeyboard.h>

 bool PC_LOCKED = true;

/****************************************************************************
 *                            Public definitions
 ***************************************************************************/

//uint8_t Password[10{}

/****************************************************************************
 *                            Public functions
 ***************************************************************************/
/****************************************************************************
 *  @name:        UnlockPC
 *  *************************************************************************
 *  @brief:       Emulates keyboard and unlocks PC
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

  delay(100);

  TrinketKeyboard.print("Dump.12345");
  TrinketKeyboard.pressKey(0, 0);

  delay(100);

  TrinketKeyboard.pressKey(0, KEYCODE_ENTER);
  TrinketKeyboard.pressKey(0, 0);

  delay(2000);

  return;

 }

 /****************************************************************************
 *  @name:        LockPC
 *  *************************************************************************
 *  @brief:       Emulates keyboard and locks PC
 *  @note:
 *  *******************************el
 * Dump.12345
 * 
 * ******************************************
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

  return;
 }
 /****************************************************************************
 *                            Setup function
 ***************************************************************************/
void setup()
{

  // start USB stuff
  TrinketKeyboard.begin();

  //Signal input pin
  pinMode(0, INPUT);

}

/****************************************************************************
 *                            Main function
 ***************************************************************************/
void loop()
{
  if(digitalRead(0) == HIGH && PC_LOCKED == true){
    UnlockPC();
  }
  else if(digitalRead(0) == HIGH && PC_LOCKED == false){
    LockPC();
  }
}
