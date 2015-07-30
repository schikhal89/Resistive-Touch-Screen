#ifndef	_GLCDPROTOTYPES_H
#define	_GLCDPROTOTYPES_H

//Controller ATmega 64/128
	/* 	PORTC  = GLCD Data pin port  DB0 - DB7	*/
#define 		GLCDPORT			PORTC//PORTF
#define 		GLCDDDR			DDRC//	DDRF
#define 		GLCDPIN			PINC//	PINF

/* 	PORTG  = GLCD Control pin port i.e D/I , R/W, E	*/
#define 		CNTRLNPORT			PORTG			
#define		CNTRLNDDR			DDRG
#define 		CNTRLNPIN		    PING

#define 		CNTRLNPORT1		PORTD			
#define		CNTRLNDDR1			DDRD
#define 		CNTRLNPIN1		    PIND

#define 		DI							PG0
#define 		RW							PG1
#define 		EN							PG2
#define 		CS1PIN					PD4//PG3
#define 		CS2PIN					PD5//PG4
 #define 	RST							??? //(May be we can connect it to GND temporarily)	
 
#define 		LINE1						0x80
#define 		LINE2						0x90
#define 		LINE3						0x88	
#define 		LINE4						0x98

#define 		SETDI						(CNTRLNPORT |= 	(1<<DI))
#define 		CLRDI						(CNTRLNPORT &= ~(1<<DI))
#define 		SETRW					(CNTRLNPORT |=		(1<<RW))
#define 		CLRRW					(CNTRLNPORT &= ~(1<<RW))
#define 		SETEN						(CNTRLNPORT |= 	(1<<EN))
#define 		CLREN					(CNTRLNPORT &= ~(1<<EN))
#define 		SETCS1					(CNTRLNPORT1 |= 	 (1<<CS1PIN))
#define 		CLRCS1					(CNTRLNPORT1 &= ~(1<<CS1PIN))
#define 		SETCS2					(CNTRLNPORT1 |=	 (1<<CS2PIN))
#define 		CLRCS2					(CNTRLNPORT1 &= ~(1<<CS2PIN))
#define		CHIPSEL1				SETCS1;CLRCS2  
#define		CHIPSEL2				SETCS2;CLRCS1	

typedef  unsigned char uchar;
typedef  unsigned int uint;
volatile uchar GlcdBuffer[8][128];// To store Entire Image
static volatile uchar Xfontpos,Yfontpos;//To keep track of where the last character was printed

void GLcdinit(void);
void ToggleENPin(void);
void GlcdCmd(uchar cmd);
void GlcdData(uchar data);
void GlcdSelHalfLcd(uchar whichhalf);
void GlcdGotoXY(uchar x,uchar y);
void GlcdClear(void);
void GlcdClearBuffer(void);
void GlcdUpdateBufferOnLCD(void);
void GlcdInvertDisplay(void);
void GlcdAllClear(void);
void GlcdModifyPixel(uchar x,uchar y,uchar changetype);
void GlcdHLine(uchar x,uchar y,uchar len,uchar changetype);
void GlcdLineCross(uchar x1,uchar y1,uchar x2,uchar y2,uchar changetype);
void GlcdLine(uint x1,uint y1,uint x2,uint y2,uchar changetype);
void GlcdRectangle(uchar x,uchar y,uchar len,uchar brdth,uchar changetype);
int    GlcdGetPtDist(signed char x1,signed char y1,signed char x2,signed char y2);
void GlcdCircle(uchar centerX,uchar centerY,uchar rad,uchar changetype);
void GlcdTriangle(uchar x1,uchar y1,uchar x2,uchar y2,uchar x3,uchar y3,uchar changetype);
void GlcdSin(uchar center,uchar amplitude,uchar  timeperiod,uchar noofcycles,uchar phase,uchar changetype);
void GlcdCos(uchar center,uchar amplitude,uchar  timeperiod,uchar noofcycles,uchar phase,uchar changetype);
void GlcdRose(uchar centerX,uchar centerY,uchar noofleaves,uchar length,uchar shrink,uchar changetype);
void GlcdDeltoid(uchar centerX,uchar centerY,uchar radinnrcircle,uchar changetype);
void GlcdCardoid(uchar centerX,uchar centerY,uchar radoutercircle,uchar changetype);
void GlcdAstroid(uchar centerX,uchar centerY,uchar radoutercircle,uchar changetype);
void GlcdHypoCycloid(uchar centerX,uchar centerY,float radinnrcircle,float noofcorners,uchar changetype);
void GlcdArchemedeanSpiral(uchar centerX,uchar centerY,uchar start,uchar space,float noofcycles,uchar changetype);
void GlcdInvoluteofCircle(uchar centerX,uchar centerY,float radius,float noofcycles,uchar changetype);
void GlcdLogarithmicSpiral(uchar centerX,uchar centerY,float start,float space,float noofcycles,uchar changetype);
void GlcdQuadrilateral(uchar x1,uchar y1,uchar x2,uchar y2,uchar x3,uchar y3,uchar x4,uchar y4,uchar changetype);
void GlcdWRPage(uchar whichhalf,uchar pgno,uchar pagedata[64],uchar changetype);
void GlcdShowImage1(uchar * img,uchar changetype);
void GlcdShowImage2(uchar * img,uchar changetype);
uint GlcdMapData(uint start,uint end,uint val,uint glcdstart,uint glcdend);

void GlcdShowNumber_P(uint num,uchar x,uchar y,uchar FontWidth,uchar FontHeight,uchar changetype);
void GlcdShowString_P(uchar *str,uchar x,uchar y,uchar FontWidth,uchar FontHeight,uchar changetype);


#endif