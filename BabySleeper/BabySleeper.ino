//****************************************************************************************************************************************************
//*  ESP32_BabySleeper -- A baby sleeping monitoring for ESP32, IR (transmitter and reciever - TBD), AM2302 sensor (DHT22), MQ sensor,and WS2812 LED.*
//*                 By Gal Brandwine.                                                                                                                *
//*                 Based on the ESP32-radio sketch                                                                                                  *
//****************************************************************************************************************************************************

// Brief description of the program:
// First a suitable WiFi network is found and a connection is made.
// If no connection is made, a Acces point wil be extablished with ssid: APPNAME, password: APPNAME


// Wiring. Note that this is just an example.  Pins (except 18,19 and 23 of the SPI interface)
// can be configured in the config page of the web interface.
// ESP32dev Signal  Wired to MQ-2        Wired to DHT22      Wired to Newpixle
// -------- ------  --------------      -------------------  ---------------------
// GPIO32           A0                  -                     -       
// GPIO5            -                   -                     -       
// GPIO4            -                   DHT22 pin             -       
// GPIO2            -                   -                     -       
// GPIO22           -                   -                     -       
// GPIO16   RXD2    -                   -                     -       
// GPIO17   TXD2    -                   -                     -       
// GPIO18   SCK     -                   -                     -       
// GPIO19   MISO    -                   -                     -

// GPIO23   MOSI    -                   -                     -       
// GPIO15           -                   -                     -       
// GPI03    RXD0    -                   -                     -       
// GPIO1    TXD0    -                   -                     -       
// GPIO34   -       -                   -                     -       
// GPIO35   -       -                   -                     -       
// GPIO25   -       -                   -                     -       
// GPIO26   -       -                   -                     -       
// GPIO27   -       -                   -                     Data pin       
// -------  ------  ---------------     -------------------  ------   
// GND      -       Gnd                 Gnd                   Gnd
// VCC 5 V  -       Vcc                 -                     Vcc
// VCC 3.3 V-       -                   Vcc                   -  
// EN       -       -                   -                     -  

// Import required libraries
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Preferences.h>

#include "DHT.h"
#define DHTPIN 4


# define APPNAME "BabySleeper"

//**********************************************************************************************************
//*                               N E O P I X L E includes & instantiations                                *
//* When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.  *
//* Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest   *
//**********************************************************************************************************
#include <NeoPixelBrightnessBus.h> // instead of NeoPixelBus.h

#define SPIRAL 1
int effectNum = 0;
int BRIGHTNESS;
String BLUELIGHT ="ON";
double neoPixleColor = 0x66ffff;
String hexval;

const uint16_t NUM_OF_PIXLES = 22; // this example assumes 3 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 27;  // make sure to set this to the correct pin, ignored for Esp8266


#define FLLYDIMMED 0.95 // if 1.0 - than when "breathing" will complity turn off the NeoPixles
#define colorSaturation 255 // saturation of color constants

RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

// Make sure to provide the correct color order feature
// for your NeoPixels
NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod> pixels(NUM_OF_PIXLES, PixelPin);

// you loose the original color the lower the dim value used
// here due to quantization
const uint8_t c_MinBrightness = 8; 
const uint8_t c_MaxBrightness = 255;



//**************************************************************************************
//*                               D H T 22 includes & instantiations                   *
//**************************************************************************************
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);


//**************************************************************************************
//*                               MQ-2 definitions                                     *
//**************************************************************************************
#define MQ_PIN 32              //define which analog input channel you are going to use (NOTE - CANT USE ADC2 PINS WHEN WIFI CONNECTED)



//**************************************************************************************
//*                               P R E F E R E N C E S
//**************************************************************************************
/* create an instance of Preferences library */
Preferences preferences;


//**************************************************************************************
//*                                 W I F I                                            *
//* Replace with your network credentials                                              *
//* If wifi fails to connect, than accespoint will be established.                     *
//**************************************************************************************
#define WIFITRYNUM 3
bool isConnected = false;
int last_IPnum = 0;                 // When button is pushed, blink lastNumOfIP times, to indicate the device IP (it should be 192.168.1.X)
const int IpButton = 17;
const int rstButton = 5;
bool interrupted = false;           // IF user pressedbutton to check what is the wifi ip, then interruped = true
// Set Access Point IP
IPAddress   apIP(10, 10, 10, 1);


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

