#include <stdio.h>
#include <stdlib.h>

#define GPS_Control (*(volatile unsigned char *)(0x84000210))
#define GPS_Status (*(volatile unsigned char *)(0x84000210))
#define GPS_TxData (*(volatile unsigned char *)(0x84000212))
#define GPS_RxData (*(volatile unsigned char *)(0x84000212))
#define GPS_Baud (*(volatile unsigned char *)(0x84000214))


#define Touchscreen_Status 		(*(volatile unsigned char *)(0x84000230))
#define Touchscreen_Control 	(*(volatile unsigned char *)(0x84000230))
#define Touchscreen_TxData 		(*(volatile unsigned char *)(0x84000232))
#define Touchscreen_RxData 		(*(volatile unsigned char *)(0x84000232))
#define Touchscreen_Baud    	(*(volatile unsigned char *)(0x84000234))

#define push_buttons (volatile int *) 0x0002060
#define leds (char *) 0x0002010
#define switches (volatile char *) 0x0002000
#define hex0_1 (char *) 0x0002030

// #defined constants representing values we write to the graphics 'command' register to get
// it to do something. You will add more values as you add hardware to the graphics chip
// Note DrawHLine, DrawVLine and DrawLine at the moment do nothing - you will modify these

#define DrawHLine		1
#define DrawVLine		2
#define DrawLine		3
#define PutAPixel		0xA
#define GetAPixel		0xB
#define ProgramPaletteColour   	0x10

#define WAIT_FOR_GRAPHICS		while((GraphicsStatusReg & 0x0001) != 0x0001);
#define XRES 800
#define YRES 480

/*******************************************************************************************
 ** This macro pauses until the graphics chip status register indicates that it is idle
 *******************************************************************************************/
//Predefined Colour Values
//Use the symbolic constants below as the values to write to the Colour Register
//When you ask the graphics chip to draw something. These values are also returned
//By the read pixel command

// the header file "Colours.h" contains symbolic names for all 256 colours e.g. RED
// while the source file ColourPaletteData.c contains the 24 bit RGB data
// that is pre-programmed into each of the 256 palettes

#define	BLACK		0
#define	WHITE		1
#define	RED			2
#define	LIME		3
#define	BLUE		4
#define	YELLOW		5
#define	CYAN		6
#define	MAGENTA		7

#define GraphicsCommandReg   		(*(volatile unsigned short int *)(0x84000000))
#define GraphicsStatusReg   		(*(volatile unsigned short int *)(0x84000000))
#define GraphicsX1Reg   		(*(volatile unsigned short int *)(0x84000002))
#define GraphicsY1Reg   		(*(volatile unsigned short int *)(0x84000004))
#define GraphicsX2Reg   		(*(volatile unsigned short int *)(0x84000006))
#define GraphicsY2Reg			(*(volatile unsigned short int *)(0x84000008))
#define GraphicsColourReg		(*(volatile unsigned short int *)(0x8400000E))
#define GraphicsBackGroundColourReg   		(*(volatile unsigned short int *)(0x84000010))

//GPS
#define arrayLength 100

int candidate1 = 0;
int candidate2 = 0;
int candidate3 = 0;

int voters[128];
int verify = 0;
int voterID;

char arr[arrayLength];

char hours[2] = {0};
char minutes[2] = {0};
char seconds[2] = {0};

char latitude[10];
char longitude[10];

char directionNS[1];
char directionEW[1];

int i ;

