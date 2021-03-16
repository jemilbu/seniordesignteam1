# 1 "c:\\Users\\Owner\\Pictures\\VSCode\\seniordesignteam1\\Projectcode.ino"
# 1 "c:\\Users\\Owner\\Pictures\\VSCode\\seniordesignteam1\\Projectcode.ino"
//#include <EEPROM.h>
# 3 "c:\\Users\\Owner\\Pictures\\VSCode\\seniordesignteam1\\Projectcode.ino" 2
# 4 "c:\\Users\\Owner\\Pictures\\VSCode\\seniordesignteam1\\Projectcode.ino" 2


// Button Pins
const int buttonUp = 40, buttonDown = 42, MainButton = 36;
// LED Pins
const int ledR = 26, ledB = 22, ledG = 24;
// Thermoelectric LED Test Pin
const int ThermoElecLED = 30;
// Thermoelectric Control Pins
const int ThermoElecA = 45, ThermoElecB = 46, savePower = 31;
// Thermistor Pins
const int Thermistor1 = A0, Thermistor2 = A1;
//  Global Set Temperature, Thermistor Temp and Light Mode
volatile double SetTempC = 0, SetTempF = 32;
// Global Light and Units Mode
volatile int LightMode, UnitsMode = 1;

double time;

//  Thermoelectric Motor Driver pins
const int speed = 0, direct = 0;

int currentStateUp; //  State of push button 1
int currentStateDown; //  State of push button 2
int currentStateMain; //  State of push button 3
const double debounceDelay = 0.5; //  Debounce time; increase if output flickers

int lastSteadyUp = 0x1; //  previous steady state from input
int lastFlickUp = 0x1; //  previous flicker state from input
unsigned long int lastDebouneUp = 0; //the last time the output was toggled

int lastSteadyDown = 0x1; //  previous steady state from input
int lastFlickDown = 0x1; //  previous flicker state from input
unsigned long int lastDebouneDown = 0; //the last time the output was toggled

int lastSteadyMain = 0x1; //  previous steady state from input
int lastFlickMain = 0x1; //  previous flicker state from input
unsigned long int lastDebouneMain = 0; //the last time the output was toggled

unsigned long previousMillisMain = 0; // Stores the main was pressed

//  Create an instance of the RGB object
RGBLed led(ledR, ledG, ledB, RGBLed::COMMON_ANODE);

//  Creating an instance of the 7 segment object
Adafruit_7segment sevseg = Adafruit_7segment();

//  Pins for 7 segment I2C

