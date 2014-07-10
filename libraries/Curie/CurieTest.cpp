//========================================================================
// CurieTest
//========================================================================

#include <CurieTest.h>
#include <CurieLCD.h>
// #include <MemoryFree.h>

//------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------

int g_num_tests  = 0;
int g_num_passed = 0;

//------------------------------------------------------------------------
// CurieTest Constructor
//------------------------------------------------------------------------

CurieTest::CurieTest( const String& name )
{
  m_test_suite_name = name;
  m_num_test_cases  = 0;
}

//------------------------------------------------------------------------
// CurieTest::begin
//------------------------------------------------------------------------

void CurieTest::begin()
{
  delay(2000);
  Serial.begin(9600);
}

//------------------------------------------------------------------------
// CurieTest::begin
//------------------------------------------------------------------------

void CurieTest::auto_begin()
{
  delay(2000);
  curie_lcd.begin();
  Serial.begin(9600);
}

//----------------------------------------------------------------------
// CurieTest::add_test_case
//----------------------------------------------------------------------

void CurieTest::add( const __FlashStringHelper* name, void (*func_ptr)() )
{
  if ( m_num_test_cases == 16 ) {
    Serial.println(F("ERROR: Added too many test cases!"));
    while (1);
  }

  m_test_case_names[m_num_test_cases]     = name;
  m_test_case_func_ptrs[m_num_test_cases] = func_ptr;
  m_num_test_cases++;
}

//----------------------------------------------------------------------
// CurieTest::run
//----------------------------------------------------------------------

void CurieTest::run()
{
  // Reset global state

  g_num_tests  = 0;
  g_num_passed = 0;

  // Print out header for test suite

  Serial.flush();
  Serial.println("");
  for ( int i = 0; i < 70; i++ )
    Serial.print("-");
  Serial.println("");
  Serial.print(F(" Test Suite: "));
  Serial.println(m_test_suite_name);
  for ( int i = 0; i < 70; i++ )
    Serial.print("-");
  Serial.println("\n");

  // Display available test cases

  Serial.println(F("  - [0] all test cases"));
  for ( int i = 0; i < m_num_test_cases; i++ ) {
    Serial.print(F("  - ["));
    Serial.print(i+1);
    Serial.print(F("] "));
    Serial.println(m_test_case_names[i]);
  }

  // Get the test case number to run

  Serial.print("\n Choose test case: ");
  while ( !Serial.available() ) {
    delay(100);
  }
  int test_case_num = Serial.parseInt();
  Serial.println(test_case_num);

  // Iterate over the test cases to run the desired test case

  for ( int i = 0; i < m_num_test_cases; i++ ) {
    if ( (test_case_num == 0) || (test_case_num == i+1) ) {

      // Print out header for test case

      Serial.print(F("\n  - Test Case: "));
      Serial.println(m_test_case_names[i]);

      // Run the test case

      // Serial.print(F("free mem: "));
      // Serial.println(freeMemory());

      m_test_case_func_ptrs[i]();

      // Serial.print(F("free mem: "));
      // Serial.println(freeMemory());

    }
  }

  // Final report

  // Serial.println(F("\n Testing is finished"));
  // Serial.print(F("  - Total number of tests  : "));
  // Serial.println(g_num_tests);
  // Serial.print(F("  - Number of passed tests : "));
  // Serial.println(g_num_passed);

  if ( g_num_passed == g_num_tests )
    Serial.println(F("\n *** PASSED ***"));
  else
    Serial.println(F("\n *** FAILED ***"));

  // Wait to see if we should run test suite again

  Serial.println(F("\n Press any key and enter to rerun test suite"));
  while ( !Serial.available() )
    delay(100);

  Serial.read();
}

//----------------------------------------------------------------------
// CurieTest::auto_run
//----------------------------------------------------------------------

void CurieTest::auto_run()
{
  // Reset global state

  g_num_tests  = 0;
  g_num_passed = 0;

  // Iterate over the test cases to run all test cases

  for ( int i = 0; i < m_num_test_cases; i++ ) {

    // Small delay to make it easier to watch on the LCD

    delay(1000);

    // Print out header for test case

    curie_lcd.clear();
    curie_lcd.print(m_test_case_names[i]);

    // Run the test case

    m_test_case_func_ptrs[i]();
  }

  // Final report

  curie_lcd.clear();
  curie_lcd.print(F("passed: "));
  curie_lcd.print(g_num_passed);
  curie_lcd.print(F("/"));
  curie_lcd.print(g_num_tests);

  curie_lcd.setCursor(0,1);
  if ( g_num_passed == g_num_tests )
    curie_lcd.print(F("*** PASSED ***"));
  else
    curie_lcd.print(F("*** FAILED ***"));

  // All done

  while (1);
}

//------------------------------------------------------------------------
// test_check
//------------------------------------------------------------------------

void test_check( bool result, const __FlashStringHelper* msg )
{
  g_num_tests++;
  if ( result ) {
    g_num_passed++;
    Serial.print(F("     [ passed ] Line "));
  }
  else
    Serial.print(F("     [ FAILED ] Line "));
  Serial.println(msg);
}

//------------------------------------------------------------------------
// test_prompt_yn
//------------------------------------------------------------------------

void test_prompt_yn( int line_num, const __FlashStringHelper* msg )
{
  Serial.print(F("     [ prompt ] "));
  Serial.print(msg);
  Serial.println(F(" [y/n] "));
  while ( !Serial.available() ) {
    delay(100);
  }
  byte answer = Serial.read();
  g_num_tests++;
  if ( (answer == 'Y') || (answer == 'y') ) {
    g_num_passed++;
    Serial.print(F("     [ passed ] Line "));
    Serial.print(line_num);
    Serial.println(F(" : Prompt successful"));
  }
  else {
    Serial.print(F("     [ FAILED ] Line "));
    Serial.print(line_num);
    Serial.println(F(" : Prompt unsuccessful"));
  }
}

//------------------------------------------------------------------------
// test_prompt_wait
//------------------------------------------------------------------------

void test_prompt_wait( const __FlashStringHelper* msg )
{
  Serial.print(F("     [ prompt ] "));
  Serial.print(msg);
  Serial.println(F(", press space/enter when ready"));
  while ( !Serial.available() ) {
    delay(100);
  }
  Serial.read();
}

