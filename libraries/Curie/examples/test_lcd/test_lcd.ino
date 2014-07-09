//========================================================================
// CURIE_LCD Unit Tests
//========================================================================

#include <Curie.h>
#include <Wire.h>
#include <SPI.h>

//------------------------------------------------------------------------
// test_basic
//------------------------------------------------------------------------

void test_basic()
{
  curie_lcd.clear();
  curie_lcd.print("test basic");
  TEST_PROMPT_YN("Does CURIE_LCD say \"test basic\"?");
}

//------------------------------------------------------------------------
// test_cursor
//------------------------------------------------------------------------

void test_cursor()
{
  curie_lcd.clear();
  TEST_PROMPT_WAIT("'o' will move across CURIE_LCD");
  for ( int i = 0; i < 2; i++ ) {
    for ( int j = 0; j < 16; j++ ) {
      curie_lcd.setCursor(j,i);
      curie_lcd.write('*');
      delay(200);
    }
  }
  TEST_PROMPT_YN("Did 'o' move across CURIE_LCD?");
  curie_lcd.clear();
}

//------------------------------------------------------------------------
// Setup
//------------------------------------------------------------------------

CurieTest tests("test_curie_lcd");

void setup()
{
  curie_lcd.begin();

  tests.begin();
  tests.add( TEST_CASE( test_basic  ) );
  tests.add( TEST_CASE( test_cursor ) );
}

//------------------------------------------------------------------------
// Loop
//------------------------------------------------------------------------

void loop()
{
  tests.run();
}

