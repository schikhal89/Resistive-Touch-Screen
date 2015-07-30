#ifndef	  	_GLCD_H
#define 		_GLCD_H

#include <avr/io.h>
#include <util/delay.h>
#include  <math.h>

/*
	File   : JHD 12864 E Black & White Graphical LCD Header file
	Date : 23/09/2012	Sunday
	Author: Jayesh Sarvaiya
	Pinouts:
	1 		= 	   Vss (Ground)/ 0 V
	2		=     Vdd (Vcc)	 / 5 V
	3		= 	   V0/Vdd/NC	 	(GLCD contrast adj./ LCD Voltage / Not Connected)	
	4		= 	   D/I		Data / Instrunction Register Select Pin (Same as RS(Register Select)):
												D/I = 0 ==> Command ,D/I = 1 ==> Data
	5		=    R/W 					(Rread / Write Selection)	(SID = Serial Data In line)
	6		=     E		  					(Enable)	(SCLK  = Serial Clock)
	7		=     DB0
	8		=     DB1
	9		=     DB2
	10		=     DB3
	11		=     DB4
	12		=     DB5
	13		=     DB6
	14		=     DB7
	15		=     CS1 			    	(Chip Select 1)
	15		=     CS2 			    	(Chip Select 2)
	17		=	    RST					(Reset)	
	18		=		Vee	
	19		=		A					   (Backlight Anode)   (+)
	20		=		K  					  (Backlight Cathod)  (-)
	
	1) DDRAM  	= Display Data RAM.
	2) CGRAM 		= Character Generator RAM.
	3) IRAM			= ICON RAM.
	4) GDRAM		= Graphic Display RAM.
	5) CGROM  	= Character Generater ROM
	6) HCGROM	= Half height character Generater ROM
	7) ACC 		 	= CGRAM Address.
	8) ADD		 	= DDRAM Address.
	9) AC				=  Address Counter used for DD and CG RAM adresses.
	
	8) S = Shift Bit
		if S = 1 => Accompanies dislay shift (i.e cursor moves to right or left after printing a character)
	
	9) D = Display  ON/OFF bit
		if, D == 1 => Display is ON
					  0 => Display is OFF
	
	10) C = Cursor Bit
		if,C == 1 => cursor ON
					  0 => cursor OFF

	11) B = Cursor Blinking ON/OFF bit
		if, B == 1 =>  Cursor Blinking ON
				      0 =>  Cursor Blinking OFF

	12) 1/D = Shift Direction bit
		if , 1/D == 1 => Increament(i.e Right)
					  == 0 => Decreament(i.e Left)	

	13) if, S/C ==1 => Display Shift (Entire Display is shifted)
					== 0 => Cursor Move 
	
	14) if, R/L == 1 => Shift to Right 
					== 0 =>  Shift to Left

	15) DL = Data Length bit
		if  DL == 1 => 8 bit mode
						0 => 4 bit mode
		
	16) N = No. of lines
			N == 1 =>
			N == 0 =>
			
	17) F==1 => 5X10 dots		
				   0 => 5X7 dots
				   
	18) BF = Busy Flag
		if,	BF ==1 => 	LCD busy in doing some internal operation		   
				BF ==0 => 	LCD is ready to accept new instruction
				
	19)	SR = Scroll or RAM address Select
			if SR == 1 =>	Enable Vertical Scroll Position
							0 =>  	Enable IRAM addess (extended instruction)
							0 => 	Enable CGRAM addres (basic instruction)
*/	
/* 	PORTF  = GLCD Data pin port  DB0 - DB7	*/
#define 		GLCDPORT			PORTC//PORTF
#define 		GLCDDDR			DDRC//	DDRF
#define 		GLCDPIN			PINC//	PINF

/* 	PORTG  = GLCD Control pin port i.e D/I , R/W, E	*/
#define 		CNTRLNPORT			PORTG			
#define		CNTRLNDDR			DDRG
#define 		CNTRLNPIN		    PING

#define 		DI								PG0a
#define 		RW							PG1
#define 		EN							PG2
#define 		PSB							PG3
#define 		RST							PG4
 
#define 		LINE1						0x80
#define 		LINE2						0x90
#define 		LINE3						0x88	
#define 		LINE4						0x98