const unsigned short int Font10x14[][14] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0},	 															// ' '
    {0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0,0,0x30,0x30}, 								// '!'
    {0xcc,0xcc,0xcc,0xcc,0,0,0,0,0,0,0,0,0,0}, 													// '"'
    {0xcc,0xcc,0xcc,0xcc,0x3ff,0x3ff,0xcc,0xcc,0x3ff,0x3ff,0xcc,0xcc,0xcc,0xcc},				// '#'
    {0x30,0x30,0xfe,0x1ff,0x3b3,0x3b0,0x1fc,0xfe,0x37,0x337,0x3fe,0x1fc,0x30,0x30},				// '$'
    {0x186,0x3C6,0x3cc,0x18c,0x18,0x18,0x30,0x30,0x60,0x60,0xc6,0xcf,0x18f,0x186}, 				// '%'
    {0xf0,0x1f8,0x30c,0x31c,0x338,0x370,0x1e0,0x1e0,0x373,0x33b,0x31e,0x38e,0x1fb,0xf3}, 		// '&'
    {0x30,0x78,0x38,0x18,0x30,0x60,0,0,0,0,0,0,0,0}, 											// '''
    {0xc,0x3c,0x70,0x60,0xe0,0xc0,0xc0,0xc0,0xc0,0xe0,0x60,0x70,0x3c,0xc}, 						// '('
    {0x60,0x78,0x1c,0xc,0xe,0x6,0x6,0x6,0x6,0xe,0xc,0x1c,0x78,0x60},			// ')'
    {0,0,0x30,0x30,0x333,0x3b7,0x1fe,0xfc,0x3b7,0x333,0x30,0x30,0,0}, 		// '*'
    {0,0,0,0x30,0x30,0x30,0x30,0x3ff,0x3ff,0x30,0x30,0x30,0x30,0}, 		// '+'
    {0,0,0,0,0,0,0,0,0x30,0x78,0x38,0x18,0x30,0x60}, 			// ','
    {0,0,0,0,0,0,0,0x3ff,0x3ff,0,0,0,0,0},			// '-'
    {0,0,0,0,0,0,0,0,0,0,0x30,0x78,0x78,0x30}, 					// '.'
    {0xc,0xc,0x18,0x18,0x30,0x30,0x60,0x60,0xc0,0xc0,0x180,0x180,0x300,0x300} ,			// '/'
    {0xfc,0x1fe,0x387,0x307,0x30f,0x31f,0x33b,0x373,0x3e3,0x3c3,0x383,0x387,0x1fe,0xfc}, // '0'
    {0x30,0x70,0xf0,0xf0,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0xfc,0xfc},	// '1'
    {0xfc,0x1fe,0x387,0x303,0x3,0x7,0xe,0x1c,0x38,0x70,0xe0,0x1c0,0x3ff,0x3ff}, // '2'
    {0xfc,0x1fe,0x387,0x303,0x3,0x3,0x1e,0x1e,0x3,0x3,0x303,0x387,0x1fe,0xfc}, // '3'
    {0xc,0x1c,0x3c,0x7c,0xec,0x1cc,0x38c,0x30c,0x3ff,0x3ff,0xc,0xc,0xc,0xc}, // '4'
    {0x3ff,0x3ff,0x300,0x300,0x3fc,0x3fe,0x7,0x3,0x3,0x3,0x303,0x387,0x1fe,0xfc}, // '5'
    {0xfc,0x1fe,0x387,0x303,0x300,0x300,0x3fc,0x3fe,0x307,0x303,0x303,0x387,0x1fe,0xfc}, // '6'
    {0x3ff,0x3ff,0x3,0x7,0xe,0x1c,0x38,0x30,0x30,0x30,0x30,0x30,0x30,0x30},	// '7'
    {0xfc,0x1fe,0x387,0x303,0x303,0x387,0x1fe,0x1fe,0x387,0x303,0x303,0x387,0x1fe,0xfc}, // '8'
    {0xfc,0x1fe,0x387,0x303,0x303,0x387,0x1ff,0xff,0x7,0xe,0x1c,0x38,0xf0,0xe0}, // '9'
    {0,0,0x30,0x78,0x78,0x30,0,0,0x30,0x78,0x78,0x30,0,0}, // ':'
    {0,0,0x30,0x78,0x78,0x30,0,0,0x30,0x78,0x38,0x18,0x30,0x60}, // ';'
    {0xc,0x1c,0x38,0x70,0xe0,0x1c0,0x380,0x380,0x1c0,0xe0,0x70,0x38,0x1c,0xc}, // '<'
    {0,0,0,0,0x3ff,0x3ff,0,0,0x3ff,0x3ff,0,0,0,0}, // '='
    {0xc0,0xe0,0x70,0x38,0x1c,0xe,0x7,0x7,0xe,0x1c,0x38,0x70,0xe0,0xc0}, // '>'
    {0xfc,0x1fe,0x387,0x303,0x3,0x7,0x1e,0x38,0x30,0x30,0x30,0,0x30,0x30}, // '?'
    {0xfc,0x1fe,0x387,0x303,0x3,0x3,0xf3,0x1f3,0x3b3,0x333,0x333,0x3b7,0x1fe,0xfc}, // '@'
    {0xfc,0x1fe,0x387,0x303,0x303,0x303,0x303,0x3ff,0x3ff,0x303,0x303,0x303,0x303,0x303}, // 'A'
    {0x3fc,0x3fe,0x307,0x303,0x303,0x307,0x3fe,0x3fe,0x307,0x303,0x303,0x307,0x3fe,0x3fc}, // 'B'
    {0xfc,0x1fe,0x387,0x303,0x300,0x300,0x300,0x300,0x300,0x300,0x303,0x387,0x1fe,0xfc}, // 'C'
    {0x3fc,0x3fe,0x307,0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x307,0x3fe,0x3fc}, // 'D'
    {0x3ff,0x3ff,0x300,0x300,0x300,0x300,0x3fc,0x3fc,0x300,0x300,0x300,0x300,0x3ff,0x3ff}, // 'E'
    {0x3ff,0x3ff,0x300,0x300,0x300,0x300,0x3fc,0x3fc,0x300,0x300,0x300,0x300,0x300,0x300}, // 'F'
    {0xfc,0x1fe,0x387,0x303,0x300,0x300,0x31f,0x31f,0x303,0x303,0x303,0x387,0x1fe,0xfc}, // 'G'
    {0x303,0x303,0x303,0x303,0x303,0x303,0x3ff,0x3ff,0x303,0x303,0x303,0x303,0x303,0x303}, // 'H'
    {0xfc,0xfc,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0xfc,0xfc}, // 'I'
    {0xff,0xff,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x30c,0x39c,0x1f8,0xf0}, // 'J'
    {0x303,0x307,0x30e,0x31c,0x338,0x3f0,0x3e0,0x3e0,0x3f0,0x338,0x31c,0x30e,0x307,0x303}, // 'K'
    {0x300,0x300,0x300,0x300,0x300,0x300,0x300,0x300,0x300,0x300,0x300,0x300,0x3ff,0x3ff}, // 'L'
    {0x303,0x387,0x3cf,0x3ff,0x37b,0x333,0x333,0x333,0x303,0x303,0x303,0x303,0x303,0x303}, // 'M'
    {0x303,0x303,0x303,0x383,0x3c3,0x3e3,0x373,0x33b,0x31f,0x30f,0x307,0x303,0x303,0x303}, // 'N'
    {0xfc,0x1fe,0x387,0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x387,0x1fe,0xfc}, // 'O'
    {0x3fc,0x3fe,0x307,0x303,0x303,0x307,0x3fe,0x3fc,0x300,0x300,0x300,0x300,0x300,0x300}, // 'P'
    {0xfc,0x1fe,0x387,0x303,0x303,0x303,0x303,0x303,0x333,0x33b,0x31f,0x38e,0x1ff,0xfb} , // 'Q'
    {0x3fc,0x3fe,0x307,0x303,0x303,0x307,0x3fe,0x3fc,0x378,0x31c,0x306,0x307,0x303,0x303} , // R
    {0xfc,0x1ff,0x383,0x300,0x300,0x380, 0x1fc,0xfe,0x7,0x3,0x3,0x307,0x3fe,0x1fc}, // 'S'
    {0x3ff,0x3ff,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30}, // 'T'
    {0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x387,0x1fe,0xfc}, // 'U'
    {0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x387,0x1ce,0xfc,0x78,0x30}, // 'V'
    {0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x333,0x333,0x37b,0x3ff,0x1fe,0xcc}, // 'W'
    {0x303,0x303,0x303,0x387,0x1ce,0xfc,0x78,0x78,0xfc,0x1ce,0x387,0x303,0x303,0x303}, // 'X'
    {0x303,0x303,0x303,0x303,0x303,0x387,0x1ce,0xfc,0x78,0x30,0x30,0x30,0x30,0x30},	// 'Y'
    {0x3ff,0x3ff,0x3,0x7,0xe,0x1c,0x38,0x70,0xe0,0x1c0,0x380,0x300,0x3ff,0x3ff}, // 'Z'
    {0x7c,0x7c,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x7c,0x7c}, // '['
    {0x180,0x180,0xc0,0xc0,0x60,0x60,0x30,0x30,0x18,0x18,0xc,0xc,0x6,0x6}, // '\'
    {0xf8,0xf8,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xf8,0xf8}, // ']'
    {0x30,0x78,0xfc,0x1ce,0x387,0x303,0,0,0,0,0,0,0,0}, // '^'
    {0,0,0,0,0,0,0,0,0,0,0,0,0x3ff,0x3ff}, // '_'
    {0xc0,0xe0,0x70,0x38,0x18,0,0,0,0,0,0,0,0,0}, // '`'
    {0,0,0,0,0xfc,0x1fe,0x187,0x3,0xff,0x1ff,0x383,0x383,0x1ff,0xfe}, // 'a'
    {0x300,0x300,0x300,0x300,0x300,0x300,0x3fc,0x3fe,0x307,0x303,0x303,0x307,0x3fe,0x3fc}, // 'b'
    {0,0,0,0,0xfc,0x1fe,0x387,0x303,0x300,0x300,0x303,0x387,0x1fe,0xfc}, // 'c'
    {0x3,0x3,0x3,0x3,0x3,0x3,0xff,0x1ff,0x383,0x303,0x303,0x383,0x1ff,0xff}, // 'd'
    {0,0,0,0,0xfc,0x1fe,0x387,0x303,0x3ff,0x3fe,0x300,0x387,0x1fe,0xfc}, // 'e'
    {0x30,0x78,0xfc,0xcc,0xc0,0xc0,0x1f0,0x1f0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0}, // 'f'
    {0,0,0,0,0xfe,0x1ff,0x383,0x383,0x1ff,0xff,0x3,0x307,0x3fe,0x1fc} , // 'g'
    {0x180,0x180,0x180,0x180,0x180,0x180,0x1fc,0x1fe,0x187,0x183,0x183,0x183,0x183,0x183}, // 'h'
    {0,0,0,0x30,0x30,0,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30}, // 'i'
    {0xc,0xc,0,0,0xc,0xc,0xc,0xc,0xc,0xc,0x30c,0x39c,0x1f8,0xf0}, // 'j'
    {0x180,0x180,0x180,0x180,0x186,0x18e,0x19c,0x1b8,0x1f0,0x1f0,0x1b8,0x19c,0x18e,0x186}, // 'k'
    {0xf0,0xf0,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0xfc,0xfc}, // 'l'
    {0,0,0,0,0x387,0x3cf,0x3ff,0x37b,0x333,0x333,0x303,0x303,0x303,0x303}, // 'm'
    {0,0,0,0,0x33c,0x37e,0x3e7,0x3c3,0x303,0x303,0x303,0x303,0x303,0x303,}, // 'n'
    {0,0,0,0,0xfc,0x1fe,0x387,0x303,0x303,0x303,0x303,0x387,0x1fe,0xfc} , // 'o'
    {0,0,0,0,0x3fc,0x3fe,0x307,0x307,0x3fe,0x3fc,0x300,0x300,0x300,0x300}, // 'p'
    {0,0,0,0,0xff,0x1ff,0x383,0x383,0x1ff,0xff,0x3,0x3,0x3,0x3}, // 'q'
    {0,0,0,0,0x33c,0x37e,0x3e7,0x3c3,0x380,0x300,0x300,0x300,0x300,0x300}, // 'r'
    {0,0,0,0,0xfe,0x1ff,0x383,0x380,0x1fc,0xfe,0x7,0x307,0x3fe,0x1fc}, // 's'
    {0x60,0x60,0x60,0x60,0x1fe,0x1fe,0x60,0x60,0x60,0x60,0x66,0x7e,0x3c,0x18}, // 't'
    {0,0,0,0,0x303,0x303,0x303,0x303,0x303,0x303,0x303,0x387,0x1fe,0xfc}, // 'u'
    {0,0,0,0,0x303,0x303,0x303,0x303,0x303,0x387,0x1ce,0xfc,0x78,0x30}, // 'v'
    {0,0,0,0,0x303,0x303,0x303,0x303,0x333,0x333,0x333,0x3ff,0x1fe,0xcc}, // 'w'
    {0,0,0,0,0x303,0x387,0x1ce,0xfc,0x78,0x78,0xfc,0x1ce,0x387,0x303}, // 'x'
    {0,0,0,0,0x303,0x387,0x1ce,0xfc,0x78,0x30,0x30,0x70,0xe0,0xc0}, // 'y'
    {0,0,0,0,0x3ff,0x3ff,0xe,0x1c,0x38,0x70,0xe0,0x1c0,0x3ff,0x3ff}, // 'z'
    {0x1e,0x3e,0x70,0x60,0x60,0xe0,0x1c0,0x1c0,0xe0,0x60,0x60,0x70,0x3e,0x1e}, // '{'
    {0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30}, // '|'
    {0x1e0,0x1f0,0x38,0x18,0x18,0x1c,0xe,0xe,0x1c,0x18,0x18,0x38,0x1f0,0x1e0}, // '}'
    {0,0,0,0,0,0,0,0xc0,0x333,0xc,0,0,0,0},			// '~'
};

