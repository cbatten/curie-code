//========================================================================
// Xively Unit Tests
//========================================================================

#include <Curie.h>
#include <Wire.h>
#include <SPI.h>
#include <string.h>

//------------------------------------------------------------------------
// test_recv
//------------------------------------------------------------------------
// Test getting data from xively.

void test_recv()
{
  CurieWifiClient client( 64,94,18,120 );
  TEST_PASS( "Successfully connected to server" );

  client.send_request(F(
    "GET /v2/feeds/324666390/datastreams/always_42.csv HTTP/1.1\n"
    "Host: api.xively.edu\n"
    "X-ApiKey: Dmk0kNcXz4bCxSPUW6ODyLFZSZYrslX09dw7Jf2WD4FA6HDk\n"
    "Connection: close\n\n"
  ));
  TEST_PASS( "Successfully sent request" );

  char buf[32];
  client.recv_response_data( buf, 32, ',' );
  TEST_PASS( "Successfully received response" );
  TEST_CHECK( strcmp( buf, "42" ) == 0 );
}

//------------------------------------------------------------------------
// test_send
//------------------------------------------------------------------------
// Test sending/receiving data to xively.

void test_send()
{
  CurieWifiClient client0( 64,94,18,120 );
  TEST_PASS( "Successfully connected to server" );

  // First set value to one

  client0.send_request(F(
    "PUT /v2/feeds/324666390.csv HTTP/1.1\n"
    "Host: api.xively.edu\n"
    "X-ApiKey: Dmk0kNcXz4bCxSPUW6ODyLFZSZYrslX09dw7Jf2WD4FA6HDk\n"
    "Content-Length: 14\n"
    "Connection: close\n\n"
    "test_channel,1\n"
  ));
  TEST_PASS( "Successfully sent request" );

  client0.recv_header();
  client0.close();
  TEST_PASS( "Successfully received response" );

  // Check that the value is indeed set to one

  CurieWifiClient client1( 64,94,18,120 );
  TEST_PASS( "Successfully connected to server" );

  client1.send_request(F(
    "GET /v2/feeds/324666390/datastreams/test_channel.csv HTTP/1.1\n"
    "Host: api.xively.edu\n"
    "X-ApiKey: Dmk0kNcXz4bCxSPUW6ODyLFZSZYrslX09dw7Jf2WD4FA6HDk\n"
    "Connection: close\n\n"
  ));
  TEST_PASS( "Successfully sent request" );

  char buf[32];
  client1.recv_response_data( buf, 32, ',' );
  TEST_PASS( "Successfully received response" );
  TEST_CHECK( strcmp( buf, "1" ) == 0 );

  // Now set data back to zero

  CurieWifiClient client2( 64,94,18,120 );
  TEST_PASS( "Successfully connected to server" );

  client2.send_request(F(
    "PUT /v2/feeds/324666390.csv HTTP/1.1\n"
    "Host: api.xively.edu\n"
    "X-ApiKey: Dmk0kNcXz4bCxSPUW6ODyLFZSZYrslX09dw7Jf2WD4FA6HDk\n"
    "Content-Length: 14\n"
    "Connection: close\n\n"
    "test_channel,0\n"
  ));
  TEST_PASS( "Successfully sent request" );

  client2.recv_header();
  client2.close();
  TEST_PASS( "Successfully received response" );

  // Check that the value is indeed set back to zero

  CurieWifiClient client3( 64,94,18,120 );
  TEST_PASS( "Successfully connected to server" );

  client3.send_request(F(
    "GET /v2/feeds/324666390/datastreams/test_channel.csv HTTP/1.1\n"
    "Host: api.xively.edu\n"
    "X-ApiKey: Dmk0kNcXz4bCxSPUW6ODyLFZSZYrslX09dw7Jf2WD4FA6HDk\n"
    "Connection: close\n\n"
  ));
  TEST_PASS( "Successfully sent request" );

  client3.recv_response_data( buf, 32, ',' );
  TEST_PASS( "Successfully received response" );
  TEST_CHECK( strcmp( buf, "0" ) == 0 );
}

//------------------------------------------------------------------------
// test_recv_multiple
//------------------------------------------------------------------------
// Test getting data from xively.

void test_recv_multiple()
{
  CurieWifiClient client( 64,94,18,120 );
  TEST_PASS( "Successfully connected to server" );

  client.send_request(F(
    "GET /v2/feeds/324666390.csv HTTP/1.1\n"
    "Host: api.xively.edu\n"
    "X-ApiKey: Dmk0kNcXz4bCxSPUW6ODyLFZSZYrslX09dw7Jf2WD4FA6HDk\n"
    "Connection: close\n\n"
  ));
  TEST_PASS( "Successfully sent request" );

  client.recv_header();

  TEST_PASS( "Successfully received header" );

  bool recv_always_13 = false;
  bool recv_always_14 = false;
  bool recv_always_15 = false;

  while ( client.available() ) {

    char name_buf[32];
    char value_buf[32];

    client.recv_response_line( name_buf, 32, value_buf, 32 );
    TEST_PASS( "Successfully received line of response" );

    if ( strcmp( name_buf, "always_13" ) == 0 ) {
      TEST_CHECK( strcmp( value_buf, "13" ) == 0 );
      recv_always_13 = true;
    }

    if ( strcmp( name_buf, "always_14" ) == 0 ) {
      TEST_CHECK( strcmp( value_buf, "14" ) == 0 );
      recv_always_14 = true;
    }

    if ( strcmp( name_buf, "always_15" ) == 0 ) {
      TEST_CHECK( strcmp( value_buf, "15" ) == 0 );
      recv_always_15 = true;
    }

  }

  TEST_CHECK( recv_always_13 );
  TEST_CHECK( recv_always_14 );
  TEST_CHECK( recv_always_15 );

  TEST_CHECK( !client.available() );
  client.close();
}

