/*
    BG96_NBIoT_Temperature_Example.ino

    Temperature example for the hellothing NB-IoT Arduino
    shield with the Quectel BG96 modem.

    Created 1 April 2019
*/

#include "hellothing_BG96_NBIoT.h"

#define INPUT_SIZE 100

NBIoT *BG96_NBIoT = new NBIoT();

char *imei;
char input[INPUT_SIZE + 1]; // Get next command from Serial (add 1 for final 0)
int tempAnalog;
float tempValue;
float analogToVoltage = 3300.0 / 1024.0; // 3.3V analog reference
// float analogToVoltage = 5000.0 / 1024.0;    // 5V analog reference

bool test;

void setup()
{
    analogReference(EXTERNAL);
    pinMode(MDM_ENABLE_PIN, OUTPUT);
    pinMode(MDM_RX_PIN, INPUT);
    pinMode(MDM_TX_PIN, OUTPUT);

    Serial.begin(19200);
    Serial.println("BG96 NB-IoT temperature example!");

    BG96_NBIoT->modemPowerUp();

    test = BG96_NBIoT->modemInit();
    Serial.println(test);
    BG96_NBIoT->setTCPAPN();

    BG96_NBIoT->setNBIoTDetails();
    delay(3000);
    BG96_NBIoT->getCCID();
    BG96_NBIoT->getIMSI();
    imei = BG96_NBIoT->getIMEI();
    BG96_NBIoT->getSignalQuality();
    BG96_NBIoT->getServiceMode();
    BG96_NBIoT->deactTCPContext();
}

void loop()
{
    BG96_NBIoT->actTCPContext();
    BG96_NBIoT->openTCPConnection();
    getTempReading();
    BG96_NBIoT->closeTCPConnection();
    BG96_NBIoT->deactTCPContext();
    delay(60000);
}

void getTempReading(void)
{
    tempAnalog = analogRead(TEMPERATURE_PIN);
    tempValue = ((tempAnalog * analogToVoltage) - 500) / 10;

    char result[8];
    dtostrf(tempValue, 6, 2, result);

    sprintf(input, "%s%s%s%s", "{\"id\":\"", imei, "\",\"data\":[{\"id\":\"temp1\",\"data\":{\"value\":", result, "}}]}");
    uint8_t test = BG96_NBIoT->sendTCPData(input);
}