//========================================================================
// CurieWifi
//========================================================================

#include <CurieWifi.h>
#include <CurieLCD.h>
#include <Arduino.h>
#include <string.h>

#include "utility/ccspi.h"
#include "utility/cc3000_common.h"
#include "utility/hci.h"
#include "utility/wlan.h"
#include "utility/nvmem.h"
#include "utility/socket.h"

// Global instance

CurieWifi curie_wifi;

//------------------------------------------------------------------------
// Debug macros
//------------------------------------------------------------------------

void curie_wifi_inner_print( const __FlashStringHelper* msg )
{
  curie_lcd.print(msg);
  curie_lcd.print(F("               "));
  Serial.print(F("CURIE_DEBUG: "));
  Serial.println(msg);
}

void curie_wifi_debug_header( const __FlashStringHelper* msg )
{
  curie_lcd.setCursor(0,0);
  curie_wifi_inner_print(msg);
}

#define CURIE_WIFI_DEBUG_HEADER( msg_ ) \
  curie_wifi_debug_header(F(msg_));

void curie_wifi_debug( const __FlashStringHelper* msg )
{
  curie_lcd.setCursor(0,1);
  curie_wifi_inner_print(msg);
}

#define CURIE_WIFI_DEBUG( msg_ ) \
  curie_wifi_debug(F(msg_));

void curie_wifi_assert( bool result, const __FlashStringHelper* msg )
{
  if ( !(result) ) {
    curie_lcd.setCursor(0,1);
    curie_lcd.print("E: ");
    curie_wifi_inner_print(msg);
    while (1);
  }
}

#define CURIE_WIFI_ASSERT( expression_, msg_ )   \
  curie_wifi_assert((expression_),F(msg_));

//------------------------------------------------------------------------
// CCSPI Global Variables
//------------------------------------------------------------------------
// Apparently ccspi uses these global variables. They are declared in
// ccspi.h. What a mess!

uint8_t g_csPin;
uint8_t g_irqPin;
uint8_t g_vbatPin;
uint8_t g_IRQnum;
uint8_t g_SPIspeed;

//------------------------------------------------------------------------
// g_dreqinttable
//------------------------------------------------------------------------
// Table from Adafruit_CC3000.cpp to map interupt pins to IRQ numbers. I
// simplified it a bit by removing the mappings corresponding to ARM
// processors.

static const uint8_t g_int2irq[] = {
  2, 0,
  3, 1,
};

// static const uint8_t g_int2irq[] = {
// #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined (__AVR_ATmega328__) || defined(__AVR_ATmega8__)
//    2, 0,
//    3, 1,
// #elif defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2561__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)
//    2, 0,
//    3, 1,
//   21, 2,
//   20, 3,
//   19, 4,
//   18, 5,
// #elif  defined(__AVR_ATmega32U4__) && defined(CORE_TEENSY)
//    5, 0,
//    6, 1,
//    7, 2,
//    8, 3,
// #elif  defined(__AVR_AT90USB1286__) && defined(CORE_TEENSY)
//    0, 0,
//    1, 1,
//    2, 2,
//    3, 3,
//   36, 4,
//   37, 5,
//   18, 6,
//   19, 7,
// #elif  defined(__AVR_ATmega32U4__)
//    7, 4,
//    3, 0,
//    2, 1,
//    0, 2,
//    1, 3,
// #endif
// };

//------------------------------------------------------------------------
// event_callback
//------------------------------------------------------------------------
// Inspired from Adafruit_CC3000.cpp: The function handles asynchronous
// events that come from CC3000 device.

uint8_t g_have_connection; // callback sets to 1 if connected
uint8_t g_have_ip;         // callback sets to 1 if have IP via DHCP
uint8_t g_socket_open;     // callback sets to 0 if the socket is closed

void event_callback( long lEventType, char* data, unsigned char length )
{

   // if ( lEventType == HCI_EVNT_WLAN_ASYNC_SIMPLE_CONFIG_DONE ){
   //   //cc3000Bitset.set(CC3000BitSet::IsSmartConfigFinished);
   //   Serial.println(F("Event 1 \n"));
   // }

   if ( lEventType == HCI_EVNT_WLAN_UNSOL_CONNECT )
     g_have_connection = 1;

   if ( lEventType == HCI_EVNT_WLAN_UNSOL_DISCONNECT )
     g_have_connection = 0;

   if ( lEventType == HCI_EVNT_WLAN_UNSOL_DHCP )
     g_have_ip = 1;

   // if ( lEventType == HCI_EVENT_CC3000_CAN_SHUT_DOWN )
   //   cc3000Bitset.set(CC3000BitSet::OkToShutDown);

   // if ( lEventType == HCI_EVNT_WLAN_ASYNC_PING_REPORT )
   // {
   //   //PRINT_F("CC3000: Ping report\n\r");
   //   /*pingReportnum++;
   //   memcpy(&pingReport, data, length);
   //   Serial.println("\n");
   //   */
   //   Serial.println(F("Event 3 \n"));
   // }

   if ( lEventType == HCI_EVNT_BSD_TCP_CLOSE_WAIT )
     g_socket_open = 0;

}

