//========================================================================
// CurieTest
//========================================================================
// This is a simple unit test library for testing code on the Arduino.

#ifndef CURIE_TEST_H
#define CURIE_TEST_H

#include <Arduino.h>

//------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------

extern int g_num_tests;
extern int g_num_passed;

//------------------------------------------------------------------------
// CurieTest
//------------------------------------------------------------------------

class CurieTest {

 public:

  // Constructor

  CurieTest( const String& name );

  // Setup the tests

  void begin();
  void auto_begin();

  // Add a new test case

  void add( const __FlashStringHelper* name, void (*func_ptr)() );

  // Run the test cases

  void run();

  // Run the test cases automatically with no serial monitor

  void auto_run();

 private:

  String                     m_test_suite_name;
  uint8_t                    m_num_test_cases;
  const __FlashStringHelper* m_test_case_names[16];

  typedef void (*TestFuncPtr)();
  TestFuncPtr                m_test_case_func_ptrs[16];

};

//------------------------------------------------------------------------
// TOSTR
//------------------------------------------------------------------------
// Macro to convert the parameter into a string literal.

#define TEST_STRINGIFY( param_ ) \
  #param_

#define TEST_TOSTR( param_ ) \
  TEST_STRINGIFY( param_ )

//------------------------------------------------------------------------
// TEST_CASE
//------------------------------------------------------------------------

#define TEST_CASE( name_ ) \
  F(#name_),name_

//------------------------------------------------------------------------
// TEST_CHECK
//------------------------------------------------------------------------

extern void test_check( bool result, const __FlashStringHelper* msg );

#define TEST_CHECK( expression_ ) \
  test_check( (expression_), F(TEST_TOSTR(__LINE__) " : " #expression_) );

//------------------------------------------------------------------------
// TEST_PASS
//------------------------------------------------------------------------

#define TEST_PASS( msg_ ) \
  test_check( 1, F(TEST_TOSTR(__LINE__) " : " msg_) );

//------------------------------------------------------------------------
// TEST_PROMPT_YN
//------------------------------------------------------------------------

extern void test_prompt_yn( int line_num, const __FlashStringHelper* msg );

#define TEST_PROMPT_YN( msg_ ) \
  test_prompt_yn( __LINE__,  F(msg_) );

//------------------------------------------------------------------------
// TEST_PROMPT_WAIT
//------------------------------------------------------------------------

extern void test_prompt_wait( const __FlashStringHelper* msg );

#define TEST_PROMPT_WAIT( msg_ ) \
  test_prompt_wait( F(msg_) );

#endif /* CURIE_TEST_H */

