/*
 This library allows an Arduino to easily display numbers and characters on a 4 digit 7-segment
 display without a separate 7-segment display controller.

 If you have feature suggestions or need support please use the github support page: https://github.com/sparkfun/SevSeg

 Original Library by Dean Reading (deanreading@hotmail.com: http://arduino.cc/playground/Main/SevenSegmentLibrary), 2012
 Improvements by Nathan Seidle, 2012

 Now works for any digital pin arrangement, common anode and common cathode displays.
 Added character support including letters A-F and many symbols.

 Hardware Setup: 4 digit 7 segment displays use 12 digital pins. You may need more pins if your display has colons or
 apostrophes.

 There are 4 digit pins and 8 segment pins. Digit pins are connected to the cathodes for common cathode displays, or anodes
 for common anode displays. 8 pins control the individual segments (seven segments plus the decimal point).

 Connect the four digit pins with four limiting resistors in series to any digital or analog pins. Connect the eight segment
 pins to any digital or analog pins (no limiting resistors needed). See the SevSeg example for more connection information.

 SparkFun has a large, 1" 7-segment display that has four digits.
 https://www.sparkfun.com/products/11408
 Looking at the display like this: 8.8.8.8. pin 1 is on the lower row, starting from the left.
 Pin 12 is the top row, upper left pin.

 Pinout:
 1: Segment E
 2: Segment D
 3: Segment DP
 4: Segment C
 5: Segment G
 6: Digit 4
 7: Segment B
 8: Digit 3
 9: Digit 2
 10: Segment F
 11: Segment A
 12: Digit 1


 Software:
 Call SevSeg.Begin in setup.
 The first argument (boolean) tells whether the display is common cathode (0) or common
 anode (1).
 The next four arguments (bytes) tell the library which arduino pins are connected to
 the digit pins of the seven segment display.  Put them in order from left to right.
 The next eight arguments (bytes) tell the library which arduino pins are connected to
 the segment pins of the seven segment display.  Put them in order a to g then the dp.

 In summary, Begin(type, digit pins 1-4, segment pins a-g, dp)

 The calling program must run the DisplayString() function repeatedly to get the number displayed.
 Any number between -999 and 9999 can be displayed.
 To move the decimal place one digit to the left, use '1' as the second
 argument. For example, if you wanted to display '3.141' you would call
 myDisplay.DisplayString("3141", 1);


 */

#include "SevSeg.h"

SevSeg::SevSeg()
{
	//Initial values
	DecAposColon = 0; //This variable tracks the decimal place, apostrophe, and colon (if the display has support)
}
void SevSeg::Begin(boolean mode_in, byte numOfDigits,
				   byte dig1, byte dig2, byte dig3, byte dig4,
				   byte digitCol, byte digitApos,
				   byte segA, byte segB, byte segC, byte segD, byte segE, byte segF, byte segG,
				   byte segDP,
				   byte segCol, byte segApos)
{
	//Bring all the variables in from the caller
	numberOfDigits = numOfDigits;
	digit1 = dig1;
	digit2 = dig2;
	digit3 = dig3;
	digit4 = dig4;
	digitApostrophe = digitApos;
	digitColon = digitCol;
	segmentA = segA;
	segmentB = segB;
	segmentC = segC;
	segmentD = segD;
	segmentE = segE;
	segmentF = segF;
	segmentG = segG;
	segmentDP = segDP;
	segmentApostrophe = segApos;
	segmentColon = segCol;

	//Assign input values to variables
	//mode is what the digit pins must be set at for it to be turned on. 0 for common cathode, 1 for common anode
	mode = mode_in;
	if (mode == COMMON_ANODE)
	{
		DigitOn = HIGH;
		DigitOff = LOW;
		SegOn = LOW;
		SegOff = HIGH;
	}
	else
	{
		DigitOn = LOW;
		DigitOff = HIGH;
		SegOn = HIGH;
		SegOff = LOW;
	}

	DigitPins[0] = digit1;
	DigitPins[1] = digit2;
	DigitPins[2] = digit3;
	DigitPins[3] = digit4;
	SegmentPins[0] = segmentA;
	SegmentPins[1] = segmentB;
	SegmentPins[2] = segmentC;
	SegmentPins[3] = segmentD;
	SegmentPins[4] = segmentE;
	SegmentPins[5] = segmentF;
	SegmentPins[6] = segmentG;
	SegmentPins[7] = segmentDP;

	//Turn everything Off before setting pin as output
	//Set all digit pins off. Low for common anode, high for common cathode
	for (byte digit = 0; digit < numberOfDigits; digit++)
	{
		digitalWrite(DigitPins[digit], DigitOff);
		pinMode(DigitPins[digit], OUTPUT);
	}
	//Set all segment pins off. High for common anode, low for common cathode
	for (byte seg = 0; seg < 8; seg++)
	{
		digitalWrite(SegmentPins[seg], SegOff);
		pinMode(SegmentPins[seg], OUTPUT);
	}

	if (digitColon != 255)
	{
		digitalWrite(digitColon, DigitOff);
		pinMode(digitColon, OUTPUT);
		digitalWrite(segmentColon, SegOff);
		pinMode(segmentColon, OUTPUT);
	}
	if (digitApostrophe != 255)
	{
		digitalWrite(digitApostrophe, DigitOff);
		pinMode(digitApostrophe, OUTPUT);
		digitalWrite(segmentApostrophe, SegOff);
		pinMode(segmentApostrophe, OUTPUT);
	}
}

