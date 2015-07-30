#include <stdio.h>
#include <i2c.h>
#include <util/delay.h>
#include <uart.h>
#include <avr/pgmspace.h>
#include "glcd.h"
#include "img.h"

#define ADC_CHNL0  0
#define ADC_CHNL1  1
#define ADC_CHNL2  2
#define ADC_CHNL3  3

#define XTHRE	1000
#define YTHRE	1000

#define HTTOUCH							1
#define MINTOUCH						2
#define SECTOUCH						3
#define AMPM24TOUCH				4
#define DTTOUCH							5
#define MNTHTOUCH						6
#define YRTOUCH							7
#define DAYTOUCH						8
#define ALTOUCH							9
#define ALHR									10
#define ALMIN								11
#define ALARMAMPM24TOUCH	12
#define ALONOFFTOUCH				13

#define MAXALARMS				8
#define TOTITEMS 						 ALONOFFTOUCH
#define ALSTARTLOC				0x08

#define INCR		1
#define DECR		0

#define BUZZER  3

int ADCData(uchar chnl);
void initsyst(void);
void initADC(void);
uchar ButtonTouched(uchar topx,uchar topy,uchar width,uchar height,uchar x,uchar y);
uint HandleDateEvent(uint dt,uint mnt,uint yr,uchar buttonype);
	
