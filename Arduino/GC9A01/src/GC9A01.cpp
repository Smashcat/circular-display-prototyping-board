#include "Arduino.h"
#include <SPI.h>
#include "GC9A01.h"

/**
 * Constructor, requires a hardware SPI port using the default pins to allow 80Mhz mode on the ESP32
 */
GC9A01::GC9A01(SPIClass *s){
	spiRef=s;
	
	pinMode(TFT_MOSI,OUTPUT);
	pinMode(TFT_SCLK,OUTPUT);
	pinMode(TFT_CS,OUTPUT);
	pinMode(TFT_DC,OUTPUT);
	pinMode(TFT_RST,OUTPUT);
	pinMode(TFT_MISO,INPUT);

	CS_HIGH;
	delay(5);
	RST_LOW;
	delay(10);
	RST_HIGH;
	delay(120);  

	displayBuffer=new RenderBuffer(240,240);
	setTargetBuffer(displayBuffer);
//	buffer=new uint16_t[DISPLAY_WIDTH*DISPLAY_HEIGHT];
//	setRenderTarget(buffer,DISPLAY_WIDTH,DISPLAY_HEIGHT);
//	renderBuffer=buffer;

	CS_LOW;

	spiRef->beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));


	//************* Start Initial Sequence **********// 
	writeCmd(SET_INTER_REGISTER_ENABLE2);

	writeCmd(0xEB);
	writeCmdData(0x14); 

	writeCmd(SET_INTER_REGISTER_ENABLE1);       
	writeCmd(SET_INTER_REGISTER_ENABLE2); 

	// No idea what these do! No documentation for these registers in datasheet!
	writeCmd(0xEB);  
	writeCmdData(0x14); 

	writeCmd(0x84);      
	writeCmdData(0x40); 

	writeCmd(0x85);      
	writeCmdData(0xFF); 

	writeCmd(0x86);      
	writeCmdData(0xFF); 

	writeCmd(0x87);      
	writeCmdData(0xFF);

	writeCmd(0x88);      
	writeCmdData(0x0A);

	writeCmd(0x89);      
	writeCmdData(0x21); 

	writeCmd(0x8A);      
	writeCmdData(0x00); 

	writeCmd(0x8B);      
	writeCmdData(0x80); 

	writeCmd(0x8C);      
	writeCmdData(0x01); 

	writeCmd(0x8D);      
	writeCmdData(0x01); 

	writeCmd(0x8E);      
	writeCmdData(0xFF); 

	writeCmd(0x8F);      
	writeCmdData(0xFF); 


	writeCmd(SET_DISPLAY_FUNCTION_CONTROL);
	writeCmdData(0x00);       
	writeCmdData(0x00); 

	writeCmd(SET_MEMORY_ACCESS_CONTROL);

	if(USE_HORIZONTAL==0)
	writeCmdData(0x18);
	else if(USE_HORIZONTAL==1)
	writeCmdData(0x28);
	else if(USE_HORIZONTAL==2)
	writeCmdData(0x48);
	else 
	writeCmdData(0x88);

	writeCmd(SET_PIXEL_FORMAT);      
	writeCmdData(0x05); // 16 bits per pixel

	writeCmd(0x90);       // Undocumented
	writeCmdData(0x08);
	writeCmdData(0x08);
	writeCmdData(0x08);
	writeCmdData(0x08); 

	writeCmd(0xBD);       // Undocumented
	writeCmdData(0x06);

	writeCmd(0xBC);       // Undocumented
	writeCmdData(0x00); 

	writeCmd(0xFF);       // Undocumented
	writeCmdData(0x60);
	writeCmdData(0x01);
	writeCmdData(0x04);

	writeCmd(SET_POWER_CONTROL_2);      
	writeCmdData(0x13);
	writeCmd(SET_POWER_CONTROL_3);      
	writeCmdData(0x13);

	writeCmd(SET_POWER_CONTROL_4);      
	writeCmdData(0x22);

	writeCmd(0xBE);       // Undocumented
	writeCmdData(0x11); 

	writeCmd(0xE1);       // Undocumented
	writeCmdData(0x10);
	writeCmdData(0x0E);

	writeCmd(0xDF);       // Undocumented
	writeCmdData(0x21);
	writeCmdData(0x0c);
	writeCmdData(0x02);

	writeCmd(SET_GAMMA_1);   
	writeCmdData(0x45);
	writeCmdData(0x09);
	writeCmdData(0x08);
	writeCmdData(0x08);
	writeCmdData(0x26);
	writeCmdData(0x2A);

	writeCmd(SET_GAMMA_2);    
	writeCmdData(0x43);
	writeCmdData(0x70);
	writeCmdData(0x72);
	writeCmdData(0x36);
	writeCmdData(0x37);  
	writeCmdData(0x6F);


	writeCmd(SET_GAMMA_3);   
	writeCmdData(0x45);
	writeCmdData(0x09);
	writeCmdData(0x08);
	writeCmdData(0x08);
	writeCmdData(0x26);
	writeCmdData(0x2A);

	writeCmd(SET_GAMMA_4);   
	writeCmdData(0x43);
	writeCmdData(0x70);
	writeCmdData(0x72);
	writeCmdData(0x36);
	writeCmdData(0x37); 
	writeCmdData(0x6F);

	writeCmd(0xED);       // Undocumented
	writeCmdData(0x1B); 
	writeCmdData(0x0B); 

	writeCmd(0xAE);       // Undocumented
	writeCmdData(0x77);

	writeCmd(0xCD);       // Undocumented
	writeCmdData(0x63);   


	writeCmd(0x70);       // Undocumented
	writeCmdData(0x07);
	writeCmdData(0x07);
	writeCmdData(0x04);
	writeCmdData(0x0E); 
	writeCmdData(0x0F); 
	writeCmdData(0x09);
	writeCmdData(0x07);
	writeCmdData(0x08);
	writeCmdData(0x03);

	writeCmd(SET_FRAME_RATE);      
	writeCmdData(0x34);

	writeCmd(0x62);      // Undocumented
	writeCmdData(0x18);
	writeCmdData(0x0D);
	writeCmdData(0x71);
	writeCmdData(0xED);
	writeCmdData(0x70); 
	writeCmdData(0x70);
	writeCmdData(0x18);
	writeCmdData(0x0F);
	writeCmdData(0x71);
	writeCmdData(SET_INTER_REGISTER_ENABLE2);
	writeCmdData(0x70); 
	writeCmdData(0x70);

	writeCmd(0x63);      // Undocumented
	writeCmdData(0x18);
	writeCmdData(0x11);
	writeCmdData(0x71);
	writeCmdData(0xF1);
	writeCmdData(0x70); 
	writeCmdData(0x70);
	writeCmdData(0x18);
	writeCmdData(0x13);
	writeCmdData(0x71);
	writeCmdData(0xF3);
	writeCmdData(0x70); 
	writeCmdData(0x70);

	writeCmd(0x64);      // Undocumented
	writeCmdData(0x28);
	writeCmdData(0x29);
	writeCmdData(0xF1);	// 241 - could this be the width/height+1 ?
	writeCmdData(0x01);	// maybe first pixel?
	writeCmdData(0xF1); // As above
	writeCmdData(0x00); // As above
	writeCmdData(0x07);

	writeCmd(0x66);      // Undocumented
	writeCmdData(0x3C);
	writeCmdData(0x00);
	writeCmdData(0xCD);
	writeCmdData(0x67);
	writeCmdData(0x45);
	writeCmdData(0x45);
	writeCmdData(0x10);
	writeCmdData(0x00);
	writeCmdData(0x00);
	writeCmdData(0x00);

	writeCmd(0x67);      // Undocumented
	writeCmdData(0x00);
	writeCmdData(0x3C);
	writeCmdData(0x00);
	writeCmdData(0x00);
	writeCmdData(0x00);
	writeCmdData(0x01);
	writeCmdData(0x54);
	writeCmdData(0x10);
	writeCmdData(0x32);
	writeCmdData(0x98);

	writeCmd(0x74);      // Undocumented
	writeCmdData(0x10); 
	writeCmdData(0x85); 
	writeCmdData(0x80);
	writeCmdData(0x00); 
	writeCmdData(0x00); 
	writeCmdData(0x4E);
	writeCmdData(0x00);         

	writeCmd(0x98);      // Undocumented
	writeCmdData(0x3e);
	writeCmdData(0x07);

	writeCmd(SET_TEARING_EFFECT_LINE_ON);  
	writeCmd(0x21);

	writeCmd(SET_EXIT_SLEEP_MODE);

	spiRef->endTransaction();

	delay(120);
	spiRef->beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
	writeCmd(SET_DISPLAY_ON);
	spiRef->endTransaction();
	delay(20);
}

