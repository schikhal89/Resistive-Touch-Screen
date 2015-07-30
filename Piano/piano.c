#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <uart.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "glcd.h"
//#include "img.h"

#define ADC_CHNL0  0
#define ADC_CHNL1  1
#define ADC_CHNL2  2
#define ADC_CHNL3  3

#define XTHRE	1000
#define YTHRE	1000

#define MULFLAG 1
#define DIVFLAG  0


#define BUZPIN  3
#define BUZZERON 		(PORTA |= (1<<BUZPIN));
#define BUZZEROFF 	(PORTA &= ~(1<<BUZPIN));

#define OCTAVE12	(float)1.0594630943592952645618252949461
#define OCTAVE8	(float)1.0905077326652576592070106557607

#define C( x )        			( x * pow(OCTAVE12,0); ) 
#define C_SHARP( x )  	( x * pow(OCTAVE12,1); )  
//#define D_FLAT( x )   		( x * pow(OCTAVE12,2); )  
#define D( x )        			( x * pow(OCTAVE12,2); )  
//#define D_SHARP( x )  	( x * pow(OCTAVE12,4); )  
#define E_FLAT( x )   		( x * pow(OCTAVE12,3); )  
#define E( x )        			( x * pow(OCTAVE12,4); )  
#define F( x )        			( x * pow(OCTAVE12,5); )  
#define F_SHARP( x )  	( x * pow(OCTAVE12,6); )  
//#define G_FLAT( x )   		( x * pow(OCTAVE12,9); )  
#define G( x )        			( x * pow(OCTAVE12,7); )  
//#define G_SHARP( x )  	( x * pow(OCTAVE12,11); ) 
#define A_FLAT( x )   		( x * pow(OCTAVE12,8); )  
#define A( x )        			( x * pow(OCTAVE12,9); ) 
//#define A_SHARP( x )  	( x * pow(0,OCTAVE12); )  
#define B_FLAT( x )   		( x * pow(OCTAVE12,10); )  
#define B( x )        			( x * pow(OCTAVE12,11); )  

volatile  unsigned long int secdelay;
uchar ButtonTouched(uchar topx,uchar topy,uchar width,uchar height,uchar x,uchar y);
int ADCData(uchar chnl);

