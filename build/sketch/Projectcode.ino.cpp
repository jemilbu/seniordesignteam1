#include <Arduino.h>
#line 1 "c:\\Users\\Owner\\Pictures\\VSCode\\seniordesignteam1\\Projectcode.ino"
#line 1 "c:\\Users\\Owner\\Pictures\\VSCode\\seniordesignteam1\\Projectcode.ino"
#include <RGBLed.h>
#include "Adafruit_LEDBackpack.h"
#include <Wire.h>
#include <EEPROM.h>

//  Creating Instance of the EEPROMClass
EEPROMClass hard;
//  The last time EEPROM was stored
volatile uint16_t eeTime;

//  Selecting Serial Output Type
//#define debug 3   //  Data Output
//#define debug 2 //  Only necassary Serial Print
#define debug 1 //  All Serial Print
//#define debug 0 //  No Serial Print

// Button Pins
const int buttonUp = 40, buttonDown = 42, MainButton = 36;
// LED Pins
const int ledR = 26, ledB = 22, ledG = 24, ledBright = 28;
// Thermoelectric Control Pins
const int RelayControl = 45;
// Thermistor Pins
const int Thermistor1 = A0, Thermistor2 = A1;
// Door Switch Pin
const int door = 50;

//  Default Set Temperature, Light Mode, Units, and Brightness
volatile float SetTempC = 0, SetTempF = 32;
volatile int LightMode = 0, UnitsMode = 1, prevUnit = 1, Brightness = 15;

//  Global Time constants
unsigned long time, previousMillis = 0;

//  Debounce for buttons Setup
volatile int currentStateUp;     //  State of push button 1
volatile int currentStateDown;   //  State of push button 2
volatile int currentStateMain;   //  State of push button 3
const float debounceDelay = 0.5; //  Debounce time; increase if output flickers

volatile int lastSteadyUp = HIGH;    //  previous steady state from input
volatile int lastFlickUp = HIGH;     //  previous flicker state from input
unsigned long int lastDebouneUp = 0; //the last time the output was toggled

volatile int lastSteadyDown = HIGH;    //  previous steady state from input
volatile int lastFlickDown = HIGH;     //  previous flicker state from input
unsigned long int lastDebouneDown = 0; //the last time the output was toggled

volatile int lastSteadyMain = HIGH;    //  previous steady state from input
volatile int lastFlickMain = HIGH;     //  previous flicker state from input
unsigned long int lastDebouneMain = 0; //the last time the output was toggled

unsigned long previousMillisMain = 0; // Stores the main was pressed

//  Bool Flags for Temperature control
volatile bool bang, cooling;

//  Door
volatile int prevLight;
volatile bool doorWasOpen;
//  Create an instance of the RGB object
RGBLed led(ledR, ledG, ledB, RGBLed::COMMON_ANODE);

//  Creating an instance of the 7 segment object
Adafruit_7segment sevseg = Adafruit_7segment();