//Begin
/*******************************************************************************************/
//Set pin modes and turns all displays off
//This second begin is used when the display does not support a colon and apostrophe
//The digitApostrophe, segmentApostrophe, and dig/segColon are set to 255 and the normal .Begin is called
void SevSeg::Begin(boolean mode_in, byte numOfDigits,
				   byte dig1, byte dig2, byte dig3, byte dig4,
				   byte segA, byte segB, byte segC, byte segD, byte segE, byte segF, byte segG,
				   byte segDP)
{
	Begin(mode_in, numOfDigits, dig1, dig2, dig3, dig4, 255, 255, segA, segB, segC,
		  segD, segE, segF, segG, segDP, 255, 255);
}

//Set the display brightness
/*******************************************************************************************/
//Given a value between 0 and 100 (0% and 100%), set the brightness variable on the display
//We need to error check and map the incoming value
void SevSeg::SetBrightness(byte percentBright)
{
	//Error check and scale brightnessLevel
	if (percentBright > 100)
		percentBright = 100;
	brightnessDelay = map(percentBright, 0, 100, 0, FRAMEPERIOD); //map brightnessDelay to 0 to the max which is framePeriod
}

//Refresh Display
/*******************************************************************************************/
//Given a string such as "-A32", we display -A32
//Each digit is displayed for ~2000us, and cycles through the 4 digits
//After running through the 4 numbers, the display is turned off
//Will turn the display on for a given amount of time - this helps control brightness
void SevSeg::DisplayString(const char *toDisplay, byte DecAposColon)
{
	//For the purpose of this code, digit = 1 is the left most digit, digit = 4 is the right most digit
	for (byte digit = 1; digit < (numberOfDigits + 1); digit++)
	{
		switch (digit)
		{
		case 1:
			digitalWrite(digit1, DigitOn);
			break;
		case 2:
			digitalWrite(digit2, DigitOn);
			break;
		case 3:
			digitalWrite(digit3, DigitOn);
			break;
		case 4:
			digitalWrite(digit4, DigitOn);
			break;
			//This only currently works for 4 digits
		}

		//Here we access the array of segments
		//This could be cleaned up a bit but it works
		//displayCharacter(toDisplay[digit-1]); //Now display this digit
		// displayArray (defined in SevSeg.h) decides which segments are turned on for each number or symbol
		unsigned char characterToDisplay = toDisplay[digit - 1];
		if (characterToDisplay & 0x80) // bit 7 enables bit-per-segment control
		{							   // Each bit of characterToDisplay turns on a single segment (from A-to-G)
			if (characterToDisplay & 0x01)
				digitalWrite(segmentA, SegOn);
			if (characterToDisplay & 0x02)
				digitalWrite(segmentB, SegOn);
			if (characterToDisplay & 0x04)
				digitalWrite(segmentC, SegOn);
			if (characterToDisplay & 0x08)
				digitalWrite(segmentD, SegOn);
			if (characterToDisplay & 0x10)
				digitalWrite(segmentE, SegOn);
			if (characterToDisplay & 0x20)
				digitalWrite(segmentF, SegOn);
			if (characterToDisplay & 0x40)
				digitalWrite(segmentG, SegOn);
		}
		else
		{
			const uint8_t chr = pgm_read_byte(&characterArray[characterToDisplay]);
			if (chr & (1 << 6))
				digitalWrite(segmentA, SegOn);
			if (chr & (1 << 5))
				digitalWrite(segmentB, SegOn);
			if (chr & (1 << 4))
				digitalWrite(segmentC, SegOn);
			if (chr & (1 << 3))
				digitalWrite(segmentD, SegOn);
			if (chr & (1 << 2))
				digitalWrite(segmentE, SegOn);
			if (chr & (1 << 1))
				digitalWrite(segmentF, SegOn);
			if (chr & (1 << 0))
				digitalWrite(segmentG, SegOn);
		}
		//Service the decimal point, apostrophe and colon
		if ((DecAposColon & (1 << (digit - 1))) && (digit < 5)) //Test DecAposColon to see if we need to turn on a decimal point
			digitalWrite(segmentDP, SegOn);

		delayMicroseconds(brightnessDelay + 1); //Display this digit for a fraction of a second (between 1us and 5000us, 500-2000 is pretty good)
		//The + 1 is a bit of a hack but it removes the possible zero display (0 causes display to become bright and flickery)
		//If you set this too long, the display will start to flicker. Set it to 25000 for some fun.

		//Turn off all segments
		digitalWrite(segmentA, SegOff);
		digitalWrite(segmentB, SegOff);
		digitalWrite(segmentC, SegOff);
		digitalWrite(segmentD, SegOff);
		digitalWrite(segmentE, SegOff);
		digitalWrite(segmentF, SegOff);
		digitalWrite(segmentG, SegOff);
		digitalWrite(segmentDP, SegOff);

		//Turn off this digit
		switch (digit)
		{
		case 1:
			digitalWrite(digit1, DigitOff);
			break;
		case 2:
			digitalWrite(digit2, DigitOff);
			break;
		case 3:
			digitalWrite(digit3, DigitOff);
			break;
		case 4:
			digitalWrite(digit4, DigitOff);
			break;
			//This only currently works for 4 digits
		}
		// The display is on for microSeconds(brightnessLevel + 1), now turn off for the remainder of the framePeriod
		delayMicroseconds(FRAMEPERIOD - brightnessDelay + 1); //the +1 is a hack so that we can never have a delayMicroseconds(0), causes display to flicker
	}

	//After we've gone through the digits, we control the colon and apostrophe (if the display supports it)

	//Turn on the colon and/or apostrophe
	if ((digitColon != 255) || (digitApostrophe != 255))
	{
		if (DecAposColon & (1 << 4)) //Test to see if we need to turn on the Colon
		{
			digitalWrite(digitColon, DigitOn);
			digitalWrite(segmentColon, SegOn);
		}
		if (DecAposColon & (1 << 5)) //Test DecAposColon to see if we need to turn on Apostrophe
		{
			digitalWrite(digitApostrophe, DigitOn);
			digitalWrite(segmentApostrophe, SegOn);
		}
		delayMicroseconds(brightnessDelay + 1); //Display this digit for a fraction of a second (between 1us and 5000us, 500-2000 is pretty good)

		//Turn off the colon and/or apostrophe
		digitalWrite(digitColon, DigitOff);
		digitalWrite(segmentColon, SegOff);
		digitalWrite(digitApostrophe, DigitOff);
		digitalWrite(segmentApostrophe, SegOff);
		delayMicroseconds(FRAMEPERIOD - brightnessDelay + 1); //the +1 is a hack so that we can never have a delayMicroseconds(0), causes display to flicker
	}
}

