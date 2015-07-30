#ifndef 	_UART_H
#define 	_UART_H

#include <avr/io.h>

void u1init(void)
{
	UBRR1H = 0;
	UBRR1L = 8;
	UCSR1B = (1<<RXEN1)|(1<<TXEN1);
	UCSR1C = (1<<UCSZ11) | (1<<UCSZ10); 
}

void u1tx(unsigned char ch)
{
	UDR1 = ch;
	while(!(UCSR1A & (1<<UDRE1)));
}

void u1str(unsigned char * str)
{
while(*str)
	u1tx(*str++);
}

#endif