void IRAM_ATTR ipButtonInterrupt() {
  Serial.println("interrupted!");
  interrupted = true;
}
void IRAM_ATTR rstButtonInterrupt() {
  Serial.println("interrupted! - reseting");
  ESP.restart();
}

//* Red: 0xff0000
//* Green: 0x00ff00
//* Blue: 0x0000ff
typedef struct RGB
{
        double r;
        double g;
        double b;
};


struct RGB colorConverter(int hexValue)
{
 struct RGB rgbColor;
 String storedBluelight;
 
 preferences.begin(APPNAME, false);
 storedBluelight = preferences.getString("bluelight","ON"); // return ON if not exist.
 preferences.end();
 
 rgbColor.r = ((hexValue >> 16) & 0xFF);// / 255.0; // Extract the RR byte
 //Serial.println(rgbColor.r);
 rgbColor.g = ((hexValue >> 8) & 0xFF);// / 255.0; // Extract the GG byte
 //Serial.println(rgbColor.g);
 if (BLUELIGHT == "OFF"){
  rgbColor.b = 0;
 }
 else{
  rgbColor.b = ((hexValue) & 0xFF);// / 255.0; // Extract the BB byte
  ;
 }
 return (rgbColor); 
}


void neoPixleSolid(int hexValue,uint8_t brightness, int delayinms){
  RGB tempRGB = colorConverter(hexValue);

  RgbColor tempcolor(tempRGB.r, tempRGB.g, tempRGB.b);
  Serial.print("Solid color.r: ");Serial.println(tempRGB.r);
  Serial.print("Solid color.g: ");Serial.println(tempRGB.g);
  Serial.print("Solid color.b: ");Serial.println(tempRGB.b);
  //pixelsClear();
  //pixels.clear(); // Set all pixel colors to 'off'
  delay(100);
  for (int i=0; i < NUM_OF_PIXLES; i++){
      pixels.SetPixelColor(i, tempcolor);
  }
  pixels.SetBrightness(brightness);
  pixels.Show();
  
  if (delayinms >=1){
    delay(delayinms);
    pixelsClear();
    //pixels.clear(); // Set all pixel colors to 'off'
  }
}

void neoPixelSpiral(int hexValue, int SpiralSpeedInMS){
  RGB tempRGB = colorConverter(hexValue);
  RgbColor tempcolor(tempRGB.r, tempRGB.g, tempRGB.b);
  RgbColor dimmed(0);
  
  uint8_t brightness = pixels.GetBrightness();
  if (brightness != BRIGHTNESS){
    brightness = BRIGHTNESS;
  }

  pixelsClear();
  delay(100);

  
  pixels.SetBrightness(brightness);

  Serial.println("Spiraling");
  Serial.print("neoPixleBreath brightness: ");Serial.println(brightness);
  Serial.print("Solid color.r: ");Serial.println(tempRGB.r);
  Serial.print("Solid color.g: ");Serial.println(tempRGB.g);
  Serial.print("Solid color.b: ");Serial.println(tempRGB.b);

  int led = 0;
  while(effectNum == SPIRAL and interrupted == false){
    
//    pixels.SetPixelColor(led, tempcolor);
//    pixels.Show();
    delay(SpiralSpeedInMS);
    // Fade in
    for(float progress=1; progress >= 0; progress = progress - 0.01){
      // apply dimming
      dimmed = tempcolor.LinearBlend(tempcolor, black, progress);
      delay(5);
      pixels.SetPixelColor(led, dimmed);
      pixels.Show();
    }

    // Fade out
    for(float progress=0.0; progress <= 1; progress = progress + 0.01){
      // apply dimming
      dimmed = tempcolor.LinearBlend(tempcolor, black, progress);
      delay(5);
      pixels.SetPixelColor(led, dimmed);
      pixels.Show();
    }

    
    led = ++led%NUM_OF_PIXLES;
//    for (int led=0; led < NUM_OF_PIXLES; led++){
//      // turn off the pixels
//      pixels.SetPixelColor(led, tempcolor);
//      pixels.Show();
//      delay(SpiralSpeedInMS);
//    }
  }
}