GC9A01::~GC9A01(){
	delete[] displayBuffer;
}

void GC9A01::setDisplayEnable(boolean tf){
	if(tf){
		CS_LOW;
	}else{
		CS_HIGH;
	}
}

void GC9A01::setDisplayWindow(uint16_t x,uint16_t y,uint16_t x2,uint16_t y2){
	spiRef->beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));

	writeCmd(SET_HORIZONTAL_WRITE_WINDOW);   
	writeCmdData(x>>8);
	writeCmdData(x);
	writeCmdData(x2>>8);
	writeCmdData(x2);

	writeCmd(SET_VERTICAL_WRITE_WINDOW);   
	writeCmdData(y>>8);
	writeCmdData(y);
	writeCmdData(y2>>8);
	writeCmdData(y2);
	writeCmd(SET_START_MEMORY_WRITE);
}

void GC9A01::drawFrame(void){
	setDisplayWindow(0,0,DISPLAY_WIDTH-1,DISPLAY_HEIGHT-1);  // This also starts the transaction
	DC_HIGH;
	spiRef->writePixels(renderBuffer,DISPLAY_WIDTH*DISPLAY_HEIGHT*2);
	spiRef->endTransaction();
	++frames;
}

void GC9A01::clearBuffer(uint16_t bColor){
	// Abuse pointers a bit here to write 4 bytes at a time instead of 2 :-)
	uint32_t bColor32=(bColor<<16)+bColor;
	uint32_t *p=(uint32_t *)renderBuffer;
	uint16_t cnt=renderBufferSize/2;
	for(uint16_t n=0;n<cnt;n++){
		*p++=bColor32;
	}
}