void main(void)
{
	//float freq = 1.147127999677373282276496147931;
	//float freq = 3.2445679498433213947065764761336;
//	float freq = 3.8584632910634696236777698583735;
	//float freq = 10.300861153527185304245055032919;
	//float freq = 20.60172230705437060849011006574;
	//float freq = 51.913087197493142315305223617279;
	//float freq = 69.295657744218024062196583400945;
	//float freq = 110.00000000000000000000000000074;
	//float freq = 220.00000000000000000000000000062;
//	float freq = 311.12698372208091073637151932641;
	float freq = 440;
	unsigned char buf[32],flagdir = MULFLAG,temp;
	unsigned long int f;
	unsigned char pianokey=0;
	uchar newtouch=0/*,touched  = 0,up = 0, dn = 0*/;	
	uint x1,y1,x2,y2,x1new=1000,y1new=1000,x2new=1000,y2new=1000,xydataok=0;
	DDRA |= (1<<3);//buzzer as o/p
	PORTA &= ~(1<<3);
	initsyst();
	secdelay = 1000000;
	u1str("\n\rStart...");
	
	for(temp = 0; temp<24;temp++)
		GlcdRectangle(temp*5,21,5,10,1);
	for(temp = 0; temp<24;temp++)
		GlcdRectangle(temp*5,21+22,5,10,1);
		
	GlcdRectangleFill(0,0,5,5,1);	
	while(1)
	{
		DDRF = (1<<2)|(1<<0);
		PORTF = (1<<0)|(1<<1);// pin2 = vcc pin0 = gnd
		y1 =  ADCData(1);//1,3

		DDRF = (1<<3)|(1<<1);
		PORTF = (1<<1)|(1<<2);// pin1 = vcc pin3 = gnd
		x1 =  ADCData(2);//0,2
		
		x1new = GlcdMapData(111,950,x1,0,127);
		y1new = GlcdMapData(120,560,y1,0,63);
		
		DDRF = (1<<2)|(1<<0);
		PORTF = (1<<0)|(1<<1);// pin2 = vcc pin0 = gnd
		y2 =  ADCData(1);//1,3

		DDRF = (1<<3)|(1<<1);
		PORTF = (1<<1)|(1<<2);// pin1 = vcc pin3 = gnd
		x2 =  ADCData(2);//0,2

		x2new = GlcdMapData(111,950,x2,0,127);
		y2new = GlcdMapData(120,560,y2,0,63);
		
/*	foll. decides that touch screen is touched first time i.e. after 
		touching / scrolling stylus was lifted and now its new touch	*/		
		//if(( (x1 < x2) && (x2 > XTHRE) )|| ( (x2<x1) && (x1 > XTHRE) ))
		if(( (x1 < XTHRE) && (x2 > XTHRE) )|| ( (x2<XTHRE) && (x1 > XTHRE) ))
			newtouch = 1;

//		if( (x1<XTHRE) && /*(x2 <XTHRE) &&*/ (y1<YTHRE) /*&& (y2<YTHRE)*/)
//			xydataok = 1;
	
//if(newtouch)
{
	newtouch = 0;
	for(temp = 0; temp<24;temp++)
	{
		if(ButtonTouched(temp*5,21,5,10,x1new,y1new))
		{
			pianokey = temp;
			GlcdRectangleFill(temp*5,21,5,10,1);
			PORTA |= (1<<3);
			Tone(freq*pow(OCTAVE8,temp),0);
			break;
		}
		else 
		{	
			GlcdRectangleFill(temp*5,21,5,10,0);
			GlcdRectangle(temp*5,21,5,10,1);
			PORTA &= ~(1<<3);
		}		
	}		
}
	
	/*
	for(temp = 0; temp<24;temp++)
	{
		if(ButtonTouched(temp*5,21+22,5,20,x1new,y1new))
			pianokey = 24 + temp;
	}
	*/
	/*
		if(freq <= 440) 
		{
			flagdir = MULFLAG;
			freq *= OCTAVE12;
		}
		else if(freq >= 40000)
		{
			flagdir = DIVFLAG;
			freq /= OCTAVE12;
		}
		else if( ((freq > 440) && (freq < 40000))  && (flagdir == MULFLAG))
			freq *= OCTAVE12;
		else if( ((freq > 440) && (freq < 40000))  && (flagdir == DIVFLAG))
			freq /= OCTAVE12;
		*/	
		//Tone(freq,20);
//		f  = (int)(freq*100);
//		sprintf(buf,"\n\r%lu ",f);
//		u1str(buf);
		sprintf(buf, "\n\rx1 = %04d y1 = %04d x1glcd = %03d y1glcd = %03d   ",x1,y1,x1new,y1new);
		u1str(buf);

	}
}

void initsyst(void)
{
	u1init();
	GLcdinit();
	InitTime0();
	initADC();
	//sei();
}


void initADC(void)
{
//	sei();//Global interrupt enable
	 //Vref=2.56 internal, ADC Left Adjust,channel 0(LDR on board)
	 ADMUX |= ((1<<REFS1) | (1<<REFS0))/*|(1<<ADLAR) */;
	//Enable ADC,free running,start convertion,ADC inrtpt enable,32 prescalar,
	ADCSRA |= ((1<<ADEN) | (1<<5) | (1<<ADIE) |(1<<ADPS2) /*|(1<<ADPS1) */|(1<<ADPS0)); 
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


void InitTime0()
{
	TCNT0 = 240;
	TIMSK = 1;
	TCCR0 = (1<<CS00);// This will start timer
}

/* This ISR runs every 1 micro seconds	*/
ISR(TIMER0_OVF_vect)
{
	TCCR0  = 0; //Stop the timer
	secdelay--;
	TCNT0 = 234;//reload timer 
	TCCR0 = (1<<CS00);// This will start timer
}

void Tone(float freq,float dly)
{
	unsigned char temp;
	float timeperiod,halftimeperiod;
	timeperiod = 1 / freq;
	halftimeperiod = (timeperiod / 2 ) * 1000 ;
// We need to decide what should be kept instead of 100 here
	for(temp = 0; temp <= 25; temp++)
	{
		BUZZERON;
		_delay_ms(halftimeperiod);
		BUZZEROFF;
		_delay_ms(halftimeperiod);
	}
	_delay_ms(dly);
}

uchar ButtonTouched(uchar topx,uchar topy,uchar width,uchar height,uchar x,uchar y)
{
	if( ( (x>= topx) && (x<= (topx + width)) ) && ( (y>= topy) && (y<= (topy + height)) ) )
		return 1;
	else
		return 0;	
}