/*
    hellothing_BG96_NBIoT.h

    NB-IoT library for the hellothing Arduino shield
    with the Quectel BG96 modem.

    Created 1 April 2019

    This project is licensed under the terms of the MIT license.
    For more information refer to the license file in the Github repository.
*/

#ifndef hellothing_BG96_NBIoT_h
#define hellothing_BG96_NBIoT_h

#define DEBUG

#ifdef DEBUG
#define DEBUG_SERIAL(x) Serial.println(x)
#else
#define DEBUG_SERIAL(x)
#endif

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include <stdio.h>
#include <string.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include "BG96_at_definitions.h"
// the #include statment and code go here...

#define RESPONSE_BUFFER_SIZE 150
#define RETURN_BUFFER_SIZE 20
#define INPUT_BUFFER_SIZE 150

#define DEFAULT_TIMEOUT 350

// Pinout
#define TEMP A0
#define MDM_CURR A1
#define MDM_TX 13
#define MDM_RX 12
#define LED1 11
#define LED0 10
#define MDM_PWR_EN 9
#define nMDM_1V8_OK 8
#define ACCEL_INT 3
#define BUTTON 2

// LTE Bands
#define LTE_B1 "1"
#define LTE_B2 "2"
#define LTE_B3 "4"
#define LTE_B4 "8"
#define LTE_B5 "10"
#define LTE_B8 "80"
#define LTE_B12 "800"
#define LTE_B13 "1000"
#define LTE_B18 "20000"
#define LTE_B19 "40000"
#define LTE_B20 "80000"
#define LTE_B26 "2000000"
#define LTE_B28 "8000000"
#define LTE_CATNB1_ANY "A0E189F"
#define LTE_NO_CHANGE "0"

// GSM Bands
#define GSM_NO_CHANGE "0"
#define GSM_900 "1"
#define GSM_1800 "2"
#define GSM_850 "4"
#define GSM_1900 "8"
#define GSM_ANY "F"

typedef enum
{
  EDGE = 0,
  NB_IOT = 1
} Access_technology_t;

typedef enum
{
  DIGITAL = 0,
  DIGITAL_INVERT = 1,
  ANALOG = 2,
} Attribute_type_t;

struct attr
{
  const char *key;
  int value;
  Attribute_type_t type;
  int8_t pin;
};

class NBIoT
{
public:
  char imei[17];
  char imsi[16];
  char iccid[23];

  StaticJsonDocument<150> JsonDoc;

  NBIoT(Access_technology_t tech);

  NBIoT(Access_technology_t tech, attr *attributes, uint8_t attributes_size);

  /**************************************************************
   * Power management functions
   * ************************************************************/

  /*
    Function to power up the modem
  */
  void modemPowerUp();

  /*
    Function to power down the modem
  */
  void modemPowerDown();

  /*
    Function to reset the modem
  */
  void modemReset();

  /**************************************************************
   * SIM functions
   * ************************************************************/

  /*
    Function to get the SIM ICCID

    return : SIM ICCID string
  */
  char *getICCID();

  /*
    Function to get the SIM IMSI

    return : SIM IMSI string
  */
  char *getIMSI();

  /**************************************************************
   * Modem functions
   * ************************************************************/

  /*
    Function to initiate the modem

    return : success status
  */
  bool modemInit();

  /*
    Function to get the modem IMEI

    return : modem IMEI string
  */
  char *getIMEI();

  /*
    Function to send the device IDs to the platform

    return : success status
  */
  bool sendDeviceID();

  /**************************************************************
   * Network functions
   * ************************************************************/

  /*
    Function to set the Extended Configuration Settings

    return : success status
    ---
    param #1 : EDGE or Cat-NB1 band
  */
  bool setExtConfig(const char *band);

  /*
    Function to get the signal quality

    return : signal quality string
  */
  char *getSignalQuality();

  /*
    Function to get the current service mode

    return : service mode string
  */
  char *getServiceMode();

  /*
    Function to send the signal details to the platform

    return : success status
  */
  bool sendSignalDetails();

  /*
    Function to get the network registration status

    return : success status
  */
  bool getNetworkReg();

  /*
    Function to set the network attatch

    return : success status
  */
  bool setNetworkAttach();

  /*
    Function to get the network attatch status

    return : success status
  */
  bool getNetworkAttach();

  /*
    Function to get the available network operators

    return : success status
  */
  bool getAvailableOperators();

  /*
    Function to get the current selected network operator

    return : success status
  */
  bool getCurrentOperator();

  /*
    Function to set the operator selection

    return : success status
    ---
    param #1 : operator code
  */
  bool setOperator(const char *oper);

  /**************************************************************
   * TCP/IP functions
   * ************************************************************/

  /*
    Function to set the APN

    return : success status
    ---
    param #1 : apn name
  */
  bool setAPN(const char *apn);

  /*
    Function to set the DNS server

    return : success status
    ---
    param #1 : DNS server IP address
  */
  bool setDNS(const char *dns);

  /*
    Function to deactivate the TCP/IP context

    return : success status
  */
  bool deactContext();

  /*
    Function to activate the TCP/IP context

    return : success status
  */
  bool actContext();

  /*
    Function to close the TCP/IP connection

    return : success status
  */
  bool closeConnection();

  /*
    Function to open the TCP/IP connection

    return : success status
    ---
    param #1 : domain name
    param #2 : port number
  */
  bool openConnection(const char *domain, const char *port);

  /*
    Function to send a data string via TCP/IP

    return : success status
    ---
    param #1 : data string
  */
  bool sendData(char *data);

  /**************************************************************
  * Sensor/attributes functions
  * ************************************************************/

  /*
    Function to get the temperature reading

    return : temperature value
  */
  float getTemp();

  /*
    Function to register the outputs on the platform

    return : success status
  */
  bool registerOutputs();

private:
  SoftwareSerial *MDM_serial;
  bool _response;
  uint32_t _timeout;
  char _buff[RESPONSE_BUFFER_SIZE + 1];
  char _return_buff[RETURN_BUFFER_SIZE + 1];
  char _input_buff[INPUT_BUFFER_SIZE + 1];
  char *_json_key;
  attr *_attributes;
  uint8_t _attributes_size;
  char _at_cmd[100];
  char _at_resp[20];
  char *_pt;
  char _error[6];
  Access_technology_t _access_tech;
  float tempValue;

  bool sendATCmdResp();
  bool readModemResp();
  void flushBuffer();
  void setAtCmd(const char *memstring);
  void setAtResp(const char *memstring);
};

#endif