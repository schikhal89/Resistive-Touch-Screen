#ifndef	  	_GLCD_H
#define 		_GLCD_H

#include <avr/io.h>
#include <util/delay.h>
#include  <math.h>
#include <string.h>

#include "glcdprototypes.h"
#include "fonts.h"

/*
	File   : JHD 12864 E (controller KS0108B)Black & White Graphical LCD Header file
	Date : 24/09/2012	Monday
	Author: Jayesh Sarvaiya
	Pinouts:
	1 		= 	Vss (Ground) 0 V
	2		=     Vdd (Vcc)	    5 V
	3		= 	V0/Vdd/NC	 	(GLCD contrast adj./ LCD Voltage / Not Connected)	
	4		=     D/I		Data / Instrunction Register Select Pin (Same as RS(Register Select)):
									D/I = 0 ==> Command ,D/I = 1 ==> Data
	5		=    R/W 					(Rread / Write Selection)	
	6		=     E		  					(Enable)	(SCLK  = Serial Clock)
	7		=     DB0
	8		=     DB1
	9		=     DB2
	10	=     DB3
	11	=     DB4
	12	=     DB5
	13	=     DB6
	14	=     DB7
	15	=     CS1 			   	(Chip Select 1)
	16	=     CS2 			   	(Chip Select 2)
	17	=	    RST					(Reset)	
	18	=		Vee					10 V (here, NC)
	19	=		A					   (Backlight Anode)   (+)
	20	=		K  				   (Backlight Cathod)  (-)
	*/

/*	Commands as per the data sheet	*/
#define 		DISPON					0b00111111		//	0x3F	//Display ON
#define 		DISPOFF				0b00111110		//	0x3E	//Display OFF
#define		YADDRS					0b01000000		//	0x40	//Y Address (0 to 63)
#define		PAGE						0b10111000		//	0xB8	//Set Page 	(0 to 7)
#define		XADDRS					0b00000000		//	0x00	//X Address	(0 to 7)
#define 		DISPSTARTLINE	0b11000000		//	0xC0	//Start line 	(0 to 63)

#define 		MAX_X		127
#define 		MAX_Y	 	63
#define 		MIN_X		0
#define 		MIN_Y		0

#define 		LEFTHALF		0
#define 		RIGHTHALF	1

#define     CLR                 0  
#define     SET                 1
#define     TGL                 2
#define     AND                 3

#define	    TORADIAN(deg)			(deg * 3.1415 / 180)	
#define 		RIGHT			0
#define 		UP				1
#define 		LEFT				2
#define 		DOWN			3

#define 		ROTATE		0
#define 		SHIFT			1
/**************************************************************************/

void GLcdinit(void)
{
	GLCDDDR = 0xff;							//	Data Port as output Port
	CNTRLNDDR = 0xff;						//	Control Port as output Port
	CNTRLNDDR1 = 0xff;					//	Control1 Port as output Port
	GlcdSelHalfLcd(LEFTHALF);		//	Select Left half of GLCD
	GlcdClear();									//	Clear Graphics on GLCD
	GlcdSelHalfLcd(RIGHTHALF);		//	Select Left half of GLCD
	GlcdClear();									//	Clear Graphics on GLCD
//	GlcdCmd(YADDRS);						//	Set Y address 0
//	GlcdCmd(PAGE);							//	Select Page 0 
//	GlcdCmd(DISPSTARTLINE+0);	//	Start Displaying from 0th line
	GlcdClearBuffer();						//	Clear GLCD Buffer 
}

void ToggleENPin(void)
{
	CNTRLNPORT 	|= 	(1 << EN);		
/*	 Provide High to Low pulse minimun for 450 nsec	*/
//	_delay_us(0.050);
	CNTRLNPORT 	&= 	~(1 << EN);
}

void GlcdCmd(uchar cmd)
{
/*	 DI = 0 (instruction regr), RW = 0 (Write mode), EN  = 0*/	
	CNTRLNPORT &= ~( (1 << DI ) | ( 1 << RW) | (1<<EN) );
	GLCDPORT = cmd;
	ToggleENPin();														//Latch the data
	_delay_us(7);
}

void GlcdData(uchar data)
{
/*	 DI = 1 (data regr), RW = 0 (Write mode), EN  = 0*/	
	CNTRLNPORT &= ~ (( 1 << RW) | (1<<EN) );	
	CNTRLNPORT |= (1<<DI); 									// DI = 1 (data regr),
	GLCDPORT = data;
	ToggleENPin();														//Latch the data
	_delay_us(7);
}

/*	Selects left/Right Half of GLCD	*/
// side = 0 (left half ) or 1 (right half)
void GlcdSelHalfLcd(uchar whichhalf)
{
	if(whichhalf == LEFTHALF)
			{   CHIPSEL1;GlcdCmd(DISPON);   }
	else if(whichhalf == RIGHTHALF)
            {   CHIPSEL2;GlcdCmd(DISPON);   }
}