/***************** Commands	(Refer Data Sheet) ***************/
/****************		BASIC INSTRUCTIONS	(RE = 0)   *************/
#define 		GLCD_CLR													0b00000001		//	Clear Data displayed on LCD
#define 		GLCD_HOME												0b00000010		//	Move cursor to Line 1 position 0
#define 		GLCD_MOV_DISP_LT										0b00000111		//	1/D = 1, S = 1
#define 		GLCD_MOV_DISP_RT										0b00000101		//	1/D = 0, S = 1   	
#define 		GLCD_DISP_ON												0b00001100		//	D = 1, C = X , B= X  (X = Don't Care)
#define 		GLCD_DISP_OFF											0b00001000     //	D = 0, C = X , B= X  (X = Don't Care)
#define 		GLCD_DISP_ON_CUR_NOBLNK 							0b00001110		//	D = 1, C = 1 , B= X  (X = Don't Care)
#define 		GLCD_DISP_ON_CUR_BLNKNG 							0b00001111		//	D = 1, C = 1 , B= 1 
#define 		GLCD_SHFT_CUR_RT										0b00011100		//	S/C = 1 , R/L = 1
#define 		GLCD_SHFT_CUR_LT										0b00011000		//	S/C = 1 , R/L = 0
#define 		GLCD_NO_CUR_SHFT										0b00010000		//	S/C = 0 , R/L = 0
#define 		GLCD_4BIT_BASIC_MODE									0b00100000		//	DL = 0, RE = 0
#define 		GLCD_4BIT_EXTND_MODE								0b00100100		//	DL = 0, RE = 1
#define 		GLCD_8BIT_BASIC_MODE									0b00110000		//	DL = 1, RE = 0
#define 		GLCD_8BIT_EXTND_MODE								0b00110100		//	DL = 1, RE = 1
#define   	GLCD_CGRAM_ADDRS										0b01000000		// 	CGRAM = 0x40
#define   	GLCD_DDRAM_ADDRS										0b10000000		// 	DDRAM = 0x80

/**************	EXTENDED INSTRUCTIONS	(RE = 0)	***************/
#define 		GLCD_STANDBY											0b00000001		//	
#define 		GLCD_EN_VER_SCROLL									0b00000011		//SR =1 
#define 		GLCD_EN_IRAM											0b00000010		//SR =0 
#define 		GLCD_EN_CGRAM											0b00000010		//SR =0 
#define 		GLCD_REVERSE_LN1										0b00000100		//R1R0 = 00
#define 		GLCD_REVERSE_LN2										0b00000101		//R1R0 = 01			
#define 		GLCD_REVERSE_LN3										0b00000110		//R1R0 = 10
#define 		GLCD_REVERSE_LN4										0b00000111		//R1R0 = 11
#define 		GLCD_ENTER_SLEEP_MODE								0b00001000		// SL = 0
#define 		GLCD_LEAVE_SLEEP_MODE								0b00001100		// SL =1

#define 		GLCD_4BIT_BASIC_GRPH_OFF							0b00100000		//	DL = 0,RE = 0,G = 0  
#define 		GLCD_4BIT_BASIC_GRPH_ON							0b00100010		//	DL = 0,RE = 0,G = 1  
#define 		GLCD_4BIT_EXTND_GRPH_OFF							0b00100100		//	DL = 0,RE = 1,G = 0  
#define 		GLCD_4BIT_EXTND_GRPH_ON							0b00100110		//	DL = 0,RE = 1,G = 1  

#define 		GLCD_8BIT_BASIC_GRPH_OFF							0b00110000		//	DL = 1,RE = 0,G = 0  
#define 		GLCD_8BIT_BASIC_GRPH_ON		 					0b00110010		//	DL = 1,RE = 0,G = 1  
#define 		GLCD_8BIT_EXTND_GRPH_OFF							0b00110100		//	DL = 1,RE = 1,G = 0  
#define 		GLCD_8BIT_EXTND_GRPH_ON							0b00110110		//	DL = 1,RE = 1,G = 1  
/*	The below macro sets iether Vertical Scroll or I RAM address	*/
/*	depending on setting of SR bit i.e. enable vertical scroll or IRAM */
#define 		GLCD_SET_VSCROLL							0b01000000		//	SR == 1 =>	AC0 - AC5 are address of vertical scroll
																												//	SR == 0	=> 	AC0 - AC3 are address of  ICON RAM
#define 	    GLCD_SET_IRAM									0b01000000		// 0x40
/*
	Set GDRAM address to address counter (AC), first set vertical 
	address and the horizontal address by consecutive  writing
	Vertical Address range = AC6 - AC0
	Horizontal Address range = AC3 - AC0
*/
#define 		GLCD_GDRAM							0b10000000		// 0x 80