void main(void)
{
	 uchar sec,min,hr,day,date,mnth,yr,AmPm=0,mode1224=1;
	 uchar buff[64],daystr[4] ="***";
	/*IMP: declaring sec,min,hr as uchar creates problem in display sec arm,
	min arm and hr arm doesnt get cleared */
	uchar secrad = 20,minrad =18,hrrad = 12;
	uint theta1=0,theta2=0,theta3=0;
	uchar newtouch=0,touched  = 0,up = 0, dn = 0;	
	uint x1,y1,x2,y2,x1new=1000,y1new=1000,x2new=1000,y2new=1000,xydataok=0;
	uchar Alarm[MAXALARMS][2] = {0},AlarmHr=1,ALAmPmVal=0;//intialise Alarm Values to 0
	uchar alhr=0,almin=0,ALOnOff = 0,BuzzerOnOff = 0;//temporary variables
	DDRA = (1<<BUZZER);//direction for buzzer
	PORTA &= ~(1<<BUZZER);
	initsyst();	
	/*
	WR_I2C(0,0x50);//Sec
    WR_I2C(1,0x40);//Min
	WR_I2C(2,0x02 | (0<<5)|(1<<6) );//Hr
    WR_I2C(3,0x06);//Day
	WR_I2C(4,0x05);//Date
    WR_I2C(5,0x10);//Month
	WR_I2C(6,0x12);//year
	*/
	GlcdShowImage2(clock1,1);
	GlcdVLine(62, 0, 20, 1);
	GlcdHLine( 0, 0, 128, 1);
	GlcdHLine( 0, 63,128, 1);
	GlcdVLine(0, 0, 64, 1);
	GlcdVLine( 127, 0, 64, 1);
	GlcdHLine( 62, 10, 65, 1);
	GlcdHLine( 62, 20, 65, 1);
	GlcdHLine( 64, 30, 63, 1);
	GlcdVLine( 64, 20, 10, 1);
	GlcdVLine( 78, 20, 10, 1);
	u1str("start");
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
//		if(( (x1 < XTHRE) && (x2 > XTHRE) )|| ( (x2<XTHRE) && (x1 > XTHRE) ))
//			newtouch = 1;

//		if( (x1<XTHRE) && /*(x2 <XTHRE) &&*/ (y1<YTHRE) /*&& (y2<YTHRE)*/)
//			xydataok = 1;
				
		sec	 = RD_I2C(0,0);//sec	
		min  = RD_I2C(1,0);//min
		hr 	 = RD_I2C(2,0);//hr
		if(hr & (1<<6))// if it is 12 hr mode
		{
			if(hr & (1<<5))
				AmPm = 1;//PM			
			else
				AmPm = 0;//AM			
			mode1224 = 1;
			hr &= 0x1F;
		}	
		else //24 hr mode// here if bit 4 & bit 5 both == 1 then it is not a valid data
		{
			mode1224 = 0;
			//foll is to make sure that we never get data more then 23
			if(hr & (1<<4)) hr &= 0x1F;//only bit 4 is 1 (time range 10 to 19)
			else if(hr & (1<<5)) hr &= 0x23;//only bit 5 is one (time range 20 to 23)
			else hr &= 0x0F; //time range 0 to 9
		}	
		day 	= RD_I2C(3,0);//day
		date 	= RD_I2C(4,0);//date
		mnth  = RD_I2C(5,0);//month
		yr 	    = RD_I2C(6,0);//year

		for(alhr = 0;alhr<MAXALARMS;alhr++)
			for(almin = 0;almin<2;almin++)
				Alarm[alhr][almin] = 	RD_I2C(ALSTARTLOC+alhr*2+almin,0);
		
		ALAmPmVal = 	RD_I2C(ALSTARTLOC+MAXALARMS*2+1,0);	
		ALOnOff = RD_I2C(ALSTARTLOC+MAXALARMS*2+2,0);	
		
		switch(day)
		{
			case 1: 	strcpy(daystr,"SUN"); 		break;
			case 2: 	strcpy(daystr,"MON"); 		break;
			case 3: 	strcpy(daystr,"TUE"); 		break;
			case 4:	strcpy(daystr,"WED"); 		break;
			case 5: 	strcpy(daystr,"THU"); 		break;
			case 6: 	strcpy(daystr,"FRI"); 		break;
			case 7: 	strcpy(daystr,"SAT"); 		break;
			default:	strcpy(daystr,"***"); 		break;
		}
		
		GlcdLineAtAngle(secrad,theta1,32,32,0);
		theta1 = Bcd2Dec(sec) * 6 - 90;
		GlcdLineAtAngle(secrad,theta1,32,32,1);

		GlcdLineAtAngle(minrad,theta2,32,32,0);
		theta2 = Bcd2Dec(min)* 6 - 90;
		GlcdLineAtAngle(minrad,theta2,32,32,1);

		GlcdLineAtAngle(hrrad,theta3,32,32,0);
		theta3 = (unsigned int)(Bcd2Dec(hr) * 30 + (float)(Bcd2Dec(min)) * 6 / 72 - 90)  ;
		GlcdLineAtAngle(hrrad,theta3,32,32,1);
					
		if(ButtonTouched(65,32,8,9,x1new,y1new) /* && (newtouch == 1)*/)
		{	
			touched--;
			if(touched  == 255)
				touched = TOTITEMS;
			GlcdPutImage_P(leftbtn,65,32,8,9,0);
		}
		else
			GlcdPutImage_P(leftbtn,65,32,8,9,1);

		if(ButtonTouched(78,32,8,9,x1new,y1new) /*&& (newtouch == 1)*/)
		{	
			touched++;
			if(touched > TOTITEMS)
				touched = 0;	
			GlcdPutImage_P(rightbtn,78,32,8,9,0);
		}
		else
			GlcdPutImage_P(rightbtn,78,32,8,9,1);
	
		if(ButtonTouched(90,32,11,8,x1new,y1new) /* && (newtouch == 1)*/)
		{	
			up = 1;
			GlcdPutImage_P(uptri,90,32,11,8,0);
		}
		else
			GlcdPutImage_P(uptri,90,32,11,8,1);

		if(ButtonTouched(105,32,11,8,x1new,y1new) /*&& (newtouch == 1)*/)
		{	
			dn = 1;
			GlcdPutImage_P(dntri,105,32,11,8,0);
		}
		else
			GlcdPutImage_P(dntri,105,32,11,8,1);
	
		if(touched == HTTOUCH)//HR
		{
			GlcdShowNumber_P(hr/16,64,2,5,8,0);
			GlcdShowNumber_P(hr%16,70,2,5,8,0);
		}
		else
		{
			GlcdShowNumber_P(hr/16,64,2,5,8,1);
			GlcdShowNumber_P(hr%16,70,2,5,8,1);	
		}		
		
		GlcdShowChar_P(':',76,2,5, 8,1);
		
		if(touched == MINTOUCH)//MIN
		{
			GlcdShowNumber_P(min/16,79,2,5,8,0);
			GlcdShowNumber_P(min%16,85,2,5,8,0);
		}
		else
		{
			GlcdShowNumber_P(min/16,79,2,5,8,1);
			GlcdShowNumber_P(min%16,85,2,5,8,1);		
		}
		
		GlcdShowChar_P(':',91,2,5, 8,1);

		if(touched == SECTOUCH)//SEC
		{		
			GlcdShowNumber_P(sec/16,94,2,5,8,0);
			GlcdShowNumber_P(sec%16,100,2,5,8,0);
		}	
		else
		{	
			GlcdShowNumber_P(sec/16,94,2,5,8,1);
			GlcdShowNumber_P(sec%16,100,2,5,8,1);
		}

		if(touched == AMPM24TOUCH) //AM/PM/24
		{		
			if(mode1224)// if it is 12 hr mode
			{
				if(AmPm)// if PM
					GlcdShowString_P("PM ",109,2,5, 8,0);
				else 		// if AM
					GlcdShowString_P("AM ",109,2,5, 8,0 );
			}
			else// if it is 24 hr mode
				GlcdShowString_P("24H",109,2,5, 8,0 );// * implies 24 hour	
		}	
		else
		{			
			if(mode1224)// if it is 12 hr mode
			{
				if(AmPm)// if PM
					GlcdShowString_P("PM ",109,2,5, 8,1);
				else 		// if AM
					GlcdShowString_P("AM ",109,2,5, 8,1);
			}
			else// if it is 24 hr mode
				GlcdShowString_P("24H",109,2,5, 8,1);// * implies 24 hour	
		}
				
		if(touched == DTTOUCH)//DATE
		{		
			GlcdShowNumber_P(date/16,64,12,5,8,0);
			GlcdShowNumber_P(date%16,70,12,5,8,0);
		}	
		else
		{	
			GlcdShowNumber_P(date/16,64,12,5,8,1);
			GlcdShowNumber_P(date%16,70,12,5,8,1);
		}

		GlcdShowChar_P('/',76,12,5, 8,1);
		
		if(touched == MNTHTOUCH)//MONTH
		{		
			GlcdShowNumber_P(mnth/16,81,12,5,8,0);
			GlcdShowNumber_P(mnth%16,87,12,5,8,0);
		}	
		else
		{	
			GlcdShowNumber_P(mnth/16,81,12,5,8,1);
			GlcdShowNumber_P(mnth%16,87,12,5,8,1);
		}
		
		GlcdShowChar_P('/',93,12,5, 8,1);

		if(touched == YRTOUCH)//YEAR
		{		
			GlcdShowNumber_P(yr/16,98,12,5,8,0);
			GlcdShowNumber_P(yr%16,104,12,5,8,0);
		}	
		else
		{	
			GlcdShowNumber_P(yr/16,98,12,5,8,1);
			GlcdShowNumber_P(yr%16,104,12,5,8,1);
		}
		
		if(touched == DAYTOUCH)//Day		
			GlcdShowString_P(daystr,110,12,5,8,0);
		else	
			GlcdShowString_P(daystr,110,12,5,8,1);		
		
		if(touched == ALTOUCH)
		{
			GlcdShowNumber_P(AlarmHr/10,66,22,5,8,0);
			GlcdShowNumber_P(AlarmHr%10,72,22,5,8,0);
		}
		else
		{
			GlcdShowNumber_P(AlarmHr/10,66,22,5,8,1);
			GlcdShowNumber_P(AlarmHr%10,72,22,5,8,1);
		}	
	
		if(touched == ALHR)
		{
			GlcdShowNumber_P(Alarm[AlarmHr-1][0]/10,80,22,5,8,0);
			GlcdShowNumber_P(Alarm[AlarmHr-1][0]%10,86,22,5,8,0);
		}
		else
		{
			GlcdShowNumber_P(Alarm[AlarmHr-1][0]/10,80,22,5,8,1);
			GlcdShowNumber_P(Alarm[AlarmHr-1][0]%10,86,22,5,8,1);
		}	
		
		GlcdShowChar_P(':',92,22,5, 8,1);
		
		if(touched == ALMIN)
		{
			GlcdShowNumber_P(Alarm[AlarmHr-1][1]/10,95,22,5,8,0);
			GlcdShowNumber_P(Alarm[AlarmHr-1][1]%10,101,22,5,8,0);
		}
		else
		{
			GlcdShowNumber_P(Alarm[AlarmHr-1][1]/10,95,22,5,8,1);
			GlcdShowNumber_P(Alarm[AlarmHr-1][1]%10,101,22,5,8,1);
		}	
	
		if(touched == ALARMAMPM24TOUCH) //AM/PM/24
		{		
			if(mode1224)// if it is 12 hr mode
			{
				if(ALAmPmVal & (1<<(AlarmHr-1)))// if PM
					GlcdShowString_P("P",107,22,5, 8,0);
				else 		// if AM
					GlcdShowString_P("A",107,22,5, 8,0 );
			}
			else// if it is 24 hr mode
				GlcdShowString_P("H",107,22,5, 8,0 );// * implies 24 hour	
		}	
		else
		{			
			if(mode1224)// if it is 12 hr mode
			{
				if(ALAmPmVal & (1<<(AlarmHr-1)))// if PM
					GlcdShowString_P("P",107,22,5, 8,1);
				else 		// if AM
					GlcdShowString_P("A",107,22,5, 8,1);
			}
			else// if it is 24 hr mode
				GlcdShowString_P("H",107,22,5, 8,1);// * implies 24 hour	
		}
		
		if(touched == ALONOFFTOUCH)
		{
			if(ALOnOff & (1<<(AlarmHr-1)))
				GlcdPutImage_P(right8X8,113,22,8,8,0);
			else
				GlcdPutImage_P(wrong8X8,113,22,8,8,0);	
		}
		else
		{
			if(ALOnOff & (1<<(AlarmHr-1)))
				GlcdPutImage_P(right8X8,113,22,8,8,1);
			else
				GlcdPutImage_P(wrong8X8,113,22,8,8,1);	
		}
		
/*	If Real Time matches with Alarm Time set corresponding alarm ON bits in ALOnOff variable*/	
		for(alhr = 0;alhr<MAXALARMS;alhr++)
		{
			if(ALOnOff & (1<<alhr))
			{	
				if(mode1224)//if clock is in 12 hr mode
				{
	// then both clock AM/PM and alarm  AM/PM match			
					if( ( AmPm	&&  ( ALAmPmVal &(1<<alhr ) ) ) || ( (!AmPm) &&  ( ! (ALAmPmVal &(1<<alhr) ) ) )) 
					{
	// now if time match then turn on alarm				
						if(  (Alarm[alhr][0] == Bcd2Dec(hr) ) && (Alarm[alhr][1] == Bcd2Dec(min) ) )
						{
//							ALOnOff |= (1<<alhr);//set Alarm ON bit
							AlarmHr = alhr+1;
							BuzzerOnOff = 1;
							break;
						}
					}
				}	
				else if( ( Alarm[alhr][0]  == 	Bcd2Dec(hr) ) && (Alarm[alhr][1] == Bcd2Dec(min) ) )
				{
//					ALOnOff |= (1<<alhr);
					AlarmHr = alhr+1;
					PORTA |= (1<<BUZZER);
				}
			}
		else	  BuzzerOnOff = 0;
		}
	
		if(BuzzerOnOff)
			PORTA |= (1<<BUZZER);
		else
			PORTA &= ~(1<<BUZZER);
		
		if(up)
		{
			switch(touched)
			{
				case HTTOUCH:
					hr = Bcd2Dec(hr);
					hr++;
					if(mode1224)//if 12 hr mode
					{						
						if(hr > 12) 	hr = 1;							
						hr = Dec2Bcd(hr);	
						hr |= (1<<6);//set  the 12/24  bit
						if(AmPm)// if yes then if PM
							hr |= (1<<5);//set  the AM/PM bit
						else 		// if AM 
							hr &= ~(1<<5);//then clear  the AM/PM bit						
					}
					else 
					{
						if(hr>23) 	hr = 0;							
						hr = Dec2Bcd(hr);	
						hr &= ~(1<<6);//clear the 12/24  bit	
					}	
					WR_I2C(2, hr);
					break; 
						
				case MINTOUCH: 
					min = Bcd2Dec(min);
					min++;
					if(min> 59) 		min = 0;								
					WR_I2C(1, Dec2Bcd(min));//Min
					break; 
						
				case SECTOUCH: 
					sec = Bcd2Dec(sec);
					sec++;
					if(sec>59)		sec = 0;
					WR_I2C(0, Dec2Bcd(sec));//Sec
					break; 
						
				case AMPM24TOUCH: //PM->AM->24H->PM->AM.....
					if(mode1224)//12 hr mode
					{
						if(!AmPm)//if AM -> PM
						{
							hr |= (1 << 5);//set  this bit
							hr |= (1 << 6);//set  this bit//IDIOTIC MISTAKKKKKKKKKE ...i was not making this = 1
						}
						else if(AmPm)//if PM -> 24H
						{
							hr &= ~(1<< 6);//clr  this bit
							hr = Bcd2Dec(hr);
							hr += 12;
							hr = Dec2Bcd(hr);

// if Any alarm was in PM mode add 12 to it while moving to 24 hr format						
						for(alhr = 0;alhr<MAXALARMS;alhr++)
						{
							 if(ALAmPmVal & (1<<alhr))
								Alarm[alhr][0] += 12; 
								if(Alarm[alhr][0]  > 23) Alarm[alhr][0]  = 0;
							WR_I2C( ALSTARTLOC + alhr * 2 ,Alarm[alhr][0] );
						}		
							
						}
					}
					else if (!mode1224)//if 24 H -> AM
					{
						hr = Bcd2Dec(hr);
						if(hr > 12)		hr -= 12;
						hr = Dec2Bcd(hr);
						hr &= ~(1 << 5);//clr  this bit for am
						hr |= (1 << 6);//set  this bit for 12 hr
//if any alarm value was > 12 then subtract 12 from it while moving to 12 hr format
						for(alhr = 0;alhr<MAXALARMS;alhr++)
						{
							 if(Alarm[alhr][0] > 12 )
								Alarm[alhr][0] -= 12; 
							WR_I2C( ALSTARTLOC + alhr * 2 ,Alarm[alhr][0] );
						}	
					}
					WR_I2C(2, hr);// as AMPM is related to hr 
					
					break; 
						
				case DTTOUCH: 
					date = Bcd2Dec(date);
					mnth = Bcd2Dec(mnth);
					yr = Bcd2Dec(yr);
					date++;
					date = HandleDateEvent( date, mnth, yr,INCR);
					WR_I2C(4, Dec2Bcd(date));//Date
					break; 
					
				case MNTHTOUCH: 
					mnth = Bcd2Dec(mnth);
					mnth++;
					 if(mnth > 12) 		mnth = 1;
					 date = Bcd2Dec(date);
					 date = HandleDateEvent( date, mnth, yr,INCR);
					 WR_I2C(5, Dec2Bcd(mnth));//Month
					 WR_I2C(4, Dec2Bcd(date));//Date
					break; 
						
				case YRTOUCH: 
					yr = Bcd2Dec(yr);
					yr++;
					 if(yr > 99) 			yr = 0;
					 date = Bcd2Dec(date);
					 date =  HandleDateEvent( date, mnth, yr,INCR);
					 WR_I2C(6,Dec2Bcd(yr));//year
					 WR_I2C(4, Dec2Bcd(date));//Date
					break; 	
						
				case DAYTOUCH: 
					 day = Bcd2Dec(day);
					 day++;
					 if(day > 7)			day = 1;
					 WR_I2C(3, Dec2Bcd(day));//Day
					 break; 
					 
				case ALTOUCH:	 
					AlarmHr++;
					if(AlarmHr >MAXALARMS) AlarmHr = 1;
					break;
					
				case ALHR:
					Alarm[AlarmHr-1][0]++;
					if(mode1224)
					{
						if(Alarm[AlarmHr-1][0] >12)	Alarm[AlarmHr-1][0] = 1;
					}
					else
						if(Alarm[AlarmHr-1][0] > 23)	Alarm[AlarmHr-1][0] = 0;
						WR_I2C( ALSTARTLOC + (AlarmHr-1) * 2 ,Alarm[AlarmHr-1][0] );
					break;

				case ALMIN:
					Alarm[AlarmHr-1][1]++;
					if(Alarm[AlarmHr-1][1] > 59)
						Alarm[AlarmHr-1][1] = 0;
						WR_I2C( ALSTARTLOC + (AlarmHr-1) * 2 + 1 ,Alarm[AlarmHr-1][1] );
					break;

				case ALARMAMPM24TOUCH:
					if(mode1224)
					{
						if(ALAmPmVal & (1<<(AlarmHr-1)))	ALAmPmVal &= ~(1<<(AlarmHr-1));
						else 	ALAmPmVal |= (1<<(AlarmHr-1));
					}	
					WR_I2C( ALSTARTLOC + MAXALARMS * 2 + 1 ,ALAmPmVal );
					break;
					
				case ALONOFFTOUCH:
					ALOnOff ^= (1<<(AlarmHr-1));
					WR_I2C( ALSTARTLOC + MAXALARMS * 2 + 2 ,ALOnOff );
					break;
			}
		up = 0;		
		}
		else if(dn)
		{			
			switch(touched)
			{
				case HTTOUCH:
					hr = Bcd2Dec(hr);
					hr--;
					if(mode1224)//if 12 hr mode
					{						
						if(hr < 1) 	hr = 12;							
						hr = Dec2Bcd(hr);	
						hr |= (1<<6);//set  the 12/24  bit
						if(AmPm)// if yes then if PM
							hr |= (1<<5);//set  the AM/PM bit
						else 		// if AM 
							hr &= ~(1<<5);//then clear  the AM/PM bit
							
					}
					else 
					{
						if(hr == 255) 	hr = 23;		//we could hve used char in place of uchar					
						hr = Dec2Bcd(hr);	
						hr &= ~(1<<6);//clear the 12/24  bit	
					}	
					WR_I2C(2, hr);
					break; 
						
				case MINTOUCH: 
					min = Bcd2Dec(min);
					min--;
					if(min == 255) 		min = 59;								
					WR_I2C(1, Dec2Bcd(min));//Min
					break; 
						
				case SECTOUCH: 
					sec = Bcd2Dec(sec);
					sec--;
					if(sec == 255)		sec = 59;
					WR_I2C(0, Dec2Bcd(sec));//Sec
					break; 
						
				case AMPM24TOUCH: //PM->AM->24H->PM->AM.....
					if(mode1224)//12 hr mode
					{
						if(!AmPm)//if AM -> 24
						{
							hr &= ~(1<< 6);//clr  this bit
							hr = Bcd2Dec(hr);
							hr += 12;
							hr = Dec2Bcd(hr);						

// if Any alarm was in PM mode add 12 to it while moving to 24 hr format						
							for(alhr = 0;alhr<MAXALARMS;alhr++)
							{
								 if(ALAmPmVal & (1<<(alhr)))
									Alarm[alhr][0] += 12; 
									if(Alarm[alhr][0]  > 23) Alarm[alhr][0]  = 0;
								WR_I2C( ALSTARTLOC + alhr * 2 ,Alarm[alhr][0] );
							}							
						}
						else if(AmPm)//if PM -> AM
						{
							hr &= ~(1 << 5);//clr  this bit for AM
							hr |= (1 << 6);//set  this bit 12 HR mode 
						}		
					}
					else if (!mode1224)//if 24 H -> PM
					{
						hr = Bcd2Dec(hr);
						if(hr > 12)		hr -= 12;
						hr = Dec2Bcd(hr);
						hr |= (1 << 5);//set  this bit for P
						hr |= (1 << 6);//set  this bit for 12 hr
//if any alarm value was > 12 then subtract 12 from it while moving to 12 hr format
						for(alhr = 0;alhr<MAXALARMS;alhr++)
						{
							 if(Alarm[alhr][0] > 12 )
								Alarm[alhr][0] -= 12; 
							WR_I2C( ALSTARTLOC + alhr * 2 ,Alarm[alhr][0] );
						}						
					}
					WR_I2C(2, hr);// as AMPM is related to hr 
					break; 
						
				case DTTOUCH: 
					date = Bcd2Dec(date);
					mnth = Bcd2Dec(mnth);
					yr = Bcd2Dec(yr);
					date--;
					date =  HandleDateEvent( date, mnth, yr,DECR);
					WR_I2C(4, Dec2Bcd(date));//Date
					break; 
					
				case MNTHTOUCH: 
					mnth = Bcd2Dec(mnth);
					mnth--;
					 if(mnth < 1) 		mnth = 12;
					 date = Bcd2Dec(date);
					 date =  HandleDateEvent( date, mnth, yr,DECR);
					 WR_I2C(5, Dec2Bcd(mnth));//Month
					 WR_I2C(4, Dec2Bcd(date));//Date
					break; 
						
				case YRTOUCH: 
					yr = Bcd2Dec(yr);
					yr--;
					 if(yr == 255) 			yr = 99;
					 date = Bcd2Dec(date);
					 date =  HandleDateEvent( date, mnth, yr,DECR);
					 WR_I2C(6,Dec2Bcd(yr));//year
					 WR_I2C(4, Dec2Bcd(date));//Date
					break; 	
						
				case DAYTOUCH: 
					 day = Bcd2Dec(day);
					 day--;
					 if(day < 1)			day = 7;
					 WR_I2C(3, Dec2Bcd(day));//Day
					 break; 

				case ALTOUCH:	 
						AlarmHr--;
						if(AlarmHr < 1) AlarmHr = MAXALARMS;
					break;

				case ALHR:
					Alarm[AlarmHr-1][0]--;
					if(mode1224)
					{
						if(Alarm[AlarmHr-1][0] < 1)	Alarm[AlarmHr-1][0] = 12;
					}
					else
						if(Alarm[AlarmHr-1][0]  == 255)	Alarm[AlarmHr-1][0] = 23;
						WR_I2C( ALSTARTLOC + (AlarmHr-1) * 2 ,Alarm[AlarmHr-1][0] );
					break;

				case ALMIN:
					Alarm[AlarmHr-1][1]--;
					if(Alarm[AlarmHr-1][1]  == 255)
						Alarm[AlarmHr-1][1]  = 59;
						WR_I2C( ALSTARTLOC + (AlarmHr-1) * 2 + 1 ,Alarm[AlarmHr-1][1] );						
					break;
	
					case ALARMAMPM24TOUCH:
					if(mode1224)
					{
						if(ALAmPmVal & (1<<(AlarmHr-1)))	ALAmPmVal &= ~(1<<(AlarmHr-1));
						else 	ALAmPmVal |= (1<<(AlarmHr-1));
					}	
					WR_I2C( ALSTARTLOC + MAXALARMS * 2 + 1 ,ALAmPmVal );
					break;

				case ALONOFFTOUCH:
					ALOnOff ^= (1<<(AlarmHr-1));
					WR_I2C( ALSTARTLOC + MAXALARMS * 2 + 2 ,ALOnOff );
					break;
					
				}
			dn = 0;
		}
	// ThinkLABS Logo	
	
		if(Bcd2Dec(sec)%2)
			GlcdPutImage_P(TL1,63,47,64,11,1);
		else
			GlcdPutImage_P(TL1,63,47,64,11,0);
 
		sprintf(buff, "\n\rx1 = %04d y1 = %04d x1glcd = %03d y1glcd = %03d  touched = %d ",x1,y1,x1new,y1new,touched);
		u1str(buff);
		sprintf(buff, "\n\rx2 = %04d y2 = %04d x2glcd = %03d y2glcd = %03d  touched = %d ",x2,y2,x2new,y2new,touched);
		u1str(buff);
		sprintf(buff, "\n\r%02d :%02d :%02d   %02d/%02d/%02d   %d %d %d ",hr,min,sec,date,mnth,yr,day,mode1224,AmPm);
		u1str(buff);
		sprintf(buff,"\n\r%d  %d %d ",sizeof(unsigned int),sizeof(unsigned long int) ,sizeof(unsigned long long int));
		u1str(buff);
	}
}