/**********************************************************************
 * This function writes a single pixel to the x,y coords specified in the specified colour
 * Note colour is a palette number (0-255) not a 24 bit RGB value
 **********************************************************************/
void WriteAPixel (int x, int y, int Colour)
{
    WAIT_FOR_GRAPHICS;			// is graphics ready for new command

    GraphicsX1Reg = x;			// write coords to x1, y1
    GraphicsY1Reg = y;
    GraphicsColourReg = Colour;		// set pixel colour with a palette number
    GraphicsCommandReg = PutAPixel;		// give graphics a "write pixel" command
}

/******************************************************************************************************************************
 ** This function draws a single ASCII character at the coord specified using the colour specified
 ** OutGraphicsCharFont2(100,100, RED, 'A', TRUE, FALSE, 1, 1) ;	// display upper case 'A' in RED at coords 100,100, erase background
 ** no scroll, scale x,y= 1,1
 **
 ******************************************************************************************************************************/
void OutGraphicsCharFont2(int x, int y, int colour, int backgroundcolour, int c, int Erase)
{
    register int 	row, column, theX = x, theY = y ;
    register int 	pixels ;
    register char 	theColour = colour  ;
    register int 	BitMask, theCharacter = c, theRow, theColumn;


    if(((short)(x) > (short)(XRES-1)) || ((short)(y) > (short)(YRES-1)))  // if start off edge of screen don't bother
        return ;

    if(((short)(theCharacter) >= (short)(' ')) && ((short)(theCharacter) <= (short)('~'))) {			// if printable character
        theCharacter -= 0x20;																			// subtract hex 20 to get index of first printable character (the space character)
        theRow = 14;
        theColumn = 10;

        for(row = 0; row < theRow ; row ++)	{
            pixels = Font10x14[theCharacter][row] ;		     								// get the pixels for row 0 of the character to be displayed
            BitMask = 512 ;							   											// set of hex 200 i.e. bit 7-0 = 0010 0000 0000
            for(column = 0; column < theColumn;   )  	{
                if((pixels & BitMask))														// if valid pixel, then write it
                    WriteAPixel(theX+column, theY+row, theColour) ;
                else {																		// if not a valid pixel, do we erase or leave it along (no erase)
                    if(Erase)
                        WriteAPixel(theX+column, theY+row, backgroundcolour) ;
                    // else leave it alone
                }
                column ++ ;
                BitMask = BitMask >> 1 ;
            }
        }
    }
}