void GlcdGotoXY(uchar x,uchar y)
{
	uchar col,page,shift;
	if(x < 64)
		col = x;
	else if(x >= 64)	
		col = x - 64;
	page = y / 8;	
	shift = y % 8;
	
	/*	Step 1 :	Set first or second vertical half 	*/
	if(x<64) 
		GlcdSelHalfLcd(LEFTHALF);
	else if(x>=64)
		GlcdSelHalfLcd(RIGHTHALF);
	
	/* 	Step 2 : set Y (column) address	*/
	/*	Column address 0 to 63 				*/
	GlcdCmd(YADDRS + col);
	
	/*	Step 3 :	Set	Page :	page address = 0 to 7	*/
	GlcdCmd(PAGE + page);
}

void GlcdClear(void)
{
	uchar row,col;
    
	GlcdSelHalfLcd(LEFTHALF);
	for(row = 0 ;row <8 ;row++)
	{
		GlcdGotoXY(0,row*8);
		for(col = 0 ;col <64;col++)
            GlcdData(0);
	}	
    
	GlcdSelHalfLcd(RIGHTHALF);
	for(row = 0 ;row <8 ;row++)
	{
		GlcdGotoXY(64,row*8);
        for(col = 0 ;col <64;col++)
            GlcdData(0);
    }   
}

void GlcdClearBuffer(void)
{
	uchar row,col;
	for(row = 0 ;row <8 ;row++)
		for(col = 0 ;col <128 ;col++)
            GlcdBuffer[row][col] = 0;
}        

void GlcdUpdateBufferOnLCD(void)
{
	uchar row,col;
	GlcdSelHalfLcd(LEFTHALF);
	for(row = 0 ;row <8 ;row++)
	{
		GlcdGotoXY(0,row*8);
		for(col = 0 ;col <64;col++)
            GlcdData(GlcdBuffer[row][col]);
	}	
    
	GlcdSelHalfLcd(RIGHTHALF);
	for(row = 0 ;row <8 ;row++)
	{
		GlcdGotoXY(64,row*8);
        for(col = 0 ;col <64;col++)
            GlcdData(GlcdBuffer[row][col+64]);
    }   
}

void GlcdInvertDisplay(void)
{
	 	uchar row,col;
	for(row=0;row<8;row++)
		for(col=0;col<128;col++)
            GlcdBuffer[row][col] = ~GlcdBuffer[row][col];	//	Inverts All data in GlcdBuffer
		
        GlcdUpdateBufferOnLCD();							//Now reflect the same on Glcd
}

void GlcdAllClear(void)
{
    GlcdClear();
    GlcdClearBuffer();
    GlcdUpdateBufferOnLCD();
}

/*
	x : 0 to 127
	y : 0 to 63
	changetype : 0,1,2,3
	0	==> 	Set pixel
	1	==>	Clear Pixel
	2	==> 	Toggle Pixel
	3	==> 	ANDing of Pixel
*/

void GlcdModifyPixel(uchar x,uchar y,uchar changetype)
{
	uchar page,shift,data,val;
	GlcdGotoXY(x,y);
	page = y / 8;	
	shift = y % 8;
/*	Read data from GLCD Buffer	*/
	data = GlcdBuffer[page][x];
	val = (1<<shift);
/*	Modify the data						*/	
	if(changetype == SET)		//	To Set pixel
		data |= val;
	else if(changetype == CLR)//	To Clear pixel
		data &= ~val;
	else if(changetype == TGL)//	To Toggle pixel
		data ^= val;
	else if(changetype == AND)//	To AND pixel value with exixting value
		data &= val;
		
/*	Write data back to GLCD Buffer	*/
	GlcdBuffer[page][x] = data;
/*	Display data on GLCD						*/	
	GlcdData(data);
//	GlcdUpdateBufferOnLCD();
}

void GlcdHLine(uchar x,uchar y,uchar len,uchar changetype)
{
	while(len--)
		GlcdModifyPixel(x++,y,changetype);
}

void GlcdVLine(uchar x,uchar y,uchar len,uchar changetype)
{
	while(len--)
		GlcdModifyPixel(x,y++,changetype);
}

/*		The Function below can draw only 45°  lines	 */
void GlcdLineCross(uchar x1,uchar y1,uchar x2,uchar y2,uchar changetype)
{
	if(x1<x2)
	{
	GlcdModifyPixel(x1,y1,changetype);		
	do
		{
			x1++;
			if(y1<y2)
					y1++;
			else if(y1>y2)
					y1--;
		}while(x1!=x2);	
		GlcdModifyPixel(x1,y1,changetype);		
	}
	else if(x1>x2)
	{
	GlcdModifyPixel(x1,y1,changetype);		
	do
		{
			x1--;
			if(y1>y2)
					y1--;
			else if(y1<y2)
					y1++;
			GlcdModifyPixel(x1,y1,changetype);		
		}while(x1!=x2);	
	}
	else if(x1==x2)
		{
			GlcdModifyPixel(x1,y1,changetype);					
			do
			{
			if(y1>y2)
					y1--;
			else if(y1<y2)
					y1++;
			GlcdModifyPixel(x1,y1,changetype);					
			}while(y1!=y2);
		}			
}

