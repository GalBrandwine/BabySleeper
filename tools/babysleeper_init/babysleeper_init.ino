//**************************************************************************************************
// Initialize the preferences op ESP32-babysleeper.  They can be edited later by the webinterface. *
// based on the ESP32-radio preferences                                                            *
//**************************************************************************************************
#include <Preferences.h>

// Note: Namespace name is limited to 15 chars.  Same name must be used in ESP32-radio.
#define NAME "ESP32Babysleeper"
// Adjust size of buffer to the longest expected string for nvsgetstr
#define NVSBUFSIZE 150

/* create an instance of Preferences library */
Preferences preferences;

void setup() {
  Serial.begin(115200);
  Serial.println();
  /* Start a namespace NAME
  in Read-Write mode: set second parameter to false 
  Note: Namespace name is limited to 15 chars */
  preferences.begin ( NAME, false ) ;                 // Open for read/write
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
//preferences.putString ( "pin_ir",      "35                                     # GPIO Pin number for IR receiver VS1838B" ) ;
//  preferences.putString ( "ir_40BF",     "upvolume = 2" ) ;
//  preferences.putString ( "ir_C03F",     "downvolume = 2" ) ;

//******************************************************************************************
//                              W I F I                                                    *
//******************************************************************************************
  preferences.putString ( "wifi_00",     "gozal_2.4/asdffdsa" ) ;
  preferences.putString ( "wifi_01",     "SSID2/YYYYYY" ) ;
  
  /* if you want to remove all preferences under opened namespace uncomment it */
  //preferences.clear();

  /* if we want to remove the reset_times key uncomment it */
  //preferences.remove("reset_times");

  /* get value of key "reset_times", if key not exist return default value 0 in second argument
  Note: Key name is limited to 15 chars too */
  //unsigned int reset_times = preferences.getUInt("reset_times", 0);

  /* we have just reset ESP then increase reset_times */
  //reset_times++;

  //Serial.printf("Number of restart times: %d\n", reset_times);

  /* Store reset_times to the Preferences */
  //preferences.putUInt("reset_times", reset_times);

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
