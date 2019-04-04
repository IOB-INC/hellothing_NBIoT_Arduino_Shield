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

#define RESPONSE_BUFFER_SIZE 50
#define RETURN_BUFFER_SIZE 20

#define MDM_ENABLE_PIN 9
#define TEMPERATURE_PIN A0
#define MDM_RX_PIN 12
#define MDM_TX_PIN 13

class NBIoT
{
public:
  NBIoT();

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
    Function to initiate the modem with persistant settings

    return : success status

  */
  bool modemInitPersistent();

  /*
    Function to check that the baud rate is correct

    return : success status

  */
  bool checkBaud();

  /*
    Function to get the modem IMEI

    return : modem IMEI string

  */
  char *getIMEI();

  /**************************************************************
   * Network functions
   * ************************************************************/

  /*
    Function to set the NB-IoT details

    return : success status

  */
  bool setNBIoTDetails();

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

  /**************************************************************
   * Power management functions
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

  */
  bool openTCPConnection();

  /*
    Function to send a data string via TCP/IP

    return : success status
    ---
    param #1 : data string
  */
  bool sendTCPData(char *data);

private:
  SoftwareSerial *BG96_serial;
  bool _response;
  uint32_t _timeout;
  char _response_buffer[RESPONSE_BUFFER_SIZE + 1];
  char _return_buffer[RETURN_BUFFER_SIZE + 1];
  char *_pt = _return_buffer;
  char _error[] = "error";

  bool sendATCommOK(const char *command);
  bool readATCommResp(const char *req_response);
};

#endif