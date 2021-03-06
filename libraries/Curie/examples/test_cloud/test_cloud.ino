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
// test_send_multiple
//------------------------------------------------------------------------
// Test sending data to cloud.

void test_send_multiple()
{
  curie_cloud.send_multiple(
    "test_channel_a", String(1),
    "test_channel_b", String(2),
    "test_channel_c", String(3),
    "test_channel_d", String(4)
  );

  int value1 = curie_cloud.recv_int( "test_channel_a" );
  TEST_CHECK( value1 == 1 );

  int value2 = curie_cloud.recv_int( "test_channel_b" );
  TEST_CHECK( value2 == 2 );

  int value3 = curie_cloud.recv_int( "test_channel_c" );
  TEST_CHECK( value3 == 3 );

  int value4 = curie_cloud.recv_int( "test_channel_d" );
  TEST_CHECK( value4 == 4 );
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
  tests.add( TEST_CASE( test_send_multiple ) );
}

//------------------------------------------------------------------------
// Loop
//------------------------------------------------------------------------

void loop()
{
  tests.run();
}

