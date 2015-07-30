#ifndef	  _EFFECTS_H
#define 	  _EFFECTS_H

#include <avr/io.h>
#include <util/delay.h>
#include  <math.h>
#include <string.h>

#define UPDIR		0
#define DNDIR		1
#define RTDIR		2
#define LTDIR		3
#define INDIR		0
#define OUTDIR	1
	
static volatile uchar GlcdEffects[8][128];// To store Effects

void GlcdCopyBuffer(void)
{
	uchar row,col;
/*Copy Entire Buffer into an array*/	
	for( col = 0; col < 128; col++)
		for(row = 0; row < 8 ; row++)
			GlcdEffects[row][col] = GlcdBuffer[row][col] ;
}

void GlcdClearEffectBuff(void)
{
	uchar row,col;
/*Copy Entire Buffer into an array*/	
	for( col = 0; col < 128; col++)
		for(row = 0; row < 8 ; row++)
			GlcdEffects[row][col] = 0;
}

void GlcdFillBuffer(unsigned char val)
{
	uchar row,col;
	for( col = 0; col < 128; col++)
		for(row = 0; row < 8 ; row++)
			 GlcdBuffer[row][col] = val;
}

void GlcdWipeUp(float dly)
{
	signed int temp,val=0,row,col;
	for(row = 7; row >=0 ; row--)
	{
		for(temp = 0; temp < 8 ; temp++)
		{
			val |= 0x80;
			for( col = 0; col < 128; col++)
				GlcdBuffer[row][col] = val ;
			GlcdUpdateBufferOnLCD();
			_delay_ms(dly);
			val >>= 1;
		}	
		val = 0;
	}
}

void GlcdWipeDown(float dly)
{
	uint temp,val=0,row,col;
	for(row = 0; row < 8 ; row++)
	{
		for(temp = 0; temp < 8 ; temp++)
		{
			val |= 0x01;
			for( col = 0; col < 128; col++)
				GlcdBuffer[row][col] = val;
			GlcdUpdateBufferOnLCD();
			_delay_ms(dly);
			val <<= 1;
		}	
		val = 0;
	}
}


void GlcdWipeRight(float dly)
{
	uint row,col;
	for( col = 0; col < 128; col++)
	{
		for(row = 0; row < 8 ; row++)
		{
			GlcdBuffer[row][col] = 0xFF;
		}
		GlcdUpdateBufferOnLCD();
	_delay_ms(dly);		
	}	
}

void GlcdWipeLeft(float dly)
{
	signed int row,col;
	for( col = 127; col >= 0; col--)
	{
		for(row = 0; row < 8 ; row++)
		{
			GlcdBuffer[row][col] = 0xFF;
		}
		GlcdUpdateBufferOnLCD();
	_delay_ms(dly);		
	}	
}

void GlcdBlackRight(float dly) 
{
	signed int row,col;
/*Copy Buffer to Effect buffer to save image hold by buffer	*/	
	GlcdCopyBuffer();
/*	Now fill buffer with oxff and update it on Gcld*/			
	GlcdFillBuffer(0xFF);		 
	GlcdUpdateBufferOnLCD();
		
	for( col = 0; col < 128; col++)
	{
		for(row = 0; row < 8 ; row++)
				GlcdBuffer[row][col] = GlcdEffects[row][col] ;			
		GlcdUpdateBufferOnLCD();
			_delay_ms(dly);		
	}
}

void GlcdBlackLeft(float dly) 
{
	signed int row,col;
/*Copy Buffer to Effect buffer to save image hold by buffer	*/	
	GlcdCopyBuffer();
/*	Now fill buffer with oxff and update it on Gcld*/			
	GlcdFillBuffer(0xFF);		 
	GlcdUpdateBufferOnLCD();
			 		
	for( col = 127; col >= 0; col--)
	{
		for(row = 0; row < 8 ; row++)
				GlcdBuffer[row][col] = GlcdEffects[row][col] ;			
		GlcdUpdateBufferOnLCD();
		_delay_ms(dly);			
	}
}

void GlcdBlackDown(float dly)
{
	signed int row,col;
	uchar val;
/*Copy Buffer to Effect buffer to save image hold by buffer	*/	
	GlcdCopyBuffer();
/*	Now fill buffer with oxff and update it on Gcld*/			
	GlcdFillBuffer(0xFF);		 
	GlcdUpdateBufferOnLCD();

	for(row = 0; row < 8; row++)	
	{
		for(val = 0; val < 8; val++)	
		{
			for(col = 0; col < 128; col++)	
			{
				if(GlcdEffects[row][col] & (0x01<<val))
					GlcdBuffer[row][col] |= 0x01 << val;
				else 	
					GlcdBuffer[row][col] &= ~(0x01 << val);
			}
		GlcdUpdateBufferOnLCD();	
		_delay_ms(dly);		
		}	
	}
}

