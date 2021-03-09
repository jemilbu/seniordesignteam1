//#include <EEPROM.h>
#include "SevSeg.h"
#include "RGBLed.h"

// Button Pins
const int buttonUp = 40, buttonDown = 42, MainButton = 36;
// LED Pins
const int ledR = 47, ledB = 49, ledG = 60;
// Thermoelectric LED Test Pin
const int ThermoElecLED = 30;
// Thermoelectric Control Pins
const int ThermoElecA = 45, ThermoElecB = 46, savePower = 69;
// Thermistor Pins
const int Thermistor1 = A0;
Thermistor2 = A1;
//  Global Set Temperature, Thermistor Temp and Light Mode
volatile double SetTemp, TthermAvg, LightMode, UnitsMode;

//  Thermoelectric Motor Driver pins
const int speed = 0, direct = 0;

int currentStateUp;          //  State of push button 1
int currentStateDown;        //  State of push button 2
int currentStateMain;        //  State of push button 3
const int debounceDelay = 2; //  Debounce time; increase if output flickers

int lastSteadyUp = HIGH;             //  previous steady state from input
int lastFlickUp = HIGH;              //  previous flicker state from input
unsigned long int lastDebouneUp = 0; //the last time the output was toggled

int lastSteadyDown = HIGH;             //  previous steady state from input
int lastFlickDown = HIGH;              //  previous flicker state from input
unsigned long int lastDebouneDown = 0; //the last time the output was toggled

int lastSteadyMain = HIGH;             //  previous steady state from input
int lastFlickMain = HIGH;              //  previous flicker state from input
unsigned long int lastDebouneMain = 0; //the last time the output was toggled

unsigned long previousMillisMain = 0; // Stores the main was pressed

//  Create an instance of the 7 segment object
SevSeg disp;

//  Create an instance of the RGB object
RGBLed led(ledR, ledG, ledB, RGBLed::COMMON_ANODE);