/*	To draw Lines with all possible angles	 */
void GlcdLine(uint x1,uint y1,uint x2,uint y2,uchar changetype)
{   
	float x,y,xinc,yinc;
	int k,dx,dy;
	int step;

	dx=x2-x1;
	dy=y2-y1;
	
	if(abs(dx)>abs(dy))
		step=abs(dx);
	else 
		step=abs(dy);
		
	xinc = (float)dx/step;
	yinc = (float)dy/step;
	x=x1;
	y=y1;
//added foll line to remove error of 1 starting corner pixel not getting displayed     
	GlcdModifyPixel((uint)x,(uint)y,changetype);
	for(k=1;k<=step;k++)
	{
		x=x+xinc;
		y=y+yinc;
		GlcdModifyPixel((uint)x,(uint)y,changetype);
	}
}

void GlcdLineAtAngle(uchar length,uint angle,uchar startx,uchar starty,uchar changetype)
{
	uchar endx,endy;
	endx = startx + (uint)(length *  cos(TORADIAN(angle)));
	endy = starty + (uint)(length *  sin(TORADIAN(angle)));
	GlcdLine(startx,starty,endx,endy,changetype);
}

void GlcdRectangle(uchar x,uchar y,uchar len,uchar brdth,uchar changetype)
{
	GlcdLine(x,y,x,y+brdth,changetype);						//right vertical
	GlcdLine(x,y,x+len,y,changetype);							//top horizontal
	GlcdLine(x,y+brdth,x+len,y+brdth,changetype);	//bottom horizontal
	GlcdLine(x+len,y,x+len,y+brdth,changetype);		//left vertical  
}

void GlcdRectangleFill(uchar x,uchar y,uchar len,uchar brdth,uchar changetype)
{
	uchar temp;
	for(temp = y ; temp< brdth ; temp++)
		GlcdLine(x,temp,x+len,temp,changetype);							//top horizontal
}

/*
Added on 19/09/2012 Wednesday

*/
void  GlcdQuadrilateral(uchar x1,uchar y1,uchar x2,uchar y2,uchar x3,uchar y3,uchar x4,uchar y4,uchar changetype)
{
	GlcdLine(x1, y1, x2, y2,changetype);
	GlcdLine(x1, y1, x3, y3,changetype);
	GlcdLine(x4, y4, x2, y2,changetype);
	GlcdLine(x4, y4, x3, y3,changetype);
}

int GlcdGetPtDist(signed char x1,signed char y1,signed char x2,signed char y2)
{
	return ((int)ceil(sqrt (((x2 - x1)  * (x2 - x1)) +  ((y2 - y1)  * (y2 - y1)))));
}

void GlcdCircle(uchar centerX,uchar centerY,uchar rad,uchar changetype)
{
signed char   x,y;
	uint theta;
	for (theta = 0; theta <=360 ; theta++)
	{
		x = rad * cos(TORADIAN(theta));
		y = rad * sin(TORADIAN(theta));
		if((GlcdGetPtDist(x+centerX,y+centerY,centerX,centerY) <=  rad )) 
			GlcdModifyPixel(centerX+x,centerY+y,changetype);
	}	
}

void GlcdTriangle(uchar x1,uchar y1,uchar x2,uchar y2,uchar x3,uchar y3,uchar changetype)
{
    GlcdLine(x1,y1,x2,y2,changetype);
    GlcdLine(x2,y2,x3,y3,changetype);
    GlcdLine(x3,y3,x1,y1,changetype);
}

/*	Time Period is in no. of Pixels	*/
void GlcdSin(uchar center,uchar amplitude,uchar  timeperiod,uchar noofcycles,uchar phase,uchar changetype)
{
	uchar x,y;
	uint theta;
	for(theta=phase;theta<(360*noofcycles+phase);theta++)
	{
		y = center - amplitude*sin(TORADIAN(theta));//-ve bcoz in GLCD we have 4th quadrant(image of 1st) and not 1st 
		x=(uchar)((int)theta/((float)360/timeperiod)) - (uchar)(phase/((float)360/timeperiod));
		GlcdModifyPixel(x,y,changetype);
	}
}

void GlcdCos(uchar center,uchar amplitude,uchar  timeperiod,uchar noofcycles,uchar phase,uchar changetype)
{
	uchar x,y;
	uint theta;
	for(theta=phase;theta<(360*noofcycles+phase);theta++)
	{
		y = center - amplitude*cos(TORADIAN(theta));//-ve bcoz in GLCD we have 4th quadrant(image of 1st) and not 1st 
		x=(uchar)((int)theta/((float)360/timeperiod)) - (uchar)(phase/((float)360/timeperiod));
		GlcdModifyPixel(x,y,changetype);
	}
}

/*
Rose:From Wiki...
r = cos(k*theta)
k = no of leaves in rose curve
if k == odd  then same no of leaves are produced
		k == even then 2 * k of leaves are produced
also equation can be written as 		
r = shrink+ cos(k*theta)
this will increase/shrink the size of alternate leaves if k = even
or	will  increase/shrinke the size of leaves at same place if  k = odd
NOTE :keep shrink  0 here.
*/
void GlcdRose(uchar centerX,uchar centerY,uchar noofleaves,uchar length,uchar shrink,uchar changetype)
{
	float r,x,y;
	uint theta;
	for(theta=0;theta<=360;theta++)
	{
		r = shrink + length * cos(noofleaves*TORADIAN(theta));
		x =centerX + r*cos(TORADIAN(theta));
		y = centerY + r*sin(TORADIAN(theta));
		GlcdModifyPixel(x,y,changetype);
	}
}

