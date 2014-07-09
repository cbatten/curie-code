//========================================================================
// CurieLCD
//========================================================================
// Shamelessly adapted from adafruit

#include "CurieLCD.h"

#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

// Global instance

CurieLCD curie_lcd;

//------------------------------------------------------------------------
// CurieLCD Constructor
//------------------------------------------------------------------------
// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set:
//    DL = 1; 8-bit interface data
//    N = 0; 1-line display
//    F = 0; 5x8 dot character font
// 3. Display on/off control:
//    D = 0; Display off
//    C = 0; Cursor off
//    B = 0; Blinking off
// 4. Entry mode set:
//    I/D = 1; Increment by 1
//    S = 0; No shift
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// CurieLCD constructor is called).

CurieLCD::CurieLCD()
{
  // Hard code the LCD to use i2c address of zero
  m_i2cAddr = 0;

  m_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

  // the I/O expander pinout

  m_rs_pin = 1;
  m_rw_pin = 255;
  m_enable_pin = 2;
  m_data_pins[0] = 3;  // really d4
  m_data_pins[1] = 4;  // really d5
  m_data_pins[2] = 5;  // really d6
  m_data_pins[3] = 6;  // really d7
}

//------------------------------------------------------------------------
// CurieLCD::begin
//------------------------------------------------------------------------

void CurieLCD::begin()
{

  uint8_t lines   = 2;
  uint8_t dotsize = LCD_5x8DOTS;

  m_i2c.begin( m_i2cAddr );

  m_i2c.pinMode( 7, OUTPUT );    // backlight
  m_i2c.digitalWrite( 7, HIGH ); // backlight

  for ( uint8_t i = 0; i < 4; i++ )
    _pinMode( m_data_pins[i], OUTPUT );

  m_i2c.pinMode( m_rs_pin, OUTPUT );
  m_i2c.pinMode( m_enable_pin, OUTPUT );

  m_displayfunction |= LCD_2LINE;
  m_numlines = lines;

  // for some 1 line displays you can select a 10 pixel high font
  if ( (dotsize != 0) && (lines == 1) )
    m_displayfunction |= LCD_5x10DOTS;

  // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
  // according to datasheet, we need at least 40ms after power rises
  // above 2.7V before sending commands. Arduino can turn on way befer
  // 4.5V so we'll wait 50

  delayMicroseconds(50000);

  // Now we pull both RS and R/W low to begin commands

  _digitalWrite( m_rs_pin,     LOW );
  _digitalWrite( m_enable_pin, LOW );
  if ( m_rw_pin != 255 )
    _digitalWrite( m_rw_pin, LOW );

  // put the LCD into 4 bit or 8 bit mode

  if ( !(m_displayfunction & LCD_8BITMODE) ) {

    // this is according to the hitachi HD44780 datasheet
    // figure 24, pg 46

    // we start in 8bit mode, try to set 4 bit mode

    write4bits(0x03);
    delayMicroseconds(4500); // wait min 4.1ms

    // second try

    write4bits(0x03);
    delayMicroseconds(4500); // wait min 4.1ms

    // third go!

    write4bits(0x03);
    delayMicroseconds(150);

    // finally, set to 8-bit interface

    write4bits(0x02);
  }

  else {

    // this is according to the hitachi HD44780 datasheet
    // page 45 figure 23

    // Send function set command sequence

    command( LCD_FUNCTIONSET | m_displayfunction );
    delayMicroseconds(4500);  // wait more than 4.1ms

    // second try

    command( LCD_FUNCTIONSET | m_displayfunction );
    delayMicroseconds(150);

    // third go

    command( LCD_FUNCTIONSET | m_displayfunction );
  }

  // finally, set # lines, font size, etc.

  command( LCD_FUNCTIONSET | m_displayfunction );

  // turn the display on with no cursor or blinking default

  m_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  display();

  // clear it off

  clear();

  // Initialize to default text direction (for romance languages)

  m_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

  // set the entry mode

  command( LCD_ENTRYMODESET | m_displaymode );
}

//------------------------------------------------------------------------
// CurieLCD::clear
//------------------------------------------------------------------------

void CurieLCD::clear()
{
  // clear display, set cursor pos to zero
  command(LCD_CLEARDISPLAY);

  // this command takes a long time!
  delayMicroseconds(2000);
}

//------------------------------------------------------------------------
// CurieLCD::home
//------------------------------------------------------------------------

void CurieLCD::home()
{
  // set cursor pos to zero
  command(LCD_RETURNHOME);

  // this command takes a long time!
  delayMicroseconds(2000);
}

//------------------------------------------------------------------------
// CurieLCD::setCursor
//------------------------------------------------------------------------

void CurieLCD::setCursor( uint8_t col, uint8_t row )
{
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if ( row > m_numlines ) {
    // we count rows starting w/ zero
    row = m_numlines-1;
  }

  command( LCD_SETDDRAMADDR | (col + row_offsets[row]) );
}

//------------------------------------------------------------------------
// CurieLCD::display
//------------------------------------------------------------------------
// Turn the display on/off (quickly)

void CurieLCD::display()
{
  m_displaycontrol |= LCD_DISPLAYON;
  command( LCD_DISPLAYCONTROL | m_displaycontrol );
}

void CurieLCD::noDisplay()
{
  m_displaycontrol &= ~LCD_DISPLAYON;
  command( LCD_DISPLAYCONTROL | m_displaycontrol );
}

//------------------------------------------------------------------------
// CurieLCD::cursor
//------------------------------------------------------------------------
// Turns the underline cursor on/off