void GC9A01::testEffect1(void){	// Just creates a random background effect every frame - used to initially test the library
	uint8_t r,g,b;
	uint16_t ix=0;
	
	setRenderType(RENDER_TYPE_OVERWRITE);

	for(uint16_t y=0;y<renderBufferHeight;y++){
		for(uint16_t x=0;x<renderBufferWidth;x++){
			r=((y+frames)%renderBufferHeight)/8;
			g=((x+(frames*3))%renderBufferWidth)/8;
			b=31-( ((ix+(frames<<9))%renderBufferSize)/1920);
			renderBuffer[ix++]=(r<<11)+(g<<5)+b;
		}
	}

	setRenderType(RENDER_TYPE_ADDITIVE);
	
	static int16_t amp=80;
	static int16_t ampDir=-1;

	static uint16_t colors[9]={
	  
	  (15<<11)+(0<<5)+0,
	  (0<<11)+(30<<5)+0,
	  (0<<11)+(0<<5)+15,

	  (20<<11)+(40<<5)+20,
	  (15<<11)+(30<<5)+15,
	  (10<<11)+(20<<5)+10,

	  (15<<11)+(30<<5)+0,
	  (0<<11)+(30<<5)+15,
	  (15<<11)+(0<<5)+15,
	};
	ix=0;
	int16_t fX,fY;
	float fA;
	const uint16_t numRects=15;
	amp+=ampDir;
	if(amp==-100)
		ampDir=1;
	else if(amp==100)
		ampDir=-1;
	for(ix=0;ix<numRects;ix++){
		fA=( ( (PI*2)/numRects)*ix ) + (frames*0.12);
		fX=(sin(fA)*amp);
		fY=(cos(fA)*amp);
		fillRect(90+fX,90+fY,150+fX,150+fY,colors[ix%9]);
	}
}