/*
***************************************************************
Deltoid is also known as hypocycloid of three cusps (cusps = sharp curves)
Equations obtained from Wikipedia... :)

a = radius of INNER rolling cirlce
x = a(2cos (t) +  cos (2t))
y = a(2sin (t) - sin (2t)) 
OR ... to change orientation by 180° just change +ve and - ve signs as follows
x = a(2cos (t) -  cos (2t))
y = a(2sin (t) + sin (2t)) 
*/
void GlcdDeltoid(uchar centerX,uchar centerY,uchar radinnrcircle,uchar changetype)
{
	uint theta;
	char x,y;
	for(theta = 0; theta <=360; theta++)
	{
		x = centerX + radinnrcircle*(2 * cos (TORADIAN(theta)) + cos (TORADIAN(2*theta))); 
		y = centerY + radinnrcircle*(2 * sin (TORADIAN(theta)) - sin (TORADIAN(2*theta))) ;
		GlcdModifyPixel(x,y,changetype);
	}
}

/*
**************************************************************
Cardoid:
Equations obtained from Wikipedia... :)
a = radius of OUTER rolling circle

x = a(2cos (t) +  cos (2t))
y = a(2sin (t) + sin (2t)) 
OR ... to change orientation by 180° just change +ve and -ve signs as follows
x = a(2cos (t) -  cos (2t))
y = a(2sin (t) - sin (2t)) 

**************************************************************
*/
void GlcdCardoid(uchar centerX,uchar centerY,uchar radoutercircle,uchar changetype)
{
	uint theta;
	char x,y;
	for(theta = 0; theta <=360; theta++)
	{
		x = centerY + radoutercircle*(2 * cos (TORADIAN(theta)) + cos (TORADIAN(2*theta))); 
		y = centerY + radoutercircle*(2 * sin (TORADIAN(theta)) + sin (TORADIAN(2*theta))) ;
		GlcdModifyPixel(x,y,changetype);
	}
}

/*
***********************************************
Astroid:From Wiki...
A circle of radius 1/4 rolls around inside a circle of radius 1 
and a point on its circumference traces an astroid.

Equations obtained from Wikipedia... :)
x^(2/3) + y^(2/3) = 1
in parametric  form...
x =cos^3 (theta)  (i.e cos cube theta)
y =sin^3 (theta)   (i.e sin cube theta)

***********************************************
*/
void GlcdAstroid(uchar centerX,uchar centerY,uchar radoutercircle,uchar changetype)
{
	uint theta;
	char x,y;
	for(theta = 0; theta <=360; theta++)
	{
		x = centerX + radoutercircle*(cos (TORADIAN(theta)) * cos (TORADIAN(theta)) *  cos (TORADIAN(theta))); 
		y = centerY + radoutercircle*(sin (TORADIAN(theta)) * sin (TORADIAN(theta)) *  sin (TORADIAN(theta))); 
		GlcdModifyPixel(x,y,changetype);
	}
}

/*
***********************************************
In geometry, a hypocycloid is a special plane curve generated by the 
trace of a fixed point on a small circle that rolls within a larger circle

Equations obtained from Wikipedia... :)
x = (R - r) cos(theta) + r cos (((R-r)/r) * theta )
x = (R - r) sin(theta) - r sin (((R-r)/r) * theta )

OR...say...R = k * r (k is an quatient obtained by deviding R by r)..then,

x = r(k - 1) cos(theta) + r cos ((k-1) * theta )
x = r(k - 1) sin(theta)   - r sin  ((k-1) * theta )
here, k decides how many cusps(sharp corner wiil be there in hypocycloid)

***********************************************
*/
void GlcdHypoCycloid(uchar centerX,uchar centerY,float radinnrcircle,float noofcorners,uchar changetype)
{
	uint theta;
	uchar x,y;
	float diff,R,r;
	r=radinnrcircle;
	R =(uint) r *(uint) noofcorners;//radoutercircle
//	R = noofcorners;
	diff=R-r;
	for(theta = 0; theta <=360; theta++)
	{
		x = centerX + diff * cos (TORADIAN(theta)) + r * cos ((diff/r)*TORADIAN(theta));
		y = centerY +  diff * sin (TORADIAN(theta)) - r * sin ((diff/r)*TORADIAN(theta));
		GlcdModifyPixel(x,y,changetype);
	}
}

/*
Archemedean Spiral : From Wiki...
r = a + b *theta
a = a turns the spiral (start)
b = distance between successive turnings. (space)
theta in radians
*/

