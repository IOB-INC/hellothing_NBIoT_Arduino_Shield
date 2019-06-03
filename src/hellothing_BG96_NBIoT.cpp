/*
    hellothing_BG96_NBIoT.cpp

    NB-IoT library for the hellothing Arduino shield
    with the Quectel BG96 modem.

    Created 1 April 2019

    This project is licensed under the terms of the MIT license.
    For more information refer to the license file in the Github repository.
*/

#include "hellothing_BG96_NBIoT.h"

NBIoT::NBIoT(access_technology tech)
{
    MDM_serial = new SoftwareSerial(MDM_RX, MDM_TX);
    _access_tech = tech;
}

bool NBIoT::sendATCommOK(const char *command)
{
    flushBuffer();

    MDM_serial->write(command);

    uint32_t start = millis();

    while ((MDM_serial->available() < 4) && (millis() - start < _timeout))
    {
        delay(500);
    }

    byte size = MDM_serial->readBytes(_response_buffer, RESPONSE_BUFFER_SIZE);
    // Add the final 0 to end the C string
    _response_buffer[size] = '\0';

    DEBUG_PRINT(_response_buffer);

    if (strstr(_response_buffer, "OK\r\n"))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool NBIoT::readATCommResp(const char *command, const char *exp_response)
{
    flushBuffer();

    MDM_serial->write(command);

    uint32_t start = millis();

    while ((MDM_serial->available() < strlen(exp_response)) && (millis() - start < _timeout))
    {
        delay(500);
    }

    byte size = MDM_serial->readBytes(_response_buffer, RESPONSE_BUFFER_SIZE);
    // Add the final 0 to end the C string
    _response_buffer[size] = '\0';

    DEBUG_PRINT(_response_buffer);

    if (strstr(_response_buffer, exp_response))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool NBIoT::sendConfirmation(const char *exp_response)
{
    MDM_serial->write(0x1A);

    uint32_t start = millis();

    while ((MDM_serial->available() < strlen(exp_response)) && (millis() - start < _timeout))
    {
        delay(500);
    }

    byte size = MDM_serial->readBytes(_response_buffer, RESPONSE_BUFFER_SIZE);
    // Add the final 0 to end the C string
    _response_buffer[size] = '\0';

    DEBUG_PRINT(_response_buffer);

    if (strstr(_response_buffer, exp_response))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void NBIoT::flushBuffer(void)
{
    if (MDM_serial->available())
    {
        byte size = MDM_serial->readBytes(_response_buffer, RESPONSE_BUFFER_SIZE);
        // Add the final 0 to end the C string
        _response_buffer[size] = '\0';

        DEBUG_PRINT(_response_buffer);
    }
}

/**************************************************************
 * Power management functions
 * ************************************************************/

void NBIoT::modemPowerUp(void)
{
    DEBUG_PRINT("Modem powering up");
    digitalWrite(MDM_PWR_EN, HIGH);
    delay(10000);
}

void NBIoT::modemPowerDown(void)
{
    DEBUG_PRINT("Modem powering down");
    _timeout = DEFAULT_TIMEOUT;
    MDM_serial->write("AT+QPOWD\r\n");
    delay(500);
    digitalWrite(MDM_PWR_EN, LOW);
}

void NBIoT::modemReset(void)
{
    DEBUG_PRINT("Modem resetting");
    modemPowerDown();
    delay(1000);
    modemPowerUp();
}

/**************************************************************
 * SIM functions
 * ************************************************************/

char *NBIoT::getCCID(void)
{
    DEBUG_PRINT("SIM CCID");
    _timeout = DEFAULT_TIMEOUT;
    _response = sendATCommOK("AT+QCCID\r\n");
    if (sscanf(_response_buffer, "\r\n+QCCID: %s\r\nOK\r\n", _return_buffer))
    {
        return _return_buffer;
    }
    else
    {
        return _error;
    }
}

char *NBIoT::getIMSI(void)
{
    DEBUG_PRINT("SIM IMSI");
    _timeout = DEFAULT_TIMEOUT;
    _response = sendATCommOK("AT+CIMI\r\n");
    if (sscanf(_response_buffer, "\r\n%s\r\nOK\r\n", _return_buffer))
    {
        return _return_buffer;
    }
    else
    {
        return _error;
    }
}

/**************************************************************
 * Modem functions
 * ************************************************************/

bool NBIoT::modemInit(void)
{
    DEBUG_PRINT("Baud setup");
    _timeout = DEFAULT_TIMEOUT;

    sprintf(_error, "%s", "ERROR");

    uint8_t rep = 0;

    do
    {
        MDM_serial->begin(115200);
        delay(500);
        MDM_serial->write("AT+IPR=19200\r\n");
        delay(500);
        MDM_serial->end();

        delay(1000);

        MDM_serial->begin(19200);
        delay(500);
        if (readATCommResp("AT+IPR?\r\n", "+IPR: 19200\r\n"))
        {
            if (sendATCommOK("ATE0\r\n"))
            {
                sendATCommOK("AT+CMEE=2\r\n");
                return true;
            }
            break;
        }
        else
        {
            MDM_serial->end();
            rep += 1;
        }
        delay(2000);
        DEBUG_PRINT("Retry baud setup");

    } while (rep < 3);

    return false;
}

char *NBIoT::getIMEI(void)
{
    DEBUG_PRINT("Modem IMEI");
    _timeout = DEFAULT_TIMEOUT;
    _response = sendATCommOK("AT+GSN\r\n");
    if (sscanf(_response_buffer, "\r\n%s\r\nOK\r\n", _return_buffer))
    {
        return _return_buffer;
    }
    else
    {
        return _error;
    }
}

/**************************************************************
 * Network functions
 * ************************************************************/

bool NBIoT::setExtConfig(void)
{
    DEBUG_PRINT("Extended Configuration Settings");
    _timeout = DEFAULT_TIMEOUT;
    _response = sendATCommOK("AT+QCFG=\"nb1/bandprior\",08\r\n");
    _response = sendATCommOK("AT+QCFG=\"band\",F,0,A0E189F\r\n");
    _response = sendATCommOK("AT+QCFG=\"nwscanseq\",0301,1\r\n");
    if (_access_tech == EDGE)
    {
        _response = sendATCommOK("AT+QCFG=\"nwscanmode\",1,1\r\n");
    }
    else
    {
        _response = sendATCommOK("AT+QCFG=\"nwscanmode\",3,1\r\n");
    }
    _response = sendATCommOK("AT+QCFG=\"iotopmode\",1,1\r\n");

    return _response;
}

char *NBIoT::getSignalQuality(void)
{
    DEBUG_PRINT("Modem signal quality");
    _timeout = DEFAULT_TIMEOUT;
    _response = sendATCommOK("AT+QCSQ\r\n");
    if (_response)
    {
        _pt = strtok(_response_buffer, ",");
        _pt = strtok(NULL, "\r\n");
        return _pt;
    }
    else
    {
        return _error;
    }
}

char *NBIoT::getServiceMode(void)
{
    DEBUG_PRINT("Current network");
    _timeout = DEFAULT_TIMEOUT;
    _response = sendATCommOK("AT+QCSQ\r\n");
    if (_response)
    {
        _pt = strtok(_response_buffer, "\"");
        _pt = strtok(NULL, "\"");
        return _pt;
    }
    else
    {
        return _error;
    }
}

bool NBIoT::setNetworkReg(void)
{
    DEBUG_PRINT("Network registration");
    _timeout = DEFAULT_TIMEOUT;
    sendATCommOK("AT+CEREG=1\r\n");
}

int NBIoT::getNetworkReg(void)
{
    DEBUG_PRINT("Network registration");
    _timeout = DEFAULT_TIMEOUT;
    sendATCommOK("AT+CEREG?\r\n");
}

bool NBIoT::setNetworkAttach(void)
{
    DEBUG_PRINT("Network attach");
    _timeout = 140000;
    return sendATCommOK("AT+CGATT=1\r\n");
}

bool NBIoT::getNetworkAttach(void)
{
    DEBUG_PRINT("Network attach status");
    _timeout = 140000;
    return readATCommResp("AT+CGATT?\r\n", "+CGATT: 1");
}

/**************************************************************
 * TCP/IP functions
 * ************************************************************/

bool NBIoT::setTCPAPN(void)
{
    DEBUG_PRINT("Modem APN");
    _timeout = 10000;
    if (_access_tech == EDGE)
    {
        return sendATCommOK("AT+QICSGP=1,1,\"internet\",\"\",\"\",0\r\n");
    }
    return sendATCommOK("AT+QICSGP=1,1,\"nbiot.vodacom.za\",\"\",\"\",0\r\n");
}

bool NBIoT::deactTCPContext(void)
{
    DEBUG_PRINT("Deactivating PDP context");
    _timeout = 40000;
    return sendATCommOK("AT+QIDEACT=1\r\n");
}

bool NBIoT::actTCPContext(void)
{
    DEBUG_PRINT("Activating PDP context");
    _timeout = 150000;
    sendATCommOK("AT+QIACT=1\r\n");
    return readATCommResp("AT+QIACT?\r\n", "+QIACT: 1,1,1,");
}

bool NBIoT::closeTCPConnection(void)
{
    DEBUG_PRINT("CLOSE TCP");
    _timeout = 10000;
    _response = sendATCommOK("AT+QICLOSE=1\r\n");

    if (_access_tech == EDGE)
    {
        return deactTCPContext();
    }
    return _response;
}

bool NBIoT::openTCPConnection(void)
{
    uint8_t i;

    if (_access_tech == EDGE)
    {
        actTCPContext();
    }

    DEBUG_PRINT("OPEN TCP");
    _timeout = DEFAULT_TIMEOUT;
    _response = sendATCommOK("AT+QIDNSCFG=1,\"8.8.8.8\"\r\n");
    delay(1000);
    _timeout = 150000;

    for (i = 0; i < 3; i++)
    {
        if (readATCommResp("AT+QIOPEN=1,1,\"TCP\",\"thingcola.hellothing.com\",30001,0,1\r\n", "OK\r\n\r\n+QIOPEN: 1,0\r\n"))
        {
            return true;
        }
    }
    return false;
}

bool NBIoT::initDataPacket(const char *id)
{
    DEBUG_PRINT("Short ID");
    _timeout = 5000;

    sprintf(_input_buffer, "%s%s%s", "{\"id\":\"", id, "\"}");
    if (readATCommResp("AT+QISEND=1\r\n", ">"))
    {
        _timeout = 10000;
        MDM_serial->write(_input_buffer);
        return sendConfirmation("SEND OK\r\n\r\n+QIURC: \"recv\",1,");
    }
}

bool NBIoT::sendTCPData(char *data)
{
    DEBUG_PRINT("Send data");
    _timeout = 5000;
    if (readATCommResp("AT+QISEND=1\r\n", ">"))
    {
        _timeout = 10000;
        MDM_serial->write(data);
        return sendConfirmation("SEND OK\r\n\r\n+QIURC: \"recv\",1,4\r\nOK");
    }

    return false;
}