#line 67 "c:\\Users\\Owner\\Pictures\\VSCode\\seniordesignteam1\\Projectcode.ino"
int GetTemp();
#line 154 "c:\\Users\\Owner\\Pictures\\VSCode\\seniordesignteam1\\Projectcode.ino"
void LED(int mode);
#line 199 "c:\\Users\\Owner\\Pictures\\VSCode\\seniordesignteam1\\Projectcode.ino"
void SetTempInput();
#line 373 "c:\\Users\\Owner\\Pictures\\VSCode\\seniordesignteam1\\Projectcode.ino"
void SetLights();
#line 468 "c:\\Users\\Owner\\Pictures\\VSCode\\seniordesignteam1\\Projectcode.ino"
void SetUnits();
#line 568 "c:\\Users\\Owner\\Pictures\\VSCode\\seniordesignteam1\\Projectcode.ino"
void SetBrightness();
#line 679 "c:\\Users\\Owner\\Pictures\\VSCode\\seniordesignteam1\\Projectcode.ino"
void DoorLight();
#line 687 "c:\\Users\\Owner\\Pictures\\VSCode\\seniordesignteam1\\Projectcode.ino"
bool TempCorrect();
#line 948 "c:\\Users\\Owner\\Pictures\\VSCode\\seniordesignteam1\\Projectcode.ino"
void MenuSelect();
#line 1056 "c:\\Users\\Owner\\Pictures\\VSCode\\seniordesignteam1\\Projectcode.ino"
void setup();
#line 1155 "c:\\Users\\Owner\\Pictures\\VSCode\\seniordesignteam1\\Projectcode.ino"
void loop();
#line 67 "c:\\Users\\Owner\\Pictures\\VSCode\\seniordesignteam1\\Projectcode.ino"
int GetTemp()
{
    float sensorvalue = analogRead(Thermistor1);     //  Read the input bin of thermistor 1
    float voltage = sensorvalue * (5.0 / 1023.0);    //  Convert to voltage
    float Rtherm = ((5.0 / voltage) - 1.0) * 5000.0; //   Calculate the resistance in Ohms

    float tempK = 1.0 / (0.00147336 + (0.000204516 * log(Rtherm)) + (0.00000023771643 * (log(Rtherm) * log(Rtherm) * log(Rtherm))));
    //  Calculate the temperature in degrees Kelvin

    float sensorvalue2 = analogRead(Thermistor2);      //  Read the input bin of thermistor 2
    float voltage2 = sensorvalue2 * (5.0 / 1023.0);    //  Convert to voltage
    float Rtherm2 = ((5.0 / voltage2) - 1.0) * 5000.0; //   Calculate the resistance in Ohms

    float tempK2 = 1.0 / (0.00147336 + (0.000204516 * log(Rtherm2)) + (0.00000023771643 * (log(Rtherm2) * log(Rtherm2) * log(Rtherm2)))); //  Calculate the temperature in degrees Kelvin

    //  Convert to degress Celcius
    float Ttherm1 = tempK - 273.15;
    float Ttherm2 = tempK2 - 273.15;

    //  Calculate the average of the two thermistors to use as the actual temperature
    float TthermAvg = (Ttherm1 + Ttherm2) / 2.0;

    // Print temp to Serial Monitor
    if (debug == 1 || debug == 2 || debug == 3)
    {
        Serial.print("Current Temp = ");
    }
    if (UnitsMode == 1)
    {
        //  Print Temp to Serial Monitor
        if (debug == 1 || debug == 2 || debug == 3)
        {
            Serial.print(TthermAvg);
        }
        if (debug == 1 || debug == 2 || debug == 3)
        {
            Serial.println("°C");
        }

        //  Converting from float to int
        int temp = (int)TthermAvg;

        //  Extracting Digits
        int x = (temp / 10U) % 10;
        int y = (temp / 1U) % 10;

        //  Printing to 7 segment
        sevseg.writeDigitNum(0, x);          //  First Digit
        sevseg.writeDigitNum(1, y);          //  Second Digit
        sevseg.writeDigitRaw(3, 0b01100011); //  Degrees Symbol
        sevseg.writeDigitNum(4, 12);         //  C
        sevseg.writeDisplay();
        return TthermAvg;
    }
    // If display is in Farenheit
    else if (UnitsMode == 2)
    {
        //  Converting from °C to °F
        float TthermF = (TthermAvg * (9.0 / 5.0)) + 32.0;

        //  Print temp to serial port if enabled
        if (debug == 1 || debug == 2 || debug == 3)
        {
            Serial.print(TthermF);
        }
        if (debug == 1 || debug == 2 || debug == 3)
        {
            Serial.println("°F");
        }

        //  Converting from float to int
        int temp = (int)TthermF;

        //  Extracting Digits
        int x = (temp / 10U) % 10;
        int y = (temp / 1U) % 10;

        //  Printing to 7 segment
        sevseg.writeDigitNum(0, x);          //  First Digit
        sevseg.writeDigitNum(1, y);          //  Second Digit
        sevseg.writeDigitRaw(3, 0b01100011); //  Degrees Symbol
        sevseg.writeDigitNum(4, 15);         //  F
        sevseg.writeDisplay();

        return TthermF;
    }
}
void LED(int mode)
{
    // //Turn all Off
    digitalWrite(ledBright, LOW);
    //  Off Mode
    if (mode == 0)
    {
        digitalWrite(ledBright, HIGH);
        digitalWrite(ledR, LOW);
        digitalWrite(ledG, LOW);
        digitalWrite(ledB, LOW);
    }
    //  Red
    else if (mode == 1)
    {
        digitalWrite(ledBright, LOW);
        digitalWrite(ledR, HIGH);
        digitalWrite(ledG, LOW);
        digitalWrite(ledB, LOW);
    }
    //  Green
    else if (mode == 2)
    {
        digitalWrite(ledBright, LOW);
        digitalWrite(ledR, LOW);
        digitalWrite(ledG, HIGH);
        digitalWrite(ledB, LOW);
    }
    //  Blue
    else if (mode == 3)
    {
        digitalWrite(ledBright, LOW);
        digitalWrite(ledR, LOW);
        digitalWrite(ledG, LOW);
        digitalWrite(ledB, HIGH);
    }
    //  White
    else if (mode == 4)
    {
        digitalWrite(ledBright, LOW);
        digitalWrite(ledR, HIGH);
        digitalWrite(ledG, HIGH);
        digitalWrite(ledB, HIGH);
    }
}
void SetTempInput()
{
    // Blink 7 segment
    sevseg.clear();
    sevseg.blinkRate(2);
    sevseg.writeDisplay();

    //  Pre-load display
    if (UnitsMode == 1)
    {
        int write = (int)SetTempC;
        int x = (write / 10U) % 10;
        int y = (write / 1U) % 10;

        sevseg.writeDigitNum(0, x);
        sevseg.writeDigitNum(1, y);
        sevseg.writeDigitRaw(3, 0b01100011);
        sevseg.writeDigitNum(4, 12);
        sevseg.writeDisplay();
    }
    else if (UnitsMode == 2)
    {
        int write = (int)SetTempF;
        int x = (write / 10U) % 10;
        int y = (write / 1U) % 10;

        sevseg.writeDigitNum(0, x);
        sevseg.writeDigitNum(1, y);
        sevseg.writeDigitRaw(3, 0b01100011);
        sevseg.writeDigitNum(4, 15);
        sevseg.writeDisplay();
    }

    // While loop this until main button is pressed, store to EEprom once complete
    // Make sure to put a counter on eeprom to prevent overwrite
    while (digitalRead(MainButton) == HIGH)
    {
        // Taking the current state of the buttons
        currentStateUp = digitalRead(buttonUp);
        currentStateDown = digitalRead(buttonDown);

        // If Up button depressed
        if (currentStateUp == LOW)
        {
            //  Checking that there has been enough time betwwen a switch to ignore bounce and noise
            if (currentStateUp != lastFlickUp)
            {
                lastDebouneUp = millis();     //  reset the debounce timer
                lastFlickUp = currentStateUp; //  save the last flicker state
            }
            if ((millis() - lastDebouneUp) > debounceDelay)
            {
                if (UnitsMode == 1)
                {
                    // Checking if SetTemp is in the useful range, changing if it is
                    if (SetTempC != 80) // Celcius
                    {
                        SetTempC += 1;
                        //  If debug mode 1 or 2 enabled, print to serial port
                        if (debug == 1 || debug == 2)
                        {
                            Serial.println(SetTempC, 0);
                        }
                        //  Print to 7 segment
                        int write = (int)SetTempC;
                        int x = (write / 10U) % 10;
                        int y = (write / 1U) % 10;

                        sevseg.writeDigitNum(0, x);
                        sevseg.writeDigitNum(1, y);
                        sevseg.writeDigitRaw(3, 0b01100011);
                        sevseg.writeDigitNum(4, 12);
                        sevseg.writeDisplay();
                    }
                }
                else if (UnitsMode == 2)
                {
                    // Checking if SetTemp is in the useful range, changing if it is
                    if (SetTempF != 176) // Farenheit
                    {
                        SetTempF += 1;
                        if (debug == 1 || debug == 2)
                        {
                            Serial.println(SetTempF, 0);
                        }
                        //  Print to 7 segment
                        int write = (int)SetTempF;
                        int x = (write / 10U) % 10;
                        int y = (write / 1U) % 10;
                        sevseg.writeDigitNum(0, x);
                        sevseg.writeDigitNum(1, y);
                        sevseg.writeDigitRaw(3, 0b01100011);
                        sevseg.writeDigitNum(4, 15);
                        sevseg.writeDisplay();
                    }
                }
            }
        }
        // If Down button depressed
        if (currentStateDown == LOW)
        {
            if (UnitsMode == 1)
            {
                // Checking if SetTemp is in the useful range, changing if it is
                if (SetTempC != 0) // Celcius
                {
                    SetTempC -= 1;
                    if (debug == 1 || debug == 2)
                    {
                        Serial.println(SetTempC, 0);
                    }
                    //  Print to 7 segment
                    int write = (int)SetTempC;
                    int x = (write / 10U) % 10;
                    int y = (write / 1U) % 10;

                    sevseg.writeDigitNum(0, x);
                    sevseg.writeDigitNum(1, y);
                    sevseg.writeDigitRaw(3, 0b01100011);
                    sevseg.writeDigitNum(4, 12);
                    sevseg.writeDisplay();
                }
            }
            else if (UnitsMode == 2)
            {
                // Checking if SetTemp is in the useful range, changing if it is
                if (SetTempF != 32) // Farenheit
                {
                    SetTempF -= 1;
                    if (debug == 1 || debug == 2)
                    {
                        Serial.println(SetTempF, 0);
                    }
                    //  Print to 7 segment
                    int write = (int)SetTempF;
                    int x = (write / 10U) % 10;
                    int y = (write / 1U) % 10;

                    sevseg.writeDigitNum(0, x);
                    sevseg.writeDigitNum(1, y);
                    sevseg.writeDigitRaw(3, 0b01100011);
                    sevseg.writeDigitNum(4, 15);
                    sevseg.writeDisplay();
                }
            }
        }
        delay(250);
    }
    //  Send to EEPROM
    if (millis() - eeTime > 10)
    {
        if (UnitsMode == 1)
        {
            hard.update(0, SetTempC);
            eeTime = millis();
            //  If debug enabled, print to serial port
            if (debug == 1 || debug == 2)
            {
                Serial.println("EEPROM Store Celcius");
            }
        }
        else if (UnitsMode == 2)
        {
            hard.update(1, SetTempF);
            eeTime = millis();
            //  If debug enabled, print to serial port
            if (debug == 1 || debug == 2)
            {
                Serial.println("EEPROM Store Farenheit");
            }
        }
    }
    return;
}
void SetLights()
{
    // Blink 7 segment
    sevseg.clear();
    sevseg.blinkRate(2);
    sevseg.writeDigitNum(4, LightMode);
    sevseg.writeDisplay();

    //  Flag for changing LEDs
    bool change = 0;

    //  Put into While loop that looks for main button press, once button press
    while (digitalRead(MainButton) == HIGH)
    {
        // Taking the current state of the buttons
        currentStateUp = digitalRead(buttonUp);
        currentStateDown = digitalRead(buttonDown);
        // If Up button depressed
        if (currentStateUp == LOW)
        {
            //  Checking that there has been enough time betwwen a switch to ignore bounce and noise
            if (currentStateUp != lastFlickUp)
            {
                lastDebouneUp = millis();     //  reset the debounce timer
                lastFlickUp = currentStateUp; //  save the last flicker state
            }
            if ((millis() - lastDebouneUp) > debounceDelay)
            {
                if (LightMode != 4)
                {
                    LightMode += 1;
                    //  Print to 7 segment
                    sevseg.writeDigitNum(4, LightMode);
                    sevseg.writeDisplay();
                    //  Flip flag
                    change = 1;
                    //  If debug enabled, print to serial port
                    if (debug == 1 || debug == 2)
                    {
                        Serial.println(LightMode);
                    }
                }
            }
        }
        // If Down button depressed
        if (currentStateDown == LOW)
        {
            //  Checking that there has been enough time betwwen a switch to ignore bounce and noise
            if (currentStateDown != lastFlickDown)
            {
                lastDebouneDown = millis();       //  reset the debounce timer
                lastFlickDown = currentStateDown; //  save the last flicker state
            }
            if ((millis() - lastDebouneDown) > debounceDelay)
            {
                if (LightMode != 0)
                {
                    LightMode -= 1;
                    //  Print to 7 segment
                    sevseg.writeDigitNum(4, LightMode);
                    sevseg.writeDisplay();
                    //  Flip Flag
                    change = 1;
                    //  If debug enabled, print to serial port
                    if (debug == 1 || debug == 2)
                    {
                        Serial.println(LightMode);
                    }
                }
            }
        }
        delay(250);
        //  Change the LED if flag up
        if (change == 1)
        {
            //  Reset Flag
            change = 0;
            //  Call LightMode
            LED(LightMode);
        }
    }
    //  Send to EEPROM
    if (millis() - eeTime > 10)
    {
        hard.update(2, LightMode);
        eeTime = millis();
        //  If debug enabled, print to serial port
        if (debug == 1 || debug == 2)
        {
            Serial.println("EEPROM Store LightMode");
        }
    }
    prevLight = LightMode;
    return;
}
void SetUnits()
{
    // Blink 7 segment and Pre-load
    sevseg.clear();
    sevseg.blinkRate(2);
    if (UnitsMode == 1)
    {
        sevseg.writeDigitRaw(3, 0b01100011);
        sevseg.writeDigitNum(4, 12);
    }
    else if (UnitsMode == 2)
    {
        sevseg.writeDigitRaw(3, 0b01100011);
        sevseg.writeDigitNum(4, 15);
    }
    sevseg.writeDisplay();

    //  Put into While loop that looks for main button press, once button press
    while (digitalRead(MainButton) == HIGH)
    {
        // Taking the current state of the buttons
        currentStateUp = digitalRead(buttonUp);
        currentStateDown = digitalRead(buttonDown);
        // If Up button depressed
        if (currentStateUp == LOW)
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
                    if (debug == 1 || debug == 2)
                    {
                        Serial.println("°F");
                    }
                    sevseg.writeDigitRaw(3, 0b01100011);
                    sevseg.writeDigitNum(4, 15);
                    sevseg.writeDisplay();
                }
            }
        }
        // If Down button depressed
        if (currentStateDown == LOW)
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
                    if (debug == 1 || debug == 2)
                    {
                        Serial.println("°C");
                    }
                    sevseg.writeDigitRaw(3, 0b01100011);
                    sevseg.writeDigitNum(4, 12);
                    sevseg.writeDisplay();
                }
            }
        }
        delay(250);
    }

    //  Converting previous user value to new units
    if (UnitsMode == 1 && prevUnit == 2)
    {
        SetTempC = (SetTempF - 32.0) * (5.0 / 9.0);
    }
    else if (UnitsMode == 2 && prevUnit == 1)
    {
        SetTempF = (SetTempC * (9.0 / 5.0)) + 32.0;
    }

    //  Resetting Previous Units
    prevUnit = UnitsMode;

    //  Send to EEPROM
    if (millis() - eeTime > 10)
    {
        hard.update(3, UnitsMode);
        delay(10);
        hard.update(0, SetTempC);
        delay(10);
        hard.update(1, SetTempF);
        eeTime = millis();
    }

    return;
}
void SetBrightness()
{
    // Blink 7 segment
    sevseg.clear();
    sevseg.blinkRate(2);

    int x = (Brightness / 10U) % 10;
    int y = (Brightness / 1U) % 10;
    if (x != 0)
    {
        sevseg.writeDigitNum(3, x);
    }
    sevseg.writeDigitNum(4, y);
    sevseg.writeDisplay();

    //  Flag for setting Brightness
    bool change = 0;

    //  Put into While loop that looks for main button press, once button press
    while (digitalRead(MainButton) == HIGH)
    {
        // Taking the current state of the buttons
        currentStateUp = digitalRead(buttonUp);
        currentStateDown = digitalRead(buttonDown);
        // If Up button depressed
        if (currentStateUp == LOW)
        {
            //  Checking that there has been enough time betwwen a switch to ignore bounce and noise
            if (currentStateUp != lastFlickUp)
            {
                lastDebouneUp = millis();     //  reset the debounce timer
                lastFlickUp = currentStateUp; //  save the last flicker state
            }
            if ((millis() - lastDebouneUp) > debounceDelay)
            {
                if (Brightness != 15)
                {
                    Brightness += 1;
                    //  Print to 7 segment
                    int x = (Brightness / 10U) % 10;
                    int y = (Brightness / 1U) % 10;

                    sevseg.writeDigitNum(3, x);

                    sevseg.writeDigitNum(4, y);
                    sevseg.writeDisplay();
                    //  Flip Flag
                    change = 1;
                    //  If debug enabled, print to serial port
                    if (debug == 1 || debug == 2)
                    {
                        Serial.println(Brightness);
                    }
                }
            }
        }

        // If Down button depressed
        if (currentStateDown == LOW)
        {
            //  Checking that there has been enough time betwwen a switch to ignore bounce and noise
            if (currentStateDown != lastFlickDown)
            {
                lastDebouneDown = millis();       //  reset the debounce timer
                lastFlickDown = currentStateDown; //  save the last flicker state
            }
            if ((millis() - lastDebouneDown) > debounceDelay)
            {
                if (Brightness != 0)
                {
                    Brightness -= 1;
                    //  Print to 7 segment
                    int x = (Brightness / 10U) % 10;
                    int y = (Brightness / 1U) % 10;

                    sevseg.writeDigitNum(3, x);
                    sevseg.writeDigitNum(4, y);
                    sevseg.writeDisplay();
                    //  Flip Flag
                    change = 1;
                    //  If debug enabled, print to serial port
                    if (debug == 1 || debug == 2)
                    {
                        Serial.println(Brightness);
                    }
                }
            }
        }
        delay(250);
        //  Change the Brightness if flag up
        if (change == 1)
        {
            //  Reset Flag
            change = 0;
            //  Call LightMode
            sevseg.setBrightness(Brightness);
        }
    }
    //  Send to EEPROM
    if (millis() - eeTime > 10)
    {
        hard.update(4, Brightness);
        eeTime = millis();
        //  If debug enabled, print to serial port
        if (debug == 1 || debug == 2)
        {
            Serial.println("EEPROM Store Brightness");
        }
    }
    return;
}
void DoorLight()
{
    LED(4);
    if (debug == 1 || debug == 2)
    {
        Serial.println("Door Open");
    }
}
bool TempCorrect()
{
    float Temp = GetTemp();

    // Actual Function
    // This is where PI control could come into play
    // If setpoint is colder than measured, turn on cooling

    //  If the units are °C
    if (UnitsMode == 1)
    {
        //  If the current temperature is above the set temperature
        if ((Temp - SetTempC) >= 0.0)
        {
            //  Switch cooling flag to cooling
            cooling = 1;
            //  Show cooling on debug
            if (debug == 1 || debug == 2)
            {
                Serial.println("Cooling");
            }
        }
        //  If the current temperature is more than 5 °C below the set temperature
        else if ((Temp - SetTempC) < -5.0)
        {
            //  Switch cooling flag to heating
            cooling = 0;
            //  Show heating on debug
            if (debug == 1 || debug == 2)
            {
                Serial.println("Heating has been disabled");
            }
        }
        else
        {
            //  Turn off the TEC
            analogWrite(RelayControl, 0);
            //  Show off on debug
            if (debug == 1 || debug == 2)
            {
                Serial.println("Off");
            }
            return 0;
        }
        //  If cooling has been enabled
        if (cooling == 1)
        {
            //  If we are above set temperature by 1 or have not reached the set temp yet
            if (bang == false)
            {
                //  Turn on the TEC
                analogWrite(RelayControl, 255);
                //  If we go below the set point
                if ((Temp - SetTempC) == -1.0)
                {
                    //  Switch the controller flag
                    bang = true;
                }
                //  Show cooling to set point
                if (debug == 1)
                {
                    Serial.println("Cooling Down to Setpoint-1");
                }
            }
            //  If we are below the set point
            else if (bang == true)
            {
                //  Turn off the TEC
                analogWrite(RelayControl, 0);
                //  If we are above the set point or have switched from heating
                if ((Temp - SetTempC) == 1.0 || (Temp - SetTempC) >= 0.0)
                {
                    bang = false;
                }
                //  Show off until past set point
                if (debug == 1)
                {
                    Serial.println("Turning Off until Setpoint +1");
                }
            }
        }

        //  If heating has been enabled
        if (cooling == 0)
        {
            // //  If we are above set temperature by 5 or have not reached the set temp yet
            // if (bang == true)
            // {
            //     //  Turn on the TEC
            //     analogWrite(RelayControl, 255);
            //     analogWrite(ThermoElecB, 0);
            //     //  If we go above the set point
            //     if ((Temp - SetTempC) == 1.0)
            //     {
            //         //  Switch the controller flag
            //         bang = false;
            //     }
            //     //  Show heating to set point
            //     if (debug == 1)
            //     {
            //         Serial.println("Heating up to Setpoint +1");
            //     }
            // }
            // //  If we are above the set point
            // else if (bang == false)
            // {
            //     //  Turn off the TEC
            //     analogWrite(RelayControl, 0);
            //     analogWrite(ThermoElecB, 0);
            //     //  If we are below the set point or have switched from cooling
            //     if ((Temp - SetTempC) == -1.0 || (Temp - SetTempC) < -5.0)
            //     {
            //         bang = true;
            //     }
            //     //  Show off until past set point
            //     if (debug == 1)
            //     {
            //         Serial.println("Turning Off until Setpoint -1");
            //     }
            // }
            //  Turn off the TEC
            analogWrite(RelayControl, 0);
            //  Show off on debug
            if (debug == 1 || debug == 2)
            {
                Serial.println("Off");
            }
            return 0;
            if (debug == 1)
            {
                Serial.println("Heating Off");
            }
        }
    }
    //  If the units are °F
    if (UnitsMode == 2)
    {
        //  If the current temperature is above the set temperature
        if ((Temp - SetTempF) >= 0.0)
        {
            //  Switch cooling flag to cooling
            cooling = 1;
            //  Show cooling on debug
            if (debug == 1 || debug == 2)
            {
                Serial.println("Cooling");
            }
        }
        //  If the current temperature is more than 10 °F below the set temperature
        else if ((Temp - SetTempF) < -10.0)
        {
            //  Switch cooling flag to heating
            cooling = 0;
            //  Show heating on debug
            if (debug == 1 || debug == 2)
            {
                Serial.println("Heating");
            }
        }
        else
        {
            //  Turn off the TEC
            analogWrite(RelayControl, 0);
            //  Show off on debug
            if (debug == 1 || debug == 2)
            {
                Serial.println("Off");
            }
            return 0;
        }
        //  If cooling has been enabled
        if (cooling == 1)
        {
            //  If we are above set temperature by 1 or have not reached the set temp yet
            if (bang == false)
            {
                //  Turn on the TEC
                analogWrite(RelayControl, 255);
                //  If we go below the set point
                if ((Temp - SetTempF) == -1.0)
                {
                    //  Switch the controller flag
                    bang = true;
                }
                //  Show cooling to set point
                if (debug == 1)
                {
                    Serial.println("Cooling Down to Setpoint-1");
                }
            }
            //  If we are below the set point
            else if (bang == true)
            {
                //  Turn off the TEC
                analogWrite(RelayControl, 0);
                //  If we are above the set point or have switched from heating
                if ((Temp - SetTempF) == 1.0 || (Temp - SetTempF) >= 0.0)
                {
                    bang = false;
                }
                //  Show off until past set point
                if (debug == 1)
                {
                    Serial.println("Turning Off until Setpoint +1");
                }
            }
        }

        //  If heating has been enabled
        if (cooling == 0)
        {
            // //  If we are above set temperature by 5 or have not reached the set temp yet
            // if (bang == true)
            // {
            //     //  Turn on the TEC
            //     analogWrite(RelayControl, 255);
            //     analogWrite(ThermoElecB, 0);
            //     //  If we go above the set point
            //     if ((Temp - SetTempF) == 1.0)
            //     {
            //         //  Switch the controller flag
            //         bang = false;
            //     }
            //     //  Show heating to set point
            //     if (debug == 1)
            //     {
            //         Serial.println("Heating up to Setpoint+1");
            //     }
            // }
            // //  If we are above the set point
            // else if (bang == false)
            // {
            //     //  Turn off the TEC
            //     analogWrite(RelayControl, 0);
            //     analogWrite(ThermoElecB, 0);
            //     //  If we are below the set point or have switched from cooling
            //     if ((Temp - SetTempF) == -1.0 || (Temp - SetTempF) < -5.0)
            //     {
            //         bang = true;
            //     }
            //     //  Show off until past set point
            //     if (debug == 1)
            //     {
            //         Serial.println("Turning Off until Setpoint -1");
            //     }
            // }
            //  Turn off the TEC
            analogWrite(RelayControl, 0);
            //  Show off on debug
            if (debug == 1 || debug == 2)
            {
                Serial.println("Off");
            }
            return 0;
            if (debug == 1)
            {
                Serial.println("Heating Off");
            }
        }
    }
}
void MenuSelect()
{
    if (debug == 1 || debug == 2)
    {
        Serial.println("Menu");
    }
    unsigned long startMillisMain = millis(); //  Storing the current time
    unsigned long endMillisMain;
    //  Do once flag for MenuSelect
    bool doOnce1 = 1, doOnce2 = 1, doOnce3 = 1, doOnce4 = 1;
    while (digitalRead(MainButton) == LOW) //  Loop until the main button goes low
    {
        if ((millis() - startMillisMain) <= 1000)
        {

            if (doOnce1 == 1)
            {
                if (debug == 1 || debug == 2)
                {
                    Serial.println("Set Temp");
                }
                sevseg.clear();
                sevseg.writeDisplay();
                sevseg.writeDigitNum(1, 1, true);
                sevseg.writeDisplay();
                doOnce1 = 0;
            }
        }
        else if ((millis() - startMillisMain) <= 2000 && (millis() - startMillisMain) > 1000)
        {

            if (doOnce2 == 1)
            {
                if (debug == 1 || debug == 2)
                {
                    Serial.println("Set Lights");
                }
                sevseg.clear();
                sevseg.writeDisplay();
                sevseg.writeDigitNum(1, 2, true);
                sevseg.writeDisplay();
                doOnce2 = 0;
            }
        }
        else if ((millis() - startMillisMain) <= 3000 && (millis() - startMillisMain) > 2000)
        {

            if (doOnce3 == 1)
            {
                if (debug == 1 || debug == 2)
                {
                    Serial.println("Set Units");
                }
                sevseg.clear();
                sevseg.writeDisplay();
                sevseg.writeDigitNum(1, 3, true);
                sevseg.writeDisplay();
                doOnce3 = 0;
            }
        }
        else
        {
            if (doOnce4 == 1)
            {
                if (debug == 1 || debug == 2)
                {
                    Serial.println("Set Brightness");
                }
                sevseg.clear();
                sevseg.writeDisplay();
                sevseg.writeDigitNum(1, 4, true);
                sevseg.writeDisplay();
                doOnce4 = 0;
            }
        }

        endMillisMain = millis(); //  Read the current time
    }

    //if main button pressed for less than one second, go to temp
    //if main button pressed for more than one second and up to five, go to lights
    //if main button pressed longer than 5 seconds, go to units
    if ((endMillisMain - startMillisMain) <= 1000)
    {
        delay(250);
        SetTempInput();
    }
    else if ((endMillisMain - startMillisMain) <= 2000 && (endMillisMain - startMillisMain) > 1000)
    {
        delay(250);
        SetLights();
    }
    else if ((endMillisMain - startMillisMain) <= 3000 && (endMillisMain - startMillisMain) > 2000)
    {
        delay(250);
        SetUnits();
    }
    else
    {
        delay(250);
        SetBrightness();
    }

    sevseg.blinkRate(0);
    doOnce1 = 1, doOnce2 = 1, doOnce3 = 1, doOnce4 = 1;
    time = millis();
    return;
}
void setup()
{
    Serial.begin(115200);
    pinMode(buttonUp, INPUT_PULLUP);
    pinMode(MainButton, INPUT_PULLUP);
    pinMode(buttonDown, INPUT_PULLUP);
    pinMode(door, INPUT_PULLUP);
    pinMode(Thermistor1, INPUT);
    pinMode(Thermistor2, INPUT);
    pinMode(RelayControl, OUTPUT);
    pinMode(ledR, OUTPUT);
    pinMode(ledB, OUTPUT);
    pinMode(ledG, OUTPUT);
    pinMode(ledBright, OUTPUT);

    // Turn all Off
    digitalWrite(ledBright, LOW);
    digitalWrite(ledR, LOW);
    digitalWrite(ledG, LOW);
    digitalWrite(ledB, LOW);
    // Turn on White if door is open
    if (digitalRead(door) == HIGH)
    {
        DoorLight();
    }

    // EEProm set temp recall
    // Checking if control digit is correct before reading the rest
    // Will only write values after first startup
    if (hard.read(5) == 8)
    {
        if (debug == 1 || debug == 2)
        {
            Serial.println("Not first startup");
        }
        //  Read setpoint of temp in Celcius and check that it is in range before setting
        uint8_t presetC = hard.read(0);
        if (presetC >= 0 && presetC <= 100)
        {
            SetTempC = presetC;
            if (debug == 1 || debug == 2)
            {
                Serial.print("Preset Celcius = ");
                Serial.println(presetC);
            }
        }
        //  Read setpoint of temp in Farenheit and check that it is in range before setting
        uint8_t presetF = hard.read(1);
        if (presetF >= 0 && presetF <= 100)
        {
            SetTempF = presetF;
            if (debug == 1 || debug == 2)
            {
                Serial.print("Preset Farenheit = ");
                Serial.println(presetF);
            }
        }
        //  Read setpoint of lights and check that it is in range before setting
        uint8_t presetL = hard.read(2);
        if (presetL >= 0 && presetL <= 100)
        {
            LightMode = presetL;
            if (debug == 1 || debug == 2)
            {
                Serial.print("Preset Light = ");
                Serial.println(presetL);
            }
        }
        //  Read setpoint of units and check that it is in range before setting
        uint8_t presetU = hard.read(3);
        if (presetU >= 0 && presetU <= 100)
        {
            UnitsMode = presetU;
            if (debug == 1 || debug == 2)
            {
                Serial.print("Preset Units = ");
                Serial.println(presetU);
            }
        }
        //  Read setpoint of Brightness and check that it is in range before setting
        uint8_t presetB = hard.read(4);
        if (presetB >= 0 && presetB <= 100)
        {
            Brightness = presetB;
            if (debug == 1 || debug == 2)
            {
                Serial.print("Preset Brightness = ");
                Serial.println(presetB);
            }
        }
    }
    // Switch control digit to designate that this is not the first startup
    hard.update(5, 8);
    //  Setting up the display
    sevseg.begin(0x70);

    //  Pre-Load the Temperature to display
    GetTemp();
}
void loop()
{
    unsigned long currentMillis = millis();

    //  Only check the temperature every 5 seconds
    if ((currentMillis - previousMillis) >= 1000)
    {
        if (debug == 3)
        {
            Serial.print((millis() - time) / 1000);
        }
        previousMillis = currentMillis;
        TempCorrect();
    }
    //  Go to the Menu if the Menu button is pressed
    if (digitalRead(MainButton) == LOW)
    {
        currentStateMain = digitalRead(MainButton);
        //  Checking that there has been enough time betwwen a switch to ignore bounce and noise
        if (currentStateUp != lastFlickUp)
        {
            lastDebouneUp = millis();     //  reset the debounce timer
            lastFlickUp = currentStateUp; //  save the last flicker state
        }
        if ((millis() - lastDebouneUp) > debounceDelay)
        {
            MenuSelect();
        }
    }
    if (digitalRead(door) == HIGH && doorWasOpen == 0)
    {
        prevLight = LightMode;
        DoorLight();
        doorWasOpen = 1;
    }
    else if (digitalRead(door) == LOW && doorWasOpen == 1)
    {
        doorWasOpen = 0;
        LED(prevLight);
        if (debug == 1 || debug == 2)
        {
            Serial.println("Door Closed");
        }
    }
    //  Delay to Prevent Display overwrite
    delay(500);
}

/*
RGB LED
https://github.com/wilmouths/RGBLed

Motor Driver
https://www.pololu.com/product-info-merged/2960

Disp
https://docs.broadcom.com/doc/AV02-0568EN
*/

