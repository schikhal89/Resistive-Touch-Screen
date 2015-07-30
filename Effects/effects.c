#include <stdio.h>
#include <i2c.h>
#include <util/delay.h>
#include <uart.h>
#include <avr/pgmspace.h>
#include "glcd.h"
#include "img.h"
#include "effects.h"

void main(void)
{
	uchar buff[20] = {'0'};
	u1init();
	GLcdinit();
	while(1)
	{		

		GlcdShowImage2_P(paint1,1);
		_delay_ms(500);
		GlcdWipeDown(15);
        
	    GlcdShowImage2_P(paint1,1);        
		_delay_ms(500);
		GlcdBlackDown(15);

	    GlcdShowImage2_P(paint1,1);        		
		_delay_ms(500);
		GlcdBlackUp(15);

		GlcdShowImage2_P(paint1,1);
		_delay_ms(500);
        GlcdWipeUp(10);

		GlcdShowImage2_P(paint1,1);        
		_delay_ms(500);
        GlcdWipeRight(10);

	    GlcdShowImage2_P(paint1,1);        
		_delay_ms(500);
	    GlcdBlackRight(10) ;
		
        GlcdShowImage2_P(paint1,1);        
		_delay_ms(500);
        GlcdWipeLeft(5);    

	    GlcdShowImage2_P(paint1,1);        
		_delay_ms(500);
		GlcdBlackLeft(10);
		
	    GlcdShowImage2_P(paint1,1);        
		_delay_ms(500);
		GlcdBlindsHoriUpDown(10,0);

	    GlcdShowImage2_P(paint1,1);        
		_delay_ms(500);
		GlcdBlindsHoriUpDown(10,1);
		
	    GlcdShowImage2_P(paint1,1);        
		_delay_ms(500);
		GlcdBlindsVertRtLft(5,2);

	    GlcdShowImage2_P(paint1,1);        
		_delay_ms(500);
		GlcdBlindsVertRtLft(5,3);

	    GlcdShowImage2_P(paint1,1);        
		GlcdCheckerHori(10);

	    GlcdShowImage2_P(paint1,1);        
		_delay_ms(500);
		GlcdCheckerVert(50);

	    GlcdShowImage2_P(paint1,1);        
		_delay_ms(500);
		GlcdRandBarHori(5);
		
	    GlcdShowImage2_P(paint1,1);        
		GlcdRandBarVert(5);

		GlcdShowImage2_P(paint1,1);        
		_delay_ms(500);
		GlcdBoxInOutBlack(60,0,0);
		
		GlcdShowImage2_P(paint1,1);        
		_delay_ms(500);
		GlcdBoxInOutBlack(60,1,0);
		
		GlcdShowImage2_P(paint1,1);        
		_delay_ms(500);
		GlcdBoxInOutBlack(60,0,1);
		
		GlcdShowImage2_P(paint1,1);        
		_delay_ms(500);
		GlcdBoxInOutBlack(60,1,1);

		GlcdShowImage2_P(paint1,1);        
		_delay_ms(500);
		GlcdBoxIn(40);

		GlcdShowImage2_P(paint1,1);        
		_delay_ms(500);
		GlcdBoxOut(40);

		GlcdShowImage2_P(paint1,1);        
		_delay_ms(500);
		SplitHoriInOut(40,0);	

		GlcdShowImage2_P(paint1,1);        
		_delay_ms(500);
		SplitHoriInOut(40,1);	

		GlcdShowImage2_P(paint1,1);        		
		_delay_ms(500);
		SplitVertInOut(40,0);

		GlcdShowImage2_P(paint1,1);        		
		_delay_ms(500);
		SplitVertInOut(40,1);

		/*
		sprintf(buff,"\n\r %d ",sizeof(unsigned long long int));
		u1str(buff);
		*/
//		_delay_ms(500);
	}
}