#define 	MAX_X		127
#define 	MAX_Y	 	63
#define 	MIN_X		0
#define 	MIN_Y		0
/***********************************************************/

#define TORADIAN(deg)			(deg * 3.1415 / 180)			


typedef  unsigned char uchar;
typedef  unsigned int uint;
static volatile uchar GlcdBuffer[64][16];
/*******************	Function Prototypes	********************/

void GlcdInit8bitBasic(void);
void GlcdInit8bitExtend(void);
void ToggleENPin(void);
void GlcdCmd(uchar cmd);
void GlcdData(uchar data);
void GlcdClear(void);
void GlcdGotoLinePos(uchar line,uchar pos);
void GlcdChar(uchar ch);
void GlcdString(uchar *str);
void GlcdHLine(uchar x,uchar y,uchar len);
void GlcdVLine(uchar x,uchar y,uchar len);
void GlcdLineCross(uchar x1,uchar y1,uchar x2,uchar y2);
void GlcdLine(int x1,int y1,int x2,int y2);
void GlcdRectangle(uchar x,uchar y,uchar len,uchar brdth);
void GlcdCircle(uchar centerX,uchar centerY,uchar rad);
int    GlcdGetPtDist(signed char x1,signed char y1,signed char x2,signed char y2);
void GlcdClearAll();
void GlcdUpdateGlcd();
void GlcdUpdateGlcd();
void GlcdShowPixel(uchar x,uchar y);
void GlcdClearPixel(uchar x,uchar y);
void GlcdTogglePixel(uchar x,uchar y);
void GlcdShowImage(uchar * img,uchar mode);
void GlcdPutImage(uchar *img,uchar x,uchar y,uchar bytesinline,uchar lines);
void GlcdSin(uchar center,uchar amplitude,uchar  timeperiod,uchar noofcycles,uchar phase);
void GlcdCos(uchar center,uchar amplitude,uchar  timeperiod,uchar noofcycles,uchar phase);
void GlcdRose(uchar centerX,uchar centerY,uchar noofleaves,uchar length,uchar shrink);
void GlcdDeltoid(uchar centerX,uchar centerY,uchar radinnrcircle);
void GlcdCardoid(uchar centerX,uchar centerY,uchar radoutercircle);
void GlcdAstroid(uchar centerX,uchar centerY,uchar radoutercircle);
void GlcdHypoCycloid(uchar centerX,uchar centerY,float radinnrcircle,float noofcorners);
void GlcdArchemedeanSpiral(uchar centerX,uchar centerY,uchar start,uchar space,float noofcycles);
void GlcdInvoluteofCircle(uchar centerX,uchar centerY,float radius,float noofcycles);
void GlcdLogarithmicSpiral(uchar centerX,uchar centerY,float start,float space,float noofcycles);

/***********************************************************/

/*********************	Function Definitions	*********************/

void GlcdInit8bitBasic(void)
{
	GLCDDDR = 0xff;												// Set direction for Data lines 
	CNTRLNDDR = 0xff;											// Set direction for Control lines 
	CNTRLNPORT  |= (1<<PSB) | (1<<RST);	// Parallel Mode Selection,display ON
	GlcdCmd(GLCD_8BIT_BASIC_MODE);	
	GlcdCmd(GLCD_DISP_ON_CUR_NOBLNK);	
	GlcdCmd(GLCD_HOME);	
}

void GlcdInit8bitExtend(void)
{
	GLCDDDR = 0xff;												// Set direction for Data lines 
	CNTRLNDDR = 0xff;											// Set direction for Control lines 
	CNTRLNPORT  |= (1<<PSB) | (1<<RST);	// Parallel Mode Selection,display ON
	GlcdCmd(GLCD_8BIT_EXTND_MODE);
	GlcdCmd(GLCD_8BIT_EXTND_GRPH_ON);
}

void ToggleENPin(void)
{
	CNTRLNPORT 	|= 	(1 << EN);		
/*	 Provide High to Low pulse minimun for 450 nsec	*/
	_delay_us(380);
	CNTRLNPORT 	&= 	~(1 << EN);
}

void GlcdCmd(uchar cmd)
{
/*	 DI = 0 (instruction regr), RW = 0 (Write mode), EN  = 0*/	
	CNTRLNPORT &= ~( (1 << DI ) | ( 1 << RW) | (1<<EN) );
	GLCDPORT = cmd;
	ToggleENPin();														//Latch the data
	_delay_us(400);
}