/*****************************************************************************************
 * This function draws the string provided
 ******************************************************************************************/
void DisplayString (int x, int y, int fg, int bg, char* phrase, int overwrite)
{
    int i = 0;
    while(phrase[i] != '\0') {
        WAIT_FOR_GRAPHICS;      // Is graphics ready for new command?
        OutGraphicsCharFont2(x, y, fg, bg, (int)phrase[i], overwrite);
        i++;
        x += 12;    //width of character
    }
}



int putCharTouch(char c){
    // Wait for TX bit in status register to turn 1 (means transmit is empty)
    while((Touchscreen_Status & 0x02) != 0x02);
    // Send the data to TX (reduce to 8 bits)
    return c & 0xFF;
}

int getCharTouch(void)
{
    // poll Rx bit in 6850 status register. Wait for it to become '1'
    // read received character from 6850 RxData register.
    while ((Touchscreen_Status & 0x01) != 0x01);
    return Touchscreen_RxData;
}

/*****************************************************************************
 ** Initialise touch screen controller
 *****************************************************************************/
void Init_Touch(void)
{
    // Program 6850 and baud rate generator to communicate with touchscreen
    // send touchscreen controller an "enable touch" command

    // Divide by 16 clock, RTS Low, 8 bits of data, no parity,
    // 1 stop bit, transmitter interrupt disabled
    Touchscreen_Control = 0x15;
    // 9600 BAUD
    Touchscreen_Baud = 0x05;

    // slight delay to process
    usleep(10000);

    // Send TOUCH_ENABLE command
    putCharTouch(0x55);
    putCharTouch(0x01);
    putCharTouch(0x12);

    usleep(10000);
}

