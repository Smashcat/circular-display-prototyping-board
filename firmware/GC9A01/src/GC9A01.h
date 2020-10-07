/*
  GC9A01.h - Library for accessing 240x240 circular display
  Author: Scott Porter (scott@smashcat.org)
  Date: 26/09/2020
  using the GC9A01 display controller chip (with SPI configured as the default control method on COG)
  Released into the public domain.
*/
#ifndef GC9A01_h
#define GC9A01_h

#include "Arduino.h"
#include <SPI.h>

// Render types

#define RENDER_TYPE_OVERWRITE			1
#define RENDER_TYPE_XOR					2
#define RENDER_TYPE_ADDITIVE			4
#define RENDER_TYPE_SUBTRACTIVE			8
#define RENDER_TYPE_MERGE				16
#define RENDER_TYPE_ANTIALIAS			32

// Commands used to set up registers in the GC9A01 controller
#define SET_HORIZONTAL_WRITE_WINDOW   	0x2A
#define SET_VERTICAL_WRITE_WINDOW     	0x2B
#define SET_START_MEMORY_WRITE        	0x2C
#define SET_INTER_REGISTER_ENABLE1    	0xFE
#define SET_INTER_REGISTER_ENABLE2    	0xEF
#define SET_MEMORY_ACCESS_CONTROL     	0x36
#define SET_DISPLAY_ON                	0x29
#define SET_DISPLAY_FUNCTION_CONTROL  	0xB6
#define SET_PIXEL_FORMAT              	0x3A
#define SET_POWER_CONTROL_2           	0xC3
#define SET_POWER_CONTROL_3           	0xC4
#define SET_POWER_CONTROL_4           	0xC9

#define SET_GAMMA_1                   	0xF0
#define SET_GAMMA_2                   	0xF1
#define SET_GAMMA_3                   	0xF2
#define SET_GAMMA_4                   	0xF3

#define SET_FRAME_RATE                	0xE8
#define SET_TEARING_EFFECT_LINE_ON    	0x35
#define SET_EXIT_SLEEP_MODE           	0x11


// IO pin definitions
#define TFT_MISO 						19
#define TFT_MOSI 						23
#define TFT_SCLK 						18
#define TFT_CS    						5  // Chip select control pin
#define TFT_DC    						2  // Data Command control pin
#define TFT_RST   						13

// Display settings
#define DISPLAY_WIDTH					240			// Default width/height for the circular display 
#define DISPLAY_HEIGHT					240
#define USE_HORIZONTAL 					2  			//Set the display direction 0,1,2,3  four directions
#define SPI_SPEED                     	80000000	// Maximum speed possible for SPI bus on ESP32

// Convenience defines
#define CS_LOW  						digitalWrite(TFT_CS,LOW);
#define CS_HIGH  						digitalWrite(TFT_CS,HIGH);

#define DC_LOW  						digitalWrite(TFT_DC,LOW);
#define DC_HIGH  						digitalWrite(TFT_DC,HIGH);

#define RST_LOW  						digitalWrite(TFT_RST,LOW);
#define RST_HIGH  						digitalWrite(TFT_RST,HIGH);

#ifndef _swap_int16_t
#define swapInt16(a, b){int16_t t=a;a=b;b=t;}
#endif

typedef struct SpriteImg {
  uint8_t bpp;
  uint8_t paletteSize;
  uint8_t frameWidth;
  uint8_t frameHeight;
  uint16_t totalHeight;
  int16_t transparentColorIndex;
  const uint16_t *palette;
  const uint8_t *data;
} SpriteImg;

class GC9A01
{
  public:

	// Inner class, used to handle buffer data for rendering
  	class RenderBuffer
	{
		public:
		uint16_t *buffer;
		uint16_t width;
		uint16_t height;
		RenderBuffer(uint16_t w,uint16_t h);
		~RenderBuffer();
		void resize(uint16_t w,uint16_t h);
	};

	RenderBuffer *displayBuffer;				// Yeah, may seem weird, but I like having easy access to the display buffer...
	uint16_t *renderBuffer=NULL;				// For performance, this can be set to point to a target RenderBuffer's buffer array for subsequent drawing operations
	uint16_t renderBufferWidth=240;				// Width of current render target
	uint16_t renderBufferHeight=240;			// Height of current render target
	uint16_t renderBufferSize=(renderBufferWidth*renderBufferHeight);


	uint32_t frames=0;							// Just a frame counter - increments each time a frame is drawn
    GC9A01(SPIClass *s);						// Constructor - note we allocate 240x240x2 bytes of memory for internal display buffer
	~GC9A01();									// Destructor - we free the buffer here to release memory back to the sketch
    void setDisplayEnable(boolean tf);			// Sets the CS pin low to enable, or high to disable the SPI port on the display
	void setDisplayWindow(uint16_t x,uint16_t y,uint16_t x2,uint16_t y2);	// Specifies the section of the display we will push data into
	void drawFrame(void);						// Blits the display buffer into the display - this sets up the display window etc, so no need to do anything else
	void clearBuffer(uint16_t bColor);			// Empties the display buffer - sets all pixels to colour (black)
	void testEffect1(void);						// Just test code to create animation on display - can be useful to quickly check display is working ok
	void fillRect(int16_t x, int16_t y,int16_t x2, int16_t y2,uint16_t color);	// Draw a filled rectangle using specified drawing mode
	void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);	// Adapted from Adafruit library
	void drawHLine(int16_t x,int16_t y, uint16_t w,uint16_t color);
	void drawHLine(int16_t x,int16_t y, uint16_t w);
	void drawSpriteImg(const SpriteImg *s,int16_t x,int16_t y,uint16_t frame);
	void setRenderType(uint16_t r);				// Sets the renderType for subsequent drawing operations
	void setMergeAmount(uint16_t a);			// Set the strength of the source pixel being merged into the destination buffer when RENDER_TYPE_MERGE is used. Range is 0 to 16 Lower = weaker
	void setRenderPaletteIndex(uint16_t i);		// Change the render palette for a sprite
	void setTargetBuffer(RenderBuffer *r);
	
	// TODO
	void copyBufferToBuffer(RenderBuffer dest,RenderBuffer src,int16_t destX,int16_t destY,uint16_t transparentColorIndex=9999);
	void scaleBufferToBuffer(RenderBuffer dest,RenderBuffer src,int16_t destX,int16_t destW,int16_t destH,uint16_t transparentColorIndex=9999);
	void copyBufferToDisplay(RenderBuffer src,int16_t destX,int16_t destY);
	
	inline uint16_t getColor16(uint8_t r,uint8_t g,uint8_t b){	// Returns the 16bit color value from the RGB values (0-255) ready to copy to the display render buffer
		return ((r>>3)<<11)+((g>>2)<<5)+(b>>3);
	}
			
  private:

	uint16_t renderPaletteIndex=0;				// Sprite images can have multiple palettes. The default is 0. If you have not added extra palettes, do not change this!
	uint16_t renderType=RENDER_TYPE_OVERWRITE;	// How pixels are written to the renderBuffer
	uint16_t mergeAmt=8;						// Opacity to use when drawing using the "merge" render type, range is 0-16

	const uint8_t bppMask[9]={0,1,3,0,15,0,0,0,255};
	uint16_t _cR,_cG,_cB,_color;
	
	SPIClass *spiRef; // Private reference to hardware SPI device

	void writeCmdData(unsigned char c);
	void writeCmd(unsigned char c);
};

#endif