void GlcdData(uchar data)
{
	CNTRLNPORT &= ~ (( 1 << RW) | (1<<EN) );	//  RW = 0 (Write mode), EN  = 0
	CNTRLNPORT |= (1<<DI); 									// DI = 1 (data regr),
	GLCDPORT = data;
	ToggleENPin();														//Latch the data
	_delay_us(380);
}

void GlcdClearCmd()
{
	GlcdCmd(GLCD_CLR);	
}

void GlcdClear(void)
{
	uchar offset,row,col;
	GlcdCmd (GLCD_8BIT_EXTND_GRPH_ON); 
	for (offset = 0; offset <9; offset +=8) 
	{
		for (row= 0; row<32; row++) //y
		{ 
			for (col = 0; col < 8 ;  col++) //x
			{ 
				GlcdCmd (0x80+ row) ; 					// row address 
				GlcdCmd (0x80 + col + offset); 			// column address 
				GlcdData (0); 													// write data the D15- D8 
				GlcdData (0); 													// write data D7-D0  
			} 
		}	
	}	
}

/*
NOTE: !!! :
It must be noted that pos value will be 1 to 7 and it will jump
double the value provided, e.g. val == 2 then pos on lcd = 2X2 = 4
*/
void GlcdGotoLinePos(uchar line,uchar pos)
{
	switch(line)
	{
		case 1:	GlcdCmd(LINE1 + pos); 	break;
		case 2:	GlcdCmd(LINE2 + pos); 	break;
		case 3:	GlcdCmd(LINE3 + pos); 	break;
		case 4:	GlcdCmd(LINE4 + pos); 	break;
		default:	break;
	}
}

void GlcdChar(uchar ch)
{
	GlcdData(ch);
}

void GlcdString(uchar *str)
{
	while(*str)
		{
		GlcdChar(*str++);
			_delay_ms(500);
		}	
}

/*
	x  in the range of 0 to 127
	y  in the range of 0 to 63
*/
void GlcdHLine(uchar x,uchar y,uchar len)
{
	while(len--)
	{
		GlcdShowPixel(x,y);
		x++;
	}
}

void GlcdVLine(uchar x,uchar y,uchar len)
{
	while(len--)
	{
		GlcdShowPixel(x,y);
		y++;
	}
}

/*		The Function below can draw only 45°  lines	 */
void GlcdLineCross(uchar x1,uchar y1,uchar x2,uchar y2)
{
	if(x1<x2)
	{
	GlcdShowPixel(x1,y1);		
	do
		{
			x1++;
			if(y1<y2)
					y1++;
			else if(y1>y2)
					y1--;
		}while(x1!=x2);	
			GlcdShowPixel(x1,y1);		
	}
	else if(x1>x2)
	{
	GlcdShowPixel(x1,y1);		
	do
		{
			x1--;
			if(y1>y2)
					y1--;
			else if(y1<y2)
					y1++;
			GlcdShowPixel(x1,y1);		
		}while(x1!=x2);	
	}
	else if(x1==x2)
		{
			GlcdShowPixel(x1,y1);					
			do
			{
			if(y1>y2)
					y1--;
			else if(y1<y2)
					y1++;
			GlcdShowPixel(x1,y1);					
			}while(y1!=y2);
		}			
}

/*	To Lines with all possible angle	 */
void GlcdLine(int x1,int y1,int x2,int y2)
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
	GlcdModifyPixel((int)x,(int)y,0);
	
	for(k=1;k<=step;k++)
	{
		x=x+xinc;
		y=y+yinc;
		GlcdShowPixel((int)x,(int)y);
	}
}

void GlcdRectangle(uchar x,uchar y,uchar len,uchar brdth)
{
	GlcdLine(x,y,x,y+brdth);						//right vertical
	GlcdLine(x,y,x+len,y);							//top horizontal
	GlcdLine(x,y+brdth,x+len,y+brdth);		//bottom horizontal
	GlcdLine(x+len,y,x+len,y+brdth);			//left vertical
}

void GlcdCircle(uchar centerX,uchar centerY,uchar rad)
{
signed char   x,y/*,temp[50]*/;
	uint theta;
	for (theta = 0; theta <=360 ; theta++)
	{
		x = rad * cos(TORADIAN(theta));
		y = rad * sin(TORADIAN(theta));
		if((GlcdGetPtDist(x+centerX,y+centerY,centerX,centerY) <=  rad )) 
			GlcdShowPixel(centerX+x,centerY+y);
//	sprintf(temp,"\n\rCX = %d,CY=%d,Theta = %03d x = %04d,y = %04d Dist = %04d",centerX,centerY,theta, x,y,GlcdGetPtDist(x+centerX,y+centerY,centerX,centerY));
//	u1str(temp);
	}	
}

