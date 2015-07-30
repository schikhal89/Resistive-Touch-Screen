#include <avr/io.h>
#include <avr/pgmspace.h>
#include "glcd.h"
#include  "img.h"
#include "uart.h"
#define DELAY 1000
#define 	MYDELAY		400
#define ADC_CHNL0  0
#define ADC_CHNL1  1
#define ADC_CHNL2  2
#define ADC_CHNL3  3

int ADCData(uchar chnl);
void initsyst(void);
void initADC();

void main(void)
{
	int x1,y1,x2,y2,x1new,y1new,x2new,y2new;
	char buff[32],xok=0,yok=0;
//	signed x,y;
initsyst();

u1init();
GLcdinit();
//GlcdLine(0,0,127,63,1);
while(1)
{
DDRF = (1<<2)|(1<<0);
PORTF = (1<<0)|(1<<1);// pin2 = vcc pin0 = gnd
y1 =  ADCData(1);//1,3
//_delay_ms(5);

DDRF = (1<<3)|(1<<1);
PORTF = (1<<1)|(1<<2);// pin1 = vcc pin3 = gnd
x1 =  ADCData(2);//0,2

DDRF = (1<<2)|(1<<0);
PORTF = (1<<0)|(1<<1);// pin2 = vcc pin0 = gnd
y2 =  ADCData(1);//1,3

DDRF = (1<<3)|(1<<1);
PORTF = (1<<1)|(1<<2);// pin1 = vcc pin3 = gnd
x2 =  ADCData(2);//0,2

 //if(  ( x1 > 25) && (x1<160)) // && ( x2 > 25) && (x2<160) )
{
	x1new = GlcdMapData(0,1023,x1,0,127);
	x2new = GlcdMapData(0,1023,x2,0,127);
	xok = 1;
	
//	if(  ( y1 > 32) && (y1<135)) // && ( y2 >32) && (y2<135) )
	{
		y1new = GlcdMapData(0,1023,y1,0,63);
		y2new = GlcdMapData(0,1023,y2,0,63);
		yok = 1;
	}
}	
	sprintf(buff,"\n\rx1 = %04d y1 = %04d x1glcd = %03d y1glcd = %03d",x1,y1,x1new,y1new);
	u1str(buff);
		sprintf(buff,"\n\rx2 = %04d y2 = %04d x2glcd = %03d y2glcd = %03d",x2,y2,x2new,y2new);
		u1str(buff);

if(	(xok == 1) &&	(yok ==  1) )
{
	//	 GlcdLine( x1new, y1new, x2new, y2new,1);
		GlcdModifyPixel(x1new,y1new,1);
		xok =  0;	
		yok =  0;
}		
/*
if( (x1 < 126) && (x2 < 126) )
 GlcdLine( x1new, y1new, x2new, y2new,1);
*/ 
}	

//	uint x;
//    uchar str[] = "";
//	uint row,col,angle;
//	DDRA = (1<<PA3);
//	GLcdinit();
/*
	for(row = 0 ;row <8 ;row++)
		for(col = 0 ;col <128 ;col++)
            GlcdBuffer[row][col] =0xE0;
			
	for(row = 0 ;row <8 ;row++)
		for(col = 112 ;col <128 ;col++)
			GlcdBuffer[row][col] =0x0;
			
			GlcdUpdateBufferOnLCD();
			
	_delay_ms(2000);
		*/	
	/*
	GlcdHLine(12,15,65,1);
	_delay_ms(1000);
	GlcdInvertDisplay();
	
	GlcdVLine(8,5,40,0);
		_delay_ms(1000);
	GlcdInvertDisplay();
	
	GlcdLineCross(55,5,5,55,1);//only 45 degree lines
	
	GlcdLine(40,1,127,63,1);
	_delay_ms(1000);
	GlcdInvertDisplay();
	
	GlcdQuadrilateral(1,1,100,2,10,50,82,56,0);
	GlcdRectangle(0,0,100,50,0);
	_delay_ms(1000);
	GlcdInvertDisplay();
	GlcdCircle(63,32,25,1);
	GlcdTriangle(70,61,21,5,119,32,1);
	_delay_ms(1000);
	GlcdAllClear();	
	GlcdRectangleFill(4,4,70,40,1);
	_delay_ms(1000);
	GlcdInvertDisplay();
	GlcdRectangleFill(65,10,40,50,2);

	GlcdSin(32,25,40,3,0);
	GlcdCos(32,25,40,3,0);
	GlcdRose(64,32,7,25,5);	
	GlcdDeltoid(64,32,10);
	GlcdCardoid(64,32,10);
	GlcdAstroid(64,32,20);
	GlcdHypoCycloid(64,32,6,5);
	GlcdArchemedeanSpiral(63,32,0,1,3);
	GlcdInvoluteofCircle(63,32,1,5);
    GlcdLogarithmicSpiral(63,32,0.05,0.15,7);
	GlcdInvertDisplay();
    GlcdWRPage(LEFTHALF,1,img1,1);
    GlcdShowImage1(img2,1);

    GlcdShowImage1(think,1);
    _delay_ms(1000);
    //GlcdClear();
	//GlcdInvertDisplay();
    _delay_ms(1000);
	//GlcdInvertDisplay();


    GlcdShowImage1(think,1);  
	_delay_ms(2000);

for(x=0;x<128;x++)
{
	GlcdRotateImg(RIGHT,ROTATE);
	   //_delay_ms(20);
}


for(x=0;x<128;x++)
{
	GlcdRotateImg(LEFT,ROTATE);
	   //_delay_ms(20);
}


for(x=0;x<64;x++)
{
	GlcdRotateImg(UP,ROTATE);
	   //_delay_ms(20);
}


for(x=0;x<64;x++)
{
	GlcdRotateImg(DOWN,ROTATE);
	   //_delay_ms(20);
}


GlcdGotoXY(0,0);
for(x=0;x<16;x++)
    GlcdData(fonts8X8[x]);


GlcdPutImage(img1,0,0,4,16,1);
_delay_ms(2000);
GlcdPutImage(img1,40,0,4,16,0);
_delay_ms(2000);
*/

/*
_delay_ms(1000);
GlcdInvert(0);
_delay_ms(1000);
GlcdInvert(0);


GlcdShowImage2_P( img2,1);
_delay_ms(1000);
GlcdShowImage2_P( jay1,1);
_delay_ms(1000);
GlcdShowImage2_P( jay2,1);
_delay_ms(1000);
GlcdShowImage2_P( scene,1);
_delay_ms(1000);
GlcdShowImage2_P( think,1);
_delay_ms(1000);

GlcdPutImage_P(img1,0,0,2,32,1);

GlcdRotateInbuit(0,63,50);
GlcdRotateInbuit(63,0,50);
*/
//GlcdShowChar('A',0,0,5,8,1);
/*
GlcdShowString((uchar *)" !\"#$%&'()*+'-./01234567",0,0,5,8,1);
GlcdShowString((uchar *)"89:;<=>?@ABCDEFGHIJKLM",0,9,5,8,1);
GlcdShowString((uchar *)"NOPQRSTUVWXYZ[\]^_'abcd",0,17,5,8,1);
GlcdShowString((uchar *)"efghijklmnopqrstuvwxyz",0,25,5,8,1);
GlcdShowString((uchar *)"{|}~",0,33,5,8,1);
*/
/*
for(x = 20; x>10; x--)
	GlcdRotateInbuit(63-x,100);
	*/
/*
_delay_ms(1000);

GlcdReverse(1);
_delay_ms(1000);
GlcdReverse(1);
*/
/*
GlcdCircle(65,32,30,1);

for(angle = 360;angle>0;angle-=6)
{
	x = 65 + radius * cos(TORADIAN(angle));
	y = 32 - radius * sin(TORADIAN(angle));
	 GlcdLine(65,32,x,y,1);
	_delay_ms(950);
	 GlcdLine(65,32,x,y,0);
	 _delay_ms(50);
}
*/
}

void initsyst(void)
{
	initADC();
}

void initADC()
{
//	sei();//Global interrupt enable
	 //Vref=2.56 internal, ADC Left Adjust,channel 0(LDR on board)
	 ADMUX |= ((1<<REFS1) | (1<<REFS0))/*|(1<<ADLAR) */;
	//Enable ADC,free running,start convertion,ADC inrtpt enable,128 prescalar,
	ADCSRA |= ((1<<ADEN) | (1<<5) | (1<<ADIE) |(1<<ADPS2) |(1<<ADPS1) |(1<<ADPS0)); 
}

int ADCData(uchar chnl)
{
	int adcval;
	ADMUX = (ADMUX & 0xE0) | chnl;
	_delay_ms(5);
	ADCSRA |= (1<<ADSC);//start the conversion
	while(!(ADCSRA & (1<<ADIF))); //wait for Conversion complete flag to raise
	ADCSRA |= (1<<ADIF);//clear flag
	adcval = ADC;

	return adcval;
}



