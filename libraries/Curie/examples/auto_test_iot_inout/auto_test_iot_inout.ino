//========================================================================
// CURIE IoT Output Device Unit Tests
//========================================================================

#include <Curie.h>
#include <Wire.h>
#include <SPI.h>

//------------------------------------------------------------------------
// Setup
//------------------------------------------------------------------------

void setup()
{
  curie_cloud.begin( CURIE_CLOUD_GROUP(0) );
  curie_lcd.clear();
}

//------------------------------------------------------------------------
// Loop
//------------------------------------------------------------------------

int counter = 0;

void loop()
{
  // Send multiple

  curie_lcd.setCursor(0,0);
  curie_lcd.print("send to cloud  ");

  curie_cloud.send_multiple(
    "test_channel_a", String(counter),
    "test_channel_b", String(counter),
    "test_channel_c", String(counter),
    "test_channel_d", String(counter)
  );

  curie_lcd.setCursor(0,0);
  curie_lcd.print("               ");

  // Wait 4 seconds

  delay(2500);

  // Recv multiple

  curie_lcd.setCursor(0,0);
  curie_lcd.print("recv from cloud");

  curie_cloud.recv_prefetch(
    "test_channel_a",
    "test_channel_b",
    "test_channel_c",
    "test_channel_d"
  );

  curie_lcd.setCursor(0,0);
  curie_lcd.print("               ");

  // Verify

  if ( curie_cloud.recv_int( "test_channel_a" ) != counter ) {
    curie_lcd.setCursor(0,1);
    curie_lcd.print("ERROR! (a)");
    while (1);
  }

  if ( curie_cloud.recv_int( "test_channel_b" ) != counter ) {
    curie_lcd.setCursor(0,1);
    curie_lcd.print("ERROR! (b)");
    while (1);
  }

  if ( curie_cloud.recv_int( "test_channel_c" ) != counter ) {
    curie_lcd.setCursor(0,1);
    curie_lcd.print("ERROR! (c)");
    while (1);
  }

  if ( curie_cloud.recv_int( "test_channel_d" ) != counter ) {
    curie_lcd.setCursor(0,1);
    curie_lcd.print("ERROR! (d)");
    while (1);
  }

  // Wait 4 seconds

  delay(2500);

  // Update counter

  counter++;

}

