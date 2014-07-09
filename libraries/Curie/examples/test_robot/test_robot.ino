//========================================================================
// CURIE Robot Unit Tests
//========================================================================

#include <Curie.h>
#include <Wire.h>
#include <SPI.h>

//------------------------------------------------------------------------
// pin assignments
//------------------------------------------------------------------------

// Digital Pins

int pin_button           = 2;
int pin_led1             = 4;
int pin_led2             = 5;
int pin_bump_right       = 6;
int pin_bump_left        = 7;

int pin_motor_left_speed     = 3;
int pin_motor_right_speed     = 11;
int pin_motor_left_dir       = 12;
int pin_motor_right_dir       = 13;

// Analog Pins

int pin_potentiometer    = A5;
int pin_grayscale_sensor = A4;
int pin_infrared_sensor  = A3;

//------------------------------------------------------------------------
// test_leds
//------------------------------------------------------------------------

void test_leds()
{
  digitalWrite( pin_led1, HIGH );
  TEST_PROMPT_YN("Is LED1 on?");

  digitalWrite( pin_led1, LOW );
  TEST_PROMPT_YN("Is LED1 off?");

  digitalWrite( pin_led2, HIGH );
  TEST_PROMPT_YN("Is LED2 on?");

  digitalWrite( pin_led2, LOW );
  TEST_PROMPT_YN("Is LED2 off?");
}

//------------------------------------------------------------------------
// test_button
//------------------------------------------------------------------------

void test_button()
{
  TEST_PROMPT_WAIT("Press and hold button");
  TEST_CHECK( digitalRead( pin_button ) == 0 );

  TEST_PROMPT_WAIT("Release button");
  TEST_CHECK( digitalRead( pin_button ) != 0 );
}

//------------------------------------------------------------------------
// test_potentiometer
//------------------------------------------------------------------------

void test_potentiometer()
{
  TEST_PROMPT_WAIT("Turn pot completely clockwise");
  TEST_CHECK( analogRead( pin_potentiometer ) > 250 );

  TEST_PROMPT_WAIT("Turn pot completely counterclockwise");
  TEST_CHECK( analogRead( pin_potentiometer ) < 5 );
}

//------------------------------------------------------------------------
// test_grayscale_sensor
//------------------------------------------------------------------------

void test_grayscale_sensor()
{
  TEST_PROMPT_WAIT("Put robot over light playing field");
  TEST_CHECK( analogRead( pin_grayscale_sensor ) < 250 );

  TEST_PROMPT_WAIT("Put robot over black playing field");
  TEST_CHECK( analogRead( pin_grayscale_sensor ) > 250 );
}

//------------------------------------------------------------------------
// test_bump_switches
//------------------------------------------------------------------------

void test_bump_switches()
{
  TEST_PROMPT_WAIT("Press and hold left bump switch");
  TEST_CHECK( digitalRead( pin_bump_left ) == 1 );

  TEST_PROMPT_WAIT("Release left bump switch");
  TEST_CHECK( digitalRead( pin_bump_left ) == 0 );

  TEST_PROMPT_WAIT("Press and hold right bump switch");
  TEST_CHECK( digitalRead( pin_bump_right ) == 1 );

  TEST_PROMPT_WAIT("Release right bump switch");
  TEST_CHECK( digitalRead( pin_bump_right ) == 0 );
}

//----------------------------------------------------------------------
// test_drive_motors
//----------------------------------------------------------------------

