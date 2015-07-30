#include <stdio.h>
#include <i2c.h>
#include <lcd.h>
#include <util/delay.h>
#include <uart.h>

void main(void)
{
	DDRC = 0xff;
	PORTC = 0xff;
	unsigned char ch,ch1;
	unsigned char buf[10];

	uart1_init();
	lcd_init();	
	lcd_char('*');
	
	init_i2c();
	
	WR_I2C(0,0x57);//Sec
    WR_I2C(1,0x59);//Min
	WR_I2C(2,0x11 | (1<<5)|(1<<6) );//Hr
    WR_I2C(3,1);//Day
	WR_I2C(4,0x31);//Date
    WR_I2C(5,0x12);//Month
	WR_I2C(6,0x99);//year
	
	while(1)
	{	
		ch = RD_I2C(0,0);//sec
		sprintf(buf,"\n\rSec = %02d",Hex2Dec(ch));
		uart1_transmit_string(buf);

		ch = RD_I2C(1,0);//min
		sprintf(buf,"\n\rMin = %02d",Hex2Dec(ch));
		uart1_transmit_string(buf);
	
		ch = RD_I2C(2,0);//hr
		sprintf(buf,"\n\rHr = %02d",Hex2Dec( ch & 0x1F ) );
		uart1_transmit_string(buf);
	
		ch = RD_I2C(3,0);//day
		sprintf(buf,"\n\rDay = %02d",Hex2Dec(ch));
		uart1_transmit_string(buf);

		ch = RD_I2C(4,0);//date
		sprintf(buf,"\n\rDate = %02d",Hex2Dec(ch));
		uart1_transmit_string(buf);
	
		ch = RD_I2C(5,0);//month
		sprintf(buf,"\n\rMonth = %02d",Hex2Dec(ch));
		uart1_transmit_string(buf);

		ch = RD_I2C(6,0);//year
		sprintf(buf,"\n\rYear = %02d",Hex2Dec(ch));
		uart1_transmit_string(buf);
		_delay_ms(1000);
	
	}
}