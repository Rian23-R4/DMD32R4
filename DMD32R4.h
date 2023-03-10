#ifndef DMD_H_
#define DMD_H_

// Arduino toolchain header, version dependent
#include "Arduino.h"

// SPI library must be included for the SPI scanning/connection method to the DMD
#include <SPI.h>

// ######################################################################################################################
// ######################################################################################################################
// #warning CHANGE THESE TO SEMI-ADJUSTABLE PIN DEFS!
// ESP32 pins used for the display connection (Using VSPI)
// ############################################ Custome
// #define PIN_DMD_nOE 25   // D22 `active low Output Enable, setting this low lights all the LEDs in the selected rows. Can pwm it at very high frequency for brightness control.
// #define PIN_DMD_A 5      // D19
// #define PIN_DMD_B 18     // D21
// #define PIN_DMD_CLK 22   // D18_SCK  is SPI Clock if SPI is used
// #define PIN_DMD_SCLK 26  // D02
// #define PIN_DMD_R_DATA 2 // D23_MOSI is SPI Master Out if SPI is used

#define MOSI PIN_DMD_R_DATA
#define MISO 4
#define SCK PIN_DMD_CLK
// Define this chip select pin that the Ethernet W5100 IC or other SPI device uses
// if it is in use during a DMD scan request then scanDisplayBySPI() will exit without conflict! (and skip that scan)
#define PIN_OTHER_SPI_nCS SS
// ######################################################################################################################
// ######################################################################################################################

// DMD I/O pin macros
#define LIGHT_DMD_ROW_01_05_09_13(PIN_DMD_B, PIN_DMD_A) \
  {                                                     \
    digitalWrite(PIN_DMD_B, LOW);                       \
    digitalWrite(PIN_DMD_A, LOW);                       \
  }
#define LIGHT_DMD_ROW_02_06_10_14(PIN_DMD_B, PIN_DMD_A) \
  {                                                     \
    digitalWrite(PIN_DMD_B, LOW);                       \
    digitalWrite(PIN_DMD_A, HIGH);                      \
  }
#define LIGHT_DMD_ROW_03_07_11_15(PIN_DMD_B, PIN_DMD_A) \
  {                                                     \
    digitalWrite(PIN_DMD_B, HIGH);                      \
    digitalWrite(PIN_DMD_A, LOW);                       \
  }
#define LIGHT_DMD_ROW_04_08_12_16(PIN_DMD_B, PIN_DMD_A) \
  {                                                     \
    digitalWrite(PIN_DMD_B, HIGH);                      \
    digitalWrite(PIN_DMD_A, HIGH);                      \
  }
#define LATCH_DMD_SHIFT_REG_TO_OUTPUT(PIN_DMD_SCLK) \
  {                                                 \
    digitalWrite(PIN_DMD_SCLK, HIGH);               \
    digitalWrite(PIN_DMD_SCLK, LOW);                \
  }
#define OE_DMD_ROWS_OFF(PIN_DMD_nOE) \
  {                                  \
    digitalWrite(PIN_DMD_nOE, LOW);  \
  }
#define OE_DMD_ROWS_ON(PIN_DMD_nOE)  \
  {                                  \
    digitalWrite(PIN_DMD_nOE, HIGH); \
  }

// Pixel/graphics writing modes (bGraphicsMode)
#define GRAPHICS_NORMAL 0
#define GRAPHICS_INVERSE 1
#define GRAPHICS_TOGGLE 2
#define GRAPHICS_OR 3
#define GRAPHICS_NOR 4

// drawTestPattern Patterns
#define PATTERN_ALT_0 0
#define PATTERN_ALT_1 1
#define PATTERN_STRIPE_0 2
#define PATTERN_STRIPE_1 3

// display screen (and subscreen) sizing
#define DMD_PIXELS_ACROSS 32 // pixels across x axis (base 2 size expected)
#define DMD_PIXELS_DOWN 16   // pixels down y axis
#define DMD_BITSPERPIXEL 1   // 1 bit per pixel, use more bits to allow for pwm screen brightness control
#define DMD_RAM_SIZE_BYTES ((DMD_PIXELS_ACROSS * DMD_BITSPERPIXEL / 8) * DMD_PIXELS_DOWN)
// (32x * 1 / 8) = 4 bytes, * 16y = 64 bytes per screen here.
// lookup table for DMD::writePixel to make the pixel indexing routine faster
static byte bPixelLookupTable[8] =
    {
        0x80, // 0, bit 7
        0x40, // 1, bit 6
        0x20, // 2. bit 5
        0x10, // 3, bit 4
        0x08, // 4, bit 3
        0x04, // 5, bit 2
        0x02, // 6, bit 1
        0x01  // 7, bit 0
};

