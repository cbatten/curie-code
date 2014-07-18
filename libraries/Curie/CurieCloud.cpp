//========================================================================
// CurieCloud
//========================================================================

#include <CurieCloud.h>
#include <CurieWifi.h>
#include <Arduino.h>
#include <stdlib.h>
// #include <MemoryFree.h>

// Global instance

CurieCloud curie_cloud;

//------------------------------------------------------------------------
// TOSTR
//------------------------------------------------------------------------
// Macro to convert the parameter into a string literal.

#define CURIE_CLOUD_STRINGIFY( param_ ) \
  #param_

#define CURIE_CLOUD_TOSTR( param_ ) \
  CURIE_CLOUD_STRINGIFY( param_ )

//------------------------------------------------------------------------
// Debug macros
//------------------------------------------------------------------------

void curie_cloud_inner_print( const __FlashStringHelper* msg )
{
  Serial.print(F("CURIE_DEBUG: "));
  Serial.println(msg);
}

void curie_cloud_debug_header( const __FlashStringHelper* msg )
{
  curie_cloud_inner_print(msg);
}

#define CURIE_CLOUD_DEBUG_HEADER( msg_ ) \
  curie_cloud_debug_header(F(msg_));

void curie_cloud_debug( const __FlashStringHelper* msg )
{
  curie_cloud_inner_print(msg);
}

#define CURIE_CLOUD_DEBUG( msg_ ) \
  curie_cloud_debug(F(msg_));

void curie_cloud_assert( bool result, const __FlashStringHelper* msg )
{
  if ( !(result) ) {
    curie_cloud_inner_print(msg);
    while (1);
  }
}

#define CURIE_CLOUD_ASSERT( expression_, msg_ )   \
  curie_cloud_assert((expression_),F(msg_));

//------------------------------------------------------------------------
// CurieCloud Constructor
//------------------------------------------------------------------------

CurieCloud::CurieCloud()
{
  m_initialized = false;
}

//------------------------------------------------------------------------
// CurieCloud::begin
//------------------------------------------------------------------------

void CurieCloud::begin( const __FlashStringHelper* feed_id,
                        const __FlashStringHelper* api_key,
                        const char* ssid )
{
  if ( m_initialized )
    return;

  curie_wifi.begin();
  curie_wifi.connect(ssid);

  m_feed_id     = feed_id;
  m_api_key     = api_key;
  m_initialized = true;
}

//------------------------------------------------------------------------
// CurieCloud::send_int
//------------------------------------------------------------------------

void CurieCloud::send_int( const char* name, int value )
{
  CurieCloud::send_str(name,String(value));
}

//------------------------------------------------------------------------
// CurieCloud::send_float
//------------------------------------------------------------------------

void CurieCloud::send_float( const char* name, float value )
{
  char buf[32];
  dtostrf(value,4,2,buf);
  CurieCloud::send_str(name,buf);
}


//------------------------------------------------------------------------
// CurieCloud::send_str
//------------------------------------------------------------------------

void CurieCloud::send_str( const char* name, const String& value)
{
  if ( !m_initialized ) {
    Serial.println(F("\nERROR: Must call begin on curie_cloud first!"));
    while (1);
  }

  CURIE_CLOUD_DEBUG_HEADER("Send to Xively");

  // Connect to Xively

  CURIE_CLOUD_DEBUG("1. connecting");

  CurieWifiClient client( 64,94,18,120 );

  // Send the request

  CURIE_CLOUD_DEBUG("2. send req");

  client.send_request(F("PUT /v2/feeds/"));
  client.send_request(m_feed_id);
  client.send_request(F(".csv HTTP/1.1\n"));
  client.send_request(F("Host: api.xively.edu\nX-ApiKey: "));
  client.send_request(m_api_key);
  client.send_request(F("\nConnection: close\nContent-Length: "));

  String data;
  data.reserve(32);
  data = name;
  data += ",";
  data += value;
  data += "\n";

  String data_len_str;
  data_len_str.reserve(5);
  data_len_str = String(data.length());
  data_len_str += "\n\n";

  client.send_request( data_len_str + data );

  // Wait for the reponse

  CURIE_CLOUD_DEBUG("3. wait for resp");

  client.print_response();

  // Success!

  CURIE_CLOUD_DEBUG("Done!");
}

//------------------------------------------------------------------------
// CurieCloud::send_multiple
//------------------------------------------------------------------------