int GetTemp()
{
    double sensorvalue = analogRead(Thermistor1); //Read Pin A0
    double voltage = sensorvalue * (5.0 / 1023.0); //convert sensor value to voltage
    double Rtherm = ((50 / voltage) - 10) * 1000; //Calculates thermistor resistance in Ohm
    double tempK = 1 / (0.001032 + (0.0002387 * log(Rtherm)) + (0.000000158 * (log(Rtherm) * log(Rtherm) * log(Rtherm)))); //calculates associated temp in k
    double Ttherm1 = tempK - 273.15;

    // double sensorvalue2 = analogRead(Thermistor2);                                                                              //Read Pin A0
    // double voltage2 = sensorvalue2 * (5.0 / 1023.0);                                                                            //convert sensor value to voltage
    // double Rtherm2 = ((50 / voltage2) - 10) * 1000;                                                                             //Calculates thermistor resistance in Ohm
    // double tempK2 = 1 / (0.001032 + (0.0002387 * log(Rtherm2)) + (0.000000158 * (log(Rtherm2) * log(Rtherm2) * log(Rtherm2)))); //calculates associated temp in k
    // double Ttherm2 = tempK2 - 273.15;

    // double TthermAvg = (Ttherm1 + Ttherm2) / 2.0;

    double TthermAvg = Ttherm1;

    // Print temp to screen here
    Serial.print("Current Temp = ");

    // If display is in Farenheit
    if (UnitsMode == 2)
    {
        double TthermF = (TthermAvg * (9.0 / 5.0)) + 32.0;
        Serial.print(TthermF);
        Serial.println("°F");
        return TthermF;
    }
    else if (UnitsMode == 1)
    {
        Serial.print(TthermAvg);
        Serial.println("°C");
        return TthermAvg;
    }
}
void LED(int mode)
{
    // Turn all Off
    digitalWrite(ledR,0x1);
    digitalWrite(ledG,0x1);
    digitalWrite(ledB,0x1);
    //  Red
    if (mode == 1)
    {
        digitalWrite(ledR,0x0);
    }
    //  Green
    else if(mode == 2)
    {
        digitalWrite(ledG,0x0);
    }
    else if(mode == 3)
    {
        digitalWrite(ledB,0x0);
    }
}
void SetTempInput()
{
    delay(500);
    // Blink 7 segment

    // While loop this until main button is pressed, store to EEprom once complete
    // Make sure to put a counter on eeprom to prevent overwrite
    delay(1000);
    while (digitalRead(MainButton) == 0x1)
    {
        // Taking the current state of the buttons
        currentStateUp = digitalRead(buttonUp);
        currentStateDown = digitalRead(buttonDown);

        // If Up button depressed
        if (currentStateUp == 0x0)
        {
            //  Checking that there has been enough time betwwen a switch to ignore bounce and noise
            if (currentStateUp != lastFlickUp)
            {
                lastDebouneUp = millis(); //  reset the debounce timer
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
                        Serial.println(SetTempC, 0);
                    }
                }
                else if (UnitsMode == 2)
                {
                    // Checking if SetTemp is in the useful range, changing if it is
                    if (SetTempF != 176) // Farenheit
                    {
                        SetTempF += 1;
                        Serial.println(SetTempF, 0);
                    }
                }
            }
        }
        // If Down button depressed
        if (currentStateDown == 0x0)
        {
            if (UnitsMode == 1)
            {
                // Checking if SetTemp is in the useful range, changing if it is
                if (SetTempC != 0) // Celcius
                {
                    SetTempC -= 1;
                    Serial.println(SetTempC, 0);
                }
            }
            else if (UnitsMode == 2)
            {
                // Checking if SetTemp is in the useful range, changing if it is
                if (SetTempF != 32) // Farenheit
                {
                    SetTempF -= 1;
                    Serial.println(SetTempF, 0);
                }
            }
        }
        delay(250);
    }
    return;
}
void SetLights()
{
    delay(500);
    // Blink 7 segment

    //  Put into While loop that looks for main button press, once button press: send to eeprom (put counter), send to LED program?

    while (digitalRead(MainButton) == 0x1)
    {
        // Taking the current state of the buttons
        currentStateUp = digitalRead(buttonUp);
        currentStateDown = digitalRead(buttonDown);
        // If Up button depressed
        if (currentStateUp == 0x0)
        {
            //  Checking that there has been enough time betwwen a switch to ignore bounce and noise
            if (currentStateUp != lastFlickUp)
            {
                lastDebouneUp = millis(); //  reset the debounce timer
                lastFlickUp = currentStateUp; //  save the last flicker state
            }
            if ((millis() - lastDebouneUp) > debounceDelay)
            {
                if (LightMode != 3)
                {
                    LightMode += 1;
                    Serial.println(LightMode);
                }
            }
        }
        // If Down button depressed
        if (currentStateDown == 0x0)
        {
            //  Checking that there has been enough time betwwen a switch to ignore bounce and noise
            if (currentStateDown != lastFlickDown)
            {
                lastDebouneDown = millis(); //  reset the debounce timer
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
        LED(LightMode);
    }
    return;
}
void SetUnits()
{
    delay(500);
    // Blink 7 segment

    //  Put into While loop that looks for main button press, once button press: send to eeprom (put counter), send to LED program?

    while (digitalRead(MainButton) == 0x1)
    {
        // Taking the current state of the buttons
        currentStateUp = digitalRead(buttonUp);
        currentStateDown = digitalRead(buttonDown);
        // If Up button depressed
        if (currentStateUp == 0x0)
        {
            //  Checking that there has been enough time betwwen a switch to ignore bounce and noise
            if (currentStateUp != lastFlickUp)
            {
                lastDebouneUp = millis(); //  reset the debounce timer
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
        if (currentStateDown == 0x0)
        {
            //  Checking that there has been enough time betwwen a switch to ignore bounce and noise
            if (currentStateDown != lastFlickDown)
            {
                lastDebouneDown = millis(); //  reset the debounce timer
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
bool TempCorrect()
{
    double Temp = GetTemp();
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
    if (UnitsMode == 1)
    {
        if ((Temp - SetTempC) > 1.0)
        {
            analogWrite(ThermoElecA, 255);
            analogWrite(ThermoElecB, 0);
        }
        // If setpoint is much warmer than measured, turn on heating
        else if ((SetTempC - Temp) > 5.0)
        {
            analogWrite(ThermoElecA, 0);
            analogWrite(ThermoElecB, 255);
        }
        // Save power if in range
        else
        {
            analogWrite(ThermoElecA, 0);
            analogWrite(ThermoElecB, 0);
        }
    }
    else if (UnitsMode == 2)
    {
        if ((Temp - SetTempF) > 5.0)
        {
            analogWrite(ThermoElecA, 255);
            analogWrite(ThermoElecB, 0);
        }
        // If setpoint is much warmer than measured, turn on heating
        else if ((SetTempF - Temp) > 10.0)
        {
            analogWrite(ThermoElecA, 0);
            analogWrite(ThermoElecB, 255);
        }
        // Save power if in range
        else
        {
            analogWrite(ThermoElecA, 0);
            analogWrite(ThermoElecB, 0);
        }
    }
}
void MenuSelect()
{
    //debounce delay
    Serial.println("Menu");
    unsigned long startMillisMain = millis(); //  Storing the current time
    unsigned long endMillisMain;
    while (digitalRead(MainButton) == 0x0) //  Loop until the main button goes low
    {
        endMillisMain = millis(); //  Read the current time
    }

    //if main button pressed for less than one second, go to temp
    //if main button pressed for more than one second and up to five, go to lights
    //if main button pressed longer than 5 seconds, go to units
    if ((endMillisMain - startMillisMain) <= 1000)
    {
        Serial.println("Set Temp");
        SetTempInput();
    }
    else if ((endMillisMain - startMillisMain) <= 4000 && (endMillisMain - startMillisMain) > 1000)
    {
        Serial.println("Set Lights");
        SetLights();
    }
    else
    {
        Serial.println("Set Units");
        SetUnits();
    }
    time = millis();
    return;
}
void setup()
{
    Serial.begin(115200);
    pinMode(buttonUp, 0x2);
    pinMode(MainButton, 0x2);
    pinMode(buttonDown, 0x2);
    pinMode(Thermistor1, 0x0);
    pinMode(Thermistor2, 0x0);
    pinMode(ThermoElecLED, 0x1);
    pinMode(ThermoElecA, 0x1);
    pinMode(ThermoElecB, 0x1);
    pinMode(savePower, 0x1);
    pinMode(ledR, 0x1);
    pinMode(ledB, 0x1);
    pinMode(ledG, 0x1);
    pinMode(speed, 0x1);
    pinMode(direct, 0x1);

    // Turn all Off
    digitalWrite(ledR,0x1);
    digitalWrite(ledG,0x1);
    digitalWrite(ledB,0x1);

    // EEProm set temp recall

}
void loop()
{
    // Add system timer interupt so temp correct doesn't run constantly but isn't in ISR
    Serial.print((millis()-time)/1000);
    TempCorrect();
    if (digitalRead(MainButton) == 0x0)
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
