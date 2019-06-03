/*
    BG96_NBIoT_Temperature_Example.ino

    Temperature example for the hellothing NB-IoT Arduino
    shield with the Quectel BG96 modem.

    Created 1 April 2019
*/

#include "hellothing_BG96_NBIoT.h"

#define INPUT_SIZE 100
#define DEBUG_BAUD_RATE 19200
#define SLEEP_PERIOD 60000

NBIoT *BG96_NBIoT = new NBIoT(NB_IOT); // EDGE or NB_IOT

char *imei;
char input[INPUT_SIZE + 1]; // Get next command from Serial (add 1 for final 0)
float tempValue;

uint8_t i;

void setup()
{
    analogReference(EXTERNAL);
    pinMode(MDM_PWR_EN, OUTPUT);
    pinMode(MDM_RX, INPUT);
    pinMode(MDM_TX, OUTPUT);

    Serial.begin(DEBUG_BAUD_RATE);
    Serial.println("BG96 NB-IoT temperature example!");

    BG96_NBIoT->modemPowerUp();

    BG96_NBIoT->modemInit();
    BG96_NBIoT->setTCPAPN();

    BG96_NBIoT->setExtConfig();
    delay(3000);
    imei = BG96_NBIoT->getIMEI();
    BG96_NBIoT->getServiceMode();
}

void loop()
{
    BG96_NBIoT->getNetworkReg();
    BG96_NBIoT->getSignalQuality();
    if (BG96_NBIoT->getNetworkAttach())
    {
        for (i = 0; i < 3; i++)
        {
            if (BG96_NBIoT->openTCPConnection())
            {
                BG96_NBIoT->initDataPacket(imei);
                getTempReading();
                break;
            }
        }
        BG96_NBIoT->closeTCPConnection();
    }
    delay(SLEEP_PERIOD);
}

void getTempReading(void)
{
    tempValue = analogRead(TEMP);
    tempValue = ((8.194 - sqrt(81.017156 - (0.02096 * tempValue))) / (-0.0052)) + 30.0;

    char result[8];
    dtostrf(tempValue, 6, 2, result);

    sprintf(input, "%s%s%s", "{\"temp\":\"", result, "\"}");
    BG96_NBIoT->sendTCPData(input);
}