//========================================================================
// WiFi Unit Tests
//========================================================================

#include <Curie.h>
#include <Wire.h>
#include <SPI.h>
#include <string.h>

//------------------------------------------------------------------------
// test_connect
//------------------------------------------------------------------------
// Test connecting to open access point.

void test_connect()
{
  curie_wifi.connect(F("RedRover"));
  TEST_PASS( "Successfully connected to RedRover" );
}

//------------------------------------------------------------------------
// test_print_resp
//------------------------------------------------------------------------
// Test printing response from server.

void test_print_resp()
{
  CurieWifiClient client( 128,84,224,10 );
  TEST_PASS( "Successfully connected to server" );

  client.send_request(F(
    "GET /curie2014/curie-wifi-test.txt HTTP/1.1\n"
    "Host: www.csl.cornell.edu\n"
    "Connection: close\n\n"
  ));
  TEST_PASS( "Successfully sent request" );

  client.print_response();
  TEST_PASS( "Successfully received response" );
}

//------------------------------------------------------------------------
// test_recv_csl
//------------------------------------------------------------------------
// Test getting data from server.

void test_recv_csl()
{
  CurieWifiClient client( 128,84,224,10 );
  TEST_PASS( "Successfully connected to server" );

  client.send_request(F(
    "GET /curie2014/curie-wifi-test.txt HTTP/1.1\n"
    "Host: www.csl.cornell.edu\n"
    "Connection: close\n\n"
  ));
  TEST_PASS( "Successfully sent request" );

  char buf[32];
  client.recv_response_data( buf, 32 );
  TEST_PASS( "Successfully received response" );
  TEST_CHECK( strcmp( buf, "wifi test : successful" ) == 0 );
}

//------------------------------------------------------------------------
// test_recv_csl_with_delim
//------------------------------------------------------------------------
// Test getting data from server with a delimiter in the data.

void test_recv_csl_with_delim()
{
  CurieWifiClient client( 128,84,224,10 );
  TEST_PASS( "Successfully connected to server" );

  client.send_request(F(
    "GET /curie2014/curie-wifi-test.txt HTTP/1.1\n"
    "Host: www.csl.cornell.edu\n"
    "Connection: close\n\n"
  ));
  TEST_PASS( "Successfully sent request" );

  char buf[32];
  client.recv_response_data( buf, 32, ':' );
  TEST_PASS( "Successfully received response" );
  TEST_CHECK( strcmp( buf, " successful" ) == 0 );
}

//------------------------------------------------------------------------
// test_recv_xively
//------------------------------------------------------------------------
// Test getting data from xively.

void test_recv_xively()
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
// test_send_xively
//------------------------------------------------------------------------
// Test sending/receiving data to xively.

void test_send_xively()
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

  delay(1000);

  client0.recv_response();
  TEST_PASS( "Successfully received response" );

  delay(1000);

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

  delay(1000);

  char buf[32];
  client1.recv_response_data( buf, 32, ',' );
  TEST_PASS( "Successfully received response" );
  TEST_CHECK( strcmp( buf, "1" ) == 0 );

  delay(1000);

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

  delay(1000);

  client2.recv_response();
  TEST_PASS( "Successfully received response" );

  delay(1000);

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

  delay(1000);

  client3.recv_response_data( buf, 32, ',' );
  TEST_PASS( "Successfully received response" );
  TEST_CHECK( strcmp( buf, "0" ) == 0 );
}

//------------------------------------------------------------------------
// test_disconnect
//------------------------------------------------------------------------
// Test disconnecting from access point.

void test_disconnect()
{
  delay(2000);
  curie_wifi.disconnect();
  TEST_PASS( "Successfully discconnected from RedRover" );
}

//------------------------------------------------------------------------
// Setup
//------------------------------------------------------------------------

CurieTest tests("test_wifi");

void setup()
{
  curie_wifi.begin();

  tests.begin();
  tests.add( TEST_CASE( test_connect             ) );
  tests.add( TEST_CASE( test_print_resp          ) );
  tests.add( TEST_CASE( test_recv_csl            ) );
  tests.add( TEST_CASE( test_recv_csl_with_delim ) );
  tests.add( TEST_CASE( test_recv_xively         ) );
  tests.add( TEST_CASE( test_send_xively         ) );
  tests.add( TEST_CASE( test_disconnect          ) );
}

//------------------------------------------------------------------------
// Loop
//------------------------------------------------------------------------

void loop()
{
  tests.run();
}

