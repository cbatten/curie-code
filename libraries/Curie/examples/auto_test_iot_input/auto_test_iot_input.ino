//========================================================================
// CURIE IoT Input Device Unit Tests
//========================================================================

#include <Curie.h>
#include <Wire.h>
#include <SPI.h>

//------------------------------------------------------------------------
// Global constants for pin assignments
//------------------------------------------------------------------------

int pin_button = 3;

//------------------------------------------------------------------------
// Setup
//------------------------------------------------------------------------

void setup()
{
  // Setup the CURIE cloud

  curie_cloud.begin( CURIE_CLOUD_GROUP(0) );

  // Set the door switch to be an input pin

  pinMode( pin_button, INPUT );

  // Initial output to LCD

  curie_lcd.clear();
  curie_lcd.print("Door Status: 0");
}

//------------------------------------------------------------------------
// Loop
//------------------------------------------------------------------------

int door_status = 0;

void loop()
{
  // Read the door status

  int current_door_status = !digitalRead( pin_button );

  // If door status has changed, then post updated status to cloud

  if ( current_door_status != door_status ) {

    // Update the LCD

    curie_lcd.setCursor(0,0);
    curie_lcd.print("Door Status: ");
    curie_lcd.print(current_door_status);

    // Send new value to the cloud

    curie_cloud.send_int( "door_status", door_status         );
    curie_cloud.send_int( "door_status", current_door_status );

    // Update the previous door status

    door_status = current_door_status;
  }

  // Wait until checking the door again

  delay(100);
}