void GC9A01::fillRect(int16_t x, int16_t y,int16_t x2, int16_t y2,uint16_t color){
	if(x2<0 || x>renderBufferWidth-1 || y2<0 || y>renderBufferHeight-1)
		return;	// Offscreen, so nothing to draw!
	if((renderType==RENDER_TYPE_MERGE) && mergeAmt==0)
		return;
	if(x>x2 || y>y2)
		return;	// inverted, nothing to draw!
	if(x<0){
		x=0;
	}
	if(x2>renderBufferWidth-1){
		x2=renderBufferWidth-1;
	}
	if(y<0){
		y=0;
	}
	if(y2>renderBufferHeight-1){
		y2=renderBufferHeight-1;
	}
	uint16_t width=(x2-x)+1;
	uint16_t pix,nPix;
	uint16_t c;
	int16_t  cR,cG,cB;
	uint16_t rt=renderType;
	// Split input color up into constituents to make processing faster in draw loops
	if(
		(rt==RENDER_TYPE_ADDITIVE)
		||
		(rt==RENDER_TYPE_SUBTRACTIVE)
		||
		(rt==RENDER_TYPE_MERGE)
	){
		cB=(color&31);
		cG=((color>>5)&63);
		cR=((color>>11)&31);
	}
	
	// Optimisation - if we're using an opacity of 16, then it's just an overwrite!
	if((rt==RENDER_TYPE_MERGE)&&(mergeAmt==16)){
		rt=RENDER_TYPE_OVERWRITE;
	}
	
	uint16_t *p=renderBuffer+(y*renderBufferWidth)+x;
	uint16_t widthAdder=renderBufferWidth-width;
	for(uint16_t row=y;row<=y2;row++){
		uint16_t w=width;
		switch(rt){
			
			case RENDER_TYPE_OVERWRITE:
			while(w-->0){
				*p++=color;
			}
			break;

			case RENDER_TYPE_XOR:
			while(w-->0){
				*p++^=color;
			}
			break;
			
			case RENDER_TYPE_ADDITIVE:
			while(w-->0){
				uint16_t pix=*p;
				
				c=(pix&31)+cB;
				if(c>31)
					c=31;
				nPix=c;
				
				c=((pix>>5)&63)+cG;
				if(c>63)
					c=63;
				nPix+=(c<<5);
				
				c=((pix>>11)&31)+cR;
				if(c>31)
					c=31;
				nPix+=(c<<11);
				
				*p++=nPix;
			}
			break;
			
			case RENDER_TYPE_SUBTRACTIVE:
			while(w-->0){
				uint16_t pix=*p;
				
				c=(pix&31)-cB;
				if(c<0)
					c=0;
				nPix=c;
				
				c=((pix>>5)&63)-cG;
				if(c<0)
					c=0;
				nPix+=(c<<5);
				
				c=((pix>>11)&31)-cR;
				if(c<0)
					c=0;
				nPix+=(c<<11);
				
				*p++=nPix;
			}
			break;
			
			case RENDER_TYPE_MERGE:
			{
				uint16_t antiMergeAmt=16-mergeAmt;
				while(w-->0){
					uint16_t pix=*p;
					uint16_t nB=( (cB*mergeAmt) + ((pix&31)       * antiMergeAmt) ) >>4;
					uint16_t nG=( (cG*mergeAmt) + (((pix>>5)& 63) * antiMergeAmt) ) >>4;
					uint16_t nR=( (cR*mergeAmt) + (((pix>>11)&31) * antiMergeAmt) ) >>4;
					*p++=(nR<<11)+(nG<<5)+nB;
				}
			}
			break;
		}
		p+=widthAdder;

	}
}