//------------------------------------------------------------------------
// scan_ssids
//------------------------------------------------------------------------

void scan_ssids( uint32_t time )
{
  const unsigned long intervalTime[16] =
    { 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000,
      2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000 };

  int err = wlan_ioctl_set_scan_params( time, 20, 100, 5, 0x1FFF,
                                          -120, 0, 300,
                                          (unsigned long*) &intervalTime );

  if ( err != 0 ) {
    Serial.println(F("ERROR: Failed scanning SSIDs!"));
    while (1);
  }
}

//------------------------------------------------------------------------
// CurieWifi Constructor
//------------------------------------------------------------------------

CurieWifi::CurieWifi()
{
  // This is the old pin out, but it uses up three PWM outputs

  // g_csPin   = 10;
  // g_irqPin  = 3;
  // g_vbatPin = 5;

  // This is our new pinout which frees up those three PWM outputs

  g_csPin   = 8;
  g_irqPin  = 2;
  g_vbatPin = 4;

  g_IRQnum  = 0xff;

  // This is from the Adafruit documentation.

  g_SPIspeed = SPI_CLOCK_DIV2;

  m_initialized = false;
}

//------------------------------------------------------------------------
// CurieWifi::begin
//------------------------------------------------------------------------

void CurieWifi::begin()
{
  if ( m_initialized )
    return;

  delay(2000);
  curie_lcd.begin();
  Serial.begin(9600);
  Serial.print("\n\n");

  CURIE_WIFI_DEBUG_HEADER("Setup wifi board");

  // Map interrupt pin to IRQ number

  CURIE_WIFI_DEBUG("1. map int pin")

  for ( uint8_t i = 0; i < sizeof(g_int2irq); i += 2 ) {
    if ( g_irqPin == g_int2irq[i] )
      g_IRQnum = g_int2irq[i+1];
  }

  CURIE_WIFI_ASSERT( g_IRQnum != 0xFF, "int pin !irq" );

  // Initialize SPI

  CURIE_WIFI_DEBUG("2. init spi");
  init_spi();

  // Initialize the CC3000

  CURIE_WIFI_DEBUG("3. init board");
  wlan_init
  (
    event_callback,        // defined above
    sendWLFWPatch,         // from ccspi.h
    sendDriverPatch,       // from ccspi.h
    sendBootLoaderPatch,   // from ccspi.h
    ReadWlanInterruptPin,  // from utility/cc3000_common.h
    WlanInterruptEnable,   // from utility/cc3000_common.h
    WlanInterruptDisable,  // from utility/cc3000_common.h
    WriteWlanPin           // from utility/cc3000_common.h
  );

  // Start the CC3000. The argument indicates that we are not doing any
  // kind of firmware patch or anything.

  CURIE_WIFI_DEBUG("4. start board");
  wlan_start(0);

  // Set the connection policy for manual connections only (no auto,
  // profiles, etc.). This means none of the fancy SmartConfig stuff.

  CURIE_WIFI_DEBUG("5. set policy");
  wlan_ioctl_set_connection_policy(0, 0, 0);
  wlan_ioctl_del_profile(255);

  // Not quite sure what this does. Maybe interrupt setup? The
  // HCI_EVNT_WLAN constants are defined in hci.h

  CURIE_WIFI_DEBUG("6. set event msk");

  int err = wlan_set_event_mask(   HCI_EVNT_WLAN_UNSOL_INIT
                                   | HCI_EVNT_WLAN_KEEPALIVE );

  CURIE_WIFI_ASSERT( err == 0, "set event msk" );

  // It worked!

  CURIE_WIFI_DEBUG("Success!");
  m_initialized = true;
}

void CurieWifi::reInit(){
  m_initialized = false;
  begin();
}

//------------------------------------------------------------------------
// CurieWifi::print_mac_addr();
//------------------------------------------------------------------------