//------------------------------------------------------------------------
// test_send_multiple
//------------------------------------------------------------------------
// Test sending/receiving multiple data items to xively.

void test_send_multiple()
{
  CurieWifiClient client0( 64,94,18,120 );
  TEST_PASS( "Successfully connected to server" );

  // First set values to two and three

  client0.send_request(F(
    "PUT /v2/feeds/324666390.csv HTTP/1.1\n"
    "Host: api.xively.edu\n"
    "X-ApiKey: Dmk0kNcXz4bCxSPUW6ODyLFZSZYrslX09dw7Jf2WD4FA6HDk\n"
    "Content-Length: 34\n"
    "Connection: close\n\n"
    "test_channel_a,2\n"
    "test_channel_b,3\n"
  ));
  TEST_PASS( "Successfully sent request" );

  client0.recv_header();
  client0.close();
  TEST_PASS( "Successfully received response" );

  // Check that values are indeed set to two and three

  CurieWifiClient client1( 64,94,18,120 );
  TEST_PASS( "Successfully connected to server" );

  client1.send_request(F(
    "GET /v2/feeds/324666390/datastreams/test_channel_a.csv HTTP/1.1\n"
    "Host: api.xively.edu\n"
    "X-ApiKey: Dmk0kNcXz4bCxSPUW6ODyLFZSZYrslX09dw7Jf2WD4FA6HDk\n"
    "Connection: close\n\n"
  ));
  TEST_PASS( "Successfully sent request" );

  char buf[32];
  client1.recv_response_data( buf, 32, ',' );
  TEST_PASS( "Successfully received response" );
  TEST_CHECK( strcmp( buf, "2" ) == 0 );

  CurieWifiClient client2( 64,94,18,120 );
  TEST_PASS( "Successfully connected to server" );

  client2.send_request(F(
    "GET /v2/feeds/324666390/datastreams/test_channel_b.csv HTTP/1.1\n"
    "Host: api.xively.edu\n"
    "X-ApiKey: Dmk0kNcXz4bCxSPUW6ODyLFZSZYrslX09dw7Jf2WD4FA6HDk\n"
    "Connection: close\n\n"
  ));
  TEST_PASS( "Successfully sent request" );

  client2.recv_response_data( buf, 32, ',' );
  TEST_PASS( "Successfully received response" );
  TEST_CHECK( strcmp( buf, "3" ) == 0 );

  // Now set both values back to zero

  CurieWifiClient client3( 64,94,18,120 );
  TEST_PASS( "Successfully connected to server" );

  client3.send_request(F(
    "PUT /v2/feeds/324666390.csv HTTP/1.1\n"
    "Host: api.xively.edu\n"
    "X-ApiKey: Dmk0kNcXz4bCxSPUW6ODyLFZSZYrslX09dw7Jf2WD4FA6HDk\n"
    "Content-Length: 34\n"
    "Connection: close\n\n"
    "test_channel_a,0\n"
    "test_channel_b,0\n"
  ));
  TEST_PASS( "Successfully sent request" );

  client3.recv_header();
  client3.close();
  TEST_PASS( "Successfully received response" );

  // Check that both values are indeed set back to zero

  CurieWifiClient client4( 64,94,18,120 );
  TEST_PASS( "Successfully connected to server" );

  client4.send_request(F(
    "GET /v2/feeds/324666390/datastreams/test_channel_a.csv HTTP/1.1\n"
    "Host: api.xively.edu\n"
    "X-ApiKey: Dmk0kNcXz4bCxSPUW6ODyLFZSZYrslX09dw7Jf2WD4FA6HDk\n"
    "Connection: close\n\n"
  ));
  TEST_PASS( "Successfully sent request" );

  client4.recv_response_data( buf, 32, ',' );
  TEST_PASS( "Successfully received response" );
  TEST_CHECK( strcmp( buf, "0" ) == 0 );

  CurieWifiClient client5( 64,94,18,120 );
  TEST_PASS( "Successfully connected to server" );

  client4.send_request(F(
    "GET /v2/feeds/324666390/datastreams/test_channel_b.csv HTTP/1.1\n"
    "Host: api.xively.edu\n"
    "X-ApiKey: Dmk0kNcXz4bCxSPUW6ODyLFZSZYrslX09dw7Jf2WD4FA6HDk\n"
    "Connection: close\n\n"
  ));
  TEST_PASS( "Successfully sent request" );

  client4.recv_response_data( buf, 32, ',' );
  TEST_PASS( "Successfully received response" );
  TEST_CHECK( strcmp( buf, "0" ) == 0 );
}


//------------------------------------------------------------------------
// Setup
//------------------------------------------------------------------------

CurieTest tests("test_wifi");

void setup()
{
  curie_wifi.begin();
  curie_wifi.connect("curie2014");

  tests.begin();
  tests.add( TEST_CASE( test_recv          ) );
  tests.add( TEST_CASE( test_send          ) );
  tests.add( TEST_CASE( test_recv_multiple ) );
  tests.add( TEST_CASE( test_send_multiple ) );
}

//------------------------------------------------------------------------
// Loop
//------------------------------------------------------------------------

void loop()
{
  tests.run();
}

