//**************************************************************************************************************************************************
//*  ESP32_BabySleeper -- A baby sleeping monitoring for ESP32, IR (transmitter and reciever), AM2302 sensor, MQ sensor,and WS2812 LED.            *
//*                 By Gal Brandwine.                                                                                                              *
//*                 Based on the ESP32-radio sketch                                                                                                *
//**************************************************************************************************************************************************
// ESP32 libraries used:
//  - WiFiMulti
//  - nvs - i've decidid not to use NVS - there's no need for that
//  - ArduinoOTA
//  - FS
//  - update

// Brief description of the program:
// First a suitable WiFi network is found and a connection is made.
// Then a connection will be made to a shoutcast server.  The server starts with some

// info in the header in readable ascii, ending with a double CRLF, like:
//  icy-name:Classic Rock Florida - SHE Radio
//  icy-genre:Classic Rock 60s 70s 80s Oldies Miami South Florida
//  icy-url:http://www.ClassicRockFLorida.com
//  content-type:audio/mpeg
//  icy-pub:1
//  icy-metaint:32768          - Metadata after 32768 bytes of MP3-data
//  icy-br:128                 - in kb/sec (for Ogg this is like "icy-br=Quality 2"
//

// For configuration of the WiFi network(s): see the global data section further on.
//
// Wiring. Note that this is just an example.  Pins (except 18,19 and 23 of the SPI interface)
// can be configured in the config page of the web interface.
// ESP32dev Signal  Wired to IR_trans        Wired to IR_rec AM2302   Wired to the rest
// -------- ------  --------------      -------------------  ------   ---------------
// GPIO32           -                   -                     -       -
// GPIO5            -                   -                     -       -
// GPIO4            -                   -                     -       -
// GPIO2            -                   -                     -       -
// GPIO22           -                   -                     -       -
// GPIO16   RXD2    -                   -                     -       -
// GPIO17   TXD2    -                   -                     -       -
// GPIO18   SCK     -                   -                     -       -
// GPIO19   MISO    -                   -            MISO    -192.168.4.1