void GlcdArchemedeanSpiral(uchar centerX,uchar centerY,uchar start,uchar space,float noofcycles,uchar changetype)
{
	float r,x,y;
	uint theta;
	for(theta=0;theta<=360 * noofcycles;theta++)
	{
		r = start + space * (TORADIAN(theta));
		x = centerX + r*cos(TORADIAN(theta));
		y = centerY + r*sin(TORADIAN(theta));
		GlcdModifyPixel(x,y,changetype);
	}
}
/*
Involute of a  cirlce: From Wiki...
x = a(cos theta  + theta *  sin theta)
y = a(sin theta  - theta  * cos theta)
a = radius of the circle around which we draw the curve (radius)
*/
void GlcdInvoluteofCircle(uchar centerX,uchar centerY,float radius,float noofcycles,uchar changetype)
{
	float x,y;
	uint theta;
	for(theta=0;theta<=360 * noofcycles;theta++)
	{
		x = centerX + radius*(cos(TORADIAN(theta)) + (TORADIAN(theta)) * sin(TORADIAN(theta)));
		y = centerY + radius*(sin(TORADIAN(theta)) -(TORADIAN(theta))* cos(TORADIAN(theta)));
		GlcdModifyPixel(x,y,changetype);
	}
}

/*
Logarithmic spiral: From Wiki...
A logarithmic spiral, equiangular spiral or growth spiral is a special kind of spiral curve
which often appears in nature. The logarithmic spiral was first described by Descartes 
and later extensively investigated by Jacob Bernoulli, who called it Spira mirabilis, 
"the marvelous spiral"

In polar coordinates:
r = a * e ^(b*theta)
e = eular number = 2.71828
in parametric form:
x = r * cos(theta) =  a * e ^(b*theta) cos(theta);
x = r * sin(theta) =   a * e ^(b*theta) sin(theta);
*/

void GlcdLogarithmicSpiral(uchar centerX,uchar centerY,float start,float space,float noofcycles,uchar changetype)
{
	float r,x,y,e = 2.71828;
	uint theta; 
	for(theta=0;theta<=360 * noofcycles;theta++)
	{
		r = start * pow(e,space * (TORADIAN(theta)));
		x = centerX + r*cos(TORADIAN(theta));
		y = centerY + r*sin(TORADIAN(theta));
		GlcdModifyPixel(x,y,changetype);
	}
}



/*
	Page Write ( To Read data from SRAM and display)
*/
void GlcdWRPage(uchar whichhalf,uchar pgno,uchar pagedata[64],uchar changetype)
{
	int temp;
	if(whichhalf == LEFTHALF) 
    {
        switch(pgno)
        {
            case 0:	GlcdGotoXY(0,0);	break;
            case 1:	GlcdGotoXY(0,8);	break;
            case 2:	GlcdGotoXY(0,16);	break;
            case 3:	GlcdGotoXY(0,24);	break;
            case 4:	GlcdGotoXY(0,32);	break;
            case 5:	GlcdGotoXY(0,40);	break;
            case 6:	GlcdGotoXY(0,48);	break;
            case 7:	GlcdGotoXY(0,56);	break;
            default:	break;
        }
    }
	else if(whichhalf == RIGHTHALF)
    {
        switch(pgno)
        {
            case 0:	GlcdGotoXY(64,0);   	break;
            case 1:	GlcdGotoXY(64,8);	    break;
            case 2:	GlcdGotoXY(64,16);	break;
            case 3:	GlcdGotoXY(64,24);	break;
            case 4:	GlcdGotoXY(64,32);	break;
            case 5:	GlcdGotoXY(64,40);	break;
            case 6:	GlcdGotoXY(64,48);	break;
            case 7:	GlcdGotoXY(64,56);	break;
            default:	break;
        }
    }

	for(temp = 0; temp < 64; temp++)
	{
		if(changetype == SET)		// Actual Data
		{
			GlcdData(pagedata[temp]);
			GlcdBuffer[pgno][whichhalf*64+temp] = pagedata[temp];//store data into Buffer
		}
		else if(changetype == CLR)	//Negative  Data
		{
			GlcdData(~pagedata[temp]);
			GlcdBuffer[pgno][whichhalf*64+temp] = ~pagedata[temp];//store data into Buffer
		}
	}
}

// mode = 0 for negative image,mode =1 for normal image
void GlcdShowImage1(uchar * img,uchar changetype)
{
	uchar page,col;
	for(page=0;page <8;page++)
	{
		GlcdSelHalfLcd(LEFTHALF);
		for(col = 0;col<64;col++)
			{
			GlcdGotoXY(col,page*8);
				if(changetype == 1)			//	Actual Image
				{
					GlcdData(*img);
					GlcdBuffer[page][col] =*img;
				}
				else if(changetype == 0)//	Negative Image
				{
					GlcdData(~(*img));
					GlcdBuffer[page][col] =~(*img);
				}
				img++;
			}
			
		GlcdSelHalfLcd(RIGHTHALF);
		for(col = 0;col<64;col++)
			{
			GlcdGotoXY(col+64,page*8);
				if(changetype == 1)		//	Actual Image
				{
					GlcdData(*img);
					GlcdBuffer[page][64+col] =*img;
				}
				else if(changetype == 0)//	Negative Image
				{
					GlcdData(~(*img));
					GlcdBuffer[page][64+col] =~(*img);
				}
				img++;
			}
	}
}

