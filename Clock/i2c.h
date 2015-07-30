#include <avr/io.h>

#define ACK 		1
#define NACK	0
void init_i2c(void);
void start_i2c(void);
void stop_i2c(void);
void write_i2c(unsigned char data);
unsigned char read_i2c_with_ack_nack(unsigned char ackbit);
void WR_I2C(unsigned char wordaddrs,unsigned char data);
unsigned char RD_I2C(unsigned char wordaddrs,unsigned char ackbit);
unsigned int Hex2Dec(unsigned  int hex);
unsigned int Dec2Hex(unsigned int dec);

//#define DEBUG

void init_i2c()
{	
#ifdef DEBUG	
	uart1_transmit_string("\n\rI2C Init.....");
#endif
	TWBR = 72;// SCL freq = 100K
	TWCR = (1<<TWEN)|( 1 << TWINT);// Enable TWI, clear TWINT
	TWSR = 0x00;//prescaler bits = 0
	start_i2c();
	write_i2c(0xD0);// Device / slave address
//	write_i2c(0);	// clear CH bit present in SEC register of DS1307
//	write_i2c(0);
	stop_i2c();
#ifdef DEBUG	
	uart1_transmit_string("\n\rI2C Init.....DONE");
#endif	
}

void start_i2c()
{
#ifdef DEBUG	
	uart1_transmit_string("\n\rI2C Start.....");
#endif
/*	Send Start Condition	*/	
	TWCR = (1 << TWINT) |(1<< TWSTA) | (1<<TWEN);
/*	wait till start ccondition is transmitted	*/
	while( ! ( TWCR & (1 << TWINT) ) );
	
#ifdef DEBUG	
	uart1_tx_number((unsigned int )TWSR);
	uart1_transmit_string("\n\rI2C Start.....DONE");
#endif		
}

void stop_i2c()
{
#ifdef DEBUG	
	uart1_transmit_string("\n\rI2C Stop.....");
#endif
/*	Send Stop Condition	*/	
	TWCR = ( 1 << TWINT)|(1<<TWSTO) | (1<<TWEN);
//	uart1_tx_number((unsigned int )TWSR);
#ifdef DEBUG	
	uart1_transmit_string("\n\rI2C Stop.....DONE");
#endif
}

void write_i2c(unsigned char data)
{
#ifdef DEBUG	
	uart1_transmit_string("\n\rI2C WR.....START");
#endif	
	TWDR = data;
	TWCR = ( 1 << TWINT) | (1 << TWEN );
	/*	wait till start condition is transmitted	*/
	while( ! ( TWCR & (1 << TWINT) ) );

#ifdef DEBUG		
	uart1_tx_number( ( unsigned int ) TWSR );
	uart1_transmit_string("\n\rI2C WR.....DONE");	
#endif
}

unsigned char read_i2c_with_ack_nack(unsigned char ackbit)
{
#ifdef DEBUG	
	uart1_transmit_string("\n\rI2C RD.....START");
#endif
	unsigned char data; 	
/*	ackbit = 0 for NACK, = 1 for ACK  	*/	
	TWCR = ( 1 << TWINT) |(1<<TWEN)|(ackbit << TWEA);
/*	wait till start condition is transmitted	*/
	while( ! ( TWCR & (1 << TWINT) ) );
	data = TWDR;
#ifdef DEBUG	
	uart1_tx_number( ( unsigned int ) TWSR );
	uart1_transmit_string("\n\rI2C RD.....DONE");	
#endif
	return data;
}

void WR_I2C(unsigned char wordaddrs,unsigned char data)
{
	start_i2c();
	write_i2c(0xD0);	// Indicate Write Mode
	write_i2c(wordaddrs);	// Indicate Write Mode
	write_i2c(data);
	stop_i2c();
}

unsigned char RD_I2C(unsigned char wordaddrs,unsigned char ackbit)
{
	unsigned char data;	
	start_i2c();
	write_i2c(0xD0);	// Indicate Write Mode
	write_i2c(wordaddrs);	// Indicate Write Mode
	start_i2c();		//repeated start condition
	write_i2c(0xD1);	// Indicate Read Mode
	data = read_i2c_with_ack_nack(ackbit);
	stop_i2c();
	return data;
}

unsigned int Bcd2Dec(unsigned  int hex)//64 hex -> 
{
	return( ( hex / 16 )  * 10 + hex % 16);
}

unsigned int Dec2Bcd(unsigned int dec)//100dec ->
{
	return( (( dec / 10 )  * 16 ) + dec % 10);
}
/*
unsigned int Bcd2Hex(unsigned  int hex)//64 hex -> 
{
	return( ( hex / 16 )  * 10 + hex % 16);
}

unsigned int Hex2Bcd(unsigned  int bcd)//64 hex -> 
{
	return( ( bcd / 16 )  <<4  + bcd % 16);
}
*/