// GPIO23   MOSI    -                   -                     -       -
// GPIO15           -                   -                     -       -
// GPI03    RXD0    -                   -                     -       Reserved serial input
// GPIO1    TXD0    -                   -                     -       Reserved serial output
// GPIO34   -       -                   -                     -       Optional pull-up resistor
// GPIO35   -       -                   -                     -       Infrared receiver VS1838B
// GPIO25   -       -                   -                     -       -
// GPIO26   -       -                   -                     -       -
// GPIO27   -       -                   -                     -       -
// -------  ------  ---------------     -------------------  ------   ----------------
// GND      -       -                   -                     -       Power supply GND
// VCC 5 V  -       -                   -                     -       Power supply
// VCC 5 V  -       -                   -                     -       Power supply
// EN       -       -                   -                     -       -
//#include <WiFi.h>
//#include <WiFiClient.h>
//#include <WebServer.h>
//#include <ESPmDNS.h>
//#include <Update.h>
//
//
////**************************************************************************************************
//// Pages, CSS and data for the webinterface.                                                       *
////**************************************************************************************************
//#include "about_html.h"
//#include "config_html.h"
//#include "index_html.h"
////#include "mp3play_html.h"
////#include "radio_css.h"
//#include "favicon_ico.h"
//#include "defaultprefs.h"
//
//
//const char* host = "esp32";
//const char* ssid = "xxx";
//const char* password = "xxxx";
//
//WebServer server(80);
//
////**************************************************************************************************
////                                       R E A D I O P R E F S                                     *
////**************************************************************************************************
//// Scan the preferences for IO-pin definitions.                                                    *
////**************************************************************************************************
//void readIOprefs()
//{
//  struct iosetting
//  {
//    const char* gname ;                                   // Name in preferences
//    int8_t*     gnr ;                                     // GPIO pin number
//    int8_t      pdefault ;                                // Default pin
//  };
//  struct iosetting klist[] = {                            // List of I/O related keys
//    { "pin_ir",        &ini_block.ir_pin,           -1 },
//    { "pin_enc_clk",   &ini_block.enc_clk_pin,      -1 },
//    { "pin_enc_dt",    &ini_block.enc_dt_pin,       -1 },
//    { "pin_enc_sw",    &ini_block.enc_sw_pin,       -1 },
//    { "pin_tft_cs",    &ini_block.tft_cs_pin,       -1 }, // Display SPI version
//    { "pin_tft_dc",    &ini_block.tft_dc_pin,       -1 }, // Display SPI version
//    { "pin_tft_scl",   &ini_block.tft_scl_pin,      -1 }, // Display I2C version
//    { "pin_tft_sda",   &ini_block.tft_sda_pin,      -1 }, // Display I2C version
//    { "pin_tft_bl",    &ini_block.tft_bl_pin,       -1 }, // Display backlight
//    { "pin_tft_blx",   &ini_block.tft_blx_pin,      -1 }, // Display backlight (inversed logic)
//    { "pin_sd_cs",     &ini_block.sd_cs_pin,        -1 },
//    { "pin_vs_cs",     &ini_block.vs_cs_pin,        -1 },
//    { "pin_vs_dcs",    &ini_block.vs_dcs_pin,       -1 },
//    { "pin_vs_dreq",   &ini_block.vs_dreq_pin,      -1 },
//    { "pin_shutdown",  &ini_block.vs_shutdown_pin,  -1 }, // Amplifier shut-down pin
//    { "pin_shutdownx", &ini_block.vs_shutdownx_pin, -1 }, // Amplifier shut-down pin (inversed logic)
//    { "pin_spi_sck",   &ini_block.spi_sck_pin,      14 },
//    { "pin_spi_miso",  &ini_block.spi_miso_pin,     12 },
//    { "pin_spi_mosi",  &ini_block.spi_mosi_pin,     13 },
//    { NULL,            NULL,                        0  }  // End of list
//  } ;
//  int         i ;                                         // Loop control
//  int         count = 0 ;                                 // Number of keys found
//  String      val ;                                       // Contents of preference entry
//  int8_t      ival ;                                      // Value converted to integer
//  int8_t*     p ;                                         // Points to variable
//
//  for ( i = 0 ; klist[i].gname ; i++ )                    // Loop trough all I/O related keys
//  {
//    p = klist[i].gnr ;                                    // Point to target variable
//    ival = klist[i].pdefault ;                            // Assume pin number to be the default
//    if ( nvssearch ( klist[i].gname ) )                   // Does it exist?
//    {
//      val = nvsgetstr ( klist[i].gname ) ;                // Read value of key
//      if ( val.length() )                                 // Parameter in preference?
//      {
//        count++ ;                                         // Yes, count number of filled keys
//        ival = val.toInt() ;                              // Convert value to integer pinnumber
//        reservepin ( ival ) ;                             // Set pin to "reserved"
//      }
//    }
//    *p = ival ;                                           // Set pinnumber in ini_block
//    dbgprint ( "%s set to %d",                            // Show result
//               klist[i].gname,
//               ival ) ;
//  }
//}
//
//
////**************************************************************************************************
////                                       R E A D P R E F S                                         *
////**************************************************************************************************
//// Read the preferences and interpret the commands.                                                *
//// If output == true, the key / value pairs are returned to the caller as a String.                *
////**************************************************************************************************
//String readprefs ( bool output )
//{
//  uint16_t    i ;                                           // Loop control
//  String      val ;                                         // Contents of preference entry
//  String      cmd ;                                         // Command for analyzCmd
//  String      outstr = "" ;                                 // Outputstring
//  char*       key ;                                         // Point to nvskeys[i]
//  uint8_t     winx ;                                        // Index in wifilist
//  uint16_t    last2char = 0 ;                               // To detect paragraphs
//
//  i = 0 ;
//  while ( *( key = nvskeys[i] ) )                           // Loop trough all available keys
//  {
//    val = nvsgetstr ( key ) ;                               // Read value of this key
//    cmd = String ( key ) +                                  // Yes, form command
//          String ( " = " ) +
//          val ;
//    if ( strstr ( key, "wifi_"  ) )                         // Is it a wifi ssid/password?
//    {
//      winx = atoi ( key + 5 ) ;                             // Get index in wifilist
//      if ( ( winx < wifilist.size() ) &&                    // Existing wifi spec in wifilist?
//           ( val.indexOf ( wifilist[winx].ssid ) == 0 ) )
//      {
//        val = String ( wifilist[winx].ssid ) +              // Yes, hide password
//              String ( "/*******" ) ;
//      }
//      cmd = String ( "" ) ;                                 // Do not analyze this
//      
//    }
//    else if ( strstr ( key, "mqttpasswd"  ) )               // Is it a MQTT password?
//    {
//      val = String ( "*******" ) ;                          // Yes, hide it
//    }
//    if ( output )
//    {
//      if ( ( i > 0 ) &&
//           ( *(uint16_t*)key != last2char ) )               // New paragraph?
//      {
//        outstr += String ( "#\n" ) ;                        // Yes, add separator
//      }
//      last2char = *(uint16_t*)key ;                         // Save 2 chars for next compare
//      outstr += String ( key ) +                            // Add to outstr
//                String ( " = " ) +
//                val +
//                String ( "\n" ) ;                           // Add newline
//    }
//    else
//    {
//      analyzeCmd ( cmd.c_str() ) ;                          // Analyze it
//    }
//    i++ ;                                                   // Next key
//  }
//  if ( i == 0 )
//  {
//    outstr = String ( "No preferences found.\n"
//                      "Use defaults or run Esp32_radio_init first.\n" ) ;
//  }
//  return outstr ;
//}
//
////**************************************************************************************************
////                                   F I N D N S I D                                               *
////**************************************************************************************************
//// Find the namespace ID for the namespace passed as parameter.                                    *
////**************************************************************************************************
//uint8_t FindNsID ( const char* ns )
//{
//  esp_err_t                 result = ESP_OK ;                 // Result of reading partition
//  uint32_t                  offset = 0 ;                      // Offset in nvs partition
//  uint8_t                   i ;                               // Index in Entry 0..125
//  uint8_t                   bm ;                              // Bitmap for an entry
//  uint8_t                   res = 0xFF ;                      // Function result
//
//  while ( offset < nvs->size )
//  {
//    result = esp_partition_read ( nvs, offset,                // Read 1 page in nvs partition
//                                  &nvsbuf,
//                                  sizeof(nvsbuf) ) ;
//    if ( result != ESP_OK )
//    {
//      dbgprint ( "Error reading NVS!" ) ;
//      break ;
//    }
//    i = 0 ;
//    while ( i < 126 )
//    {
//
//      bm = ( nvsbuf.Bitmap[i / 4] >> ( ( i % 4 ) * 2 ) ) ;    // Get bitmap for this entry,
//      bm &= 0x03 ;                                            // 2 bits for one entry
//      if ( ( bm == 2 ) &&
//           ( nvsbuf.Entry[i].Ns == 0 ) &&
//           ( strcmp ( ns, nvsbuf.Entry[i].Key ) == 0 ) )
//      {
//        res = nvsbuf.Entry[i].Data & 0xFF ;                   // Return the ID
//        offset = nvs->size ;                                  // Stop outer loop as well
//        break ;
//      }
//      else
//      {
//        if ( bm == 2 )
//        {
//          i += nvsbuf.Entry[i].Span ;                         // Next entry
//        }
//        else
//        {
//          i++ ;
//        }
//      }
//    }
//    offset += sizeof(nvs_page) ;                              // Prepare to read next page in nvs
//  }
//  return res ;
//}
//
//
////**************************************************************************************************
////                            B U B B L E S O R T K E Y S                                          *
////**************************************************************************************************
//// Bubblesort the nvskeys.                                                                         *
////**************************************************************************************************
//void bubbleSortKeys ( uint16_t n )
//{
//  uint16_t i, j ;                                             // Indexes in nvskeys
//  char     tmpstr[16] ;                                       // Temp. storage for a key
//
//  for ( i = 0 ; i < n - 1 ; i++ )                             // Examine all keys
//  {
//    for ( j = 0 ; j < n - i - 1 ; j++ )                       // Compare to following keys
//    {
//      if ( strcmp ( nvskeys[j], nvskeys[j + 1] ) > 0 )        // Next key out of order?
//      {
//        strcpy ( tmpstr, nvskeys[j] ) ;                       // Save current key a while
//        strcpy ( nvskeys[j], nvskeys[j + 1] ) ;               // Replace current with next key
//        strcpy ( nvskeys[j + 1], tmpstr ) ;                   // Replace next with saved current
//      }
//    }
//  }
//}
//
//
////**************************************************************************************************
////                                      F I L L K E Y L I S T                                      *
////**************************************************************************************************
//// File the list of all relevant keys in NVS.                                                      *
//// The keys will be sorted.                                                                        *
////**************************************************************************************************
//void fillkeylist()
//{
//  esp_err_t    result = ESP_OK ;                                // Result of reading partition
//  uint32_t     offset = 0 ;                                     // Offset in nvs partition
//  uint16_t     i ;                                              // Index in Entry 0..125.
//  uint8_t      bm ;                                             // Bitmap for an entry
//  uint16_t     nvsinx = 0 ;                                     // Index in nvskey table
//
//  keynames.clear() ;                                            // Clear the list
//  while ( offset < nvs->size )
//  {
//    result = esp_partition_read ( nvs, offset,                  // Read 1 page in nvs partition
//                                  &nvsbuf,
//                                  sizeof(nvsbuf) ) ;
//    if ( result != ESP_OK )
//    {
//      dbgprint ( "Error reading NVS!" ) ;
//      break ;
//    }
//    i = 0 ;
//    while ( i < 126 )
//    {
//      bm = ( nvsbuf.Bitmap[i / 4] >> ( ( i % 4 ) * 2 ) ) ;      // Get bitmap for this entry,
//      bm &= 0x03 ;                                              // 2 bits for one entry
//      if ( bm == 2 )                                            // Entry is active?
//      {
//        if ( nvsbuf.Entry[i].Ns == namespace_ID )               // Namespace right?
//        {
//          strcpy ( nvskeys[nvsinx], nvsbuf.Entry[i].Key ) ;     // Yes, save in table
//          if ( ++nvsinx == MAXKEYS )
//          {
//            nvsinx-- ;                                          // Prevent excessive index
//          }
//        }
//        i += nvsbuf.Entry[i].Span ;                             // Next entry
//      }
//      else
//      {
//        i++ ;
//      }
//    }
//    offset += sizeof(nvs_page) ;                                // Prepare to read next page in nvs
//  }
//  nvskeys[nvsinx][0] = '\0' ;                                   // Empty key at the end
//  dbgprint ( "Read %d keys from NVS", nvsinx ) ;
//  bubbleSortKeys ( nvsinx ) ;                                   // Sort the keys
//}
//
////**************************************************************************************************
////                                     G E T E N C R Y P T I O N T Y P E                           *
////**************************************************************************************************
//// Read the encryption type of the network and return as a 4 byte name                             *
////**************************************************************************************************
//const char* getEncryptionType ( wifi_auth_mode_t thisType )
//{
//  switch ( thisType )
//  {
//    case WIFI_AUTH_OPEN:
//      return "OPEN" ;
//    case WIFI_AUTH_WEP:
//      return "WEP" ;
//    case WIFI_AUTH_WPA_PSK:
//      return "WPA_PSK" ;
//    case WIFI_AUTH_WPA2_PSK:
//      return "WPA2_PSK" ;
//    case WIFI_AUTH_WPA_WPA2_PSK:
//      return "WPA_WPA2_PSK" ;
//    case WIFI_AUTH_MAX:
//      return "MAX" ;
//    default:
//      break ;
//  }
//  return "????" ;
//}
//
//
////**************************************************************************************************
////                                        L I S T N E T W O R K S                                  *
////**************************************************************************************************
//// List the available networks.                                                                    *
//// Acceptable networks are those who have an entry in the preferences.                             *
//// SSIDs of available networks will be saved for use in webinterface.                              *
////**************************************************************************************************
//void listNetworks()
//{
//  WifiInfo_t       winfo ;            // Entry from wifilist
//  wifi_auth_mode_t encryption ;       // TKIP(WPA), WEP, etc.
//  const char*      acceptable ;       // Netwerk is acceptable for connection
//  int              i, j ;             // Loop control
//
//  dbgprint ( "Scan Networks" ) ;                         // Scan for nearby networks
//  numSsid = WiFi.scanNetworks() ;
//  dbgprint ( "Scan completed" ) ;
//  if ( numSsid <= 0 )
//  {
//    dbgprint ( "Couldn't get a wifi connection" ) ;
//    return ;
//  }
//  // print the list of networks seen:
//  dbgprint ( "Number of available networks: %d",
//             numSsid ) ;
//  // Print the network number and name for each network found and
//  for ( i = 0 ; i < numSsid ; i++ )
//  {
//    acceptable = "" ;                                    // Assume not acceptable
//    for ( j = 0 ; j < wifilist.size() ; j++ )            // Search in wifilist
//    {
//      winfo = wifilist[j] ;                              // Get one entry
//      if ( WiFi.SSID(i).indexOf ( winfo.ssid ) == 0 )    // Is this SSID acceptable?
//      {
//        acceptable = "Acceptable" ;
//        break ;
//      }
//    }
//    encryption = WiFi.encryptionType ( i ) ;
//    dbgprint ( "%2d - %-25s Signal: %3d dBm, Encryption %4s, %s",
//               i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i),
//               getEncryptionType ( encryption ),
//               acceptable ) ;
//    // Remember this network for later use
//    networks += WiFi.SSID(i) + String ( "|" ) ;
//  }
//  dbgprint ( "End of list" ) ;
//}
//
////**************************************************************************************************
////                                          I S R _ I R                                            *
////**************************************************************************************************
//// Interrupts received from VS1838B on every change of the signal.                                 *
//// Intervals are 640 or 1640 microseconds for data.  syncpulses are 3400 micros or longer.         *
//// Input is complete after 65 level changes.                                                       *
//// Only the last 32 level changes are significant and will be handed over to common data.          *
////**************************************************************************************************
//void IRAM_ATTR isr_IR()
//{
//  sv uint32_t      t0 = 0 ;                          // To get the interval
//  sv uint32_t      ir_locvalue = 0 ;                 // IR code
//  sv int           ir_loccount = 0 ;                 // Length of code
//  uint32_t         t1, intval ;                      // Current time and interval since last change
//  uint32_t         mask_in = 2 ;                     // Mask input for conversion
//  uint16_t         mask_out = 1 ;                    // Mask output for conversion
//
//  t1 = micros() ;                                    // Get current time
//  intval = t1 - t0 ;                                 // Compute interval
//  t0 = t1 ;                                          // Save for next compare
//  if ( ( intval > 300 ) && ( intval < 800 ) )        // Short pulse?
//  {
//    ir_locvalue = ir_locvalue << 1 ;                 // Shift in a "zero" bit
//    ir_loccount++ ;                                  // Count number of received bits
//    ir_0 = ( ir_0 * 3 + intval ) / 4 ;               // Compute average durartion of a short pulse
//  }
//  else if ( ( intval > 1400 ) && ( intval < 1900 ) ) // Long pulse?
//  {
//    ir_locvalue = ( ir_locvalue << 1 ) + 1 ;         // Shift in a "one" bit
//    ir_loccount++ ;                                  // Count number of received bits
//    ir_1 = ( ir_1 * 3 + intval ) / 4 ;               // Compute average durartion of a short pulse
//  }
//  else if ( ir_loccount == 65 )                      // Value is correct after 65 level changes
//  {
//    while ( mask_in )                                // Convert 32 bits to 16 bits
//    {
//      if ( ir_locvalue & mask_in )                   // Bit set in pattern?
//      {
//        ir_value |= mask_out ;                       // Set set bit in result
//      }
//      mask_in <<= 2 ;                                // Shift input mask 2 positions
//      mask_out <<= 1 ;                               // Shift output mask 1 position
//    }
//    ir_loccount = 0 ;                                // Ready for next input
//  }
//  else
//  {
//    ir_locvalue = 0 ;                                // Reset decoding
//    ir_loccount = 0 ;
//  }
//}
//
//
////**************************************************************************************************
////                                    C O N N E C T T O H O S T                                    *
////**************************************************************************************************
//// Connect to the Internet radio server specified by newpreset.                                    *
////**************************************************************************************************
//bool connecttohost()
//{
//  int         inx ;                                 // Position of ":" in hostname
//  uint16_t    port = 80 ;                           // Port number for host
//  String      extension = "/" ;                     // May be like "/mp3" in "skonto.ls.lv:8002/mp3"
//  String      hostwoext = host ;                    // Host without extension and portnumber
//  String      auth  ;                               // For basic authentication
//
//  stop_mp3client() ;                                // Disconnect if still connected
//  dbgprint ( "Connect to new host %s", host.c_str() ) ;
//  tftset ( 0, "ESP32-Radio" ) ;                     // Set screen segment text top line
//  displaytime ( "" ) ;                              // Clear time on TFT screen
//  datamode = INIT ;                                 // Start default in metamode
//  chunked = false ;                                 // Assume not chunked
//  if ( host.endsWith ( ".m3u" ) )                   // Is it an m3u playlist?
//  {
//    playlist = host ;                               // Save copy of playlist URL
//    datamode = PLAYLISTINIT ;                       // Yes, start in PLAYLIST mode
//    if ( playlist_num == 0 )                        // First entry to play?
//    {
//      playlist_num = 1 ;                            // Yes, set index
//    }
//    dbgprint ( "Playlist request, entry %d", playlist_num ) ;
//  }
//  // In the URL there may be an extension, like noisefm.ru:8000/play.m3u&t=.m3u
//  inx = host.indexOf ( "/" ) ;                      // Search for begin of extension
//  if ( inx > 0 )                                    // Is there an extension?
//  {
//    extension = host.substring ( inx ) ;            // Yes, change the default
//    hostwoext = host.substring ( 0, inx ) ;         // Host without extension
//  }
//  // In the host there may be a portnumber
//  inx = hostwoext.indexOf ( ":" ) ;                 // Search for separator
//  if ( inx >= 0 )                                   // Portnumber available?
//  {
//    port = host.substring ( inx + 1 ).toInt() ;     // Get portnumber as integer
//    hostwoext = host.substring ( 0, inx ) ;         // Host without portnumber
//  }
//  dbgprint ( "Connect to %s on port %d, extension %s",
//             hostwoext.c_str(), port, extension.c_str() ) ;
//  if ( mp3client.connect ( hostwoext.c_str(), port ) )
//  {
//    dbgprint ( "Connected to server" ) ;
//    auth = nvsgetstr ( "basicauth" ) ;              // Use basic authentication?
//    if ( auth != "" )                               // Should be user:passwd
//    { 
//       auth = base64::encode ( auth.c_str() ) ;     // Encode
//       auth = String ( "Authorization: Basic " ) +
//              auth + String ( "\r\n" ) ;
//    }
//    mp3client.print ( String ( "GET " ) +
//                      extension +
//                      String ( " HTTP/1.1\r\n" ) +
//                      String ( "Host: " ) +
//                      hostwoext +
//                      String ( "\r\n" ) +
//                      String ( "Icy-MetaData:1\r\n" ) +
//                      auth +
//                      String ( "Connection: close\r\n\r\n" ) ) ;
//    return true ;
//  }
//  dbgprint ( "Request %s failed!", host.c_str() ) ;
//  return false ;
//}
//
//
////**************************************************************************************************
////                                       C O N N E C T W I F I                                     *
////**************************************************************************************************
//// Connect to WiFi using the SSID's available in wifiMulti.                                        *
//// If only one AP if found in preferences (i.e. wifi_00) the connection is made without            *
//// using wifiMulti.                                                                                *
//// If connection fails, an AP is created and the function returns false.                           *
////**************************************************************************************************
//bool connectwifi()
//{
//  char*      pfs ;                                      // Pointer to formatted string
//  char*      pfs2 ;                                     // Pointer to formatted string
//  bool       localAP = false ;                          // True if only local AP is left
//
//  WifiInfo_t winfo ;                                    // Entry from wifilist
//
//  WiFi.disconnect() ;                                   // After restart the router could
//  WiFi.softAPdisconnect(true) ;                         // still keep the old connection
//  if ( wifilist.size()  )                               // Any AP defined?
//  {
//    if ( wifilist.size() == 1 )                         // Just one AP defined in preferences?
//    {
//      winfo = wifilist[0] ;                             // Get this entry
//      WiFi.begin ( winfo.ssid, winfo.passphrase ) ;     // Connect to single SSID found in wifi_xx
//      dbgprint ( "Try WiFi %s", winfo.ssid ) ;          // Message to show during WiFi connect
//    }
//    else                                                // More AP to try
//    {
//      wifiMulti.run() ;                                 // Connect to best network
//    }
//    if (  WiFi.waitForConnectResult() != WL_CONNECTED ) // Try to connect
//    {
//      localAP = true ;                                  // Error, setup own AP
//    }
//  }
//  else
//  {
//    localAP = true ;                                    // Not even a single AP defined
//  }
//  if ( localAP )                                        // Must setup local AP?
//  {
//    dbgprint ( "WiFi Failed!  Trying to setup AP with name %s and password %s.", NAME, NAME ) ;
//    WiFi.softAP ( NAME, NAME ) ;                        // This ESP will be an AP
//    pfs = dbgprint ( "IP = 192.168.4.1" ) ;             // Address for AP
//  }
//  else
//  {
//    ipaddress = WiFi.localIP().toString() ;             // Form IP address
//    pfs2 = dbgprint ( "Connected to %s", WiFi.SSID().c_str() ) ;
//    tftlog ( pfs2 ) ;
//    pfs = dbgprint ( "IP = %s", ipaddress.c_str() ) ;   // String to dispay on TFT
//  }
//  tftlog ( pfs ) ;                                      // Show IP
//  delay ( 3000 ) ;                                      // Allow user to read this
//  tftlog ( "\f" ) ;                                     // Select new page if NEXTION 
//  return ( localAP == false ) ;                         // Return result of connection
//}
//
//
////**************************************************************************************************
////                                           O T A S T A R T                                       *
////**************************************************************************************************
//// Update via WiFi has been started by Arduino IDE or update request.                              *
////**************************************************************************************************
//void otastart()
//{
//  char* p ;
//
//  p = dbgprint ( "OTA update Started" ) ;
//  tftset ( 2, p ) ;                                   // Set screen segment bottom part
//}
//
////**************************************************************************************************
////                                D O _ S O F T W A R E _ U P D A T E                              *
////**************************************************************************************************
//// Update software from OTA stream.                                                                *
////**************************************************************************************************
//bool do_software_update ( uint32_t clength )
//{
//  bool res = false ;                                          // Update result
//  
//  if ( Update.begin ( clength ) )                             // Update possible?
//  {
//    dbgprint ( "Begin OTA update, length is %d",
//               clength ) ;
//    if ( Update.writeStream ( otaclient ) == clength )        // writeStream is the real download
//    {
//      dbgprint ( "Written %d bytes successfully", clength ) ;
//    }
//    else
//    {
//      dbgprint ( "Write failed!" ) ;
//    }
//    if ( Update.end() )                                       // Check for successful flash
//    {
//      dbgprint( "OTA done" ) ;
//      if ( Update.isFinished() )
//      {
//        dbgprint ( "Update successfully completed" ) ;
//        res = true ;                                          // Positive result
//      }
//      else
//      {
//        dbgprint ( "Update not finished!" ) ;
//      }
//    }
//    else
//    {
//      dbgprint ( "Error Occurred. Error %s", Update.getError() ) ;
//    }
//  }
//  else
//  {
//    // Not enough space to begin OTA
//    dbgprint ( "Not enough space to begin OTA" ) ;
//    otaclient.flush() ;
//  }
//  return res ;
//}
//
//
////**************************************************************************************************
////                                        U P D A T E _ S O F T W A R E                            *
////**************************************************************************************************
//// Update software by download from remote host.                                                   *
////**************************************************************************************************
//void update_software ( const char* lstmodkey, const char* updatehost, const char* binfile )
//{
//  uint32_t    timeout = millis() ;                              // To detect time-out
//  String      line ;                                            // Input header line
//  String      lstmod = "" ;                                     // Last modified timestamp in NVS
//  String      newlstmod ;                                       // Last modified from host
//  
//  updatereq = false ;                                           // Clear update flag
//  otastart() ;                                                  // Show something on screen
//  stop_mp3client () ;                                           // Stop input stream
//  lstmod = nvsgetstr ( lstmodkey ) ;                            // Get current last modified timestamp
//  dbgprint ( "Connecting to %s for %s",
//              updatehost, binfile ) ;
//  if ( !otaclient.connect ( updatehost, 80 ) )                  // Connect to host
//  {
//    dbgprint ( "Connect to updatehost failed!" ) ;
//    return ;
//  }
//  otaclient.printf ( "GET %s HTTP/1.1\r\n"
//                     "Host: %s\r\n"
//                     "Cache-Control: no-cache\r\n"
//                     "Connection: close\r\n\r\n",
//                     binfile,
//                     updatehost ) ;
//  while ( otaclient.available() == 0 )                          // Wait until response appears
//  {
//    if ( millis() - timeout > 5000 )
//    {
//      dbgprint ( "Connect to Update host Timeout!" ) ;
//      otaclient.stop() ;
//      return ;
//    }
//  }
//  // Connected, handle response
//  while ( otaclient.available() )
//  {
//    line = otaclient.readStringUntil ( '\n' ) ;                 // Read a line from response
//    line.trim() ;                                               // Remove garbage
//    dbgprint ( line.c_str() ) ;                                 // Debug info
//    if ( !line.length() )                                       // End of headers?
//    {
//      break ;                                                   // Yes, get the OTA started
//    }
//    // Check if the HTTP Response is 200.  Any other response is an error.
//    if ( line.startsWith ( "HTTP/1.1" ) )                       // 
//    {
//      if ( line.indexOf ( " 200 " ) < 0 )
//      {
//        dbgprint ( "Got a non 200 status code from server!" ) ;
//        return ;
//      }
//    }
//    scan_content_length ( line.c_str() ) ;                      // Scan for content_length
//    if ( line.startsWith ( "Last-Modified: " ) )                // Timestamp of binary file
//    {
//      newlstmod = line.substring ( 15 ) ;                       // Isolate timestamp
//    }
//  }
//  // End of headers reached
//  if ( newlstmod == lstmod )                                    // Need for update?
//  {
//    dbgprint ( "No new version available" ) ;                   // No, show reason
//    otaclient.flush() ;
//    return ;    
//  }
//  if ( clength > 0 )
//  {
//    if ( strstr ( binfile, ".bin" ) )                           // Update of the sketch?
//    {
//      if ( do_software_update ( clength ) )                     // Flash updated sketch
//      {
//        nvssetstr ( lstmodkey, newlstmod ) ;                    // Update Last Modified in NVS
//      }
//    }
//    if ( strstr ( binfile, ".tft" ) )                           // Update of the NEXTION image?
//    {
//      if ( do_nextion_update ( clength ) )                      // Flash updated NEXTION
//      {
//        nvssetstr ( lstmodkey, newlstmod ) ;                    // Update Last Modified in NVS
//      }
//    }
//  }
//  else
//  {
//    dbgprint ( "There was no content in the response" ) ;
//    otaclient.flush() ;
//  }
//}
//
////**************************************************************************************************
////                                     S C A N I R                                                 *
////**************************************************************************************************
//// See if IR input is available.  Execute the programmed command.                                  *
////**************************************************************************************************
//void scanIR()
//{
//  char        mykey[20] ;                                   // For numerated key
//  String      val ;                                         // Contents of preference entry
//  const char* reply ;                                       // Result of analyzeCmd
//
//  if ( ir_value )                                           // Any input?
//  {
//    sprintf ( mykey, "ir_%04X", ir_value ) ;                // Form key in preferences
//    if ( nvssearch ( mykey ) )
//    {
//      val = nvsgetstr ( mykey ) ;                           // Get the contents
//      dbgprint ( "IR code %04X received. Will execute %s",
//                 ir_value, val.c_str() ) ;
//      reply = analyzeCmd ( val.c_str() ) ;                  // Analyze command and handle it
//      dbgprint ( reply ) ;                                  // Result for debugging
//    }
//    else
//    {
//      dbgprint ( "IR code %04X received, but not found in preferences!  Timing %d/%d",
//                 ir_value, ir_0, ir_1 ) ;
//    }
//    ir_value = 0 ;                                          // Reset IR code received
//  }
//}
//
//
////**************************************************************************************************
////                                           M K _ L S A N                                         *
////**************************************************************************************************
//// Make al list of acceptable networks in preferences.                                             *
//// Will be called only once by setup().                                                            *
//// The result will be stored in wifilist.                                                          *
//// Not that the last found SSID and password are kept in common data.  If only one SSID is         *
//// defined, the connect is made without using wifiMulti.  In this case a connection will           *
//// be made even if de SSID is hidden.                                                              *
////**************************************************************************************************
//void  mk_lsan()
//{
//  uint8_t     i ;                                        // Loop control
//  char        key[10] ;                                  // For example: "wifi_03"
//  String      buf ;                                      // "SSID/password"
//  String      lssid, lpw ;                               // Last read SSID and password from nvs
//  int         inx ;                                      // Place of "/"
//  WifiInfo_t  winfo ;                                    // Element to store in list
//
//  dbgprint ( "Create list with acceptable WiFi networks" ) ;
//  for ( i = 0 ; i < 100 ; i++ )                          // Examine wifi_00 .. wifi_99
//  {
//    sprintf ( key, "wifi_%02d", i ) ;                    // Form key in preferences
//    if ( nvssearch ( key  ) )                            // Does it exists?
//    {
//      buf = nvsgetstr ( key ) ;                          // Get the contents
//      inx = buf.indexOf ( "/" ) ;                        // Find separator between ssid and password
//      if ( inx > 0 )                                     // Separator found?
//      {
//        lpw = buf.substring ( inx + 1 ) ;                // Isolate password
//        lssid = buf.substring ( 0, inx ) ;               // Holds SSID now
//        dbgprint ( "Added %s to list of networks",
//                   lssid.c_str() ) ;
//        winfo.inx = i ;                                  // Create new element for wifilist ;
//        winfo.ssid = strdup ( lssid.c_str() ) ;          // Set ssid of element
//        winfo.passphrase = strdup ( lpw.c_str() ) ;
//        wifilist.push_back ( winfo ) ;                   // Add to list
//        wifiMulti.addAP ( winfo.ssid,                    // Add to wifi acceptable network list
//                          winfo.passphrase ) ;
//      }
//    }
//  }
//  dbgprint ( "End adding networks" ) ; ////
//}
//
////**************************************************************************************************
////                                     G E T S E T T I N G S                                       *
////**************************************************************************************************
//// Send some settings to the webserver.                                                            *
//// Included are the presets, the current station, the volume and the tone settings.                *
////**************************************************************************************************
//void getsettings()
//{
//  String              val ;                              // Result to send
//  String              statstr ;                          // Station string
//  int                 inx ;                              // Position of search char in line
//  int16_t             i ;                                // Loop control, preset number
//
//  for ( i = 0 ; i < MAXPRESETS ; i++ )                   // Max number of presets
//  {
//    statstr = readhostfrompref ( i ) ;                    // Get the preset from NVS
//    if ( statstr != "" )                                 // Preset available?
//    {
//      // Show just comment if available.  Otherwise the preset itself.
//      inx = statstr.indexOf ( "#" ) ;                    // Get position of "#"
//      if ( inx > 0 )                                     // Hash sign present?
//      {
//        statstr.remove ( 0, inx + 1 ) ;                  // Yes, remove non-comment part
//      }
//      chomp ( statstr ) ;                                // Remove garbage from description
//      dbgprint ( "statstr is %s", statstr.c_str() ) ;
//      val += String ( "preset_" ) +
//             String ( i ) +
//             String ( "=" ) +
//             statstr +
//             String ( "\n" ) ;                           // Add delimeter
//      if ( val.length() > 1000 )                         // Time to flush?
//      {
//        cmdclient.print ( val ) ;                        // Yes, send
//        val = "" ;                                       // Start new string
//      }
//    }
//  }
//  val += getradiostatus() +                              // Add radio setting
//         String ( "\n\n" ) ;                             // End of reply
//  cmdclient.print ( val ) ;                              // And send
//}
//
//
///*
// * Login page
// */
//
//const char* loginIndex = 
// "<form name='loginForm'>"
//    "<table width='20%' bgcolor='A09F9F' align='center'>"
//        "<tr>"
//            "<td colspan=2>"
//                "<center><font size=4><b>ESP32 Login Page</b></font></center>"
//                "<br>"
//            "</td>"
//            "<br>"
//            "<br>"
//        "</tr>"
//        "<td>Username:</td>"
//        "<td><input type='text' size=25 name='userid'><br></td>"
//        "</tr>"
//        "<br>"
//        "<br>"
//        "<tr>"
//            "<td>Password:</td>"
//            "<td><input type='Password' size=25 name='pwd'><br></td>"
//            "<br>"
//            "<br>"
//        "</tr>"
//        "<tr>"
//            "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
//        "</tr>"
//    "</table>"
//"</form>"
//"<script>"
//    "function check(form)"
//    "{"
//    "if(form.userid.value=='admin' && form.pwd.value=='admin')"
//    "{"
//    "window.open('/serverIndex')"
//    "}"
//    "else"
//    "{"
//    " alert('Error Password or Username')/*displays error message*/"
//    "}"
//    "}"
//"</script>";
// 
///*
// * Server Index Page
// */
// 
//const char* serverIndex = 
//"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
//"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
//   "<input type='file' name='update'>"
//        "<input type='submit' value='Update'>"
//    "</form>"
// "<div id='prg'>progress: 0%</div>"
// "<script>"
//  "$('form').submit(function(e){"
//  "e.preventDefault();"
//  "var form = $('#upload_form')[0];"
//  "var data = new FormData(form);"
//  " $.ajax({"
//  "url: '/update',"
//  "type: 'POST',"
//  "data: data,"
//  "contentType: false,"
//  "processData:false,"
//  "xhr: function() {"
//  "var xhr = new window.XMLHttpRequest();"
//  "xhr.upload.addEventListener('progress', function(evt) {"
//  "if (evt.lengthComputable) {"
//  "var per = evt.loaded / evt.total;"
//  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
//  "}"
//  "}, false);"
//  "return xhr;"
//  "},"
//  "success:function(d, s) {"
//  "console.log('success!')" 
// "},"
// "error: function (a, b, c) {"
// "}"
// "});"
// "});"
// "</script>";
//
///*
// * setup function
// */
//void setup(void) {
//  Serial.begin(115200);
//
//  // Version tests for some vital include files
//  if ( about_html_version   < 170626 ) dbgprint ( wvn, "about" ) ;
//  if ( config_html_version  < 180806 ) dbgprint ( wvn, "config" ) ;
//  if ( index_html_version   < 180102 ) dbgprint ( wvn, "index" ) ;
//  if ( mp3play_html_version < 180918 ) dbgprint ( wvn, "mp3play" ) ;
//  if ( defaultprefs_version < 180816 ) dbgprint ( wvn, "defaultprefs" ) ;
//
//  // check if senssors attached and initiate them
//  if ( ini_block.ir_pin >= 0 )    // change to global IR (because im not using NVS
//  {
//    dbgprint ( "Enable pin %d for IR",
//               ini_block.ir_pin ) ;
//    pinMode ( ini_block.ir_pin, INPUT ) ;                // Pin for IR receiver VS1838B
//    attachInterrupt ( ini_block.ir_pin,                  // Interrupts will be handle by isr_IR
//                      isr_IR, CHANGE ) ;
//  }
//
//  // handle wifi, and all its periferials.
//  mk_lsan() ;                                            // Make a list of acceptable networks
//                                                         // in preferences.
//  WiFi.mode ( WIFI_STA ) ;                               // This ESP is a station
//  WiFi.persistent ( false ) ;                            // Do not save SSID and password
//  WiFi.disconnect() ;                                    // After restart router could still
//  delay ( 100 ) ;                                        // keep old connection
//  listNetworks() ;                                       // Find WiFi networks
//  readprefs ( false ) ;                                  // Read preferences
//  tcpip_adapter_set_hostname ( TCPIP_ADAPTER_IF_STA,
//                               NAME ) ;
//                               
//  // Connect to WiFi network
//  WiFi.begin(ssid, password);
//  Serial.println("");
//
//  // Wait for connection
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    Serial.print(".");
//  }
//  Serial.println("");
//  Serial.print("Connected to ");
//  Serial.println(ssid);
//  Serial.print("IP address: ");
//  Serial.println(WiFi.localIP());
//
//  /*use mdns for host name resolution*/
//  if (!MDNS.begin(host)) { //http://esp32.local
//    Serial.println("Error setting up MDNS responder!");
//    while (1) {
//      delay(1000);
//    }
//  }
//  Serial.println("mDNS responder started");
//  /*return index page which is stored in serverIndex */
//  server.on("/", HTTP_GET, []() {
//    server.sendHeader("Connection", "close");
//    server.send(200, "text/html", loginIndex);
//  });
//  server.on("/serverIndex", HTTP_GET, []() {
//    server.sendHeader("Connection", "close");
//    server.send(200, "text/html", serverIndex);
//  });
//  /*handling uploading firmware file */
//  server.on("/update", HTTP_POST, []() {
//    server.sendHeader("Connection", "close");
//    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
//    ESP.restart();
//  }, []() {
//    HTTPUpload& upload = server.upload();
//    if (upload.status == UPLOAD_FILE_START) {
//      Serial.printf("Update: %s\n", upload.filename.c_str());
//      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
//        Update.printError(Serial);
//      }
//    } else if (upload.status == UPLOAD_FILE_WRITE) {
//      /* flashing firmware to ESP*/
//      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
//        Update.printError(Serial);
//      }
//    } else if (upload.status == UPLOAD_FILE_END) {
//      if (Update.end(true)) { //true to set the size to the current progress
//        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
//      } else {
//        Update.printError(Serial);
//      }
//    }
//  });
//  server.begin();
//}
//
//void loop(void) {
//  server.handleClient();
//  delay(1);
//}



