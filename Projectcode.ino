#include "lcdhelper.h"

lcdhelper oLCD(ILI9163_4L, 3, 2, 9, 10, 7);

void ShowDisplay(screen val, String optionstate, String keypressed);

const int TempUp = 30;
const int TempDown = 34;
const int MainButton = 45;
const int Light = 44;
const int ThermoElec = 2;
int SetTemp;
int Ttherm;

int currentStateUp;           //  State of push button 1
int currentStateDown;         //  State of push button 2
int currentStateMain;         //  State of push button 3
const int debounceDelay = 10; //  Debounce time; increase if output flickers

int lastSteadyUp = HIGH;             //  previous steady state from input
int lastFlickUp = HIGH;              //  previous flicker state from input
unsigned long int lastDebouneUp = 0; //the last time the output was toggled

int lastSteadyDown = HIGH;             //  previous steady state from input
int lastFlickDown = HIGH;              //  previous flicker state from input
unsigned long int lastDebouneDown = 0; //the last time the output was toggled

int lastSteadyMain = HIGH;             //  previous steady state from input
int lastFlickMain = HIGH;              //  previous flicker state from input
unsigned long int lastDebouneMain = 0; //the last time the output was toggled

int GetTemp(int Ttherm)
{
    int sensorvalue = analogRead(A0);              //Read Pin A0
    double voltage = sensorvalue * (5.0 / 1023.0); //convert sensor value to voltage
    double Rtherm = ((50 / voltage) - 10) * 1000;  //Calculates thermistor resistance in Ohm
    Ttherm = 25;                                   //1/(0.001032+(0.0002387*log(Rtherm))+(0.000000158*(log(Rtherm)*log(Rtherm)*log(Rtherm))));//calculates associated temp in k
    oLCD.setFont(BigFont);
    oLCD.printNumI(Ttherm, 20, 55);
    return Ttherm;
    delay(5000);
}
int SetTempInput(int SetTemp)
{
    // Taking the current state of the buttons 
    currentStateUp = digitalRead(TempUp);
    currentStateDown = digitalRead(TempDown);

    // If Up button depressed
    if (currentStateUp == HIGH)
    {
        //  Checking that there has been enough time betwwen a switch to ignore bounce and noise
        if (currentStateUp != lastFlickUp)
        {
            lastDebouneUp = millis();     //  reset the debounce timer
            lastFlickUp = currentStateUp; //  save the last flicker state
        }
        if ((millis() - lastDebouneUp) > debounceDelay)
        {
            SetTemp += 1;
        }
    }
    // If Down button depressed
    if (currentStateDown == HIGH)
    {
        //  Checking that there has been enough time betwwen a switch to ignore bounce and noise
        if (currentStateDown != lastFlickDown)
        {
            lastDebouneDown = millis();     //  reset the debounce timer
            lastFlickDown = currentStateDown; //  save the last flicker state
        }
        if ((millis() - lastDebouneDown) > debounceDelay)
        {
            SetTemp -= 1;
        }
    }
    else
    {
        SetTemp = SetTemp;
    }
    oLCD.setFont(BigFont);
    oLCD.printNumI(SetTemp, 100, 55);
    return SetTemp;
    delay(50);
}
void MenuSelect()
{
    //debounce delay
    //if main button pressed for less than one second, go to temp
    //if main button pressed for more than one second and up to five, go to lights
}
void Lights()
{
    if (digitalRead(LightButton) == HIGH)
    {
        digitalWrite(Light, HIGH);
        delay(300000);
        digitalWrite(Light, LOW);

        if (digitalRead(LightButton) == HIGH)
        {
            digitalWrite(Light, LOW);
        }
    }
    else
        ;
}
void TempCorrection()
{
    int GetThisValue;
    if (GetTemp(Ttherm) - SetTempInput(SetTemp) > 5 or GetTemp(Ttherm) - SetTempInput(SetTemp) < -5)
    {
        analogWrite(ThermoElec, GetThisValue); //PWM duty cycle, 255 is max. will need to test this on thermoelectric
    }
}
void ShowDisplay(screen val, String optionstate, String keypressed)
{
    oLCD.LCDInitialize(LANDSCAPE);
    oLCD.drawRoundRect(18, 53, 52, 71);
    oLCD.drawRoundRect(98, 53, 134, 71);
    oLCD.setFont(SmallFont);
    oLCD.print("Temp(F)", 16, 38);
    oLCD.print("Set Temp", 90, 38);
}
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


/*
Add task scheduler to code to allow for background processes
GetTemp and TempCorrection to run every minute
SetTempInput to only run on button press, clear after short button press or 5 second timeout
SetLightColor to only run on long button press, clear after long button press or 5 second timout

URL for article on task scheduling
https://create.arduino.cc/projecthub/GadgetsToGrow/don-t-delay-use-an-arduino-task-scheduler-today-215cfe

GitHub for task Scheduling
https://github.com/gadgetstogrow/TaskScheduler

Task Scheduler Original Code
http://bleaklow.com/2010/07/20/a_very_simple_arduino_task_manager.html

Advanced Task Scheduler 
https://github.com/arkhipenko/TaskScheduler

PID Controller Example
https://www.teachmemicro.com/arduino-pid-control-tutorial/

PID Library
https://github.com/ettoreleandrotognoli/ArcPID
*/
