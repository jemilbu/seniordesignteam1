#include "lcdhelper.h"

lcdhelper oLCD(ILI9163_4L,3,2,9,10,7);

void ShowDisplay(screen val, String optionstate, String keypressed);

const int TempUp = 30;
const int TempDown = 34;
const int LightButton = 45;
const int Light = 44;
const int ThermoElec = 2;
int SetTemp;
int Ttherm;

void setup()
{
    Serial.begin(115200);
    pinMode(TempUp, INPUT);
    pinMode(TempDown, INPUT);
    pinMode(LightButton, INPUT);
    pinMode(Light, OUTPUT);
    ShowDisplay(SC_MAIN, "", "");
}
void loop()
{
    GetTemp(Ttherm);
    SetTempInput(SetTemp);
    Lights();
    TempCorrection();
}
int GetTemp(int Ttherm)
{
    int sensorvalue = analogRead(A0); //Read Pin A0
    double voltage = sensorvalue * (5.0 / 1023.0); //convert sensor value to voltage
    double Rtherm = ((50/voltage) - 10)*1000;//Calculates thermistor resistance in Ohm
    Ttherm = 25; //1/(0.001032+(0.0002387*log(Rtherm))+(0.000000158*(log(Rtherm)*log(Rtherm)*log(Rtherm))));//calculates associated temp in k
    oLCD.setFont(BigFont);
    oLCD.printNumI(Ttherm, 20, 55);
    return Ttherm;
    delay(5000);
}
int SetTempInput(int SetTemp)
{
    SetTemp = 7;
    if(digitalRead(TempUp)==HIGH)
    {
        SetTemp += 1;
    }
    if(digitalRead(TempDown)==HIGH)
    {
        SetTemp -= 1;
    }
    else
    {
        SetTemp = SetTemp;
    }
    oLCD.setFont(BigFont);
    oLCD.printNumI(SetTemp,100,55);
    return SetTemp;
    delay(50);
}
void Lights()
{
    if(digitalRead(LightButton)==HIGH)
    {
        digitalWrite(Light,HIGH);
        delay(300000);
        digitalWrite(Light,LOW);

        if(digitalRead(LightButton)==HIGH)
        {
            digitalWrite(Light,LOW);
        }
    }
    else;
}
void TempCorrection()
{
    int GetThisValue;
    if(GetTemp(Ttherm) - SetTempInput(SetTemp) > 5 or GetTemp(Ttherm) - SetTempInput(SetTemp) < -5)
    {
        analogWrite(ThermoElec,GetThisValue);//PWM duty cycle, 255 is max. will need to test this on thermoelectric
    }
}
void ShowDisplay(screen val, String optionstate, String keypressed)
{
    oLCD.LCDInitialize(LANDSCAPE);
    oLCD.drawRoundRect(18,53,52,71);
    oLCD.drawRoundRect(98,53,134,71);
    oLCD.setFont(SmallFont);
    oLCD.print("Temp(F)",16,38);
    oLCD.print("Set Temp",90,38);
}