void neoPixleBreath(int hexValue, int breathingSpeedInMS){
  
  RGB tempRGB = colorConverter(hexValue);
  RgbColor tempcolor(tempRGB.r, tempRGB.g, tempRGB.b);
  RgbColor dimmed(0);
  
  
  uint8_t brightness = pixels.GetBrightness();
  if (brightness != BRIGHTNESS){
    brightness = BRIGHTNESS;
  }
  pixels.SetBrightness(brightness);

  Serial.println("Breathing");
  Serial.print("neoPixleBreath brightness: ");Serial.println(brightness);
  Serial.print("Solid color.r: ");Serial.println(tempRGB.r);
  Serial.print("Solid color.g: ");Serial.println(tempRGB.g);
  Serial.print("Solid color.b: ");Serial.println(tempRGB.b);
  
  // ------------------------------------------------------------------------
  // LinearBlend between two colors by the amount defined by progress variable
  // left - the color to start the blend at
  // right - the color to end the blend at
  // progress - (0.0 - 1.0) value where 0 will return left and 1.0 will return right
  //     and a value between will blend the color weighted linearly between them
  // ------------------------------------------------------------------------

  // Fade out
  for(float progress=0.0; progress <= FLLYDIMMED and interrupted == false; progress = progress + 0.01){
    // apply dimming
    dimmed = tempcolor.LinearBlend(tempcolor, black, progress);
    delay(breathingSpeedInMS);
    for (int i=0; i < NUM_OF_PIXLES; i++){
      pixels.SetPixelColor(i, dimmed);
    }
    pixels.Show();
  }

  // Fade in
  for(float progress=FLLYDIMMED; progress >= 0 and interrupted == false; progress = progress - 0.01){
    // apply dimming
    dimmed = tempcolor.LinearBlend(tempcolor, black, progress);
    delay(breathingSpeedInMS);
    for (int i=0; i < NUM_OF_PIXLES; i++){
      pixels.SetPixelColor(i, dimmed);
    }
    pixels.Show();
  }
}


void NeoPixleBlink(int numOfBlinks, int color){
  uint8_t brightness = pixels.GetBrightness();
  
  pixelsClear();
  delay(100);
  
  neoPixleSolid(color,brightness, -1);
  for (int i=0; i < numOfBlinks; i++){
    pixels.SetBrightness(c_MinBrightness);
    pixels.Show();
    delay(500);
    pixels.SetBrightness(BRIGHTNESS);
    pixels.Show();
    delay(1000);  
  }
}


void pixelsClear(){
  for (int i=0; i < NUM_OF_PIXLES; i++){
    // turn off the pixels
    pixels.SetPixelColor(i, black);
  }
  pixels.Show();
}


bool wifiConnect(){
  int tries=0;

  WiFi.disconnect(true) ;                               // After restart the router could
  WiFi.softAPdisconnect(true) ;                         // still keep the old connection
  delay ( 100 ) ; 
  

  /* get value of key "ssid", if key not exist return default value "NOT_EXIST" in second argument
  Note: Key name is limited to 15 chars too */
  preferences.begin(APPNAME, false);
  String ssidFrompreft = preferences.getString("ssid", "NOT_EXIST");
  String passwordFromPrefs = preferences.getString("password", "NOT_EXIST");
  
  Serial.println("Wifi cardentials from Preferences: ");
  Serial.println(ssidFrompreft);
  Serial.println(passwordFromPrefs);
  
  WiFi.begin(ssidFrompreft.c_str(), passwordFromPrefs.c_str());
  
  while (WiFi.status() != WL_CONNECTED and tries <= WIFITRYNUM) {
    Serial.println("Connecting to WiFi..");
    neoPixleBreath(0xff0000, 10); //red
    tries++;
  }
  
  isConnected = (WiFi.status() == WL_CONNECTED);
  if (isConnected){
    // Print ESP32 Local IP Address
    IPAddress broadCast = WiFi.localIP();
    Serial.println(broadCast);
    last_IPnum = broadCast[3];
    int blue = 0x9955ff;
    
    neoPixleSolid( 0X00ff00,BRIGHTNESS, 3000); // solid green for 3 seconds
    NeoPixleBlink(last_IPnum, blue); // blink in blue - the value of IP's last number - so user can enter the ip to browser: 192.168.1.X
    pixelsClear(); // Set all pixel colors to 'off'
  }
  else{
    neoPixleSolid( 0Xff0000,BRIGHTNESS, 3000); // solid red for 3 seconds
    Serial.print( "WiFi Failed!  Trying to setup AP with name ");Serial.print(APPNAME);Serial.print(" and password ");Serial.print(APPNAME);Serial.println(".");
    WiFi.disconnect(true); //Disable STA
    

    // Set Access Point configuration
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));   // subnet FF FF FF 00
    WiFi.softAP(APPNAME,APPNAME);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");Serial.println(IP);
    
    neoPixleSolid( 0Xff6600,BRIGHTNESS, -1); // solid orange until to inform that AP is on
  }

  /* Close the Preferences */
  preferences.end();
  return isConnected; // if connected return true;
}


