//**************************************************************************************************
// Initialize the preferences op ESP32-babysleeper.  They can be edited later by the webinterface. *
// based on the ESP32-radio preferences                                                            *
//**************************************************************************************************
#include <Preferences.h>

// Note: Namespace name is limited to 15 chars.  Same name must be used in ESP32-radio.
# define APPNAME "BabySleeper"
// Adjust size of buffer to the longest expected string for nvsgetstr
#define NVSBUFSIZE 150

/* create an instance of Preferences library */
Preferences preferences;

void setup() {
  Serial.begin(115200);
  Serial.println();
  /* Start a namespace APPNAME
  in Read-Write mode: set second parameter to false 
  Note: Namespace name is limited to 15 chars */
  preferences.begin ( APPNAME, false ) ;              // Open for read/write
  preferences.clear() ;                               // Remove all preferences under opened namespace


  // Store the preferences for ESP32-babysleeper
  /*
  few examples:

  //  preferences.putString ( "gpio_00",  "uppreset = 1" ) ;
  //  preferences.putString ( "gpio_12",  "upvolume = 2" ) ;
  //  preferences.putString ( "gpio_13",  "downvolume = 2" ) ;

  preferences.putString ( "mqttbroker",   "none" ) ;
  preferences.putString ( "mqttport",     "1883" ) ;
  preferences.putString ( "mqttuser",     "none" ) ;
  preferences.putString ( "mqttpasswd",   "none" ) ;
  preferences.putString ( "mqttprefix",   "none" ) ;
  //
  preferences.putString ( "wifi_00",     "gozal_2.4/asdffdsa" ) ;
  preferences.putString ( "wifi_01",     "SSID2/YYYYYY" ) ;
  //
    //  preferences.putString ( "pin_ir",      "35                                     # GPIO Pin number for IR receiver VS1838B" ) ;
    //  preferences.putString ( "ir_40BF",     "upvolume = 2" ) ;
    //  preferences.putString ( "ir_C03F",     "downvolume = 2" ) ;
  //
  */


//******************************************************************************************
//                              I R                                                        *
//******************************************************************************************
// after getting to know how to work with IR i can change is here to the right parameters..
// preferences.putString ( "pin_ir",      "35                                     # GPIO Pin number for IR receiver VS1838B" ) ;
//  preferences.putString ( "ir_40BF",     "upvolume = 2" ) ;
//  preferences.putString ( "ir_C03F",     "downvolume = 2" ) ;

//******************************************************************************************
//                              W I F I                                                    *
//******************************************************************************************
  preferences.putString ( "ssid",     "gozal_2.4" ) ;
  preferences.putString ( "password", "asdffdsa" ) ;
  preferences.putULong("hexRGB",0x00ff00);    // initial color
  /* Close the Preferences */
  preferences.end() ;
  delay ( 1000 ) ;
}

//******************************************************************************************
//                              L O O P                                                    *
//******************************************************************************************
void loop()
{
  Serial.println ( "ESP32_Babysleeper_init completed..." ) ;
  delay ( 10000 ) ;
}