// Example Code

// /*
//  7-23-2012
//  Spark Fun Electronics
//  Nathan Seidle
 
//  This code is originally based Dean Reading's Library deanreading@hotmail.com
//  http://arduino.cc/playground/Main/SevenSegmentLibrary
//  He didn't have a license on it so I hope he doesn't mind me making it public domain: 
//  This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 
//  This example is a centi-second counter to demonstrate the use of the SevSeg library. To light
//  the display you have to call myDisplay.DisplayNumber(#, decimalPlace) multiple times a second. Put this
//  in the main loop.
 
//  SparkFun has a large, 1" 7-segment display that has four digits.
//  https://www.sparkfun.com/products/11408
//  Looking at the display like this: 8.8.8.8. pin 1 is on the lower row, starting from the left. 
//  Pin 12 is the top row, upper left pin.
 
//  Pinout:
//  1: Segment E
//  2: Segment D
//  3: Segment DP
//  4: Segment C
//  5: Segment G
//  6: Digit 4
//  7: Segment B
//  8: Digit 3
//  9: Digit 2
//  10: Segment F
//  11: Segment A
//  12: Digit 1
 
//  ToDo:
//  Picture of setup with pin 1 indicator
//  Covert big byte array to binary: http://arduino.cc/forum/index.php/topic,39760.0.html
//  Measure current going through limiting resistors to make sure we're getting 20mA per segment per digit (should be 80mA for four digits)
 