String readDHTTemperature() {
  // use the functions which are supplied by library.
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Convert temperature to Fahrenheit
  //t = 1.8 * t + 32;
  
  if (isnan(t)) {    
    Serial.println("Failed to read from AM2302 sensor!");
    return "Reading..";
  }
  else {
    Serial.println(t);
    return String(t);
  }
}


String readDHTHumidity() {
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from AM2302 sensor!");
    return "Reading..";
  }
  else {
    Serial.println(h);
    return String(h);
  }
}


String readMq2() {
  
  int MQ2_analogRead = analogRead(MQ_PIN);

  Serial.print("MQ2_analogRead : ");
  Serial.println(MQ2_analogRead);

  return String(MQ2_analogRead);  
}


String readDHTPressure() {
  float p = 00000; // Dummy pressure sensor
  if (isnan(p)) {
    Serial.println("Failed to read from BME280 sensor!");
    return "";
  }
  else {
    Serial.println(p);
    return String(p);
  }
}


// Replaces all placeholders
String processor(const String& var){
  Serial.println(var);
  if (var == "WIFISTATUS"){
    return isConnected ? "YES" : "NO";
  }
  if(var == "GAS"){
    return readMq2();
  }
  else if(var == "TEMPERATURE"){
    return readDHTTemperature();
  }
  else if(var == "HUMIDITY"){
    return readDHTHumidity();
  }
  else if (var == "HEXVALUE"){
    
    preferences.begin(APPNAME, false);
    hexval = preferences.getString("hexRGBstr","f0f0f0");
    preferences.end();
    return "#"+hexval; // A solution for setting the value of thml's color picker.
  }
  else if (var == "BLUELIGHT"){
    String temp;
    preferences.begin(APPNAME, false);
    temp = preferences.getString("bluelight","ON"); // return ON if not exist.
    preferences.end();
    return temp;
  }
  return String();
}