// Modified version of triangle fill from Adafruit library
void GC9A01::fillTriangle(
	int16_t x0, int16_t y0, 
	int16_t x1, int16_t y1, 
	int16_t x2, int16_t y2, 
	uint16_t color
){

    int16_t a, b, y, last;

	if(
		(renderType==RENDER_TYPE_ADDITIVE)
		||
		(renderType==RENDER_TYPE_SUBTRACTIVE)
		||
		(renderType==RENDER_TYPE_MERGE)
		||
		(renderType==RENDER_TYPE_ANTIALIAS)
	){

		_cB=color&31;
		_cG=(color>>5)&63;
		_cR=(color>>11)&31;

	}
	_color=color;
	
    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
        swapInt16(y0, y1); 
		swapInt16(x0, x1);
    }
    if (y1 > y2) {
        swapInt16(y2, y1); 
		swapInt16(x2, x1);
    }
    if (y0 > y1) {
        swapInt16(y0, y1); 
		swapInt16(x0, x1);
    }

    if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if(x1 < a)
			a = x1;
        else if(x1 > b)
			b = x1;
        if(x2 < a)
			a = x2;
        else if(x2 > b)
			b = x2;
        drawHLine(a, y0, b-a+1);
        return;
    }

    int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1;
	
    int32_t
    sa   = 0,
    sb   = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if(y1 == y2)
		last = y1;   // Include y1 scanline
    else
		last = y1-1; // Skip it

    for(y=y0; y<=last; y++) {
        a   = x0 + sa / dy01;
        b   = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
        a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b)
			swapInt16(a,b);
        drawHLine(a, y, b-a+1);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for(; y<=y2; y++) {
        a   = x1 + sa / dy12;
        b   = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
        a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b)
			swapInt16(a,b);
        drawHLine(a, y, b-a+1);
    }
}

void GC9A01::drawHLine(int16_t x,int16_t y, uint16_t w,uint16_t color){
	if(
		(renderType==RENDER_TYPE_ADDITIVE)
		||
		(renderType==RENDER_TYPE_SUBTRACTIVE)
		||
		(renderType==RENDER_TYPE_ANTIALIAS)
	){
		_cB=color&31;
		_cG=(color>>5)&63;
		_cR=(color>>11)&31;
	}
	_color=color;
	drawHLine(x,y,w);
}

void GC9A01::drawHLine(int16_t x,int16_t y, uint16_t w){
	if(x>=renderBufferWidth || x+w<0 || y>=renderBufferHeight || y<0)
		return;
	if(x<0){
		w+=x;
		x=0;
	}else if(x+w>=renderBufferWidth){
		w=renderBufferWidth-x;
	}
	uint16_t *p=renderBuffer+x+(y*renderBufferWidth);
	switch(renderType){
			
			case RENDER_TYPE_OVERWRITE:
			while(w--){
				*p++=_color;
			}
			break;

			case RENDER_TYPE_XOR:
			while(w--){
				*p++^=_color;
			}
			break;
			
			case RENDER_TYPE_ADDITIVE:
			{
			uint16_t c,nPix;
			while(w-->0){
				uint16_t pix=*p;
				nPix=_cB+(pix&31);
				if(nPix>31)
					nPix=31;

				c=_cG+((pix>>5)&63);
				if(c>63)
					c=63;
				nPix+=(c<<5);
				
				c=_cR+((pix>>11)&31);
				if(c>31)
					c=31;
				nPix+=(c<<11);
		
				*p++=nPix;
			}
			}
			break;
			
			case RENDER_TYPE_SUBTRACTIVE:
			while(w-->0){
				uint16_t pix=*p;
				int16_t nPix=(pix&31)-_cB;
				if(nPix<0)
					nPix=0;
				
				int16_t c=((pix>>5)&63)-_cG;
				if(c<0)
					c=0;
				nPix+=(c<<5);
				
				c=((pix>>11)&31)-_cR;
				if(c<0)
					c=0;
				nPix+=(c<<11);
				
				*p++=nPix;
			}
			break;
			
			case RENDER_TYPE_MERGE:
			{
				uint16_t antiMergeAmt=16-mergeAmt;
				while(w-->0){
					uint16_t pix=*p;
					uint16_t nB=( (_cB*mergeAmt) + ((pix&31)       * antiMergeAmt) ) >>4;
					uint16_t nG=( (_cG*mergeAmt) + (((pix>>5)& 63) * antiMergeAmt) ) >>4;
					uint16_t nR=( (_cR*mergeAmt) + (((pix>>11)&31) * antiMergeAmt) ) >>4;
					*p++=(nR<<11)+(nG<<5)+nB;
				}
			}
			break;

			case RENDER_TYPE_ANTIALIAS:
			{
				while(w--){
					*p++=_color;
				}
			}
			break;
			
		}
}