void CurieWifi::print_mac_addr()
{
  curie_lcd.setCursor(0,0);
  curie_lcd.print(F("MAC Address     "));
  Serial.print(F(" Wifi Board Mac Address: "));

  uint8_t mac_address[6];
  int err = nvmem_read( NVMEM_MAC_FILEID, 6, 0, mac_address );

  if ( err != 0 ) {
    curie_lcd.setCursor(0,1);
    curie_lcd.print(F("ERROR!"));
    Serial.println(F("\nERROR: Could not get MAC address!"));
    while (1);
  }

  curie_lcd.setCursor(0,1);
  for ( int i = 0; i < 6; i++ ) {
    if ( mac_address[i] < 0xF ) {
      curie_lcd.print("0");
      Serial.print("0");
    }
    curie_lcd.print(mac_address[i],HEX);
    Serial.print(mac_address[i],HEX);
    if ( i != 5 )
      Serial.print(":");
  }
}

//------------------------------------------------------------------------
// CurieWifi::connect
//------------------------------------------------------------------------

void CurieWifi::connect( const char* ssid )
{
  if ( strlen(ssid) != 0 )
    m_ssid = ssid;

  CURIE_WIFI_DEBUG_HEADER("Connect to AP");

  // This forces a check of the interrupts. Note sure why we need this?

  cc3k_int_poll();

  // Scan SSIDs. Adafruit is not sure why we need to do this SSID scan
  // ... but it doesn't work without it!

  CURIE_WIFI_DEBUG("1. scan SSIDs");
  scan_ssids(4000);

  CURIE_WIFI_DEBUG("2. wait 5 secs");
  delay(4500);
  scan_ssids(0);

  // Start connection to access point. The driver returns before the
  // connection is established. The driver will call the EventCallback
  // when the connection is established, and that callback will set the
  // g_have_connection and g_have_ip variables.

  CURIE_WIFI_DEBUG("3. start connect");

  g_have_connection = 0;
  g_have_ip         = 0;
  int err = wlan_connect(m_ssid,strlen(m_ssid));

  CURIE_WIFI_ASSERT( err == 0, "no connect" );

  // Now we wait up to 15 seconds until the connection is established.

  CURIE_WIFI_DEBUG("4. wait connect");

  int timer = 30000;
  while ( (timer > 0) && !g_have_connection ) {
    cc3k_int_poll();
    delay(10);
    timer -= 10;
  }

  CURIE_WIFI_ASSERT( timer > 0, "conn. timeout" );

  // Now we wait up to 15 seconds for client IP

  CURIE_WIFI_DEBUG("5. wait dhcp");

  timer = 30000;
  while ( (timer > 0) && !g_have_ip ) {
    cc3k_int_poll();
    delay(10);
    timer -= 10;
  }

  CURIE_WIFI_ASSERT( timer > 0, "dhcp timeout " );
  CURIE_WIFI_DEBUG("Connected!");
}

//------------------------------------------------------------------------
// CurieWifi::disconnect
//------------------------------------------------------------------------

void CurieWifi::disconnect()
{
  if ( !m_initialized )
    return;

  int err = wlan_disconnect();

  if ( err != 0 ) {
    Serial.println(F("ERROR: Could not disconnect!"));
    while (1);
  }
}

bool CurieWifi::isConnected(){
  return g_have_connection == 1;
}

bool CurieWifi::isSocketOpen(){
  return g_socket_open == 1;
}


//------------------------------------------------------------------------
// CurieWifiClient Constructor
//------------------------------------------------------------------------

CurieWifiClient::CurieWifiClient( uint8_t ip_a, uint8_t ip_b, uint8_t ip_c, uint8_t ip_d )
{
  m_rx_buf_sz  = 0;
  m_rx_buf_idx = 0;

  createSocket();

  CURIE_WIFI_ASSERT( m_socket != -1, "socket create" );

  sockaddr socket_addr;
  uint16_t dest_port = 80;
  memset( &socket_addr, 0x00, sizeof(socket_addr) );
  socket_addr.sa_family = AF_INET;
  socket_addr.sa_data[0] = (dest_port & 0xFF00) >> 8; // port num
  socket_addr.sa_data[1] = (dest_port & 0x00FF);
  socket_addr.sa_data[2] = ip_a;
  socket_addr.sa_data[3] = ip_b;
  socket_addr.sa_data[4] = ip_c;
  socket_addr.sa_data[5] = ip_d;

  verifyConnection();

  int  count = 0;
  bool retry = true;
  while ( (count < 5) && retry ) {
    int err = connect( m_socket, &socket_addr, sizeof(socket_addr) );
    if ( err != -1 ) {
      retry = false;
    }
    else {
      CURIE_WIFI_DEBUG("Could not connect, retrying ...");
      closesocket( m_socket );
      count++;
    }
  }

  // Failed in connecting, will restart the wifi module
  /* for(int c = 0; c < 4 && err == -1; ++c){
     Serial.println(F("Restarting wifi, failed to connect"));
     curie_wifi.disconnect();
     curie_wifi.reInit();
     curie_wifi.connect(CURIESSID);
     err = connect( m_socket, &socket_addr, sizeof(socket_addr) );
     }*/

  CURIE_WIFI_ASSERT( !retry,        "socket conn.a" );
  CURIE_WIFI_ASSERT( m_socket >= 0, "socket conn.b" );
}

