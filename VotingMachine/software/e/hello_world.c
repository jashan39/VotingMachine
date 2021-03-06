
#define RS232_Control (*(volatile unsigned char *)(0x84000200))
#define RS232_Status (*(volatile unsigned char *)(0x84000200))
#define RS232_TxData (*(volatile unsigned char *)(0x84000202))
#define RS232_RxData (*(volatile unsigned char *)(0x84000202))
#define RS232_Baud (*(volatile unsigned char *)(0x84000204))


/**************************************************************************
/* Subroutine to initialise the RS232 Port by writing some data
** to the internal registers.
** Call this function at the start of the program before you attempt
** to read or write to data via the RS232 port
**
** Refer to 6850 data sheet for details of registers and
***************************************************************************/
void Init_RS232(void)
{
 // set up 6850 Control Register to utilise a divide by 16 clock,
 // set RTS low, use 8 bits of data, no parity, 1 stop bit,
 // transmitter interrupt disabled
 // program baud rate generator to use 115k baud
	RS232_Control = 0x55;
	RS232_Baud = 0x01;
}
int putcharRS232(int c)
{
 // poll Tx bit in 6850 status register. Wait for it to become '1'
 // write 'c' to the 6850 TxData register to output the character
 // Bit 1 will be set to 1 when ready to take a value


	 while(!RS232_Status & 0x02 == 0x02)
     putcharRS232(1);

	 RS232_TxData = c;
	 return c;


}



int getcharRS232( void )
{
 // poll Rx bit in 6850 status register. Wait for it to become '1'
 // read received character from 6850 RxData register.
	 while(!RS232_Status & 0x01 == 0x01)
     getcharRS232();

	 printf("\nData here");
	 return (1);
}
// the following function polls the 6850 to determine if any character
// has been received. It doesn't wait for one, or read it, it simply tests
// to see if one is available to read
int RS232TestForReceivedData(void)
{
 // Test Rx bit in 6850 serial comms chip status register
 // if RX bit is set, return TRUE, otherwise return FALSE

	putcharRS232(1);
	if(getcharRS232() == 1)
		return 1;

	else
		return (0);
}

int main(){

Init_RS232();
if(RS232TestForReceivedData() == 1)
printf("\nTest Worked!!!!!!!!");

else
	printf("\nTry Again");
return 0;

}