// mode = 0 for negative image,mode =1 for normal image
void GlcdShowImage2(uchar * img,uchar changetype)
{
	uchar page;
	for(page=0; page <8; page++)
	{
		GlcdWRPage(LEFTHALF		,page,	img + page * 128,      changetype);
		GlcdWRPage(RIGHTHALF	,page,	img + page * 128+64, changetype);
	}	
}

/*
	This is  a function to display a small image on lcd
*/
void GlcdPutImage(const uchar *img,uchar x,uchar y,uchar width,uchar height,uchar changetype)
{
	uchar lines,bytesinline,bytes,bit,bitmax,temp,ht = height;
	bytesinline =  (height - 1) / 8 + 1;
	for(bytes=0;bytes<bytesinline;bytes++)
	{
		if(ht <= 8)
			bitmax = ht;
		else if (ht > 8)	
		{
			bitmax = 8;
			ht -= 8;
		}	
		for(lines=0;lines<width;lines++)
		{
		temp = img[bytes*width+lines];
		for(bit=0;bit<bitmax;bit++)
			{
				if(temp & 0x01) 
				{
					if(changetype == SET)
						GlcdModifyPixel(x+lines,y+bytes*8+bit,SET);
					else if	(changetype == CLR)
						GlcdModifyPixel(x+lines,y+bytes*8+bit,CLR);
				}		
				else if(!(temp & 0x01) )
				{	
					if(changetype == SET)
						GlcdModifyPixel(x+lines,y+bytes*8+bit,CLR);
					else if	(changetype == CLR)
						GlcdModifyPixel(x+lines,y+bytes*8+bit,SET);			
				}		
				temp >>=1;
			}			
		}
	}		
}

/*
void GlcdPutImage(uchar *img,uchar x,uchar y,uchar bytesinline,uchar noofvertlines,uchar changetype)
{
	uchar lines,bytes,bit,temp;
	for(bytes=0;bytes<bytesinline;bytes++)
		for(lines=0;lines<noofvertlines;lines++)
		{
		temp = img[bytes*noofvertlines+lines] ;
		for(bit=0;bit<8;bit++)
			{
				if(temp & 0x01) 
				{
					if(changetype == SET)
						GlcdModifyPixel(x+lines,y+bytes*8+bit,SET);
					else if	(changetype == CLR)
						GlcdModifyPixel(x+lines,y+bytes*8+bit,CLR);
				}		
				else if(!(temp & 0x01) )
				{	
					if(changetype == SET)
						GlcdModifyPixel(x+lines,y+bytes*8+bit,CLR);
					else if	(changetype == CLR)
						GlcdModifyPixel(x+lines,y+bytes*8+bit,SET);			
				}		
				temp >>=1;
			}			
		}
}
*/


/*
************************************************************
Some of Above  functions are modified to read data (image) stored  in 
FLASH (which is not read back...in to SRAM) _P means functions deals with 
program memory i.e FLASH and not SRAM
modified functions are..
1) GlcdWRPage()
2) GlcdShowImage1()
3) GlcdShowImage2()
4) GlcdPutImage()
************************************************************
*/
void GlcdWRPage_P(uchar whichhalf,uchar pgno,const uchar pagedata[64],uchar changetype)
{
	int temp;
	if(whichhalf == LEFTHALF) 
    {
        switch(pgno)
        {
            case 0:	GlcdGotoXY(0,0);	break;
            case 1:	GlcdGotoXY(0,8);	break;
            case 2:	GlcdGotoXY(0,16);	break;
            case 3:	GlcdGotoXY(0,24);	break;
            case 4:	GlcdGotoXY(0,32);	break;
            case 5:	GlcdGotoXY(0,40);	break;
            case 6:	GlcdGotoXY(0,48);	break;
            case 7:	GlcdGotoXY(0,56);	break;
            default:	break;
        }
    }
	else if(whichhalf == RIGHTHALF)
    {
        switch(pgno)
        {
            case 0:	GlcdGotoXY(64,0);   	break;
            case 1:	GlcdGotoXY(64,8);	    break;
            case 2:	GlcdGotoXY(64,16);	break;
            case 3:	GlcdGotoXY(64,24);	break;
            case 4:	GlcdGotoXY(64,32);	break;
            case 5:	GlcdGotoXY(64,40);	break;
            case 6:	GlcdGotoXY(64,48);	break;
            case 7:	GlcdGotoXY(64,56);	break;
            default:	break;
        }
    }

	for(temp = 0; temp < 64; temp++)
	{
		if(changetype == SET)		// Actual Data
		{
			GlcdData(pgm_read_byte(&pagedata[temp]));
			GlcdBuffer[pgno][whichhalf*64+temp] = pgm_read_byte(&pagedata[temp]);//store data into Buffer
		}
		else if(changetype == CLR)	//Negative  Data
		{
			GlcdData(~(pgm_read_byte(&pagedata[temp])));
			GlcdBuffer[pgno][whichhalf*64+temp] = ~(pgm_read_byte(&pagedata[temp]));//store data into Buffer
		}
	}
}

