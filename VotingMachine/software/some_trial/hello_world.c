

#include<stdio.h>
#include<time.h>
#include<stdlib.h>

#define RS232_Control (*(volatile unsigned char *)(0x84000200))
#define RS232_Status (*(volatile unsigned char *)(0x84000200))
#define RS232_TxData (*(volatile unsigned char *)(0x84000202))
#define RS232_RxData (*(volatile unsigned char *)(0x84000202))
#define RS232_Baud (*(volatile unsigned char *)(0x84000204))

#define GPS_Control (*(volatile unsigned char *)(0x84000210))
#define GPS_Status (*(volatile unsigned char *)(0x84000210))
#define GPS_TxData (*(volatile unsigned char *)(0x84000212))
#define GPS_RxData (*(volatile unsigned char *)(0x84000212))
#define GPS_Baud (*(volatile unsigned char *)(0x84000214))

#define hex0_1 (char *) 0x0002030
#define hex2_3 (char *) 0x0002040
#define hex4_5 (char *) 0x0002050


/**************************************************************************
/* Subroutine to initialise the RS232 Port by writing some data
** to the internal registers.
** Call this function at the start of the program before you attempt
** to read or write to data via the RS232 port
**
** Refer to 6850 data sheet for details of registers and
***************************************************************************/

#define arrayLength 100

char arr[arrayLength];

char hours[3] = {0};
char minutes[3] = {0};
char seconds[3] = {0};

int latitude[10];
int longitude[10];

char *latitude2;
char longitude2[10];

int lat;

char directionNS[1];
char directionEW[1];


int i = 0;



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
	 while(RS232_Status & 0x01 != 1)
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

void clearGPSData(){
	GPS_TxData = "$PMTK184,1*22\r\n";
}

void startLogging(){
	GPS_TxData = "$PMTK185,0*22\r\n";
}

void outputGPSData(){
	GPS_TxData = "$PMTK622,1*29\r\n";
}


int swapEndian(char *s)
{
register int val;
val = strtoul(s, NULL, 16);
val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF );
val = (val << 16) | ((val >> 16) & 0xFFFF);
return val ;
}



char *FloatToLatitudeConversion(int x) //output format is xx.yyyy
{
static char buff[100] ;
float *ptr = (float *)(&x) ; // cast int to float
float f = *ptr ; // get the float
sprintf(buff, "%2.4f", f); // write in string to an array
return buff ;
}


char *FloatToLongitudeConversion(int x) // output format is (-)xxx.yyyy
{
static char buff[100] ;
float *ptr = (float *)(&x) ;
float f = *ptr ;
sprintf(buff, "%3.4f", f);
return buff ;
}

void intialiseArray(){
	int j;
	for(j = 0; j < 100; j++){
		arr[j] = '-';
	}
}

void getInfo(){
	char count;
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

		int skip = i + 4;
		//Extract Latitude
		for( k = 0; k < 9; k++){
			if(i == skip){
				i++;
				continue;
			}
			latitude[k] = arr[i];
			i++;
			}

		//To skip the comma
		i++;

		//Extract NS Direction
		directionNS[0] = arr[i];

		//To skip the comma
		i = i + 2;

		skip = i+5;
		//Extract the longitude
		for( k = 0; k < 10; k++){
			if(i == skip){
				i++;
				continue;
				}
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


void printOnHex(){
	*hex0_1 = 00000001;
	*hex2_3 = 00000011;
	*hex4_5 = 00000100;
}

int main(){

Init_GPS();
intialiseArray();
getInfo();
extractInformation();


for(i = 0; i < 100; i++)
{
	printf("%c",arr[i]);
}

//latitude2 = FloatToLatitudeConversion(latitude);
printf("\n%d", lat);

printGPS();
printOnHex();

return (0);

}