void test_drive_motors()
{
  // Left drive motor

  digitalWrite( pin_motor_left_dir,  LOW );
  analogWrite( pin_motor_left_speed, 100 );
  TEST_PROMPT_YN("Is left drive motor rotating forward?");

  digitalWrite( pin_motor_left_dir,  LOW );
  analogWrite( pin_motor_left_speed, 200 );
  TEST_PROMPT_YN("Did left drive motor speed increase?");

  digitalWrite( pin_motor_left_dir,  HIGH );
  analogWrite( pin_motor_left_speed, 100  );
  TEST_PROMPT_YN("Is left drive motor rotating backward?");

  digitalWrite( pin_motor_left_dir,  HIGH );
  analogWrite( pin_motor_left_speed, 200  );
  TEST_PROMPT_YN("Did left drive motor speed increase?");

  digitalWrite( pin_motor_left_dir,  LOW );
  analogWrite( pin_motor_left_speed, 0   );
  TEST_PROMPT_YN("Did the left drive motor stop?");

  // Right drive motor

  digitalWrite( pin_motor_right_dir,  LOW );
  analogWrite( pin_motor_right_speed, 100 );
  TEST_PROMPT_YN("Is right drive motor rotating forward?");

  digitalWrite( pin_motor_right_dir,  LOW );
  analogWrite( pin_motor_right_speed, 200 );
  TEST_PROMPT_YN("Did right drive motor speed increase?");

  digitalWrite( pin_motor_right_dir,  HIGH );
  analogWrite( pin_motor_right_speed, 100  );
  TEST_PROMPT_YN("Is right drive motor rotating backward?");

  digitalWrite( pin_motor_right_dir,  HIGH );
  analogWrite( pin_motor_right_speed, 200  );
  TEST_PROMPT_YN("Did right drive motor speed increase?");

  digitalWrite( pin_motor_right_dir,  LOW );
  analogWrite( pin_motor_right_speed, 0   );
  TEST_PROMPT_YN("Did the right drive motor stop?");

  // Both motors

  digitalWrite( pin_motor_left_dir,  LOW );
  analogWrite( pin_motor_left_speed, 100 );
  digitalWrite( pin_motor_right_dir,  LOW );
  analogWrite( pin_motor_right_speed, 100 );
  TEST_PROMPT_YN("Are both drive motors rotating forward?");

  digitalWrite( pin_motor_left_dir,  HIGH );
  analogWrite( pin_motor_left_speed, 100 );
  digitalWrite( pin_motor_right_dir,  HIGH );
  analogWrite( pin_motor_right_speed, 100 );
  TEST_PROMPT_YN("Are both drive motors rotating backward?");

  digitalWrite( pin_motor_right_dir,  LOW );
  analogWrite( pin_motor_right_speed, 0   );
  digitalWrite( pin_motor_left_dir,  LOW );
  analogWrite( pin_motor_left_speed, 0   );
  TEST_PROMPT_YN("Are both drive motors stopped?");
}

//------------------------------------------------------------------------
// test_infrared_sensor
//------------------------------------------------------------------------

void test_infrared_sensor()
{
  TEST_PROMPT_WAIT("Put obstacle ~12 inches in front of robot");
  TEST_CHECK( analogRead( pin_infrared_sensor ) > 190 );

  TEST_PROMPT_WAIT("Put obstacle ~24 inches in front of robot");
  int value = analogRead( pin_infrared_sensor );
  TEST_CHECK( (value > 90) && (value < 190)  );
}

//------------------------------------------------------------------------
// Setup
//------------------------------------------------------------------------

CurieTest tests("test_robot");

void setup()
{
  pinMode( pin_button,        INPUT  );
  pinMode( pin_led1,          OUTPUT );
  pinMode( pin_led2,          OUTPUT );
  pinMode( pin_bump_left,     INPUT  );
  pinMode( pin_bump_right,    INPUT  );

  pinMode( pin_motor_left_speed,  OUTPUT );
  pinMode( pin_motor_right_speed,  OUTPUT );
  pinMode( pin_motor_left_dir,    OUTPUT );
  pinMode( pin_motor_right_dir,    OUTPUT );

  // Makes the input button have a pull-up resistor

  digitalWrite( pin_button, HIGH );

  // Motors are initially turning forward but with zero speed

  digitalWrite( pin_motor_left_dir, LOW );
  digitalWrite( pin_motor_right_dir, LOW );
  analogWrite( pin_motor_left_speed, 0 );
  analogWrite( pin_motor_right_speed, 0 );

  // Setup tests

  tests.begin();
  tests.add( TEST_CASE( test_leds             ) );
  tests.add( TEST_CASE( test_button           ) );
  tests.add( TEST_CASE( test_potentiometer    ) );
  tests.add( TEST_CASE( test_grayscale_sensor ) );
  tests.add( TEST_CASE( test_bump_switches    ) );
  tests.add( TEST_CASE( test_drive_motors     ) );
  tests.add( TEST_CASE( test_infrared_sensor  ) );
}

//------------------------------------------------------------------------
// Loop
//------------------------------------------------------------------------

void loop()
{
  tests.run();
}

