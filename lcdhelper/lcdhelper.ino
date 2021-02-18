#include "lcdhelper.h"

lcdhelper::lcdhelper(byte model, int RS, int WR, int CS, int RST, int SER):UTFT(model, RS, WR, CS, RST, SER)
{}

//
//  Iniallize the LCD
//
void lcdhelper::LCDInitialize(byte orientation)
{
    UTFT::InitLCD(orientation);
    UTFT::setFont(BigFont);
    UTFT::clrScr();
    UTFT::setBackColor(VGA_BLACK);
    UTFT::fillScr(VGA_BLACK);
    //
    // Get screem coordinates.
    //
    int x = UTFT::getDisplayXSize();
    int y = UTFT::getDisplayYSize();
    //
    //  Draw rectangular border to screen.
    //
    UTFT::drawRoundRect(5, 5, x - 5, y - 5);
    //
    //  Setup fonts
    //
    UTFT::setColor(VGA_WHITE);
    UTFT::setFont(BigFont);
}