/*****************************************************************************
 ** test if screen touched
 *****************************************************************************/
int ScreenTouched( void )
{
    // return TRUE if any data received from 6850 connected to touchscreen
    // or FALSE otherwise

    return (Touchscreen_RxData == 0x80);
}

/*****************************************************************************
 ** wait for screen to be touched
 *****************************************************************************/
//void WaitForTouch()
//{
//while(!ScreenTouched())
//;
//}

/* a data type to hold a point/coord */
typedef struct { int x, y; } Point ;

Point GetPen(void){
    Point p1;
    int packets[4];
    // wait for a pen down command then return the X,Y coord of the point
    // calibrated correctly so that it maps to a pixel on screen

    // Wait for first packet of touch
    //WaitForTouch();

    int i;
    for(i = 0; i < 4; i++){
        packets[i] = getCharTouch();
    }

    // Get x11 : x7 from 2nd packet, and concatenate to x6 : x0 from 1st packet
    p1.x = (packets[1] << 7) | packets[0];
    p1.y = (packets[3] << 7) | packets[2];

    // Map from controller resolution to screen pixel
    p1.x = p1.x * 799 / 4095;
    p1.y = p1.y  * 479 / 4095;

    //printf("x = %d\n", p1.x);
    //printf("y = %d\n", p1.y);



    return p1;
}