void GC9A01::drawSpriteImg(const SpriteImg *s,int16_t x,int16_t y,uint16_t frame, uint16_t renderDirection){
	if((y+s->frameHeight)<0 || (y>=renderBufferHeight) || (x+s->frameWidth)<0 || (x>=renderBufferWidth))
		return;	// Offscreen
	uint16_t srcLineWidth=(s->frameWidth/8) * s->bpp;
	uint16_t offset=( srcLineWidth * s->frameHeight * frame);
	uint16_t fw=s->frameWidth;
	uint16_t fh=s->frameHeight;
	uint16_t w=s->frameWidth;
	uint16_t h=s->frameHeight;
	int16_t transIndex=s->transparentColorIndex;
	
	// If sprite is partially off top/bottom, then work out new src and dest offsets to save wasted cycles
	if(y<0){	// crop top
		offset+=(-y*srcLineWidth);
		h+=y;
		y=0;
	}else if(y+fh>renderBufferHeight-1){	// crop bottom
		h-=((y+fh)-(renderBufferHeight-1));
	}
	const uint8_t *src=s->data+offset;
	uint16_t *dest=renderBuffer+x+(y*renderBufferWidth);
	uint8_t srcPos=0;
	uint16_t color,c;
	uint8_t mask=bppMask[s->bpp];
	uint16_t destAdd=(renderBufferWidth-s->frameWidth);
	uint16_t bpp=s->bpp;
	const uint16_t *currentPalette=s->palette+(renderPaletteIndex*s->paletteSize);
	
	if(x>=0 && x+w<renderBufferWidth){	// Fully onscreen, so don't bother clipping left/right
//	Serial.printf("Width: %i, Height: %i\n",w,h);
		if(renderType==RENDER_TYPE_MERGE){
			uint16_t antiMergeAmt=16-mergeAmt;
			uint16_t nB,nG,nR,pC,bC;
			for(uint16_t y=0;y<h;y++){
				for(uint16_t x=0;x<w;x++){
					c=(*src>>srcPos)&mask;
					srcPos+=bpp;
					if(srcPos==8){
						++src;
						srcPos=0;
					}
					if(c!=transIndex){
						bC=*dest;
						pC=currentPalette[c];
						nB=( ((pC&31)*mergeAmt) + ((bC&31)    		   * antiMergeAmt) ) >>4;
						nG=( (((pC>>5)&63)*mergeAmt) + (((bC>>5)& 63)  * antiMergeAmt) ) >>4;
						nR=( (((pC>>11)&31)*mergeAmt) + (((bC>>11)&31) * antiMergeAmt) ) >>4;
						*dest=(nR<<11)+(nG<<5)+nB;
					}
					++dest;
				}
				dest+=destAdd;
			}

		}else{
			for(uint16_t y=0;y<h;y++){
				for(uint16_t x=0;x<w;x++){
					c=(*src>>srcPos)&mask;
					srcPos+=bpp;
					if(srcPos==8){
						++src;
						srcPos=0;
					}
					if(c!=transIndex)
						*dest=currentPalette[c];
					++dest;
				}
				dest+=destAdd;
			}
		}
	}else{
		uint16_t clipLeft=(x<0?-x:0);
		uint16_t clipRight=w-(x+w>=renderBufferWidth?((x+w)-(renderBufferWidth-1)):0);
		
		if(renderType==RENDER_TYPE_MERGE){
			uint16_t antiMergeAmt=16-mergeAmt;
			uint16_t nB,nG,nR,pC,bC;
			for(uint16_t y=0;y<h;y++){
				for(uint16_t x=0;x<w;x++){
					c=(*src>>srcPos)&mask;
					srcPos+=bpp;
					if(srcPos==8){
						++src;
						srcPos=0;
					}
					if(x>=clipLeft && x<=clipRight && c!=transIndex){
						bC=*dest;
						pC=currentPalette[c];
						nB=( ((pC&31)*mergeAmt) + ((bC&31)    		   * antiMergeAmt) ) >>4;
						nG=( (((pC>>5)&63)*mergeAmt) + (((bC>>5)& 63)  * antiMergeAmt) ) >>4;
						nR=( (((pC>>11)&31)*mergeAmt) + (((bC>>11)&31) * antiMergeAmt) ) >>4;
						*dest=(nR<<11)+(nG<<5)+nB;
					}
					++dest;
				}
				dest+=destAdd;
			}

		}else{
			
			for(uint16_t y=0;y<h;y++){
				for(uint16_t x=0;x<w;x++){
					c=(*src>>srcPos)&mask;
					srcPos+=bpp;
					if(srcPos==8){
						++src;
						srcPos=0;
					}
					if(x>=clipLeft && x<=clipRight && c!=transIndex)
						*dest=currentPalette[c];
					++dest;
				}
				dest+=destAdd;
			}
		}
	}
}

