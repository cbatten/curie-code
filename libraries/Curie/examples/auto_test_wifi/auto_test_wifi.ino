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

  tests.auto_begin();
  tests.add( TEST_CASE( test_connect             ) );
  tests.add( TEST_CASE( test_print_resp          ) );
  tests.add( TEST_CASE( test_recv_csl            ) );
  tests.add( TEST_CASE( test_recv_csl_with_delim ) );
  tests.add( TEST_CASE( test_disconnect          ) );
}

//------------------------------------------------------------------------
// Loop
//------------------------------------------------------------------------

void loop()
{
  tests.auto_run();
}