// mode = 0 for negative image,mode =1 for normal image
void GlcdShowImage1_P(const uchar * img,uchar changetype)
{
	uchar page,col;
	for(page=0;page <8;page++)
	{
		GlcdSelHalfLcd(LEFTHALF);
		for(col = 0;col<64;col++)
			{
			GlcdGotoXY(col,page*8);
				if(changetype == 1)			//	Actual Image
				{
					GlcdData(pgm_read_byte(img));
					GlcdBuffer[page][col] =pgm_read_byte(img);
				}
				else if(changetype == 0)//	Negative Image
				{
					GlcdData(~(pgm_read_byte(img)));
					GlcdBuffer[page][col] =~(pgm_read_byte(img));
				}
				img++;
			}
			
		GlcdSelHalfLcd(RIGHTHALF);
		for(col = 0;col<64;col++)
			{
			GlcdGotoXY(col+64,page*8);
				if(changetype == 1)		//	Actual Image
				{
					GlcdData(pgm_read_byte(img));
					GlcdBuffer[page][64+col] =pgm_read_byte(img);
				}
				else if(changetype == 0)//	Negative Image
				{
					GlcdData(~(pgm_read_byte(img)));
					GlcdBuffer[page][64+col] =~(pgm_read_byte(img));
				}
				img++;
			}
	}
}

// mode = 0 for negative image,mode =1 for normal image
void GlcdShowImage2_P(const uchar * img,uchar changetype)
{
	uchar page;
	for(page=0; page <8; page++)
	{
		GlcdWRPage_P(LEFTHALF		,page, img + page * 128,  changetype);
		GlcdWRPage_P(RIGHTHALF	,page, img + page * 128+64, changetype);
	}	
}

/*
	This is  a function to display a small image on lcd
*/
void GlcdPutImage_P(const uchar *img,uchar x,uchar y,uchar width,uchar height,uchar changetype)
{
	uchar lines,bytesinline,bytes,bit,bitmax,temp,ht = height;
	bytesinline =  (height - 1) / 8 + 1;
	for(bytes=0;bytes<bytesinline;bytes++)
	{
		if(ht <= 8)
			bitmax = ht;
		else if (ht > 8)	
		{
			bitmax = 8;
			ht -= 8;
		}	
		for(lines=0;lines<width;lines++)
		{
		temp = pgm_read_byte(&img[bytes*width+lines]);
		for(bit=0;bit<bitmax;bit++)
			{
				if(temp & 0x01) 
				{
					if(changetype == SET)
						GlcdModifyPixel(x+lines,y+bytes*8+bit,SET);
					else if	(changetype == CLR)
						GlcdModifyPixel(x+lines,y+bytes*8+bit,CLR);
				}		
				else if(!(temp & 0x01) )
				{	
					if(changetype == SET)
						GlcdModifyPixel(x+lines,y+bytes*8+bit,CLR);
					else if	(changetype == CLR)
						GlcdModifyPixel(x+lines,y+bytes*8+bit,SET);			
				}		
				temp >>=1;
			}			
		}
	}		
}

//						'j'					0				0								5						8						1
void GlcdShowChar_P(uchar ch,uchar x,uchar y,uchar FontWidth,uchar FontHeight,uchar changetype)
{
	if((FontWidth == 5) && (FontHeight == 8))
		GlcdPutImage_P(&font5X8[(ch - ' ')*(FontWidth+1)+1] , x, y,  FontWidth, FontHeight, changetype);
}

/*
	In typography, kerning (less commonly mortising) is the process of adjusting the spacing
	between characters in a proportional font, usually to achieve a visually pleasing result. 
*/
void GlcdShowString_P(uchar *str,uchar x,uchar y,uchar FontWidth,uchar FontHeight,uchar changetype)
{
	uchar len,temp;
	len = strlen(str);
	for(temp = 0;temp<len;temp++)
	{
		if((FontWidth == 5) && (FontHeight == 8))
			GlcdPutImage_P(&font5X8[(str[temp] - ' ')*(FontWidth+1)+1] , x, y, FontWidth,FontHeight,  changetype);
		x +=  pgm_read_byte(&font5X8[(str[temp] - ' ')*(FontWidth+1)] )+1;
	}		
}

void GlcdShowNumber_P(uint num,uchar x,uchar y,uchar FontWidth,uchar FontHeight,uchar changetype)
{
	uchar buff[12];
	sprintf(buff,"%d",num);
	GlcdShowString_P(buff,x,y,FontWidth,FontHeight,changetype);
}
 
/************************************************************/
/************************************************************/