int GetTemp()
{
    double sensorvalue = analogRead(Thermistor1);                                                                          //Read Pin A0
    double voltage = sensorvalue * (5.0 / 1023.0);                                                                         //convert sensor value to voltage
    double Rtherm = ((50 / voltage) - 10) * 1000;                                                                          //Calculates thermistor resistance in Ohm
    double tempK = 1 / (0.001032 + (0.0002387 * log(Rtherm)) + (0.000000158 * (log(Rtherm) * log(Rtherm) * log(Rtherm)))); //calculates associated temp in k
    double Ttherm1 = tempK - 273.15;
    double sensorvalue2 = analogRead(Thermistor2);                                                                              //Read Pin A0
    double voltage2 = sensorvalue2 * (5.0 / 1023.0);                                                                            //convert sensor value to voltage
    double Rtherm2 = ((50 / voltage2) - 10) * 1000;                                                                             //Calculates thermistor resistance in Ohm
    double tempK2 = 1 / (0.001032 + (0.0002387 * log(Rtherm2)) + (0.000000158 * (log(Rtherm2) * log(Rtherm2) * log(Rtherm2)))); //calculates associated temp in k
    double Ttherm2 = tempK2 - 273.15;

    TthermAvg = (Ttherm1 + Ttherm2) / 2.0

    // Print temp to screen here
    //  disp.DisplayString("25°C",0);
    Serial.print("Current Temp = ");

    // If display is in Farenheit
    if (UnitsMode == 2)
    {
        double TthermF = (TthermAvg * (9.0/5.0)) + 32.0;
        Serial.println(TthermF);
        Serial.print("°F");
    }
    else
    {
        Serial.println(TthermAvg);
        Serial.print("°C");
    }
    return TthermAvg;
}
int SetTempInput()
{
    // Blink 7 segment
    Serial.println("Setting Temp");
    // While loop this until main button is pressed, store to EEprom once complete
    // Make sure to put a counter on eeprom to prevent overwrite
    delay(1000);
    while (digitalRead(MainButton) == LOW)
    {
        // Taking the current state of the buttons
        currentStateUp = digitalRead(buttonUp);
        currentStateDown = digitalRead(buttonDown);

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
                // Checking if SetTemp is in the useful range, changing if it is
                if (SetTemp != 80) // 80 Celcius
                {
                    SetTemp += 1;
                    Serial.println(SetTemp);
                }
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
                // Checking if SetTemp is in the useful range, changing if it is
                if (SetTemp != 0) // 0 Celcius
                {
                    SetTemp -= 1;
                    Serial.println(SetTemp);
                }
            }
        }
        delay(250);
    }
    return SetTemp;
}
void SetLights()
{
    // Blink 7 segment

    //  Put into While loop that looks for main button press, once button press: send to eeprom (put counter), send to LED program?

    // Taking the current state of the buttons
    currentStateUp = digitalRead(buttonUp);
    currentStateDown = digitalRead(buttonDown);
    while (digitalRead(MainButton) == LOW)
    {
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
                if (LightMode != 12)
                {
                    LightMode += 1;
                    Serial.println(LightMode);
                }
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
                if (LightMode != 1)
                {
                    LightMode -= 1;
                    Serial.println(LightMode);
                }
            }
        }
        delay(250);
    }
    return;
}
void SetUnits()
{
    // Blink 7 segment

    //  Put into While loop that looks for main button press, once button press: send to eeprom (put counter), send to LED program?

    // Taking the current state of the buttons
    currentStateUp = digitalRead(buttonUp);
    currentStateDown = digitalRead(buttonDown);
    while (digitalRead(MainButton) == LOW)
    {
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
                if (UnitsMode != 2)
                {
                    UnitsMode += 1;
                    Serial.println("°F");
                }
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
                if (UnitsMode != 1)
                {
                    UnitsMode -= 1;
                    Serial.println("°C");
                }
            }
        }
        delay(250);
    }
    return;
}
}
bool TempCorrect()
{
    GetTemp();
    // Test using LED
    // if (abs((SetTemp - Ttherm)) > 5)
    // {
    //     digitalWrite(ThermoElecLED, HIGH);
    //     Serial.println("On");
    //     Serial.println(Ttherm);
    // }
    // else
    // {
    //     digitalWrite(ThermoElecLED, LOW);
    //     Serial.println("Off");
    //     Serial.println(Ttherm);
    // }

    // Actual Function
    // This is where PI control could come into play
    // If setpoint is colder than measured, turn on cooling
    if ((TthermAvg - SetTemp) > 5.0)
    {
        analogWrite(ThermoElecA, HIGH);
        analogWrite(ThermoElecB, LOW);
    }
    // If setpoint is much warmer than measured, turn on heating
    else if ((SetTemp - TthermAvg) > 10)
    {
        analogWrite(ThermoElecA, LOW);
        analogWrite(ThermoElecB, HIGH);
    }
    // Save power if in range
    else
    {
        analogWrite(savePower, LOW);
    }
}
void SegDisp()
{
    // Conrol of the 7 segment will go here
}
void MenuSelect()
{
    //debounce delay
    Serial.println("Menu");
    unsigned long startMillisMain = millis(); //  Storing the current time
    unsigned long endMillisMain;
    while (digitalRead(MainButton) == HIGH) //  Loop until the main button goes low
    {
        endMillisMain = millis(); //  Read the current time
    }

    //if main button pressed for less than one second, go to temp
    //if main button pressed for more than one second and up to five, go to lights
    //if main button pressed longer than 5 seconds, go to units
    if ((endMillisMain - startMillisMain) <= 2000)
    {
        Serial.println("Set Temp");
        SetTempInput();
    }
    else if ((endMillisMain - startMillisMain) <= 5000 && (endMillisMain - startMillisMain) > 2000)
    {
        Serial.println("Set Lights");
        SetLights();
    }
    else
    {
        Serial.print("Set Units");
        SetUnits();
    }

    return;
}
void setup()
{
    Serial.begin(115200);
    pinMode(buttonUp, INPUT);
    pinMode(MainButton, INPUT);
    pinMode(buttonDown, INPUT);
    pinMode(Thermistor1, INPUT);
    pinMode(Thermistor2, INPUT);
    pinMode(ThermoElecLED, OUTPUT);
    pinMode(ThermoElecA, OUTPUT);
    pinMode(ThermoElecB, OUTPUT);
    pinMode(savePower, OUTPUT);
    pinMode(ledR, OUTPUT);
    pinMode(ledB, OUTPUT);
    pinMode(ledG, OUTPUT);
    pinMode(speed, OUTPUT);
    pinMode(direct, OUTPUT);

    //  Digit Pins for 7 segment
    const int dig1 = 23, dig2 = 25, dig3 = 27, dig4 = 29;
    //  Segment Pins for 7 segment
    const int segA = 31, segB = 33, segC = 35, segD = 37, segE = 39, segF = 41, segG = 43;
    //  Common Cathode/Anode
    int digits = 4;

    disp.Begin(COMMON_CATHODE_7, digits, dig1, dig2, dig3, dig4, segA, segB, segC, segD, segE, segF, segG, 53);

    disp.SetBrightness(50);

    //attachInterrupt(digitalPinToInterrupt(MainButton), MenuSelect, RISING); // If menu button is constant on, change to falling or change

    // EEProm set temp
}
void loop()
{
    // Add system timer interupt so temp correct doesn't run constantly but isn't in ISR
    TempCorrect();
    if (digitalRead(MainButton) == HIGH)
    {
        MenuSelect();
    }
    delay(1000);
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

7 Seg
https://github.com/sparkfun/SevSeg

RGB LED
https://github.com/wilmouths/RGBLed

Motor Driver
https://www.pololu.com/product-info-merged/2960

Disp
https://docs.broadcom.com/doc/AV02-0568EN
*/