//#include "DHT.h"
// 
//#define DHTPIN 4
////our sensor is DHT22 type
//#define DHTTYPE DHT22
////create an instance of DHT sensor
//DHT dht(DHTPIN, DHTTYPE);
//void setup() {
//  Serial.begin(115200);
//  Serial.println("DHT22 sensor!");
//  //call begin to start sensor
//  dht.begin();
//}
// 
//void loop() {
//  //use the functions which are supplied by library.
//  float h = dht.readHumidity();
//  // Read temperature as Celsius (the default)
//  float t = dht.readTemperature();
//  // Check if any reads failed and exit early (to try again).
//  if (isnan(h) || isnan(t)) {
//  Serial.println("Failed to read from DHT sensor!");
//  return;
//  }
//  // print the result to Terminal
//  Serial.print("Humidity: ");
//  Serial.print(h);
//  Serial.print(" %\t");
//  Serial.print("Temperature: ");
//  Serial.print(t);
//  Serial.println(" *C ");
//  //we delay a little bit for next read
//  delay(2000);
//}



/* ********************************************************************************* */
/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <Adafruit_Sensor.h> // Not necessary
#include <Adafruit_NeoPixel.h>
#include "DHT.h"
#define DHTPIN 4


//**************************************************************************************
//* Neopixle includes & instantiations                                                    *
//**************************************************************************************
# define Data_Pin 27
# define NUM_OF_PIXLES 72
// We define birghtness of NeoPixel LEDs
#define BRIGHTNESS  20
// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_OF_PIXLES, Data_Pin, NEO_GRB + NEO_KHZ800);

