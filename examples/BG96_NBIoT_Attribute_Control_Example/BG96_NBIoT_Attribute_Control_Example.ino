/*
    BG96_NBIoT_Attribute_Control_Example.ino

    Attribute control example for the hellothing NB-IoT Arduino
    shield with the Quectel BG96 modem.

    Created 1 Augustus 2019
*/

#include "hellothing_BG96_NBIoT.h"

#define INPUT_SIZE 50
#define DEBUG_BAUD_RATE 19200
#define REPORTING_PERIOD 60000

typedef enum
{
    netAttach = 0,
    openTCP = 1,
    initPacket = 2,
    regOutputs = 3,
    sendID = 4,
    sendSignal = 5,
    sendData = 6,
    closeTCP = 7,
} Process_state_t;

Process_state_t state = netAttach;

bool sendTemperature(void);

attr outputs[3] = {
    {"LED0", 0, DIGITAL_INVERT, 10},
    {"LED1", 0, DIGITAL_INVERT, 11},
    {"SLEEP", 60, ANALOG, -1}};

NBIoT *BG96_NBIoT = new NBIoT(NB_IOT, outputs, 3); // EDGE or NB_IOT

char input[INPUT_SIZE + 1];

bool outputsRegistered = false;
uint32_t sendTime = 0;

void setup()
{
    analogReference(EXTERNAL);
    pinMode(MDM_PWR_EN, OUTPUT);
    pinMode(MDM_RX, INPUT);
    pinMode(MDM_TX, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP);
    pinMode(LED0, OUTPUT);
    pinMode(LED1, OUTPUT);

    digitalWrite(LED0, HIGH);
    digitalWrite(LED1, HIGH);

    Serial.begin(DEBUG_BAUD_RATE);
    Serial.println(F("BG96 NB-IoT Attribute Control example!"));

    BG96_NBIoT->modemPowerUp();

    BG96_NBIoT->modemInit();
    BG96_NBIoT->setAPN("nbiot.vodacom.za");
    BG96_NBIoT->setOperator("65501");
    BG96_NBIoT->setExtConfig(LTE_B8);
    delay(3000);

    strcpy(BG96_NBIoT->imei, BG96_NBIoT->getIMEI());
    strcpy(BG96_NBIoT->imsi, BG96_NBIoT->getIMSI());
    strcpy(BG96_NBIoT->iccid, BG96_NBIoT->getICCID());
}

void loop()
{
    switch (state)
    {
    case netAttach:
        if (millis() - sendTime > REPORTING_PERIOD)
        {
            sendTime = millis();
            if (BG96_NBIoT->getNetworkAttach())
            {
                state = openTCP;
            }
        }
        break;

    case openTCP:
        if (BG96_NBIoT->openConnection("thingcola.hellothing.com", "30001"))
        {
            state = initPacket;
        }
        else
        {
            state = closeTCP;
        }
        break;

    case initPacket:
        BG96_NBIoT->JsonDoc["id"] = BG96_NBIoT->imei;
        serializeJson(BG96_NBIoT->JsonDoc, input);
        BG96_NBIoT->JsonDoc.clear();

        if (BG96_NBIoT->sendData(input))
        {
            if (!outputsRegistered)
            {
                state = regOutputs;
            }
            else
            {
                state = sendSignal;
            }
        }
        break;

    case regOutputs:
        if (BG96_NBIoT->registerOutputs())
        {
            outputsRegistered = true;
            state = sendID;
        }
        break;

    case sendID:
        if (BG96_NBIoT->sendDeviceID())
        {
            state = sendSignal;
        }
        break;

    case sendSignal:
        if (BG96_NBIoT->sendSignalDetails())
        {
            state = sendData;
        }
        break;

    case sendData:
        if (sendTemperature())
        {
            state = closeTCP;
        }
        break;

    case closeTCP:
        if (BG96_NBIoT->closeConnection())
        {
            state = netAttach;
        }
        break;

    default:
        break;
    }
}

bool sendTemperature(void)
{
    BG96_NBIoT->JsonDoc["temp"] = round(BG96_NBIoT->getTemp() * 100) / 100;
    serializeJson(BG96_NBIoT->JsonDoc, input);
    BG96_NBIoT->JsonDoc.clear();

    return BG96_NBIoT->sendData(input);
}