//------------------------------------------------------------------------
// CurieWifiClient::createSocket
//------------------------------------------------------------------------

void CurieWifiClient::createSocket(){

  m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

  // Failed in creating socket, restart wifi module and try again

  for ( int c = 0; c < 4 && m_socket < 0; ++c ) {
    //Serial.println(F("Restarting wifi because of socket create"));
    //curie_wifi.begin();
    //curie_wifi.connect(F(CURIE2014));
    Serial.print(F("Retrying to create socket: Try #"));
    Serial.println(c);
    m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
  }
  if ( m_socket >= 0 )
    g_socket_open = 1;
}


//------------------------------------------------------------------------
// CurieWifiClient::verifyConnection
//------------------------------------------------------------------------

void CurieWifiClient::verifyConnection(){

  // If the socket got closed then recreate it

  if ( !curie_wifi.isSocketOpen() ) {
    Serial.println("Socket was closed! \n");
    createSocket();
  }

  // If the internet is not connected, try and reconnect
  if ( !curie_wifi.isConnected() ) {
    Serial.println("Lost Internet! \n");
    curie_wifi.disconnect();
    curie_wifi.connect();
  }

  // If the socket is still not open or the board is still
  // not connected then restart the wifi board

  if ( !curie_wifi.isSocketOpen() || !curie_wifi.isConnected() ) {
    curie_wifi.disconnect();
    curie_wifi.reInit();
    curie_wifi.connect();
    createSocket();
    CURIE_WIFI_ASSERT( curie_wifi.isSocketOpen(), "socket conn.1"  );
    CURIE_WIFI_ASSERT( curie_wifi.isConnected(),  "internet conn." );
  }
}

//------------------------------------------------------------------------
// CurieWifiClient::send_request
//------------------------------------------------------------------------

void CurieWifiClient::send_request( const String& str )
{
  char      tx_buf[CURIE_WIFI_TX_BUF_SIZE];
  int       tx_buf_idx = 0;

  // Send data CURIE_WIFI_TX_BUF_SIZE bytes at a time

  for ( unsigned int i = 0; i < str.length(); i++ ) {
    tx_buf[tx_buf_idx] = str.charAt(i);
    tx_buf_idx++;
    if ( tx_buf_idx == CURIE_WIFI_TX_BUF_SIZE ) {
      verifyConnection();
      int err = send( m_socket, tx_buf, CURIE_WIFI_TX_BUF_SIZE, 0 );
      CURIE_WIFI_ASSERT( err != -1, "socket send  " );
      tx_buf_idx = 0;
    }
  }

  // Send any remaining data in the transmit buffer

  if ( tx_buf_idx > 0 ) {
    verifyConnection();
    int err = send( m_socket, tx_buf, tx_buf_idx, 0 );
    CURIE_WIFI_ASSERT( err != -1, "socket send  " );
  }
}

//------------------------------------------------------------------------
// CurieWifiClient::send_request
//------------------------------------------------------------------------

void CurieWifiClient::send_request( const __FlashStringHelper* str )
{
  char      tx_buf[CURIE_WIFI_TX_BUF_SIZE];
  int       tx_buf_idx = 0;

  // Send data CURIE_WIFI_TX_BUF_SIZE bytes at a time

  const char PROGMEM *p = (const char PROGMEM*) str;

  for ( uint8_t c; (c = pgm_read_byte(p)); p++ ) {
    tx_buf[tx_buf_idx] = (char) c;
    tx_buf_idx++;
    if ( tx_buf_idx == CURIE_WIFI_TX_BUF_SIZE ) {
      verifyConnection();
      int err = send( m_socket, tx_buf, CURIE_WIFI_TX_BUF_SIZE, 0 );
      CURIE_WIFI_ASSERT( err != -1, "socket send  " );
      tx_buf_idx = 0;
    }
  }

  // Send any remaining data in the transmit buffer

  if ( tx_buf_idx > 0 ) {
    verifyConnection();
    int err = send( m_socket, tx_buf, tx_buf_idx, 0 );
    CURIE_WIFI_ASSERT( err != -1, "socket send  " );
  }
}