// Font Indices
#define FONT_LENGTH 0
#define FONT_FIXED_WIDTH 2
#define FONT_HEIGHT 3
#define FONT_FIRST_CHAR 4
#define FONT_CHAR_COUNT 5
#define FONT_WIDTH_TABLE 6

typedef uint8_t (*FontCallback)(const uint8_t *);

const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

// The main class of DMD library functions
class DMD
{
public:
  // Instantiate the DMD
  DMD(byte PIN_nOE = 25, byte PIN_A = 5, byte PIN_B = 18, byte PIN_CLK = 22, byte PIN_SCLK = 26, byte PIN_R_DATA = 02);
  // virtual ~DMD();

  // Set or clear a pixel at the x and y location (0,0 is the top left corner)
  void writePixel(unsigned int bX, unsigned int bY, byte bGraphicsMode, byte bPixel);

  // Draw a string
  void drawString(int bX, int bY, const char *bChars, byte length, byte bGraphicsMode);

  // Select a text font
  void selectFont(const uint8_t *font);

  // Draw a single character
  int drawChar(const int bX, const int bY, const unsigned char letter, byte bGraphicsMode);

  // Find the width of a character
  int charWidth(const unsigned char letter);

  // Draw a scrolling string
  void drawMarquee(const char *bChars, byte length, int left, int top);

  // Move the maquee accross by amount
  boolean stepMarquee(int amountX, int amountY);

  // Move the marquee left 1 place - special case of stepMarquee()
  //  which only scrolls a limited portion of the display
  boolean stepSplitMarquee3(int topRow, int bottomRow, int StartColumn);

  // Clear the screen in DMD RAM
  void clearScreen(byte bNormal);

  // Draw or clear a line from x1,y1 to x2,y2
  void drawLine(int x1, int y1, int x2, int y2, byte bGraphicsMode);

  // Draw or clear a circle of radius r at x,y centre
  void drawCircle(int xCenter, int yCenter, int radius, byte bGraphicsMode);

  // Draw or clear a box(rectangle) with a single pixel border
  void drawBox(int x1, int y1, int x2, int y2, byte bGraphicsMode);

  // Draw or clear a filled box(rectangle) with a single pixel border
  void drawFilledBox(int x1, int y1, int x2, int y2, byte bGraphicsMode);

  // Draw the selected test pattern
  void drawTestPattern(byte bPattern);

  // Scan the dot matrix LED panel display, from the RAM mirror out to the display hardware.
  // Call 4 times to scan the whole display which is made up of 4 interleaved rows within the 16 total rows.
  // Insert the calls to this function into the main loop for the highest call rate, or from a timer interrupt
  void scanDisplayBySPI(byte PIN_nOE = 25, byte PIN_A = 5, byte PIN_B = 18, byte PIN_CLK = 22, byte PIN_SCLK = 26, byte PIN_R_DATA = 02, bool stageSPI = true);

  void setBrightness(uint16_t crh);

  void changeDMD(byte panelsWide, byte panelsHigh);

private:
  void drawCircleSub(int cx, int cy, int x, int y, byte bGraphicsMode);

  // Mirror of DMD pixels in RAM, ready to be clocked out by the main loop or high speed timer calls
  byte *bDMDScreenRAM;

  uint16_t cr;

  // Marquee values
  char marqueeText[256];
  byte marqueeLength;
  int marqueeWidth;
  int marqueeHeight;
  int marqueeOffsetX;
  int marqueeOffsetY;

  // Pointer to current font
  const uint8_t *Font;

  // Display information
  byte DisplaysWide;
  byte DisplaysHigh;
  byte DisplaysTotal;
  int row1, row2, row3;

  // scanning pointer into bDMDScreenRAM, setup init @ 48 for the first valid scan
  volatile byte bDMDByte;

  // uninitalised pointer to SPI object
  SPIClass *vspi = NULL;
  static const int spiClk = 4000000; // 4 MHz SPI clock
};

#endif /* DMD_H_ */
