/*
    BG96_NBIoT_Leak_Detector_Example.ino

    Leak detection example for the hellothing NB-IoT Arduino
    shield with the Quectel BG96 modem.

    Created 1 April 2019
*/

#include "hellothing_BG96_NBIoT.h"

#define INPUT_SIZE 100

NBIoT *BG96_NBIoT = new NBIoT(NB_IOT); // EDGE or NB_IOT

int SENSOR = 3;
int LED = 5;
int M1A = 6;
int M1B = 7;

char input[INPUT_SIZE + 1];

uint8_t i;

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
    BG96_NBIoT->setAPN("nbiot.vodacom.za");
    BG96_NBIoT->setOperator("65501");
    BG96_NBIoT->setExtConfig(GSM_ANY);
    delay(3000);

    strcpy(BG96_NBIoT->imei, BG96_NBIoT->getIMEI());
    strcpy(BG96_NBIoT->imsi, BG96_NBIoT->getIMSI());
    strcpy(BG96_NBIoT->iccid, BG96_NBIoT->getICCID());
    BG96_NBIoT->getSignalQuality();
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
    if (BG96_NBIoT->getNetworkAttach())
    {
        for (i = 0; i < 3; i++)
        {
            if (BG96_NBIoT->openConnection("thingcola.hellothing.com", "30001"))
            {
                BG96_NBIoT->initDataPacket();
                sprintf(input, "%s%d%s", "{\"leak\":\"", state, "\"}");
                BG96_NBIoT->sendData(input);
                break;
            }
        }
        BG96_NBIoT->closeConnection();
    }
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