void GC9A01::drawSpriteImg(const SpriteImg *s,int16_t x,int16_t y,uint16_t w,uint16_t h,uint16_t frame, uint16_t renderDirection){
	if((y+h)<0 || (y>=renderBufferHeight) || (x+w)<0 || (x>=renderBufferWidth) || (w==0) || (h==0))
		return;	// Offscreen
	uint16_t srcLineWidth=(s->frameWidth/8) * s->bpp;
	uint16_t offset=( srcLineWidth * s->frameHeight * frame);	// Start of src data in Sprite 
	uint16_t fw=s->frameWidth;
	uint16_t fh=s->frameHeight;
	int16_t transIndex=s->transparentColorIndex;
	uint16_t oh=h;
	int16_t dh=oh;

	// If sprite is partially off top/bottom, then work out new src and dest offsets to save wasted cycles
	if(y<0){	// crop top
		for(int16_t n=0;n<-y;n++){
			dh-=fh;
			while(dh<=0){
				dh+=oh;
				offset+=srcLineWidth;
			}
		}
		h+=y;
		y=0;
	}
	
	if(y+h>renderBufferHeight-1){	// crop bottom
		h=renderBufferHeight-y;
	}
	
	const uint8_t *src=s->data+offset;
	uint16_t *dest=renderBuffer+x+(y*renderBufferWidth);
	uint8_t srcPos=0;
	uint16_t color,c;
	uint8_t mask=bppMask[s->bpp];
	int16_t destAdd=(renderBufferWidth-w);
	uint16_t bpp=s->bpp;
	const uint16_t *currentPalette=s->palette+(renderPaletteIndex*s->paletteSize);

	uint16_t clipLeft=(x<0?-x:0);
	uint16_t clipRight=w-(x+w>=renderBufferWidth?((x+w)-(renderBufferWidth-1)):0);
	if(renderType==RENDER_TYPE_MERGE){
		uint16_t antiMergeAmt=16-mergeAmt;
		uint16_t nB,nG,nR,pC,bC;
		for(uint16_t y=0;y<h;y++){
			const uint8_t *srcL = src;
			int16_t dw=w;
			for(uint16_t x=0;x<w;x++){
				c=(*srcL>>srcPos)&mask;
				dw-=fw;
				while(dw<=0){
					srcPos+=bpp;
					if(srcPos==8){
						++srcL;
						srcPos=0;
					}
					dw+=w;
				}
				if(x>=clipLeft && x<=clipRight && c!=transIndex){
					bC=*dest;
					pC=currentPalette[c];
					nB=( ((pC&31)*mergeAmt) + ((bC&31)    		   * antiMergeAmt) ) >>4;
					nG=( (((pC>>5)&63)*mergeAmt) + (((bC>>5)& 63)  * antiMergeAmt) ) >>4;
					nR=( (((pC>>11)&31)*mergeAmt) + (((bC>>11)&31) * antiMergeAmt) ) >>4;
					*dest=(nR<<11)+(nG<<5)+nB;
				}
				++dest;
			}
			dest+=destAdd;
			dh-=fh;
			while(dh<=0){
				dh+=oh;
				src+=srcLineWidth;
			}
		}

	}else{
		for(uint16_t y=0;y<h;y++){
			const uint8_t *srcL = src;
			int16_t dw=w;
			for(uint16_t x=0;x<w;x++){
				c=(*srcL>>srcPos)&mask;
				dw-=fw;
				while(dw<=0){
					srcPos+=bpp;
					if(srcPos==8){
						++srcL;
						srcPos=0;
					}
					dw+=w;
				}
				if(x>=clipLeft && x<=clipRight && c!=transIndex)
					*dest=currentPalette[c];
				++dest;
			}
			dest+=destAdd;
			dh-=fh;
			while(dh<=0){
				dh+=oh;
				src+=srcLineWidth;
			}
		}
		
	}
}

