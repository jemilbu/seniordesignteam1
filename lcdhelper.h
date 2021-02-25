//
//  Why ifndef?  So errors are not issued if header included more than once.
//
// Check if variabled defined
#ifndef LCDHELPER_H
//  If not defined, then define symbol
#define LCDHELPER_H
//  Include UTFT library.
#include <UTFT.h>
//  Declare screens
enum screen
{
    SC_MAIN = 0,
    SC_SUB1 = 1,
    SC_SUB2 = 2,
    SC_SUB3 = 3,
    SC_SUB4 = 4,
    SC_SUB5 = 5,
    SC_SUB6 = 6,
    SC_SUB7 = 7,
    SC_SUB8 = 8,
    SC_SUB9 = 9,
    SC_SUB10 = 10,
    SC_SUB11 = 11,
    SC_SUB12 = 12,
    SC_SUB13 = 13,
    SC_SUB14 = 14,
    SC_SUB15 = 15,
    SC_SUB16 = 16,
    SC_SUB17 = 17,
    SC_SUB18 = 18,
    SC_SUB19 = 19,
    SC_SUB20 = 20
};
// Declare variable to hold screen state
short int SCREEN_STATE=0;
//
//  Variable to store the current state of a screen.
//  For instance, a menu screen might display 
//  different information depending on the state
//  of the input
//
String OPTION_STATE="";
//
// Declare which fonts we will be using
// extern declare this variable without 
// allocating memory to the variable.  The
// memory is actually allocated in the header.
//
extern uint8_t SmallFont[]; //SmallFont[]
extern uint8_t BigFont[]; //BigFont[]

class lcdhelper : public UTFT{
public:
   // Setup constructor to pass values to the UTFT constructor
   lcdhelper(byte model =ILI9163_4L, int RS = 3, int WR=2, int CS = 9, int RST=10, int SER = 7);
   void LCDInitialize(byte orientation);
private:
    
};

#endif /* LCDHELPER_H */