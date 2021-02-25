//#include "lcdhelper.h"
//#include <EEPROM.h>

// 7 segment library
// LED library?

// lcdhelper oLCD(ILI9163_4L, 3, 2, 9, 10, 7);

// void ShowDisplay(screen val, String optionstate, String keypressed);

const int buttonUp = 40, buttonDown = 42, MainButton = 37; // Button Pins
const int ledR = 44, ledB = 49, ledG = 60;                 // LED Pins
const int ThermoElec = 30, Thermistor = A0;                // Thermoelectric (make sure PWM) and Thermistor Pins
int SetTemp, Ttherm, LightMode;                            //  Global Set Temperature, Thermistor Temp and Light Mode

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

int GetTemp()
{
    double sensorvalue = analogRead(Thermistor);                                                                           //Read Pin A0
    double voltage = sensorvalue * (5.0 / 1023.0);                                                                         //convert sensor value to voltage
    double Rtherm = ((50 / voltage) - 10) * 1000;                                                                          //Calculates thermistor resistance in Ohm
    double tempK = 1 / (0.001032 + (0.0002387 * log(Rtherm)) + (0.000000158 * (log(Rtherm) * log(Rtherm) * log(Rtherm)))); //calculates associated temp in k
    Ttherm = tempK - 273.15;
    // oLCD.setFont(BigFont);
    // oLCD.printNumI(Ttherm, 20, 55);
    return Ttherm;
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
                else
                {
                    SetTemp = SetTemp;
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
                else
                {
                    SetTemp = SetTemp;
                }
            }
        }
        else
        {
            SetTemp = SetTemp;
        }
        // oLCD.setFont(BigFont);
        // oLCD.printNumI(SetTemp, 100, 55);
        delay(250);
    }
    return SetTemp;
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
    if ((endMillisMain - startMillisMain) <= 2000)
    {
        Serial.println("Set Temp");
        SetTempInput();
    }
    else
    {
        Serial.println("Set Lights");
        SetLights();
    }
    return;
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
                else
                {
                    LightMode = LightMode;
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
                else
                {
                    LightMode = LightMode;
                }
            }
        }
        else
        {
            LightMode = LightMode;
        }
        delay(250);
    }
    return;
}
bool TempCorrect()
{
    GetTemp();
    if (abs((SetTemp - Ttherm)) > 5)
    {
        digitalWrite(ThermoElec, HIGH);
        Serial.println("On");
        Serial.println(Ttherm);
    }
    else
    {
        digitalWrite(ThermoElec, LOW);
        Serial.println("Off");
        Serial.println(Ttherm);
    }
}
// void ShowDisplay(screen val, String optionstate, String keypressed)
// {
//     oLCD.LCDInitialize(LANDSCAPE);
//     oLCD.drawRoundRect(18, 53, 52, 71);
//     oLCD.drawRoundRect(98, 53, 134, 71);
//     oLCD.setFont(SmallFont);
//     oLCD.print("Temp(F)", 16, 38);
//     oLCD.print("Set Temp", 90, 38);
// }
void SegDisp()
{
    // Conrol of the 7 segment will go here
}
void setup()
{
    Serial.begin(115200);
    pinMode(buttonUp, INPUT);
    pinMode(MainButton, INPUT);
    pinMode(buttonDown, INPUT);
    pinMode(Thermistor, INPUT);
    pinMode(ThermoElec, OUTPUT);
    pinMode(ledR, OUTPUT);
    pinMode(ledB, OUTPUT);
    pinMode(ledG, OUTPUT);
    // ShowDisplay(SC_MAIN, "", "");

    //attachInterrupt(digitalPinToInterrupt(MainButton), MenuSelect, RISING); // If menu button is constant on, change to falling or change

    // EEProm set temp
}
void loop()
{
    unsigned long currentMillis = millis();
    TempCorrect();
    if (digitalRead(MainButton) == HIGH)
    {
        MenuSelect();
    }
    delay(500);
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

/* LED Colors
                R   G   B
Red             255 0   0
Red-Violet      129 40  144
Purple          128 0   128
Blue            0   0   255
Blue-Green      0   115 54
Green           0   255 0
*/