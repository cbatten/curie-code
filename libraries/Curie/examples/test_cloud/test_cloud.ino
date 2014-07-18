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
// test_recv_multiple
//------------------------------------------------------------------------

void test_recv_multiple()
{
  curie_cloud.recv_prefetch(
    "always_13",
    "always_14",
    "always_15",
    "always_16"
  );

  int value1 = curie_cloud.recv_int( "always_13" );
  TEST_CHECK( value1 == 13 );

  int value2 = curie_cloud.recv_int( "always_14" );
  TEST_CHECK( value2 == 14 );

  int value3 = curie_cloud.recv_int( "always_15" );
  TEST_CHECK( value3 == 15 );

  int value4 = curie_cloud.recv_int( "always_16" );
  TEST_CHECK( value4 == 16 );
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

  curie_cloud.recv_prefetch(
    "test_channel_a",
    "test_channel_b",
    "test_channel_c",
    "test_channel_d"
  );

  int value1 = curie_cloud.recv_int( "test_channel_a" );
  TEST_CHECK( value1 == 1 );

  int value2 = curie_cloud.recv_int( "test_channel_b" );
  TEST_CHECK( value2 == 2 );

  int value3 = curie_cloud.recv_int( "test_channel_c" );
  TEST_CHECK( value3 == 3 );

  int value4 = curie_cloud.recv_int( "test_channel_d" );
  TEST_CHECK( value4 == 4 );

  curie_cloud.send_multiple(
    "test_channel_a", String(5),
    "test_channel_b", String(6)
  );

  curie_cloud.recv_prefetch(
    "test_channel_a",
    "test_channel_b"
  );

  int value5 = curie_cloud.recv_int( "test_channel_a" );
  TEST_CHECK( value5 == 5 );

  int value6 = curie_cloud.recv_int( "test_channel_b" );
  TEST_CHECK( value6 == 6 );

  curie_cloud.send_multiple(
    "test_channel_c", String(7),
    "test_channel_d", String(8),
    "test_channel_a", String(9)
  );

  curie_cloud.recv_prefetch(
    "test_channel_c",
    "test_channel_d",
    "test_channel_a"
  );

  int value7 = curie_cloud.recv_int( "test_channel_c" );
  TEST_CHECK( value7 == 7 );

  int value8 = curie_cloud.recv_int( "test_channel_d" );
  TEST_CHECK( value8 == 8 );

  int value9 = curie_cloud.recv_int( "test_channel_a" );
  TEST_CHECK( value9 == 9 );
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