//**************************************************************************************
//* DHT22 includes & instantiations                                                    *
//**************************************************************************************
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

//**************************************************************************************
//* MQ-2 definitions                                                                   *
//**************************************************************************************
#define calibrationLed 13                      //when the calibration start , LED pin 13 will light up , off when finish calibrating
#define MQ_PIN 32                              //define which analog input channel you are going to use (NOTE - CANT USE ADC2 PINS WHEN WIFI CONNECTED)


//**************************************************************************************
//*                                 W I F I                                            *
//* Replace with your network credentials                                              *
//**************************************************************************************
#define WIFITRYNUM 4
const char* ssid = "gozal_2.4";
const char* password = "asdffdsa";
bool isConnected = false;


//**************************************************************************************
//* Simple led on/off for an example to how to controll pind using buttons in the web  *
//**************************************************************************************
// Set LED GPIO
const int ledPin = 2;
// Stores LED state
String ledState;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

//**************************************************************************************************
// Forward declaration and prototypes of various functions.                                        *
//**************************************************************************************************
String processor(const String& var);

//void neoPixle_lightAll(String &effect, int time_length, int R, int G, int B){
//  if (effect == "FADE_IN"){
//    
//  }
//  else if (effect == "NORMAL"){
//    
//  }
//  
//}
//void neoPixle_lightSpecific(int ledNum, int R, int G, int B){
//  pixels.setPixelColor(ledNum, pixels.Color(R, G,B));
//  pixels.show(); // This sends the updated pixel color to the hardware.
//}
//void neoPixle_KillAll(){
//  pixels.clear(); // Set all pixel colors to 'off'
//}
//
//void neoPixle_KillSpecific(int ledNum){
//  
//}