/*****************************************************************************
 * This function waits for a touch screen press event and returns X,Y coord
 *****************************************************************************/

void compare(){
    Point p1 = GetPen();

    int yValue = p1.y;
    //printf("y= %d\n", yValue);

    if(0<yValue && yValue<200){

        //while(0<yValue && yValue<200){
        //p1 = GetPen();
        //yValue = p1.y;
        //}

        if(check(voterID)){
            incrementCandidate1();
            printVote();
            storeID();
        }

        else{
            //printf("\n\nYou have already voted once!! :D\n");
        }

    }else if(200<yValue && yValue<350){

        //	while(200<yValue && yValue<350){
        //	p1 = GetPen();
        //yValue = p1.y;
        //}

        if(check(voterID)){
            incrementCandidate2();
            printVote();
            storeID();
        }

        else{
            //	printf("\n\nYou have already voted once!! :D\n");
        }
    }else if(350<yValue && yValue<500){

        //while(350<yValue && yValue<500){
        //p1 = GetPen();
        //yValue = p1.y;
        //}

        if(check(voterID)){
            incrementCandidate3();
            printVote();
            storeID();
        }

        else{
            //printf("\n\nYou have already voted once!! :D\n");
        }
    }else {
        printf("voter wants to vote on board");
    }
}
Point GetPress(void)
{
    Point p1;
    // wait for a pen down command then return the X,Y coord of the point
    // calibrated correctly so that it maps to a pixel on screen

    return GetPen();
}
/*****************************************************************************
 * This function waits for a touch screen release event and returns X,Y coord
 *****************************************************************************/