uchar ButtonTouched(uchar topx,uchar topy,uchar width,uchar height,uchar x,uchar y)
{
	if( ( (x>= topx) && (x<= (topx + width)) ) && ( (y>= topy) && (y<= (topy + height)) ) )
		return 1;
	else
		return 0;	
}

void initsyst(void)
{
	initADC();
	u1init();
	init_i2c();
	GLcdinit();
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

uint HandleDateEvent(uint dt,uint mnt,uint yr,uchar buttonype)
{
	if(buttonype == INCR)
	{
		switch(mnt)
		{
	//jan/mar/may/jul/aug/oct/dec  =>	 days = 31	
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
				if(dt >31)	dt = 1;			break;	
			
	//Apr/jun/sept/nov => days = 30			
			case 4:
			case 6:
			case 9:
			case 11:
				if(dt >30)	dt = 1;			break;			
				
	//feb => if leap yr days = 29 else days = 28
			case 2:
			if(!(yr%4))	// if leap yr (not an accurate measurement for leap yr.....ha ha ha... :) )
			{
				if(dt >29)			dt = 1;
			} 
			else if(dt >28)		dt = 1;	break;	
		
			default : 								break;
		}
	}
	else if(buttonype == DECR)
	{
		switch(mnt)
		{
	//jan/mar/may/jul/aug/oct/dec  =>	 days = 31	
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
				if(dt <1)	dt = 31;			break;	
			
	//Apr/jun/sept/nov => days = 30			
			case 4:
			case 6:
			case 9:
			case 11:
				if(dt < 1)	dt = 30;			break;			
				
	//feb => if leap yr days = 29 else days = 28
			case 2:
			if(!(yr%4))	// if leap yr (not an accurate measurement for leap yr.....ha ha ha... :) )
			{
				if(dt < 1)			dt = 29;
			} 
			else if(dt <1)		dt = 28;	break;	
		
			default : 								break;
		}
	}
		return dt;		
} 