//------------------------------------------------------------------------
// CurieWifiClient::print_response
//------------------------------------------------------------------------
// Print everything, including the headers.

void CurieWifiClient::print_response()
{
  wait();
  while ( available() )
    Serial.print(read());
  closesocket( m_socket );
  // Waits until the socket is closed before proceeding
  // while ( g_socket_open );
}

//------------------------------------------------------------------------
// CurieWifiClient::recv_response_data
//------------------------------------------------------------------------
// We only want to capture a single line of the actual response data
// without any HTTP headers. The headers are separated from the data by
// two back-to-back newlines, but the only catch is that newlines are
// represented as a carriage-return/linefeed pair (i.e., "\r\n"). The
// response data is written into the given buffer. We use a little state
// machine to implement this logic.

void CurieWifiClient::recv_response_data( char* buf, int buf_sz, char delim )
{
  recv_header();
  recv_response_line( 0, 0, buf, buf_sz, delim );
  close();
}

//------------------------------------------------------------------------
// CurieWifiClient::recv_header
//------------------------------------------------------------------------

void CurieWifiClient::recv_header()
{
  // Before we do anything, wait to get at least some data

  wait();

  // We store the first 12 characters of the header to verify that we
  // received a valid HTTP response.

  int8_t header_str_idx = 0;
  char   header_str[13];
  header_str[12] = '\0';

  // Here are the variables we will use to track what state we are in

  const int8_t STATE_CHECK_HEADER        = 0;
  const int8_t STATE_FIND_FIRST_NEWLINE  = 1;
  const int8_t STATE_FIND_SECOND_NEWLINE = 2;
  const int8_t STATE_EAT_CHAR            = 3;

  int8_t state = STATE_CHECK_HEADER;

  bool done   = false;
  bool failed = true;
  while ( available() && !done ) {
    char c = read();

    // FIND_CHECK_HEADER: Copy first 12 characters to check that this
    // first line is correct. This helps us catch issues where we cannot
    // get what we want from the server.

    if ( state == STATE_CHECK_HEADER ) {
      if ( header_str_idx == 12 ) {

        if ( strcmp( header_str, "HTTP/1.1 200" ) != 0 ) {
          curie_lcd.setCursor(0,1);
          curie_lcd.print(F("E: Invalid resp!"));
          Serial.print(F("ERROR: Invalid response from server: "));
          Serial.println(header_str);
          while (1);
        }

        failed = false;
        state = STATE_FIND_FIRST_NEWLINE;
      }
      else
        header_str[header_str_idx++] = c;
    }

    // FIND_FIRST_NEWLINE: Wait until we see a newline character
    // then move into the FIND_SECOND_NEWLINE state.

    else if ( state == STATE_FIND_FIRST_NEWLINE ) {
      if ( c == '\n' )
        state = STATE_FIND_SECOND_NEWLINE;
    }

    // FIND_SECOND_NEWLINE: If this character is not a newline then we
    // need to go back to the FIND_FIRST_NEWLINE state to keep looking.
    // If this character is a newline, then we have found the end of the
    // headers. We need to go to the EAT_CHAR state to eat the extra '\n'
    // which is right after the '\r'.

    else if ( state == STATE_FIND_SECOND_NEWLINE ) {
      if ( c == '\r' )
        state = STATE_EAT_CHAR;
      else
        state = STATE_FIND_FIRST_NEWLINE;
    }

    // EAT_CHAR: Basically just eat one character.

    else if ( state == STATE_EAT_CHAR ) {
      done = true;
    }

  }

  if ( failed ) {
    curie_lcd.setCursor(0,1);
    curie_lcd.print(F("E: No resp!"));
    Serial.println(F("ERROR: No response from server!"));
    while (1);
  }

}

//------------------------------------------------------------------------
// CurieWifiClient::recv_response_value
//------------------------------------------------------------------------