Point GetRelease(void)
{
    Point p1;
    // wait for a pen up command then return the X,Y coord of the point
    // calibrated correctly so that it maps to a pixel on screen
    return GetPen();
}





void printVote(){

    getInfo();
    extractInformation();

    printf("\n\n\n\n\nCandidate 1: %d", candidate1);
    printf("\nCandidate 2: %d", candidate2);
    printf("\nCandidate 3: %d", candidate3);
    printf("\n\nVoter ID: %d\n\n", *switches);
    printf("Voting Stats: \n");
    printGPS();
}

void incrementCandidate1(){
    candidate1 = candidate1 + 1;
}

void incrementCandidate2(){
    candidate2 = candidate2 + 1;
}

void incrementCandidate3(){
    candidate3 = candidate3 + 1;
}




int check(){
    int verify;
    int compare = *switches;

    int counter;
    for(counter = 0; counter < 128; counter++){
        if(voters[counter] == compare){
            return 0;
        }
    }

    return 1;
}


void storeID(){
    voters[verify] = *switches;
    verify++;
}


//GPS
void Init_GPS(void){
    GPS_Control = 0x55;
    GPS_Baud = 0x05;
}

//Receiving from GPS
char getcharGPS(){

    while((GPS_Status & 0x01) != 1){
    }

    return GPS_RxData;
}

void intialiseArray(){
    int j;
    for(j = 0; j < 100; j++){
        arr[j] = '-';
    }
}


void getInfo(){
    char count;
    i = 0;
    while(1){

        count = getcharGPS();
        if(count == '$'){
            if(getcharGPS() == 'G')
                if(getcharGPS() == 'P')
                    if(getcharGPS() == 'G')
                        if(getcharGPS() == 'G')
                            if(getcharGPS() == 'A'){
                                count = getcharGPS();
                                while(count != '\n'){
                                    arr[i] =  count;
                                    count = getcharGPS();
                                    i++;
                                }
                                break;
                            }
        }

    }

}

