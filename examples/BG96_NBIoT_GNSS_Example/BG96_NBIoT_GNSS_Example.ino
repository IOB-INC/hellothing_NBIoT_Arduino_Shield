/*
    BG96_NBIoT_GNSS_Example.ino

    GNSS example for the hellothing NB-IoT Arduino
    shield with the Quectel BG96 modem.

    Created 1 April 2019
*/

#include "hellothing_BG96_NBIoT.h"

#define INPUT_SIZE 100
#define DEBUG_BAUD_RATE 19200

typedef enum
{
    reg = 0,
    tracking = 1,
    turn_off = 2,
    off = 3,
} Process_state_t;

Process_state_t state = turn_off;

void getGPSPosition(void);
void buttonISR(void);

attr outputs[4] = {
    {"LED0", 0, DIGITAL_INVERT, LED0},
    {"LIVE_GPS", 1, DIGITAL_INVERT, LED1},
    {"PERIOD", 60, ANALOG, -1},
    {"HEARTBEAT", 600, ANALOG, -1}};

NBIoT *BG96_NBIoT = new NBIoT(EDGE, outputs, 4); // EDGE or NB_IOT

char input[INPUT_SIZE + 1];
char *coordinate;

uint8_t i;
uint32_t time = 0;
uint32_t sleep = 60000;

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
    digitalWrite(LED1, LOW);

    attachInterrupt(digitalPinToInterrupt(BUTTON), buttonISR, RISING);

    Serial.begin(DEBUG_BAUD_RATE);
    Serial.println(F("BG96 NB-IoT GNSS example!"));

    BG96_NBIoT->modemPowerUp();

    BG96_NBIoT->modemInit();
    BG96_NBIoT->setAPN("internet");
    BG96_NBIoT->setOperator("65501");
    BG96_NBIoT->setExtConfig(GSM_ANY);
    delay(3000);

    strcpy(BG96_NBIoT->imei, BG96_NBIoT->getIMEI());
    strcpy(BG96_NBIoT->imsi, BG96_NBIoT->getIMSI());
    strcpy(BG96_NBIoT->iccid, BG96_NBIoT->getICCID());
}

void loop()
{
    if (outputs[1].value)
    {
        sleep = outputs[2].value;
    }
    else
    {
        sleep = outputs[3].value;
    }

    switch (state)
    {
    case reg:
        BG96_NBIoT->getNetworkReg();
        BG96_NBIoT->getSignalQuality();
        if (BG96_NBIoT->getNetworkAttach())
        {
            for (i = 0; i < 3; i++)
            {
                if (BG96_NBIoT->openConnection("thingcola-stage.hellothing.com", "30001"))
                {
                    BG96_NBIoT->JsonDoc["id"] = BG96_NBIoT->imei;
                    serializeJson(BG96_NBIoT->JsonDoc, input);
                    BG96_NBIoT->JsonDoc.clear();
                    BG96_NBIoT->sendData(input);

                    BG96_NBIoT->registerOutputs();
                    BG96_NBIoT->sendDeviceID();
                    break;
                }
            }
            BG96_NBIoT->closeConnection();
        }
        if (BG96_NBIoT->turnOnGNSS())
        {
            state = tracking;
        }
        break;

    case tracking:
        if (((millis() - time) / 1000) > sleep)
        {
            time = millis();
            BG96_NBIoT->getNetworkReg();
            BG96_NBIoT->getSignalQuality();
            if (BG96_NBIoT->getNetworkAttach())
            {
                for (i = 0; i < 3; i++)
                {
                    if (BG96_NBIoT->openConnection("thingcola-stage.hellothing.com", "30001"))
                    {
                        BG96_NBIoT->JsonDoc["id"] = BG96_NBIoT->imei;
                        serializeJson(BG96_NBIoT->JsonDoc, input);
                        BG96_NBIoT->JsonDoc.clear();
                        BG96_NBIoT->sendData(input);

                        BG96_NBIoT->sendSignalDetails();
                        getGPSPosition();

                        break;
                    }
                }
                BG96_NBIoT->closeConnection();
            }
        }
        break;

    case turn_off:
        BG96_NBIoT->turnOffGNSS();
        state = off;
        break;

    case off:
        break;

    default:
        break;
    }
}

void buttonISR(void)
{
    if (state == tracking)
    {
        state = turn_off;
    }
    else if (state == off)
    {
        state = reg;
    }
}

void getGPSPosition(void)
{
    strcpy(input, BG96_NBIoT->getGNSSPositionInformation());

    coordinate = strtok(input, ",");
    coordinate = strtok(NULL, ",");
    BG96_NBIoT->JsonDoc["lat"] = coordinate;
    coordinate = strtok(NULL, ",");
    BG96_NBIoT->JsonDoc["long"] = coordinate;

    serializeJson(BG96_NBIoT->JsonDoc, input);
    BG96_NBIoT->JsonDoc.clear();

    BG96_NBIoT->sendData(input);
}