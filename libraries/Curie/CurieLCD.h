//========================================================================
// CurieLCD
//========================================================================
// Shamelessly adapted from adafruit

#ifndef CURIE_LCD_H
#define CURIE_LCD_H

#include <inttypes.h>
#include <Print.h>
#include <utility/Adafruit_MCP23008.h>

//------------------------------------------------------------------------
// commands
//------------------------------------------------------------------------

#define LCD_CLEARDISPLAY   0x01
#define LCD_RETURNHOME     0x02
#define LCD_ENTRYMODESET   0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT    0x10
#define LCD_FUNCTIONSET    0x20
#define LCD_SETCGRAMADDR   0x40
#define LCD_SETDDRAMADDR   0x80

//------------------------------------------------------------------------
// flags for display entry mode
//------------------------------------------------------------------------

#define LCD_ENTRYRIGHT          0x00
#define LCD_ENTRYLEFT           0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

//------------------------------------------------------------------------
// flags for display on/off control
//------------------------------------------------------------------------

#define LCD_DISPLAYON  0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON   0x02
#define LCD_CURSOROFF  0x00
#define LCD_BLINKON    0x01
#define LCD_BLINKOFF   0x00

//------------------------------------------------------------------------
// flags for display/cursor shift
//------------------------------------------------------------------------

#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE  0x00
#define LCD_MOVERIGHT   0x04
#define LCD_MOVELEFT    0x00

//------------------------------------------------------------------------
// flags for function set
//------------------------------------------------------------------------

#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE    0x08
#define LCD_1LINE    0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS  0x00

class CurieLCD : public Print
{
 public:

  CurieLCD();

  void begin();

  // High-Level Commands

  void clear();
  void home();
  void setCursor( uint8_t col, uint8_t row );

  void display();
  void noDisplay();

  void cursor();
  void noCursor();

  void blink();
  void noBlink();

  void scrollDisplayLeft();
  void scrollDisplayRight();
  void leftToRight();
  void rightToLeft();
  void autoscroll();
  void noAutoscroll();

  void createChar( uint8_t location, uint8_t charmap[] );

  // Mid-Level Commands

 public:

  void command( uint8_t value );
  virtual size_t write( uint8_t value );

  // Low-Level Commands

 private:

  void _digitalWrite( uint8_t p, uint8_t d );
  void _pinMode( uint8_t p, uint8_t d );

  void send( uint8_t value, uint8_t mode );
  void pulseEnable();
  void write4bits( uint8_t value );
  void write8bits( uint8_t value);

  // Data members

  uint8_t m_rs_pin;     // LOW: command       HIGH: character
  uint8_t m_rw_pin;     // LOW: write to LCD  HIGH: read from LCD
  uint8_t m_enable_pin; // activated by a HIGH pulse.
  uint8_t m_data_pins[8];

  uint8_t m_displayfunction;
  uint8_t m_displaycontrol;
  uint8_t m_displaymode;

  uint8_t m_numlines;

  uint8_t m_i2cAddr;
  Adafruit_MCP23008 m_i2c;
};

// Global instance

extern CurieLCD curie_lcd;

#endif /* CURIE_LCD_H */