void setup(){

  char *ptr; // needed to convert pulled string represanting hexvalue into hexadecimal.
 
  Serial.begin(115200);
  Serial.println("BabySleeper Setup!");
  

  pixels.Begin(); // This initializes the NeoPixel library.
  
  // get brightness from Preferences, if not-exist return: 20
  preferences.begin(APPNAME, false);
  BRIGHTNESS  = preferences.getInt("brightness",20);      // its more convinient to store string than actual value, because later on we need to send that value as a string to the server.
  hexval = preferences.getString("hexRGBstr","f0f0f0");   // Try to restore last color picked from web.
  preferences.end();

  // setting restored color to NeoPixle.
  neoPixleColor = strtoul(hexval.c_str(), &ptr, 16); // convert value fromt string to long (rgb values are in hexadecimel)
  pixels.SetBrightness(BRIGHTNESS);
  
  dht.begin(); //call begin to start sensor

  // Setting pins and interrupts.
  pinMode(MQ_PIN, INPUT); // MQ-2 analog pin
  pinMode(IpButton, INPUT_PULLUP);
  pinMode(rstButton, INPUT_PULLUP);
  attachInterrupt(IpButton, ipButtonInterrupt, FALLING); // If pushed, device will blink the last IP number: 192.168.1.X
  attachInterrupt(rstButton, rstButtonInterrupt, FALLING); // If pushed, device will restart
  
  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  // Connect to Wi-Fi.
  isConnected = wifiConnect();

  
  // Route for root / web page.
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html", false, processor);   
  });

  
  // Route to load style.css file.
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });


  server.on("/wifiCardentials", HTTP_GET, [](AsyncWebServerRequest *request){
    // store cardentials in Preferences(or NVS)
    // perform reboot
    
    bool ssidFlag = false;
    bool pswFlag = false;
    int paramsNr = request->params();
    
    preferences.begin(APPNAME, false);
    for(int i=0;i<paramsNr;i++){
     AsyncWebParameter* p = request->getParam(i);
     
     if (p->name() == "ssid"){
      ssidFlag = true;
      preferences.putString("ssid", p->value());
     }
     else if (p->name() == "psw" and p->value().length() >=8 ){ // length of password greather than 8, minimum size of WPA encryption key.
      pswFlag = true;
      preferences.putString("password", p->value());
     }
    }
  
    if (pswFlag == true and ssidFlag == true){
      request->send_P(200, "text/plain", "Cardential recieved. RESTARTING...");
      preferences.end();                                    // Close the Preferences
      WiFi.disconnect() ;                                   // After restart the router could
      WiFi.softAPdisconnect(true) ;                         // still keep the old connection
      delay(100);
      ESP.restart();
    }
    else{
      request->send_P(406, "text/plain", "Cardential not legal, try again.");
    }
  });

  
  server.on("/hexFromFevcolor", HTTP_GET, [](AsyncWebServerRequest *request){
    int paramsNr = request->params();
    String value;
    
    for(int i=0;i<paramsNr;i++){
  
     AsyncWebParameter* p = request->getParam(i);
     value = p->value();
     const char* str = p->value().c_str();
     char *ptr;
     neoPixleColor = strtoul(str, &ptr, 16); // convert value fromt string to long (rgb values are in hexadecimel)
    }
    
    
    preferences.begin(APPNAME, false);
    preferences.putString("hexRGBstr",value);     // its more convinient to store string than actual value, because later on we need to send that value as a string to the server.
    preferences.end();
    
    request->send_P(200, "text/plain", "new RGB value recieved");
  });

  server.on("/brightness", HTTP_GET, [](AsyncWebServerRequest *request){
    int paramsNr = request->params();
    for(int i=0;i<paramsNr;i++){
  
     AsyncWebParameter* p = request->getParam(i);
     BRIGHTNESS = p->value().toInt();
     Serial.print("BRIGHTNESS: ");Serial.println(BRIGHTNESS);
     // store brightness in preferences;
     preferences.begin(APPNAME, false);
     preferences.putInt("brightness",BRIGHTNESS);     // its more convinient to store string than actual value, because later on we need to send that value as a string to the server.
     preferences.end();
    }  
    request->send_P(200, "text/plain", "Brightness changed");
  });
  
  server.on("/bluelightON", HTTP_GET, [](AsyncWebServerRequest *request){
    BLUELIGHT = "ON";
    preferences.begin(APPNAME, false);
    preferences.putString("bluelight",BLUELIGHT);
    preferences.end();
    request->send(SPIFFS, "/index.html", String(), false, processor);   // then go to html, and update the graphics
  });
  server.on("/bluelightOFF", HTTP_GET, [](AsyncWebServerRequest *request){
    BLUELIGHT = "OFF";
    preferences.begin(APPNAME, false);
    preferences.putString("bluelight",BLUELIGHT);
    preferences.end();
    request->send(SPIFFS, "/index.html", String(), false, processor);   // then go to html, and update the graphics
  });

  // Read AIR Quality sensors
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTTemperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTHumidity().c_str());
  });
  server.on("/gas", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readMq2().c_str());
  });


  server.on("/ledeffect", HTTP_GET, [](AsyncWebServerRequest *request){
     AsyncWebParameter* p = request->getParam(0);
     effectNum = p->value().toInt();
     Serial.print("EffectNum: ");Serial.println(effectNum);
     // store brightness in preferences;
//     preferences.begin(APPNAME, false);
//     preferences.putInt("brightness",BRIGHTNESS);     // its more convinient to store string than actual value, because later on we need to send that value as a string to the server.
//     preferences.end();
      
    request->send_P(200, "text/plain", "Effect changed");
  });
  
  // Start server
  server.begin();
}


void loop(){

  if (interrupted == true){
    int blue = 0x9955ff;
    NeoPixleBlink(last_IPnum, blue); // blink in blue - the value of IP's last number - so user can enter the ip to browser: 192.168.1.X
    pixelsClear(); // Set all pixel colors to 'off'
    
    interrupted = false;
    
  }
  if (isConnected == true){
    
    
    // do something different depending on the range value:
    switch (effectNum) {
      case 0:    // your hand is on the sensor
        //Serial.println("Breathing");
        neoPixleBreath(neoPixleColor, 100); 
        break;
      case 1:    // your hand is close to the sensor
        //Serial.println("Spiral");
        neoPixelSpiral(neoPixleColor, 100); 
        break;
    }
  }

}