void extractInformation(){


    char end = '-';
    int i = 1;

    while(1){

        int k;
        //Extract hours
        for(k = 0; k < 2; k++){
            hours[k] = arr[i];
            i++;
        }

        //Extract minutes
        for( k = 0; k < 2; k++){
            minutes[k] = arr[i];
            i++;
        }

        //Extract seconds
        for( k = 0; k < 2; k++){
            seconds[k] = arr[i];
            i++;
        }

        //To skip the .sss from the seconds
        i = i+5;

        //Extract Latitude
        for( k = 0; k < 9; k++){
            latitude[k] = arr[i];
            i++;
        }

        //To skip the comma
        i++;

        //Extract NS Direction
        directionNS[0] = arr[i];

        //To skip the comma
        i = i + 2;

        //Extract the longitude
        for( k = 0; k < 10; k++){
            longitude[k] = arr[i];
            i++;
        }

        i = i+1;

        //Extract EW Direction
        directionEW[0] = arr[i];

        break;
    }
}

void printGPS(){

    int counter;

    //Printing Time
    printf("\nTime: ");
    for(counter = 0; counter < 2; counter++){
        printf("%c", hours[counter]);
    }

    printf(":");

    for(counter = 0; counter < 2; counter++){
        printf("%c", minutes[counter]);
    }

    printf(":");

    for(counter = 0; counter < 2; counter++){
        printf("%c", seconds[counter]);
    }

    //Latitude
    printf("\nLatitude: ");
    for(counter = 0; counter < 9; counter++){
        printf("%c", latitude[counter]);
    }

    //Longitude
    printf("\nLongitude: ");
    for(counter = 0; counter < 10; counter++){
        printf("%c", longitude[counter]);
    }

    //DirectionNS
    printf("\nDirection(NS): ");
    for(counter = 0; counter < 1; counter++){
        printf("%c", directionNS[counter]);
    }

    //DirectionEW
    printf("\nDirection(EW): ");
    for(counter = 0; counter < 1; counter++){
        printf("%c", directionEW[counter]);
    }
}







void voteForIt(){

    while(1){
        *hex0_1 = *switches;
        voterID = *switches;

        //Candidate3
        if(~(*push_buttons) & 4){
            *leds = 4;

            while(~(*push_buttons) & 4);

            if(check(voterID)){
                incrementCandidate3();
                printVote();
                storeID();
            }

            else{
                printf("\n\nYou have already voted once!! :D\n");
            }
        }

        //Candidate2
        else if(~(*push_buttons) & 2){
            *leds = 2;

            while(~(*push_buttons) & 2);

            if(check(voterID)){
                incrementCandidate2();
                printVote();
                storeID();
            }

            else{
                printf("\n\nYou have already voted once!! :D\n");
            }

        }

        //Candidate1
        else if(~(*push_buttons) & 1){
            *leds = 1;

            while(~(*push_buttons) & 1);

            if(check(voterID)){
                incrementCandidate1();
                printVote();
                storeID();
            }

            else{
                printf("\n\nYou have already voted once!! :D\n");
            }

        }
        else if(ScreenTouched()){
            compare();

        }

        else{
            *leds= 0;
        }
    }
}

int main()
{
    printf("Hello from Nios II!\n");

    /* GRAPHICS START */

    int i, j;

    // Clear screen
    for(j = 0; j < 480; j++)
        for(i = 0; i < 800; i ++)
            WriteAPixel(i, j, BLACK);

    // Boxes to hold candidates
    for(j = 0; j < 480; j++) {
        WriteAPixel(266, j, WHITE);
        WriteAPixel(533, j, WHITE);
    }

    DisplayString(100, 220, WHITE, WHITE, "CANDIDATE 1", 0);
    DisplayString(142, 320, WHITE, RED, "VOTE", 1);
    DisplayString(350, 220, WHITE, WHITE, "CANDIDATE 2", 0);
    DisplayString(392, 320, WHITE, RED, "VOTE", 1);
    DisplayString(600, 220, WHITE, WHITE, "CANDIDATE 3", 0);
    DisplayString(642, 320, WHITE, RED, "VOTE", 1);

    /* GRAPHICS END */

    Init_Touch();
    Init_GPS();
    intialiseArray();
    voteForIt();

    return 0;
}
