//========================================================================
// CURIE IoT Output Device Unit Tests
//========================================================================

#include <Curie.h>
#include <Wire.h>
#include <SPI.h>
#include <MemoryFree.h>

//------------------------------------------------------------------------
// Global constants for pin assignments
//------------------------------------------------------------------------

int pin_led = 3;
int times_passed = 0;

//------------------------------------------------------------------------
// Setup
//------------------------------------------------------------------------

void setup()
{
  // Setup the CURIE cloud

  curie_cloud.begin( CURIE_CLOUD_GROUP(0) );

  // Set the LED to be an output pin

  pinMode( pin_led, OUTPUT );
  digitalWrite( pin_led, LOW );

  // Initial output to LCD

  curie_lcd.clear();
}

//------------------------------------------------------------------------
// Loop
//------------------------------------------------------------------------

void loop()
{
  // Read the door status from the cloud

  curie_lcd.setCursor(0,1);
  curie_lcd.print("checking cloud");

  int door_status = curie_cloud.recv_int( "door_status" );
  ++times_passed;
  curie_lcd.setCursor(0,1);
  curie_lcd.print("              ");

  // Update the LCD
  curie_lcd.setCursor(0,0);
  curie_lcd.print("# Success: ");
  curie_lcd.print(times_passed);

  // Update the LED

  if ( door_status )
    digitalWrite( pin_led, HIGH );
  else
    digitalWrite( pin_led, LOW  );

  // Wait until checking the door again

  delay(4000);
}
