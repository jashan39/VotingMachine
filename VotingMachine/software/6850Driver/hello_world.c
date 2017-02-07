#include <stdio.h>

#define RS232_Control (*(volatile unsigned char *)(0x84000200))
#define RS232_Status (*(volatile unsigned char *)(0x84000200))
#define RS232_TxData (*(volatile unsigned char *)(0x84000202))
#define RS232_RxData (*(volatile unsigned char *)(0x84000202))
#define RS232_Baud (*(volatile unsigned char *)(0x84000204))
#define TouchScreen_TxData (*(volatile unsigned char *)(0x84000232))
#define TouchScreen_RxData (*(volatile unsigned char *)(0x84000232))
#define TouchScreen_Baud (*(volatile unsigned char *)(0x84000234))
#define TouchScreen_Control (*(volatile unsigned char *)(0x84000230))
#define TouchScreen_Status (*(volatile unsigned char *)(0x84000230))

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

	//The 6850 Control Register write only
	// RS232_Control(7 DOWNTO 0) = |X|1|0|1|0|1|0|1| = 0b01010101 = 0x55 rts high & interrupt disable
	// RS232_Control(7 DOWNTO 0) = |X|0|0|1|0|1|0|1| = 0b00010101 = 0x15
	RS232_Control = 0x15;
	RS232_Baud = 0x01; // program for 115k baud

}
int putcharRS232(int c)
{
// poll Tx bit in 6850 status register. Wait for it to become '1'

// write 'c' to the 6850 TxData register to output the character

	while (!(RS232_Status & 0x2));
	RS232_TxData = c;
	return RS232_TxData;
}

int getcharRS232( void )
{
 // poll Rx bit in 6850 status register. Wait for it to become '1'
 // read received character from 6850 RxData register.
	while (!(RS232_Status & 0x1));
	return RS232_RxData;
}

// the following function polls the 6850 to determine if any character
// has been received. It doesn't wait for one, or read it, it simply tests
// to see if one is available to read
int RS232TestForReceivedData(void)
{
 // Test Rx bit in 6850 serial comms chip status register
 // if RX bit is set, return TRUE, otherwise return FALSE

	// RS232_Status: XXXX XXXX
	// We want bit0: 0000 0001
	return (RS232_Status & 0x1);
}

int RS232TestForTransmitData(void) {
	// Test Tx bit in 6850 serial communications chip status register
	// if TX bit is set, return TRUE, otherwise return FALSE

	// RS232_Status: XXXX XXXX
	// We want bit1: 0000 0010
	return (RS232_Status & 0x2);
}


/*****************************************************************************
** Initialise touch screen controller
*****************************************************************************/
void Init_Touch(void)
{
	TouchScreen_Control = 0x15;
	//baud rate 9600
	TouchScreen_Baud = 0x11;
 // Program 6850 and baud rate generator to communicate with touchscreen
 // send touchscreen controller an "enable touch" command
	usleep(10000);

	//command TOUCH_ENABLE
	putcharTS(0x55);//SYNC
	putcharTS(0x01);//SIZE
	putcharTS(0x12);//COMMAND

	usleep(10000);
}
/*****************************************************************************
** test if screen touched
*****************************************************************************/
//send command
int putcharTS(int c)
{
// poll Tx bit in 6850 status register. Wait for it to become '1'

// write 'c' to the 6850 TxData register to output the character

	while (!(TouchScreen_Status & 0x02));

	return c & 0xFF;
}
//receive command
int getcharTS( void )
{
 // poll Rx bit in 6850 status register. Wait for it to become '1'
 // read received character from 6850 RxData register.
	while (!(TouchScreen_Status & 0x1));
	return TouchScreen_RxData;
}


int ScreenTouched( void )
{
 // return TRUE if any data received from 6850 connected to touchscreen
 // or FALSE otherwise

		return (TouchScreen_RxData == 0x80);
	}

/*****************************************************************************
** wait for screen to be touched
*****************************************************************************/
void WaitForTouch()
{
while(!ScreenTouched())
 ;

}
/* a data type to hold a point/coord */
typedef struct { int x, y; } Point ;
/*****************************************************************************
* This function waits for a touch screen press event and returns X,Y coord
*****************************************************************************/

Point GetPen(void){
	Point p1;
	int packets[4];
	// wait for a pen down command then return the X,Y coord of the point
	// calibrated correctly so that it maps to a pixel on screen

	// Wait for first packet of touch
	WaitForTouch();

	int i;
	for(i = 0; i < 4; i++){
		packets[i] = getcharTS();
		printf("%s",packets[i]);
	}

	// Get x11 : x7 from 2nd packet, and concatenate to x6 : x0 from 1st packet
	p1.x = (packets[1] << 7) | packets[0];
	p1.y = (packets[3] << 7) | packets[2];

	// Map from controller resolution to screen pixel
	p1.x = p1.x * 799 / 4095;
	p1.y = p1.y  * 479 / 4095;

	printf("x = %d ", p1.x);
	printf("y = %d\n", p1.y);

	return p1;
}

Point GetPress(void){

Point p1;
	 // wait for a pen down command then return the X,Y coord of the point
	 // calibrated correctly so that it maps to a pixel on screen

	 return GetPen();
}
/*****************************************************************************
* This function waits for a touch screen release event and returns X,Y coord
*****************************************************************************/
Point GetRelease(void){

	 Point p1;
	 // wait for a pen up command then return the X,Y coord of the point
	 // calibrated correctly so that it maps to a pixel on screen
	 return GetPen();
}
/**
**
All the initializations goes from here
**
**/

int main() {
	printf("Test RS232\n");
	Init_RS232();
    Init_Touch();

    while(1){
     	  GetPress();
     	  GetRelease();
     }

     return 0;

}