void GC9A01::setRenderType(uint16_t r){
	renderType=r;
}

void GC9A01::setMergeAmount(uint16_t a){
	if(a<0)
		a=1;
	else if(a>16)
		a=16;
	mergeAmt=a;
}

void GC9A01::setRenderPaletteIndex(uint16_t i){
	renderPaletteIndex=i;
}

void GC9A01::setTargetBuffer(RenderBuffer *r){
	renderBuffer=r->buffer;
	renderBufferWidth=r->width;
	renderBufferHeight=r->height;
	renderBufferSize=renderBufferWidth*renderBufferHeight;
}

void GC9A01::copyBufferToBuffer(
	RenderBuffer dest,RenderBuffer src,int16_t destX,int16_t destY,uint16_t transparentColorIndex
){
	
}

void GC9A01::scaleBufferToBuffer(
	RenderBuffer dest,RenderBuffer src,int16_t destX,int16_t destW,int16_t destH,uint16_t transparentColorIndex
){
	
}

void GC9A01::copyBufferToDisplay(RenderBuffer src,int16_t destX,int16_t destY){
	
}

// The following are private methods
void GC9A01::writeCmdData(unsigned char c){
	DC_HIGH;
	spiRef->transfer(c); 
}

void GC9A01::writeCmd(unsigned char c){
	DC_LOW;
	spiRef->transfer(c); 
}


// Renderbuffer inner class functions

// Constructor
GC9A01::RenderBuffer::RenderBuffer(uint16_t w,uint16_t h){
	width=w;
	height=h;
	buffer=new uint16_t[w*h];
}

// Destructor
GC9A01::RenderBuffer::~RenderBuffer(){
	delete[] buffer;
}

// Resize buffer
void GC9A01::RenderBuffer::resize(uint16_t w,uint16_t h){
	delete[] buffer;
	width=w;
	height=h;
	buffer=new uint16_t[w*h];
}