int GlcdGetPtDist(signed char x1,signed char y1,signed char x2,signed char y2)
{
	return ((int)ceil(sqrt (((x2 - x1)  * (x2 - x1)) +  ((y2 - y1)  * (y2 - y1)))));
}

/*
	Following Function helps to display data on Glcd without reading:
	For this an array GlcdBuffer[][] is added that will represent entire data being 
	displayed on Glcd, thus when ever we display anything  on Glcd make sure
	that we reflect these changes back to this GlcdBuffer[][] array as well.
 */
 /*
	Clears entire Glcd as well as GlcdBuffer[][]
 */
 
void GlcdClearAll()
{
	uchar row,col;
	for(row=0;row<64;row++)
		for(col=0;col<16;col++)
		GlcdBuffer[row][col] = 0;	//	Clear All data in GlcdBuffer
		
	GlcdUpdateGlcd();							//Now reflect the same on Glcd
}

/*
	Update Glcd with data stored in GlcdBuffer
*/
void GlcdUpdateGlcd()
{
	uchar row,col;
	GlcdCmd (GLCD_8BIT_EXTND_GRPH_ON); 

		for (row= 0; row<32; row++) 
		{ 
			for (col = 0; col < 8 ;  col++) 
			{ 
				GlcdCmd (0x80 + row) ; 									// row address 
				GlcdCmd (0x80 + col ); 										// column address 
				GlcdData (GlcdBuffer[row][col*2]); 			// write data the D15- D8 
				GlcdData (GlcdBuffer[row][col*2+1]); 				// write data D7-D0  
			} 
		}
		for (row= 0; row<32; row++) 
		{ 
			for (col = 0; col < 8 ;  col++) 
			{ 
				GlcdCmd (0x80 + row) ; 								// row address 
				GlcdCmd (0x88 + col ); 		// column address 
				GlcdData (GlcdBuffer[row+32][col*2]); 						// write data the D15- D8 
				GlcdData (GlcdBuffer[row+32][col*2+1]); 				// write data D7-D0  
			} 
		}	
}

void GlcdShowPixel(uchar x,uchar y)
{
	uchar row,col,pthigh,ptlow,shftpos,temp1,temp2;
	uint  shftval;
	col = x / 16;
	GlcdCmd (GLCD_8BIT_EXTND_GRPH_ON); 
	shftpos =15 - ( x % 16)  ;
	shftval = (1 << shftpos);
	pthigh =   (uchar) (shftval >> 8);
	ptlow = (uchar)  (shftval & 0xff);	
	
	temp1 = GlcdBuffer[y][(x/16) * 2];
	temp2 = GlcdBuffer[y][(x/16) * 2 +1];

	temp1 |=  pthigh;
	temp2 |=  ptlow;
	
	GlcdBuffer[y][(x/16) * 2] = temp1 ;
	GlcdBuffer[y][(x/16) * 2+1] = temp2 ;
	
	if(y < 32)
	{
		row = y  ;										// y
		GlcdCmd (0x80 + row) ; // row address 
		GlcdCmd (0x80 + col ); 	// column address 
	}
	 else if(y >= 32)
	{
		row = y - 32  ;							// y
		GlcdCmd (0x80 + row) ; // row address 
		GlcdCmd (0x88 + col ); 	// column address 
	}
	GlcdData (temp1); 				// write data the D15- D8 
	GlcdData (temp2); 				// write data D7-D0  
}

void GlcdClearPixel(uchar x,uchar y)
{
	uchar row,col,pthigh,ptlow,shftpos,temp1,temp2;
	uint  shftval;
	col = x / 16;
	GlcdCmd (GLCD_8BIT_EXTND_GRPH_ON); 
	shftpos =15 - ( x % 16)  ;
	shftval = ~(1 << shftpos);
	pthigh =   (uchar) (shftval >> 8);
	ptlow = (uchar)  (shftval & 0xff);	
	
	temp1 = GlcdBuffer[y][(x/16) * 2];
	temp2 = GlcdBuffer[y][(x/16) * 2 +1];

	temp1 &=  pthigh;
	temp2 &=  ptlow;
	
	GlcdBuffer[y][(x/16) * 2] = temp1 ;
	GlcdBuffer[y][(x/16) * 2+1] = temp2 ;
	
	if(y < 32)
	{
		row = y  ;										// y
		GlcdCmd (0x80 + row) ; // row address 
		GlcdCmd (0x80 + col ); 	// column address 
	}
	 else if(y >= 32)
	{
		row = y - 32  ;							// y
		GlcdCmd (0x80 + row) ; // row address 
		GlcdCmd (0x88 + col ); 	// column address 
	}
	GlcdData (temp1); 				// write data the D15- D8 
	GlcdData (temp2); 				// write data D7-D0  
}