void CurieCloud::send_multiple(
  const char* name0, const String& value0,
  const char* name1, const String& value1,
  const char* name2, const String& value2,
  const char* name3, const String& value3 )
{
  if ( !m_initialized ) {
    Serial.println(F("\nERROR: Must call begin on curie_cloud first!"));
    while (1);
  }

  CURIE_CLOUD_DEBUG_HEADER("Send to Xively");

  // Connect to Xively

  CURIE_CLOUD_DEBUG("1. connecting");

  CurieWifiClient client( 64,94,18,120 );

  // Send the request

  CURIE_CLOUD_DEBUG("2. send req");

  client.send_request(F("PUT /v2/feeds/"));
  client.send_request(m_feed_id);
  client.send_request(F(".csv HTTP/1.1\n"));
  client.send_request(F("Host: api.xively.edu\nX-ApiKey: "));
  client.send_request(m_api_key);
  client.send_request(F("\nConnection: close\nContent-Length: "));

  String data;
  data.reserve(32);

  // First value

  data = name0;
  data += ",";
  data += value0;
  data += "\n";

  // Second value

  if ( strlen(name1) != 0 ) {
    data += name1;
    data += ",";
    data += value1;
    data += "\n";
  }

  // Third value

  if ( strlen(name2) != 0 ) {
    data += name2;
    data += ",";
    data += value2;
    data += "\n";
  }

  // Fourth value

  if ( strlen(name3) != 0 ) {
    data += name3;
    data += ",";
    data += value3;
    data += "\n";
  }

  String data_len_str;
  data_len_str.reserve(5);
  data_len_str = String(data.length());
  data_len_str += "\n\n";

  client.send_request( data_len_str + data );

  // Wait for the reponse

  CURIE_CLOUD_DEBUG("3. wait for resp");

  client.print_response();

  // Success!

  CURIE_CLOUD_DEBUG("Done!");
}

//------------------------------------------------------------------------
// CurieCloud::recv_int
//------------------------------------------------------------------------

int CurieCloud::recv_int( const char* name)
{
   return CurieCloud::recv_str(name).toInt();
}

//------------------------------------------------------------------------
// CurieCloud::recv_float
//------------------------------------------------------------------------

float CurieCloud::recv_float( const char* name )
{
  String val = CurieCloud::recv_str(name);
  char buf[32];
  val.toCharArray(buf,32);
  return atof(buf);
}

//------------------------------------------------------------------------
// CurieCloud::recv_prefetch
//------------------------------------------------------------------------

void CurieCloud::recv_prefetch( const char* name0,
                                const char* name1,
                                const char* name2,
                                const char* name3 )
{
  if ( !m_initialized ) {
    Serial.println(F("\nERROR: Must call begin on curie_cloud first!"));
    while (1);
  }

  m_name0 = name0;
  m_name1 = name1;
  m_name2 = name2;
  m_name3 = name3;

  CURIE_CLOUD_DEBUG_HEADER("Receive prefetch from Xively");

  // Connect to Xively

  CURIE_CLOUD_DEBUG("1. connecting");

  CurieWifiClient client( 64,94,18,120 );

  // Send the request

  CURIE_CLOUD_DEBUG("2. send req");

  client.send_request(F("GET /v2/feeds/"));
  client.send_request(m_feed_id);
  client.send_request(F(".csv HTTP/1.1\n"));
  client.send_request(F("Host: api.xively.edu\nX-ApiKey: "));
  client.send_request(m_api_key);
  client.send_request(F("\nConnection: close\n\n"));

  // Wait for the reponse

  CURIE_CLOUD_DEBUG("3. wait for resp");

  client.recv_header();

  while ( client.available() ) {

    char name_buf[32];
    char value_buf[32];

    client.recv_response_line( name_buf, 32, value_buf, 32 );

    if ( strcmp( name_buf, name0 ) == 0 ) {
      m_buf0 = String(value_buf);
    }

    else if ( strcmp( name_buf, name1 ) == 0 ) {
      m_buf1 = String(value_buf);
    }

    else if ( strcmp( name_buf, name2 ) == 0 ) {
      m_buf2 = String(value_buf);
    }

    else if ( strcmp( name_buf, name3 ) == 0 ) {
      m_buf3 = String(value_buf);
    }

  }

  client.close();

  // Success!

  CURIE_CLOUD_DEBUG("Done!");
}

//------------------------------------------------------------------------
// CurieCloud::recv_str
//------------------------------------------------------------------------

String CurieCloud::recv_str( const char* name )
{
  if ( !m_initialized ) {
    Serial.println(F("\nERROR: Must call begin on curie_cloud first!"));
    while (1);
  }

  if ( strcmp( name, m_name0 ) == 0 ) {
    m_name0 = "";
    return m_buf0;
  }

  else if ( strcmp( name, m_name1 ) == 0 ) {
    m_name1 = "";
    return m_buf1;
  }

  else if ( strcmp( name, m_name2 ) == 0 ) {
    m_name2 = "";
    return m_buf2;
  }

  else if ( strcmp( name, m_name3 ) == 0 ) {
    m_name3 = "";
    return m_buf3;
  }

  CURIE_CLOUD_DEBUG_HEADER("Receive from Xively");

  // Connect to Xively

  CURIE_CLOUD_DEBUG("1. connecting");

  CurieWifiClient client( 64,94,18,120 );

  // Send the request

  CURIE_CLOUD_DEBUG("2. send req");

  client.send_request(F("GET /v2/feeds/"));
  client.send_request(m_feed_id);
  client.send_request(F("/datastreams/"));
  client.send_request(name);
  client.send_request(F(".csv HTTP/1.1\n"));
  client.send_request(F("Host: api.xively.edu\nX-ApiKey: "));
  client.send_request(m_api_key);
  client.send_request(F("\nConnection: close\n\n"));

  // Wait for the reponse

  CURIE_CLOUD_DEBUG("3. wait for resp");

  char buf[32];
  client.recv_response_data( buf, 32, ',' );

  // Parse the response

  CURIE_CLOUD_DEBUG("4. parse resp");

  //int value = String(buf).toInt();

  // Success!

  CURIE_CLOUD_DEBUG("Done!");

  return String(buf);
}

