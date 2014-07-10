//========================================================================
// CURIE Cloud Unit Tests
//========================================================================

#include <Curie.h>
#include <Wire.h>
#include <SPI.h>

//------------------------------------------------------------------------
// test_recv
//------------------------------------------------------------------------
// Test receiving data from cloud.

void test_recv()
{
  int value = curie_cloud.recv_int( "always_42" );
  TEST_CHECK( value == 42 );
}

//------------------------------------------------------------------------
// test_send
//------------------------------------------------------------------------
// Test sending data to cloud.

void test_send()
{
  curie_cloud.send_int( "test_channel", 1 );
  int value = curie_cloud.recv_int( "test_channel" );
  TEST_CHECK( value == 1 );

  curie_cloud.send_int( "test_channel", 0 );
  value = curie_cloud.recv_int( "test_channel" );
  TEST_CHECK( value == 0 );
}

//------------------------------------------------------------------------
// Setup
//------------------------------------------------------------------------

CurieTest tests("test_wifi");

void setup()
{
  curie_cloud.begin( CURIE_CLOUD_GROUP(0) );

  tests.begin();
  tests.add( TEST_CASE( test_recv          ) );
  tests.add( TEST_CASE( test_send          ) );
}

//------------------------------------------------------------------------
// Loop
//------------------------------------------------------------------------

void loop()
{
  tests.run();
}