void GlcdTogglePixel(uchar x,uchar y)
{
	uchar row,col,pthigh,ptlow,shftpos,temp1,temp2;
	uint  shftval;
	col = x / 16;
	GlcdCmd (GLCD_8BIT_EXTND_GRPH_ON); 
	shftpos =15 - ( x % 16)  ;
	shftval = (1 << shftpos);
	pthigh =   (uchar) (shftval >> 8);
	ptlow = (uchar)  (shftval & 0xff);	
	
	temp1 = GlcdBuffer[y][(x/16) * 2];
	temp2 = GlcdBuffer[y][(x/16) * 2 +1];

	temp1 ^=  pthigh;
	temp2 ^=  ptlow;
	
	GlcdBuffer[y][(x/16) * 2] = temp1 ;
	GlcdBuffer[y][(x/16) * 2+1] = temp2 ;
	
	if(y < 32)
	{
		row = y  ;										// y
		GlcdCmd (0x80 + row) ; // row address 
		GlcdCmd (0x80 + col ); 	// column address 
	}
	 else if(y >= 32)
	{
		row = y - 32  ;							// y
		GlcdCmd (0x80 + row) ; // row address 
		GlcdCmd (0x88 + col ); 	// column address 
	}
	GlcdData (temp1); 				// write data the D15- D8 
	GlcdData (temp2); 				// write data D7-D0  
}

// mode = 0 for negative image,mode =1 for normal image
void GlcdShowImage(uchar * img,uchar mode)
{
	uchar row,col;
	GlcdCmd (GLCD_8BIT_EXTND_GRPH_OFF); 
	uchar *ptr;
	ptr = img;//ptr also points to image array
		for (row= 0; row<32; row++) 
		{ 
			for (col = 0; col < 8 ;  col++) 
			{ 
				GlcdCmd (0x80 + row) ; 								// row address 
				GlcdCmd (0x80 + col ); 		// column address 
					if(mode == 1)
					{
						GlcdData (* img++); 										// write data the D15- D8 
						GlcdData (* img++); 										// write data D7-D0  
					}
					else if(mode == 0)
					{
						GlcdData (~(* img++)); 										// write data the D15- D8 
						GlcdData (~(* img++)); 										// write data D7-D0  
					}
			} 
		}	

		for (row= 0; row<32; row++) 
		{ 
			for (col = 0; col < 8 ;  col++) 
			{ 
				GlcdCmd (0x80 + row) ; 								// row address 
				GlcdCmd (0x88 + col ); 		// column address 
					if(mode == 1)
					{
						GlcdData (* img++); 										// write data the D15- D8 
						GlcdData (* img++); 										// write data D7-D0  
					}
					else if(mode == 0)
					{
						GlcdData (~(* img++)); 										// write data the D15- D8 
						GlcdData (~(* img++)); 										// write data D7-D0  
					}
			} 
		}	
	GlcdCmd (GLCD_8BIT_EXTND_GRPH_ON); 
		
		for (row= 0; row<32; row++)  
			for (col = 0; col < 8 ;  col++) 
			{
				if(mode == 1)
					GlcdBuffer[row][col] = *img++;
				else if(mode == 0)
					GlcdBuffer[row][col] =~(*img++);
			}	

}

/*
	This is  a function to display small image on lcd
*/
void GlcdPutImage(uchar *img,uchar x,uchar y,uchar bytesinline,uchar lines)
{
	uchar line,byte,bit,temp;
	GlcdCmd (GLCD_8BIT_EXTND_GRPH_ON); 

	for(line = 0; line< lines ; line++)
		for(byte = 0 ; byte < bytesinline; byte++ )
			{
				temp = img[byte+bytesinline*line];
				for(bit = 0 ; bit < 8; bit++ )
					{
							if((  temp & 0x80) )//check msb
								GlcdShowPixel(x+byte*8+bit,y+line);
							else
								GlcdClearPixel(x+byte*8+bit,y+line);
							temp = temp<<1;
					}
			}	
	}