void GlcdBlackUp(float dly)
{
	signed int row,col;
	uchar val;
/*Copy Buffer to Effect buffer to save image hold by buffer	*/	
	GlcdCopyBuffer();
/*	Now fill buffer with oxff and update it on Gcld*/			
	GlcdFillBuffer(0xFF);		 
	GlcdUpdateBufferOnLCD();

	for(row = 7; row >= 0; row--)	
	{
		for(val = 0; val < 8; val++)	
		{
			for(col = 0; col < 128; col++)	
			{
				if(GlcdEffects[row][col] & (0x80>>val))
					GlcdBuffer[row][col] |= 0x80 >> val;
				else 	
					GlcdBuffer[row][col] &= ~(0x80 >> val);
			}
		GlcdUpdateBufferOnLCD();	
		_delay_ms(dly);		
		}	
	}
}

void GlcdBlindsHoriUpDown(float dly,uchar dir)
{
	signed int row,col,val;
/*Copy Buffer to Effect buffer to save image hold by buffer	*/	
	GlcdCopyBuffer();
/*	Now fill buffer with oxff and update it on Gcld*/			
	GlcdFillBuffer(0xFF);		 
	GlcdUpdateBufferOnLCD();

	for(val = 0; val < 8; val++)	
	{
		for(row = 0; row < 8 ; row++)
		{
			for( col = 0; col < 128; col++)
			{
				if(dir == UPDIR)
				{
					if(GlcdEffects[row][col] & (0x80>>val))
						GlcdBuffer[row][col] |= 0x80 >> val;
					else 	
						GlcdBuffer[row][col] &= ~(0x80 >> val);
				}
				else if (dir ==DNDIR)
				{
					if(GlcdEffects[row][col] & (0x01<<val))
						GlcdBuffer[row][col] |= 0x01 << val;
					else 	
						GlcdBuffer[row][col] &= ~(0x01 << val);
				}
			}
			GlcdUpdateBufferOnLCD();
			_delay_ms(dly);			
		}
	}
}

void GlcdBlindsVertRtLft(float dly,uchar dir)
{
	signed char row,col,coljump;
/*Copy Buffer to Effect buffer to save image hold by buffer	*/	
	GlcdCopyBuffer();
/*	Now fill buffer with oxff and update it on Gcld*/			
	GlcdFillBuffer(0xFF);		 
	GlcdUpdateBufferOnLCD();
	
	for(coljump = 0 ; coljump < 16 ; coljump++)
	{
	for( col = 0; col < 8; col++)		
		{
			for(row = 0; row < 8 ; row++)
			{
				if(dir == RTDIR)
					GlcdBuffer[row][col*16+coljump] = GlcdEffects[row][col*16+coljump] ;	
				else if(dir == LTDIR)
					GlcdBuffer[row][col*16+(15-coljump)] = GlcdEffects[row][col*16+(15-coljump)] ;	
			}
			GlcdUpdateBufferOnLCD();
				_delay_ms(dly);	
		}	
	}	
}

void GlcdCheckerHori(float dly)
{
	signed char row,col,coljump;
/*Copy Buffer to Effect buffer to save image hold by buffer	*/	
	GlcdCopyBuffer();
/*	Now fill buffer with oxff and update it on Gcld*/			
	GlcdFillBuffer(0xFF);		 
	GlcdUpdateBufferOnLCD();
	
	for(coljump = 0 ; coljump < 16 ; coljump++)
	{
		for( col = 0; col < 8; col++)		
			{
				for(row = 0; row < 8 ; row++)
				{
					if(row % 2)
						GlcdBuffer[row][col*16+coljump] = GlcdEffects[row][col*16+coljump] ;	
					else 
						GlcdBuffer[row][col*16+(15-coljump)] = GlcdEffects[row][col*16+(15-coljump)] ;	
				}
				GlcdUpdateBufferOnLCD();
					_delay_ms(dly);	
			}	
	}		
}