void CurieWifiClient::recv_response_line
(
  char* name_buf,
  int   name_buf_sz,
  char* value_buf,
  int   value_buf_sz,
  char  delim
){
  // Clear the return buffer. This also ensures the returned string
  // terminates with a null character.

  if ( name_buf != 0 )
    memset( name_buf, 0, name_buf_sz );

  if ( value_buf != 0 )
    memset( value_buf, 0, value_buf_sz );

  // Here are the variables we will use to track what state we are in

  const int8_t STATE_FIND_DELIM1         = 0;
  const int8_t STATE_FIND_DELIM2         = 1;
  const int8_t STATE_COPY_TO_BUFFER      = 2;

  int8_t state = STATE_FIND_DELIM1;
  if ( name_buf == 0 )
    state = STATE_FIND_DELIM2;
  if ( delim == '\0' )
    state = STATE_COPY_TO_BUFFER;

  int8_t name_buf_idx  = 0;
  int8_t value_buf_idx = 0;

  bool done   = false;
  bool failed = true;
  while ( available() && !done ) {
    char c = read();

    // FIND_DELIM1: Copy to name buffer and wait until we see delim.

    if ( state == STATE_FIND_DELIM1 ) {
      if ( c == ',' )
        state = STATE_FIND_DELIM2;
      else
        name_buf[name_buf_idx++] = c;

      if ( name_buf_idx == (name_buf_sz-1) )
        done = true;
    }

    // FIND_DELIM: Wait until we see the delim character then go back to
    // copying into value buffer.

    else if ( state == STATE_FIND_DELIM2 ) {
      if ( c == ',' )
        state = STATE_COPY_TO_BUFFER;
    }

    // COPY_TO_BUFFER: Copy each character to the output buffer until we
    // see a newline or we run out of room in the output buffer.

    else if ( state == STATE_COPY_TO_BUFFER ) {
      if ( c == '\n' )
        done = true;
      else
        value_buf[value_buf_idx++] = c;

      if ( value_buf_idx == (value_buf_sz-1) )
        done = true;
    }

  }
}

void CurieWifiClient::close()
{
  closesocket( m_socket );
}

//------------------------------------------------------------------------
// CurieWifiClient::wait (protected)
//------------------------------------------------------------------------
// Wait for response up to 10 seconds, then assume a time out.

void CurieWifiClient::wait()
{
  int timer = 10000;
  while ( (timer > 0) && ((m_rx_buf_sz <= 0) || (m_rx_buf_sz == m_rx_buf_idx)) ) {
    cc3k_int_poll();

    // buffer in some more data
    m_rx_buf_sz = recv( m_socket, m_rx_buf, sizeof(m_rx_buf), 0 );

    if ( m_rx_buf_sz == -57 ) {
      closesocket( m_socket );
      return;
    }

    m_rx_buf_idx = 0;
    delay(10);
    timer -= 10;
  }

  CURIE_WIFI_ASSERT( timer > 0, "clnt timeout " );
}

//------------------------------------------------------------------------
// CurieWifiClient::read (protected)
//------------------------------------------------------------------------

char CurieWifiClient::read()
{
  while ( (m_rx_buf_sz <= 0) || (m_rx_buf_sz == m_rx_buf_idx) ) {
    cc3k_int_poll();

    // buffer in some more data
    m_rx_buf_sz = recv( m_socket, m_rx_buf, sizeof(m_rx_buf), 0 );

    if ( m_rx_buf_sz == -57 ) {
      closesocket( m_socket );
      return 0;
    }

    m_rx_buf_idx = 0;
  }

  uint8_t ret = m_rx_buf[m_rx_buf_idx];
  m_rx_buf_idx++;
  return ret;
}

//------------------------------------------------------------------------
// CurieWifiClient::available (protected)
//------------------------------------------------------------------------

uint8_t CurieWifiClient::available()
{
  // If we still have data in the internal buffer and we have not
  // returned it all through the read() function, then return how much
  // data is still left in the internal buffer.

  if ( (m_rx_buf_sz > 0) && (m_rx_buf_idx < m_rx_buf_sz) )
    return ( m_rx_buf_sz - m_rx_buf_idx );

  // Do a select() call on this socket

  timeval timeout;
  fd_set fd_read;

  memset( &fd_read, 0, sizeof(fd_read) );
  FD_SET( m_socket, &fd_read );

  timeout.tv_sec  = 0;
  timeout.tv_usec = 5000; // 5 millisec

  int16_t s = select( m_socket+1, &fd_read, NULL, NULL, &timeout );

  if ( s == 1 )
    return 1;  // some data is available to read
  else
    return 0;  // no data is available
}