/*	Time Period is in no. of Pixels	*/
void GlcdSin(uchar center,uchar amplitude,uchar  timeperiod,uchar noofcycles,uchar phase)
{
	uchar x,y;
	uint theta;
	for(theta=phase;theta<(360*noofcycles+phase);theta++)
	{
		y = center - amplitude*sin(TORADIAN(theta));//-ve bcoz in GLCD we have 4th quadrant(image of 1st) and not 1st 
		x=(uchar)((int)theta/((float)360/timeperiod)) - (uchar)(phase/((float)360/timeperiod));
		GlcdShowPixel(x,y);
	}
}

void GlcdCos(uchar center,uchar amplitude,uchar  timeperiod,uchar noofcycles,uchar phase)
{
	uchar x,y;
	uint theta;
	for(theta=phase;theta<(360*noofcycles+phase);theta++)
	{
		y = center - amplitude*cos(TORADIAN(theta));//-ve bcoz in GLCD we have 4th quadrant(image of 1st) and not 1st 
		x=(uchar)((int)theta/((float)360/timeperiod)) - (uchar)(phase/((float)360/timeperiod));
		GlcdShowPixel(x,y);
	}
}

/*
Rose:
r = cos(k*theta)
k = no of leaves in rose curve
if k == odd  then same no of leaves are produced
		k == even then 2 * k of leaves are produced
also equation can be written as 		
r = shrink+ cos(k*theta)
this will increase/shrink the size of alternate leaves if k = even
or	will  increase/shrinke the size of leaves at same place if  k = odd
NOTE : keep shrink  0 here.
*/
void GlcdRose(uchar centerX,uchar centerY,uchar noofleaves,uchar length,uchar shrink)
{
	float r,x,y;
	uint theta;
	for(theta=0;theta<=360;theta++)
	{
		r = shrink + length * cos(noofleaves * TORADIAN(theta));
		x =centerX + r*cos(TORADIAN(theta));
		y = centerY + r*sin(TORADIAN(theta));
		GlcdShowPixel(x,y);
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

***************************************************************
*/
void GlcdDeltoid(uchar centerX,uchar centerY,uchar radinnrcircle)
{
	uint theta;
	char x,y;
	for(theta = 0; theta <=360; theta++)
	{
		x = centerX + radinnrcircle*(2 * cos (TORADIAN(theta)) + cos (TORADIAN(2*theta))); 
		y = centerY + radinnrcircle*(2 * sin (TORADIAN(theta)) - sin (TORADIAN(2*theta))) ;
		GlcdShowPixel(x,y);
	}
}

/*
**************************************************************
Cardoid:
Equations obtained from Wikipedia... :)
a = radius of OUTER rolling cirlce

x = a(2cos (t) +  cos (2t))
y = a(2sin (t) + sin (2t)) 
OR ... to change orientation by 180° just change +ve and -ve signs as follows
x = a(2cos (t) -  cos (2t))
y = a(2sin (t) - sin (2t)) 

**************************************************************
*/
void GlcdCardoid(uchar centerX,uchar centerY,uchar radoutercircle)
{
	uint theta;
	char x,y;
	for(theta = 0; theta <=360; theta++)
	{
		x = centerX + radoutercircle*(2 * cos (TORADIAN(theta)) + cos (TORADIAN(2*theta))); 
		y = centerY + radoutercircle*(2 * sin (TORADIAN(theta)) + sin (TORADIAN(2*theta))) ;
		GlcdShowPixel(x,y);
	}
}

/*
***********************************************
Astroid:
A circle of radius 1/4 rolls around inside a circle of radius 1 
and a point on its circumference traces an astroid.

Equations obtained from Wikipedia... :)
x^(2/3) + y^(2/3) = 1
in parametric  form...
x =cos^3 (theta)  (i.e cos cube theta)
y =sin^3 (theta)   (i.e sin cube theta)

***********************************************
*/
void GlcdAstroid(uchar centerX,uchar centerY,uchar radoutercircle)
{
	uint theta;
	char x,y;
	for(theta = 0; theta <=360; theta++)
	{
		x = centerY + radoutercircle*(cos (TORADIAN(theta)) * cos (TORADIAN(theta)) *  cos (TORADIAN(theta))); 
		y = centerY + radoutercircle*(sin (TORADIAN(theta)) * sin (TORADIAN(theta)) *  sin (TORADIAN(theta))); 
		GlcdShowPixel(x,y);
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
void GlcdHypoCycloid(uchar centerX,uchar centerY,float radinnrcircle,float noofcorners)
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
		GlcdShowPixel(x,y);
	}
}

/*
Archemedean Spiral :
r = a + b *theta
a = a turns the spiral (start)
b = distance between successive turnings. (space)
theta in radians
*/

void GlcdArchemedeanSpiral(uchar centerX,uchar centerY,uchar start,uchar space,float noofcycles)
{
	float r,x,y,shrink=0;
	uint theta;
	for(theta=0;theta<=360 * noofcycles;theta++)
	{
		r = start + space * (TORADIAN(theta));
		x = centerX + r*cos(TORADIAN(theta));
		y = centerY + r*sin(TORADIAN(theta));
		GlcdShowPixel(x,y);
	}
}
/*
Involute of a  cirlce:
x = a(cos theta  + theta *  sin theta)
y = a(sin theta  - theta  * cos theta)
a = radius of the circle around which we draw the curve (radius)
*/
void GlcdInvoluteofCircle(uchar centerX,uchar centerY,float radius,float noofcycles)
{
	float x,y;
	uint theta;
	for(theta=0;theta<=360 * noofcycles;theta++)
	{
		x = centerX + radius*(cos(TORADIAN(theta)) + (TORADIAN(theta)) * sin(TORADIAN(theta)));
		y = centerY + radius*(sin(TORADIAN(theta)) -(TORADIAN(theta))* cos(TORADIAN(theta)));
		GlcdShowPixel(x,y);
	}
}

/*
Logarithmic spiral:
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

void GlcdLogarithmicSpiral(uchar centerX,uchar centerY,float start,float space,float noofcycles)
{
	float r,x,y,e = 2.71828;
	uint theta; 
	for(theta=0;theta<=360 * noofcycles;theta++)
	{
		r = start * pow(e,space * (TORADIAN(theta)));
		x = centerX + r*cos(TORADIAN(theta));
		y = centerY + r*sin(TORADIAN(theta));
		GlcdShowPixel(x,y);
	}
}

/*
Added on 19/09/2012 Wednesday

*/
void  GlcdQuadrilateral(uchar x1,uchar y1,uchar x2,uchar y2,uchar x3,uchar y3,uchar x4,uchar y4)
{
	GlcdLine(x1, y1, x2, y2);
	GlcdLine(x1, y1, x3, y3);
	GlcdLine(x4, y4, x2, y2);
	GlcdLine(x4, y4, x3, y3);
}
/* 
Following functionalities added on 15/09/12 Saturday
*/

void GlcdInvertDisplay()
{
	 	uchar row,col;
	for(row=0;row<64;row++)
		for(col=0;col<16;col++)
		GlcdBuffer[row][col] = ~GlcdBuffer[row][col];	//	Invert sAll data in GlcdBuffer
		
	GlcdUpdateGlcd();							//Now reflect the same on Glcd
}


void GlcdCGRAMStore(uchar *data,uchar location,uchar mode)
{
	uchar pos,row,col;
	GlcdCmd(GLCD_EN_CGRAM);
	GlcdCmd(GLCD_CGRAM_ADDRS +  location * 16);
	_delay_us(1000);
	for(pos=0;pos<32;pos++)
	{
		if (mode == 1)
			GlcdData(*data++);	
		else if (mode == 0)
			GlcdData(~(*data++));	
	}		

	for (row= 0; row<32; row++)  
		for (col = 0; col < 8 ;  col++) 
		{
			if(mode == 1)
				GlcdBuffer[row][col] = *data++;
			else if(mode == 0)
				GlcdBuffer[row][col] =~(*data++);
		}
}

void GlcdCGRAMShow(uchar line,uchar pos,uchar location)
{
	GlcdGotoLinePos(line,pos);
	GlcdData(0);
	GlcdData(location * 2); // Valid locations are 0,2,4 and 6
}

/*
	Yet to finish !!!
	ICON RAM
	pos : 0 to 14
*/

void GlcdICONRAM(uchar pos,uchar data)
{
	GlcdCmd(GLCD_EN_IRAM);
	GlcdCmd(GLCD_SET_IRAM+pos);
		GlcdData(data);
}
/*
	This finction maps coordinstes of some systems into 
	Glcd coordinates with following values
	MAX_X
	MAX_Y
	MIN_X
	MIN_Y
*/

uchar GlcdMapData(uint start,uchar end,uchar val,uchar glcdstart,uchar glcdend)
{
	int diff1,diff2,i;
	float divfact,finalval=0;    

	diff1 =  abs(start - end);   
	diff2 =  abs(glcdstart - glcdend);     
	divfact = (float)diff1/diff2;
	finalval=(float)(val - start)/ divfact  + glcdstart;  
	return (uchar)finalval;
}
#endif