void GlcdCheckerVert(float dly)
{
	signed int row,col,val;
	uchar flag = 0;
/*Copy Buffer to Effect buffer to save image hold by buffer	*/	
	GlcdCopyBuffer();
/*	Now fill buffer with oxff and update it on Gcld*/			
	GlcdFillBuffer(0xFF);		 
	GlcdUpdateBufferOnLCD();

	for(val = 0; val < 8; val++)	
	{
		for(row = 0; row < 8 ; row++)
		{
			for( col = 0; col < 128; col++)
			{
				if(flag)
				{
					if(GlcdEffects[row][col] & (0x80>>val))
						GlcdBuffer[row][col] |= 0x80 >> val;
					else 	
						GlcdBuffer[row][col] &= ~(0x80 >> val);
				}
				else 
				{
					if(GlcdEffects[row][col] & (0x01<<val))
						GlcdBuffer[row][col] |= 0x01 << val;
					else 	
						GlcdBuffer[row][col] &= ~(0x01 << val);
				}
				if(( col != 0 ) && (col % 8 == 0))
					flag = ! flag;
			}
		GlcdUpdateBufferOnLCD();
		}
	_delay_ms(dly);			
	}
}

void GlcdRandBarHori(float dly)
{
	unsigned int row,val = 0,rand ;
	unsigned int bitval[8] = {0};
/*	Copy Buffer to Effect buffer to save image hold by buffer	*/	
	GlcdCopyBuffer();
/*	Now fill buffer with oxff and update it on Gcld						*/			
	GlcdFillBuffer(0xFF);		 
	GlcdUpdateBufferOnLCD();

	while(val != 128)
	{		
		rand = GetRandom(0,128);
		if(! (bitval[rand/16]  & (unsigned int )(1<<(rand%16))))
		{
			for(row = 0; row<8; row++)
				GlcdBuffer[row][rand] = GlcdEffects[row][rand] ;		
			bitval[rand/16]  |= (unsigned int )(1<<(rand%16));
			val++;
		}		
		GlcdUpdateBufferOnLCD();
		_delay_ms(dly);		
	}		
}

void GlcdRandBarVert(float dly)
{
	unsigned int col,rand ,val=0;
	unsigned int bitval[4] = {0};
/*	Copy Buffer to Effect buffer to save image hold by buffer	*/	
	GlcdCopyBuffer();
/*	Now fill buffer with oxff and update it on Gcld						*/			
	GlcdFillBuffer(0xFF);		 
	GlcdUpdateBufferOnLCD();

	while(val != 64)
	{		
		rand = GetRandom(0,64);
		if(! (bitval[rand/16]  & (unsigned int )(1<<(rand%16))))
		{			
			for(col = 0; col < 128; col++)	
			{
				if(GlcdEffects[rand/8][col] & (0x01<<(rand%8)))
					GlcdBuffer[rand/8][col] |= 0x01 << (rand%8);
				else 	
					GlcdBuffer[rand/8][col] &= ~(0x01 << (rand%8));
			}		
			bitval[rand/16]  |= (unsigned int )(1<<(rand%16));
			val++;
		}		
		GlcdUpdateBufferOnLCD();
		_delay_ms(dly);		
	}		
}

void GlcdBoxInOutBlack(float dly,uchar dir,uchar changetype)
{
	signed char temp,l,b;
	if(dir == INDIR)
	{
		l = 127 ; b = 63;
		for(temp = 0; temp<32; temp++)
		{
			GlcdRectangle(temp,temp,l,b,changetype);
			l -= 2;
			b -= 2;
			_delay_ms(dly);
		}
	}
	else if(dir == OUTDIR)
	{
		l = 63 ; b = 0;
		for(temp = 31; temp>=0; temp--)
		{
			GlcdRectangle(temp,temp,l,b,changetype);
			l += 2;
			b += 2;
			_delay_ms(dly);
		}
	}
}

void GlcdBoxIn(float dly)
{
	signed int colcnt,row,col ;
/*	Copy Buffer to Effect buffer to save image hold by buffer	*/	
	GlcdCopyBuffer();
/*	Now fill buffer with oxff and update it on Gcld						*/			
	GlcdFillBuffer(0xFF);		 
	GlcdUpdateBufferOnLCD();

		for(colcnt = 0; colcnt <= 63; colcnt++)		
		{
			for(col= 0; col <= 127;col++)
			{
				if(GlcdEffects[colcnt/8][col] & (1<<(colcnt%8)))
					GlcdBuffer[colcnt/8][col]  |=  (1<<(colcnt%8));
				else	
					GlcdBuffer[colcnt/8][col]  &= ~(1<<(colcnt%8));
					
				if(GlcdEffects[7 - (colcnt/8)][col] & (1<<(7-(colcnt%8))))
					GlcdBuffer[7 - (colcnt/8)][col]  |=  (1<<(7-(colcnt%8)));
				else	
					GlcdBuffer[7 - (colcnt/8)][col]  &= ~(1<<(7-(colcnt%8)));	
			}					
			for(row = 0; row < 8 ; row++)
			{
				GlcdBuffer[row][colcnt]  = GlcdEffects[row][colcnt];
				GlcdBuffer[row][127-colcnt]  = GlcdEffects[row][127-colcnt];
			}
			GlcdUpdateBufferOnLCD();
			_delay_ms(dly);
		}	
}

