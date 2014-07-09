//========================================================================
// CURIE Lab 2 Template
//========================================================================

//------------------------------------------------------------------------
// Global constants for pin assignments
//------------------------------------------------------------------------

// Digital Pins

int pin_button            = 2;
int pin_led1              = 4;
int pin_led2              = 5;
int pin_bump_right        = 6;
int pin_bump_left         = 7;

int pin_motor_left_speed  = 3;
int pin_motor_right_speed = 11;
int pin_motor_left_dir    = 12;
int pin_motor_right_dir   = 13;

// Analog Pins

int pin_potentiometer     = A5;
int pin_grayscale_sensor  = A4;
int pin_infrared_sensor   = A3;

//------------------------------------------------------------------------
// Setup: runs once after reset
//------------------------------------------------------------------------

void setup()
{
  pinMode( pin_button,            INPUT  );
  pinMode( pin_led1,              OUTPUT );
  pinMode( pin_led2,              OUTPUT );
  pinMode( pin_bump_right,        INPUT  );
  pinMode( pin_bump_left,         INPUT  );

  pinMode( pin_motor_left_dir,    OUTPUT );
  pinMode( pin_motor_right_dir,   OUTPUT );
  pinMode( pin_motor_left_speed,  OUTPUT );
  pinMode( pin_motor_right_speed, OUTPUT );

  // Makes the input button have a pull-up resistor

  digitalWrite( pin_button, HIGH );

  // Motors are initially turning forward but with zero speed

  digitalWrite( pin_motor_left_dir,  LOW );
  digitalWrite( pin_motor_right_dir, LOW );

  analogWrite( pin_motor_left_speed,  0 );
  analogWrite( pin_motor_right_speed, 0 );

  // Initialize serial communications at 9600 bps

  Serial.begin(9600);
}

//------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------

// ... your global variables here ...

//------------------------------------------------------------------------
// Loop: runs forever
//------------------------------------------------------------------------

void loop() {

  // ... your code goes here ...

}
