//========================================================================
// CurieWifi
//========================================================================
// This class encapsulates the wifi board provided with our Curie IoT
// devices. It handles initializing the board and then connecting to an
// access point.

#ifndef CURIE_WIFI_H
#define CURIE_WIFI_H

#include <Arduino.h>
#include <stdint.h>

#define CURIE_WIFI_TX_BUF_SIZE 32
#define CURIE_WIFI_RX_BUF_SIZE 64

//------------------------------------------------------------------------
// CurieWifi
//------------------------------------------------------------------------

class CurieWifi {

 public:

  // Constructor

  CurieWifi();

  // Initialize the wifi board

  void begin();

  // Connect to an open access point

  void connect( const __FlashStringHelper* ssid );

  // Display mac address

  void print_mac_addr();

  // Disconnect from open access point

  void disconnect();

 private:

  bool m_initialized;

};

//------------------------------------------------------------------------
// CurieWifiClient
//------------------------------------------------------------------------

class CurieWifiClient {

 public:

  // Constructor

  CurieWifiClient( uint8_t ip_a, uint8_t ip_b, uint8_t ip_c, uint8_t ip_d );

  // Send a request to the server

  void send_request( const String& str );
  void send_request( const __FlashStringHelper* str );

  // Print response headers and data to the serial terminal

  void print_response();

  // Receive response and verify HTTP status code of 200

  void recv_response();

  // Receive just the data and write it into the given buffer

  void recv_response_data( char* buf, int buf_sz, char delimiter = '\0' );

 protected:

  void    wait();
  char    read();
  uint8_t available();

 private:

  int     m_socket;
  uint8_t m_rx_buf[CURIE_WIFI_RX_BUF_SIZE];
  int     m_rx_buf_idx;
  int     m_rx_buf_sz;

};

// Global instance

extern CurieWifi curie_wifi;

#endif /* CURIE_WIFI_H */