void GlcdBoxOut(float dly)
{
	signed int col,temp,val ;
/*	Copy Buffer to Effect buffer to save image hold by buffer	*/	
	GlcdCopyBuffer();
/*	Now fill buffer with oxff and update it on Gcld						*/			
	GlcdFillBuffer(0xFF);		 
	GlcdUpdateBufferOnLCD();

	for(temp = 31; temp >= 0; temp--)
	{	
		for(col = temp; col <= 96+(31-temp); col ++)
		{
			for(val = 31- temp; val >= 0; val--)
			{
				if(GlcdEffects[3-val/8][col] & (1<<(7-(val%8))))
					GlcdBuffer[3-val/8][col] |= (1<<(7-(val%8)));
				else 
					GlcdBuffer[3-val/8][col] &= ~(1<<(7-(val%8)));	

				if(GlcdEffects[4+(val/8)][col] & (1<<((val%8))))
					GlcdBuffer[4+(val/8)][col] |= (1<<((val%8)));
				else 
					GlcdBuffer[4+(val/8)][col] &= ~(1<<((val%8)));	
			}		
		}
			GlcdUpdateBufferOnLCD();
			_delay_ms(dly);
	}
}

void SplitHoriInOut(float dly,uchar dir)
{
signed char 	row,col;
	GlcdClearEffectBuff();
if(dir == INDIR)
	for(col = 63; col> 0; col--)		
	{			
		for(row = 7; row >= 0 ; row--)
		{
			GlcdBuffer[row][col]  = GlcdEffects[row][col];
			GlcdBuffer[row][127-col]  = GlcdEffects[row][127-col];
		}
		GlcdUpdateBufferOnLCD();
		_delay_ms(dly);
	}
else if (dir ==OUTDIR)		
	for(col = 0; col <= 63; col++)		
	{			
		for(row = 7; row >= 0 ; row--)
		{
			GlcdBuffer[row][col]  = GlcdEffects[row][col];
			GlcdBuffer[row][127-col]  = GlcdEffects[row][127-col];
		}
		GlcdUpdateBufferOnLCD();
		_delay_ms(dly);
	}	
}

void SplitVertInOut(float dly,uchar dir)
{
	signed int row,col,pixel;
	GlcdClearEffectBuff();
	if(dir == INDIR)
	{
		for(row = 0; row <=3; row++)
		{
			for(pixel = 0; pixel <= 7; pixel++)
			{
				 for(col = 0; col <= 127; col++ )
				 {
					if(GlcdEffects[row][col] & (1<<(pixel)))
						GlcdBuffer[row][col] |= (1<<(pixel));
					else 
						GlcdBuffer[row][col] &= ~(1<<(pixel));	

					if(GlcdEffects[7-row][col] & (1<<(7-pixel)))
						GlcdBuffer[7-row][col] |= (1<<((7-pixel)));
					else 
						GlcdBuffer[7-row][col] &= ~(1<<(7-pixel));						
				 }
					GlcdUpdateBufferOnLCD();
					_delay_ms(dly);				 
			}
		}
	}

	else if(dir == OUTDIR)
	{
		for(row = 3; row >=0; row--)
		{
			for(pixel = 0; pixel <= 7; pixel++)
			{
				 for(col = 0; col <= 127; col++ )
				 {
					if(GlcdEffects[row][col] & (1<<(7-pixel)))
						GlcdBuffer[row][col] |= (1<<(7-pixel));
					else 
						GlcdBuffer[row][col] &= ~(1<<(7-pixel));	

					if(GlcdEffects[7-row][col] & (1<<pixel))
						GlcdBuffer[7-row][col] |= (1<<pixel);
					else 
						GlcdBuffer[7-row][col] &= ~(1<<pixel);						
				 }
					GlcdUpdateBufferOnLCD();
					_delay_ms(dly);				 
			}
		}
	}

}

#endif