void GlcdRotateImg(uchar shiftdirection,uchar shifttype)
{
	signed int col,page,temp[16] = {0};//use 8 byte for right left shift and all 16 bytes for up down shift
	//uchar col; 
	switch(shiftdirection)
	{
		 case RIGHT:
			for(page = 0 ;  page < 8 ; page++)
					 temp[page] = GlcdBuffer[page][127] ;
		 
			for(page = 0 ;  page < 8 ; page++)
				for(col = 126 ;  col >=0 ; col--)
					 GlcdBuffer[page][col+1] = GlcdBuffer[page][col];
		
		if(shifttype == ROTATE)
			for(page = 0 ;  page < 8 ; page++)
					 GlcdBuffer[page][0] = temp[page] ;
					 
		 break;

		 case LEFT:
			for(page = 0 ;  page < 8 ; page++)
					 temp[page] = GlcdBuffer[page][0] ;
		 
			for(page = 0 ;  page < 8 ; page++)
				for(col = 1 ;  col <=127 ; col++)// going upto 126 only,else if for 127 it can oveflow
					 GlcdBuffer[page][col-1] = GlcdBuffer[page][col];
		
		if(shifttype == ROTATE)
			for(page = 0 ;  page < 8 ; page++)
					 GlcdBuffer[page][127] = temp[page] ;
		break;
			
		 case UP:
/*	Collect bit 0 data to temp array		(col 0 to 127)*/
				for(col = 0 ;  col <=127 ; col++)
				{
					temp[col/8] <<=1;
					temp[col/8]  |= GlcdBuffer[0][col] & 0x01;
				}	
/*	Now shift all data  	*/					
			for(page = 0 ;  page < 7 ; page++)
				for(col = 0 ;  col <=127 ; col++)
				{
					GlcdBuffer[page][col] >>= 1;
					GlcdBuffer[page][col]  |= ((GlcdBuffer[page+1][col] & 0x01) << 7);
				}	
/*	Shifting for last page	*/

				for(col = 0 ;  col <=127 ; col++)
				{
					GlcdBuffer[7][col] >>= 1;
					if(shifttype == ROTATE)
						GlcdBuffer[7][col] |= (( temp[col/8] >> (7 - (col % 8) ) ) & 0x01) << 7;
				}
				
		 break;
		 
		 case DOWN:

				for(col = 0 ;  col <=127 ; col++)
				{
					temp[col/8] <<=1;
					temp[col/8]  |= (GlcdBuffer[7][col] & 0x80) >> 7;
				}	

/*	Now shift all data  	*/					
			for(page = 7 ;  page > 0 ; page--)
				for(col = 0 ;  col <=127 ; col++)
				{
					GlcdBuffer[page][col] <<= 1;
					GlcdBuffer[page][col]  |= ((GlcdBuffer[page-1][col] & 0x80) >> 7);
				}	
				
/*	Shifting for first page	*/
				for(col = 0 ;  col <=127 ; col++)
				{
					GlcdBuffer[0][col] <<= 1;
					if(shifttype == ROTATE)
						GlcdBuffer[0][col] |= (( temp[col/8] >> (7 - (col % 8) ) ) & 0x01) ;
				}
		 
		 break;
	}
		GlcdUpdateBufferOnLCD();
}

void GlcdReverse(uchar mirroreffect)
{
	uchar temp[8]={0},row,col;
	for(row = 0; row < 8; row++)
		for(col = 0; col<63 ; col++ )
		{
			temp[row] = GlcdBuffer[row][col];
			if(!mirroreffect)
				GlcdBuffer[row][col] = GlcdBuffer[row][127-col];
			GlcdBuffer[row][127-col] = temp[row];
		}	
		GlcdUpdateBufferOnLCD();
}

uchar GlcdRevrsByte(uchar byte)
{
	uchar temp = 0,i;
	for(i=0 ; i< 8; i++)
		temp |= (((byte >>(7-i)) & 0x01 ) << i);
	
	return temp;
}

void GlcdInvert(uchar mirroreffect)
{
	uchar page,temp = 0;
	uint col;
	for(page=0;page<4;page++)
	{
		for(col=0 ; col < 128 ; col++)
		{
			temp = GlcdBuffer[page][col];
			if(!mirroreffect)
				GlcdBuffer[page][col] = GlcdRevrsByte(GlcdBuffer[7-page][col]);
			GlcdBuffer[7-page][col] = GlcdRevrsByte(temp);
		}
	}	
	GlcdUpdateBufferOnLCD();			
}
/*
	from , to : 0 to 63
*/
void GlcdRotateInbuit(uchar from,uchar to,float delay)
{
	signed char temp;
	if(from < to)
	{
		for(temp = from; temp <= to ; temp++)
		{
			GlcdGotoXY(0,0);
			GlcdCmd(DISPSTARTLINE|temp);
			GlcdGotoXY(64,0);
			GlcdCmd(DISPSTARTLINE|temp);
			_delay_ms(delay);
		}
	}
	else if(to < from)
	{
		for(temp = from; temp >= to ; temp--)
		{
			GlcdGotoXY(0,0);
			GlcdCmd(DISPSTARTLINE|temp);
			GlcdGotoXY(64,0);
			GlcdCmd(DISPSTARTLINE|temp);
			_delay_ms(delay);
		}
	}
}

/*
*		This function maps coordinates of some system into GLCD coordinates
*/
uint GlcdMapData(uint start,uint end,uint val,uint glcdstart,uint glcdend)
{
	int diff1,diff2;
	float divfact,finalval=0;    

	diff1 =  abs(start - end);   
	diff2 =  abs(glcdstart - glcdend);     
	divfact = (float)diff1/diff2;
	finalval=(float)(val - start)/ divfact  + glcdstart;  
	return (uint)finalval;
}

uint GetRandom(uint start,uint end)
{
	uint num;
	num = rand() % end + start;
	if(num > end)
		num = end;
	return num;
}

#endif