bool wifiConnect(){
  int tries=0;
  WiFi.begin(ssid, password);
  pixels.clear(); // Set all pixel colors to 'off'
  for (int i=0; i < NUM_OF_PIXLES; i++){
      pixels.setPixelColor(i, pixels.Color(255, 0,0));
  }
  pixels.setBrightness(5);
  pixels.show();
  
  while (WiFi.status() != WL_CONNECTED and tries <= WIFITRYNUM) {
    Serial.println("Connecting to WiFi..");
    for(int brightness=6; brightness <= BRIGHTNESS; brightness++){
      // Set NeoPixel configuration 
      pixels.setBrightness(brightness);
      pixels.show();
      Serial.println(brightness);
      delay(100);
    }
    for(int brightness=BRIGHTNESS; brightness > 0; brightness--){
      // Set NeoPixel configuration 
      pixels.setBrightness(brightness);
      pixels.show();
      Serial.println(brightness);
      delay(100);
    }
    tries++;
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  pixels.clear(); // Set all pixel colors to 'off'
}
String readDHTTemperature() {
  //u se the functions which are supplied by library.
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Convert temperature to Fahrenheit
  //t = 1.8 * t + 32;
  
  if (isnan(t)) {    
    Serial.println("Failed to read from AM2302 sensor!");
    return "";
  }
  else {
    Serial.println(t);
    return String(t);
  }
}


String readDHTHumidity() {
  float h = dht.readHumidity();
  //float h = bme.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from AM2302 sensor!");
    return "";
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
  //float p = bme.readPressure() / 100.0F;
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
  if(var == "GAS"){
    return readMq2();
  }
  else if(var == "TEMPERATURE"){
    return readDHTTemperature();
  }
  else if(var == "HUMIDITY"){
    return readDHTHumidity();
  }
  else if(var == "STATE"){
    if(digitalRead(ledPin)){
      ledState = "ON";
    }
    else{
      ledState = "OFF";
    }
    Serial.print(ledState);
    return ledState;
  }
  return String();
}


void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println("BabySleeper Setup!");
  

  pixels.begin(); // This initializes the NeoPixel library.
  
  dht.begin(); //call begin to start sensor
  
  pinMode(MQ_PIN, INPUT); // MQ-2 analog pin
  pinMode(ledPin, OUTPUT); // simple led initiation
  pinMode(calibrationLed,OUTPUT);
  
//  digitalWrite(calibrationLed,HIGH);
//  do smoething in between led operations
//  digitalWrite(calibrationLed,LOW);
  
  
  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  isConnected = wifiConnect();
  
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html", false, processor);
  });
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });






  
  // Simple example for changing GPIO via http request
  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, HIGH);                                         // inside arduino sketch, turn off the pin   
    request->send(SPIFFS, "/index.html", String(), false, processor);   // then go to html, and update the graphics
  });

  
  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, processor); // Here we call processor just for updating the html text. the led state is changed in the line above.
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

  
  // Start server
  server.begin();
}
 
void loop(){
}
