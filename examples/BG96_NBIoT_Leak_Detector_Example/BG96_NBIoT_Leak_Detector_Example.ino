/*
    BG96_NBIoT_Leak_Detector_Example.ino

    Leak detection example for the hellothing NB-IoT Arduino
    shield with the Quectel BG96 modem.

    Created 1 April 2019
*/

#include "hellothing_BG96_NBIoT.h"

#define INPUT_SIZE 100

NBIoT *BG96_NBIoT = new NBIoT();

int BUTTON = 2;
int SENSOR = 3;
int LED = 5;
int M1A = 6;
int M1B = 7;

char input[INPUT_SIZE + 1];
char *imei;

void setup()
{

    Serial.begin(19200);

    pinMode(9, OUTPUT);
    digitalWrite(9, HIGH);

    // Set LED pin to output and turn LED off
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);

    // Set output pins for H-bridge
    pinMode(M1A, OUTPUT);
    pinMode(M1B, OUTPUT);
    digitalWrite(M1A, LOW);
    digitalWrite(M1B, LOW);

    // Set input for pushbutton
    pinMode(BUTTON, INPUT_PULLUP);

    // Set input for water sensor
    pinMode(SENSOR, INPUT_PULLUP);

    BG96_NBIoT->modemPowerUp();

    BG96_NBIoT->modemInit();
    BG96_NBIoT->setTCPAPN();

    BG96_NBIoT->setNBIoTDetails();
    delay(3000);
    imei = BG96_NBIoT->getIMEI();
    BG96_NBIoT->deactTCPContext();
}

void closeValve()
{
    digitalWrite(M1A, LOW);
    digitalWrite(M1B, HIGH);
    delay(3000);
    digitalWrite(M1B, LOW);
}

void openValve()
{
    digitalWrite(M1A, HIGH);
    digitalWrite(M1B, LOW);
    delay(3000);
    digitalWrite(M1A, LOW);
}

void RemoveValveDrive()
{
    digitalWrite(M1A, LOW);
    digitalWrite(M1B, LOW);
}

void LEDon()
{
    digitalWrite(LED, LOW);
}

void LEDoff()
{
    digitalWrite(LED, HIGH);
}

void sendLeakState(uint8_t state)
{
    BG96_NBIoT->actTCPContext();
    BG96_NBIoT->openTCPConnection();
    sprintf(input, "%s%s%s%d%s", "{\"id\":\"", imei, "\",\"data\":[{\"id\":\"leak1\",\"data\":{\"value\":", state, "}}]}");
    BG96_NBIoT->sendTCPData(input);
    BG96_NBIoT->closeTCPConnection();
    BG96_NBIoT->deactTCPContext();
}

void loop()
{

    if (digitalRead(SENSOR) == LOW)
    {
        Serial.println("Leak detected");
        // Close the water shutoff valve
        closeValve();
        sendLeakState(1);
        while ((digitalRead(BUTTON) == HIGH) || (digitalRead(SENSOR) == LOW))
        {
            LEDon();
            delay(200);
            LEDoff();
            delay(200);
        }
        // Once the leak has been cleared and the button is pressed open the valve
        openValve();
        sendLeakState(0);
    }
    else
    {
        Serial.println("No leak detected");
    }

    delay(1000);
    LEDon();
    delay(10);
    LEDoff();
}
