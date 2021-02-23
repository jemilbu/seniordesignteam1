#include "lcdhelper.h"
#include "PID_v1.h"

lcdhelper oLCD(ILI9163_4L, 3, 2, 9, 10, 7);

void ShowDisplay(screen val, String optionstate, String keypressed);

const int TempUp = 30;
const int TempDown = 34;
const int MainButton = 45;
const int Light = 44;
const int ThermoElec = 2;
const int Thermistor = A0;
int SetTemp, Ttherm;

double consKp = 4.0;                                                     //  Tuning parameter for conservative Proportional Response
double consKi = 4.0;                                                     //  Tuning parameter for conservative Integral Response
double consKd = 4.0;                                                     //  Tuning parameter for conservative Derivative Response
double aggKp = 3.0;                                                      //  Tuning parameter for aggressive Proportional Response
double aggKi = 3.0;                                                      //  Tuning parameter for aggressive Integral Response
double aggKd = 3.0;                                                      //  Tuning parameter for aggressive Derivative Response
PID pid(&Ttherm, &ThermoElec, &SetTemp, consKp, consKi, consKd, DIRECT); //  PID control for temperature

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

int GetTemp()
{
    double sensorvalue = analogRead(Thermistor);                                                                     //Read Pin A0
    double voltage = sensorvalue * (5.0 / 1023.0);                                                                   //convert sensor value to voltage
    double Rtherm = ((50 / voltage) - 10) * 1000;                                                                    //Calculates thermistor resistance in Ohm
    Ttherm = 1 / (0.001032 + (0.0002387 * log(Rtherm)) + (0.000000158 * (log(Rtherm) * log(Rtherm) * log(Rtherm)))); //calculates associated temp in k
    oLCD.setFont(BigFont);
    oLCD.printNumI(Ttherm, 20, 55);
    return Ttherm;
}
int SetTempInput()
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
            lastDebouneDown = millis();       //  reset the debounce timer
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
}
void MenuSelect()
{
    //debounce delay
    //if main button pressed for less than one second, go to temp
    //if main button pressed for more than one second and up to five, go to lights
}
void Lights()
{
}
bool TempCorrect()
{
    double gap = abs(SetTemp - Ttherm); //distance away from setpoint
    if (gap < 10)
    { //we're close to setpoint, use conservative tuning parameters
        myPID.SetTunings(consKp, consKi, consKd);
    }
    else
    {
        //we're far from setpoint, use aggressive tuning parameters
        myPID.SetTunings(aggKp, aggKi, aggKd);
    }
    if (myPID.Compute() == true)
    {
        return true;
    }
    else
    {
        return false;
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
void SegDisp() void setup()
{
    Serial.begin(115200);
    pinMode(TempUp, INPUT);
    pinMode(TempDown, INPUT);
    pinMode(MainButton, INPUT);
    pinMode(Light, OUTPUT);
    pid.SetMode(AUTOMATIC);
    ShowDisplay(SC_MAIN, "", "");
}
void loop()
{
    Lights();
    TempCorrect();
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

Interupts
https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
*/