void CurieLCD::cursor()
{
  m_displaycontrol |= LCD_CURSORON;
  command( LCD_DISPLAYCONTROL | m_displaycontrol );
}

void CurieLCD::noCursor()
{
  m_displaycontrol &= ~LCD_CURSORON;
  command( LCD_DISPLAYCONTROL | m_displaycontrol );
}

//------------------------------------------------------------------------
// CurieLCD::blink
//------------------------------------------------------------------------
// Turn on and off the blinking cursor

void CurieLCD::blink()
{
  m_displaycontrol |= LCD_BLINKON;
  command( LCD_DISPLAYCONTROL | m_displaycontrol );
}

void CurieLCD::noBlink()
{
  m_displaycontrol &= ~LCD_BLINKON;
  command( LCD_DISPLAYCONTROL | m_displaycontrol );
}

//------------------------------------------------------------------------
// CurieLCD::scroll
//------------------------------------------------------------------------
// These commands scroll the display without changing the RAM

void CurieLCD::scrollDisplayLeft()
{
  command( LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT );
}

void CurieLCD::scrollDisplayRight()
{
  command( LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT );
}

// This is for text that flows Left to Right

void CurieLCD::leftToRight()
{
  m_displaymode |= LCD_ENTRYLEFT;
  command( LCD_ENTRYMODESET | m_displaymode );
}

// This is for text that flows Right to Left

void CurieLCD::rightToLeft()
{
  m_displaymode &= ~LCD_ENTRYLEFT;
  command( LCD_ENTRYMODESET | m_displaymode );
}

// This will 'right justify' text from the cursor

void CurieLCD::autoscroll()
{
  m_displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command( LCD_ENTRYMODESET | m_displaymode );
}

// This will 'left justify' text from the cursor

void CurieLCD::noAutoscroll()
{
  m_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command( LCD_ENTRYMODESET | m_displaymode );
}

//------------------------------------------------------------------------
// CurieLCD::createChar
//------------------------------------------------------------------------
// Allows us to fill the first 8 CGRAM locations with custom characters

void CurieLCD::createChar( uint8_t location, uint8_t charmap[] )
{
  // we only have 8 locations 0-7
  location &= 0x7;
  command( LCD_SETCGRAMADDR | (location << 3) );
  for ( int i = 0; i < 8; i++ )
    write(charmap[i]);
}

//------------------------------------------------------------------------
// CurieLCD Mid-Level Commands
//------------------------------------------------------------------------

inline void CurieLCD::command( uint8_t value )
{
  send( value, LOW );
}

inline size_t CurieLCD::write( uint8_t value )
{
  send( value, HIGH );
  return 1;
}

//------------------------------------------------------------------------
// CurieLCD::_digitalWrite
//------------------------------------------------------------------------
// little wrapper for i/o writes

void CurieLCD::_digitalWrite( uint8_t p, uint8_t d )
{
  m_i2c.digitalWrite( p, d );
}

//------------------------------------------------------------------------
// CurieLCD::_pinMode
//------------------------------------------------------------------------
// little wrapper for i/o directions

void CurieLCD::_pinMode( uint8_t p, uint8_t d )
{
  m_i2c.pinMode( p, d );
}

//------------------------------------------------------------------------
// CurieLCD::send
//------------------------------------------------------------------------
// write either command or data, with automatic 4/8-bit selection

void CurieLCD::send( uint8_t value, uint8_t mode )
{
  _digitalWrite( m_rs_pin, mode );

  // if there is a RW pin indicated, set it low to Write

  if ( m_rw_pin != 255 )
    _digitalWrite( m_rw_pin, LOW );

  if ( m_displayfunction & LCD_8BITMODE )
    write8bits(value);
  else {
    write4bits(value>>4);
    write4bits(value);
  }
}

//------------------------------------------------------------------------
// CurieLCD::pulseEnable
//------------------------------------------------------------------------

void CurieLCD::pulseEnable()
{
  // enable pulse must be >450ns

  _digitalWrite( m_enable_pin, LOW );
  delayMicroseconds(1);
  _digitalWrite( m_enable_pin, HIGH );
  delayMicroseconds(1);
  _digitalWrite( m_enable_pin, LOW );

  // commands need > 37us to settle

  delayMicroseconds(100);
}

//------------------------------------------------------------------------
// CurieLCD::write4bits
//------------------------------------------------------------------------

void CurieLCD::write4bits( uint8_t value )
{
  uint8_t out = 0;

  out = m_i2c.readGPIO();

  // speed up for i2c since its sluggish

  for (int i = 0; i < 4; i++) {
    out &= ~_BV(m_data_pins[i]);
    out |= ((value >> i) & 0x1) << m_data_pins[i];
  }

  // make sure enable is low

  out &= ~ _BV(m_enable_pin);
  m_i2c.writeGPIO(out);

  // pulse enable

  delayMicroseconds(1);

  out |= _BV(m_enable_pin);
  m_i2c.writeGPIO(out);

  delayMicroseconds(1);

  out &= ~_BV(m_enable_pin);
  m_i2c.writeGPIO(out);

  delayMicroseconds(100);
}

//------------------------------------------------------------------------
// CurieLCD::write8bits
//------------------------------------------------------------------------

void CurieLCD::write8bits( uint8_t value )
{
  for (int i = 0; i < 8; i++) {
    _pinMode( m_data_pins[i], OUTPUT );
    _digitalWrite( m_data_pins[i], (value >> i) & 0x01 );
  }

  pulseEnable();
}

