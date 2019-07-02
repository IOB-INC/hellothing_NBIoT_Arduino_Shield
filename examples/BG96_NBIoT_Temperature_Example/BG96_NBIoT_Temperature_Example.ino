/*
    BG96_NBIoT_Temperature_Example.ino

    Temperature example for the hellothing NB-IoT Arduino
    shield with the Quectel BG96 modem.

    Created 1 April 2019
*/

#include "hellothing_BG96_NBIoT.h"

#define INPUT_SIZE 50
#define DEBUG_BAUD_RATE 19200
#define SLEEP_PERIOD 60000

bool sendTemperatue(void);

NBIoT *BG96_NBIoT = new NBIoT(NB_IOT); // EDGE or NB_IOT

char input[INPUT_SIZE + 1];

uint8_t i;

void setup()
{
    analogReference(EXTERNAL);
    pinMode(MDM_PWR_EN, OUTPUT);
    pinMode(MDM_RX, INPUT);
    pinMode(MDM_TX, OUTPUT);

    Serial.begin(DEBUG_BAUD_RATE);
    Serial.println(F("BG96 NB-IoT temperature example!"));

    BG96_NBIoT->modemPowerUp();

    BG96_NBIoT->modemInit();
    BG96_NBIoT->setAPN("nbiot.vodacom.za");
    BG96_NBIoT->setOperator("65501");
    BG96_NBIoT->setExtConfig(GSM_ANY);
    delay(3000);

    strcpy(BG96_NBIoT->imei, BG96_NBIoT->getIMEI());
    strcpy(BG96_NBIoT->imsi, BG96_NBIoT->getIMSI());
    strcpy(BG96_NBIoT->iccid, BG96_NBIoT->getICCID());
}

void loop()
{
    BG96_NBIoT->getNetworkReg();
    BG96_NBIoT->getSignalQuality();
    if (BG96_NBIoT->getNetworkAttach())
    {
        for (i = 0; i < 3; i++)
        {
            if (BG96_NBIoT->openConnection("thingcola.hellothing.com", "30001"))
            {
                BG96_NBIoT->initDataPacket();
                BG96_NBIoT->sendCommDetails();
                sendTemperatue();
                break;
            }
        }
        BG96_NBIoT->closeConnection();
    }
    delay(SLEEP_PERIOD);
}

bool sendTemperatue(void)
{
    char result[8];
    dtostrf(BG96_NBIoT->getTemp(), 6, 2, result);

    sprintf(input, "%s%s%s", "{\"temp\":\"", result, "\"}");
    return BG96_NBIoT->sendData(input);
}