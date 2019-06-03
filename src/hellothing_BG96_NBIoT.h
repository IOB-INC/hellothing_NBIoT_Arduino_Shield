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
#define DEBUG_PRINT(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#endif

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include <stdio.h>
#include <string.h>
#include <SoftwareSerial.h>
// the #include statment and code go here...

#define RESPONSE_BUFFER_SIZE 100 //response buffer to small for data packet with modem echo.
#define RETURN_BUFFER_SIZE 20
#define INPUT_BUFFER_SIZE 50

#define DEFAULT_TIMEOUT 350

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

typedef enum
{
  EDGE = 0,
  NB_IOT = 1
} access_technology;

class NBIoT
{
public:
  NBIoT(access_technology tech);

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
    Function to get the SIM CCID

    return : SIM CCID string
  */
  char *getCCID();

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

  /**************************************************************
   * Network functions
   * ************************************************************/

  /*
    Function to set the Extended Configuration Settings

    return : success status
  */
  bool setExtConfig();

  /*
    Function to get the signal quality

    return : signal quality string
  */
  char *getSignalQuality();

  /*
    Function to get the current service mode

    return : success status
  */
  char *getServiceMode();

  /*
    Function to set the network registration

    return : success status
  */
  bool setNetworkReg();

  /*
    Function to get the network registration status

    return : success status
  */
  int getNetworkReg();

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

  /**************************************************************
   * TCP/IP functions
   * ************************************************************/

  /*
    Function to set the APN

    return : success status
  */
  bool setTCPAPN();

  /*
    Function to deactivate the TCP/IP context

    return : success status
  */
  bool deactTCPContext();

  /*
    Function to activate the TCP/IP context

    return : success status
  */
  bool actTCPContext();

  /*
    Function to close the TCP/IP connection

    return : success status
  */
  bool closeTCPConnection();

  /*
    Function to open the TCP/IP connection

    return : success status
    ---
    param #1 : domain name
    param #2 : port number
  */
  bool openTCPConnection();

  /*
    Function to sync device data packet to digital representation

    return : success status
    ---
    param #1 : imei string
  */
  bool initDataPacket(const char *id);

  /*
    Function to send a data string via TCP/IP

    return : success status
    ---
    param #1 : data string
  */
  bool sendTCPData(char *data);

private:
  SoftwareSerial *MDM_serial;
  bool _response;
  uint32_t _timeout;
  char _response_buffer[RESPONSE_BUFFER_SIZE + 1];
  char _return_buffer[RETURN_BUFFER_SIZE + 1];
  char _input_buffer[INPUT_BUFFER_SIZE + 1];
  char *_pt;
  char *_error;
  access_technology _access_tech;

  bool sendATCommOK(const char *command);
  bool readATCommResp(const char *command, const char *req_response);
  bool sendConfirmation(const char *req_response);
  void flushBuffer();
};

#endif