//  2264 bytes
//  2134 bytes with new BigTime functions
//  2214 if full DP support
 
//  */

// #include "SevSeg.h"

// //Create an instance of the object.
// SevSeg myDisplay;

// //Create global variables
// unsigned long timer;
// int deciSecond = 0;

// void setup()
// {

// 	int displayType = COMMON_CATHODE; //Your display is either common cathode or common anode

// 	/*
//   //This pinout is for a regular display
//    //Declare what pins are connected to the digits
//    int digit1 = 2; //Pin 12 on my 4 digit display
//    int digit2 = 3; //Pin 9 on my 4 digit display
//    int digit3 = 4; //Pin 8 on my 4 digit display
//    int digit4 = 5; //Pin 6 on my 4 digit display
   
//    //Declare what pins are connected to the segments
//    int segA = 6; //Pin 11 on my 4 digit display
//    int segB = 7; //Pin 7 on my 4 digit display
//    int segC = 8; //Pin 4 on my 4 digit display
//    int segD = 9; //Pin 2 on my 4 digit display
//    int segE = 10; //Pin 1 on my 4 digit display
//    int segF = 11; //Pin 10 on my 4 digit display
//    int segG = 12; //Pin 5 on my 4 digit display
//    int segDP= 13; //Pin 3 on my 4 digit display
//    */

// 	//This pinout is for OpenSegment PCB layout
// 	//Declare what pins are connected to the digits
// 	int digit1 = 9;	 //Pin 12 on my 4 digit display
// 	int digit2 = 16; //Pin 9 on my 4 digit display
// 	int digit3 = 17; //Pin 8 on my 4 digit display
// 	int digit4 = 3;	 //Pin 6 on my 4 digit display

// 	//Declare what pins are connected to the segments
// 	int segA = 14; //Pin 11 on my 4 digit display
// 	int segB = 2;  //Pin 7 on my 4 digit display
// 	int segC = 8;  //Pin 4 on my 4 digit display
// 	int segD = 6;  //Pin 2 on my 4 digit display
// 	int segE = 7;  //Pin 1 on my 4 digit display
// 	int segF = 15; //Pin 10 on my 4 digit display
// 	int segG = 4;  //Pin 5 on my 4 digit display
// 	int segDP = 5; //Pin 3 on my 4 digit display

// 	int numberOfDigits = 4; //Do you have a 1, 2 or 4 digit display?

// 	myDisplay.Begin(displayType, numberOfDigits, digit1, digit2, digit3, digit4, segA, segB, segC, segD, segE, segF, segG, segDP);

// 	myDisplay.SetBrightness(100); //Set the display to 100% brightness level

// 	timer = millis();
// }

// void loop()
// {
// 	//Example ways of displaying a decimal number
// 	char tempString[10];					//Used for sprintf
// 	sprintf(tempString, "%4d", deciSecond); //Convert deciSecond into a string that is right adjusted
// 	//sprintf(tempString, "%d", deciSecond); //Convert deciSecond into a string that is left adjusted
// 	//sprintf(tempString, "%04d", deciSecond); //Convert deciSecond into a string with leading zeros
// 	//sprintf(tempString, "%4d", deciSecond * -1); //Shows a negative sign infront of right adjusted number
// 	//sprintf(tempString, "%4X", deciSecond); //Count in HEX, right adjusted

// 	//Produce an output on the display
// 	myDisplay.DisplayString(tempString, 3); //(numberToDisplay, decimal point location)

// 	//Other examples
// 	//myDisplay.DisplayString(tempString, 0); //Display string, no decimal point
// 	//myDisplay.DisplayString("-23b", 3); //Display string, decimal point in third position

// 	//Check if 10ms has elapsed
// 	if (millis() - timer >= 100)
// 	{
// 		timer = millis();
// 		deciSecond++;
// 	}

// 	delay(5);
// }