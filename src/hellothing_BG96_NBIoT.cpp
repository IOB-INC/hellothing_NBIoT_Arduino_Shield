/*
    hellothing_BG96_NBIoT.cpp

    NB-IoT library for the hellothing Arduino shield
    with the Quectel BG96 modem.

    Created 1 April 2019

    This project is licensed under the terms of the MIT license.
    For more information refer to the license file in the Github repository.
*/

#include "hellothing_BG96_NBIoT.h"

NBIoT::NBIoT()
{
    BG96_serial = new SoftwareSerial(MDM_RX_PIN, MDM_TX_PIN);
}

bool NBIoT::sendATCommOK(const char *command)
{
    BG96_serial->write(command);

    uint32_t start = millis();

    while ((BG96_serial->available() < 4) && (millis() - start < _timeout))
    {
        delay(500);
    }

    byte size = BG96_serial->readBytes(_response_buffer, RESPONSE_BUFFER_SIZE);
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

bool NBIoT::readATCommResp(const char *exp_response)
{
    uint32_t start = millis();

    while ((BG96_serial->available() < strlen(exp_response)) && (millis() - start < _timeout))
    {
        delay(500);
    }

    byte size = BG96_serial->readBytes(_response_buffer, RESPONSE_BUFFER_SIZE);
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

/**************************************************************
 * Power management functions
 * ************************************************************/

void NBIoT::modemPowerUp(void)
{
    DEBUG_PRINT("Modem powering up");
    digitalWrite(MDM_ENABLE_PIN, HIGH);
    delay(10000);
}

void NBIoT::modemPowerDown(void)
{
    DEBUG_PRINT("Modem powering down");
    _timeout = 350;
    BG96_serial->write("AT+QPOWD\r\n");
    delay(500);
    digitalWrite(MDM_ENABLE_PIN, LOW);
}

void NBIoT::modemReset(void)
{
    DEBUG_PRINT("Modem resetting");
    modemPowerDown();
    delay(1000);
    modemPowerUp();
    delay(5000);
}

/**************************************************************
 * SIM functions
 * ************************************************************/

char *NBIoT::getCCID(void)
{
    DEBUG_PRINT("SIM CCID");
    _timeout = 350;
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
    _timeout = 350;
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
    _timeout = 350;

    uint8_t rep = 0;

    do
    {
        BG96_serial->begin(115200);
        delay(500);
        BG96_serial->write("AT+IPR=19200\r\n");
        delay(500);
        BG96_serial->end();

        delay(1000);

        BG96_serial->begin(19200);
        delay(500);
        BG96_serial->write("AT+IPR?\r\n");
        _response = readATCommResp("+IPR: 19200\r\n");
        if (_response)
        {
            break;
        }
        else
        {
            BG96_serial->end();
            rep += 1;
        }
        delay(2000);
        DEBUG_PRINT("Retry baud setup");

    } while (rep < 3);

    if (_response)
    {
        _response = sendATCommOK("ATE0\r\n");
        _response = sendATCommOK("AT+CMEE=2\r\n");
    }

    return _response;
}

bool NBIoT::modemInitPersistent(void)
{
    DEBUG_PRINT("Persistent baud setup");
    _timeout = 350;
    BG96_serial->begin(115200);
    delay(500);
    BG96_serial->write("AT+IPR=19200;&W\r\n");
    delay(500);
    BG96_serial->end();

    delay(1000);

    BG96_serial->begin(19200);
    delay(500);
    _response = sendATCommOK("AT\r\n");
    _response = sendATCommOK("ATE0;&W\r\n");
    BG96_serial->write("AT+IPR?\r\n");
    _response = readATCommResp("+IPR: 19200\r\n");
    _response = sendATCommOK("AT+CMEE=2;&W\r\n");
    return _response;
}

bool NBIoT::checkBaud(void)
{
    DEBUG_PRINT("Check baud rate");
    _timeout = 350;
    BG96_serial->begin(19200);
    delay(500);
    _response = sendATCommOK("AT\r\n");
    _response = sendATCommOK("ATE0\r\n");
    BG96_serial->write("AT+IPR?\r\n");
    _response = readATCommResp("+IPR: 19200\r\n");
    _response = sendATCommOK("AT+CMEE=2\r\n");
    return _response;
}

char *NBIoT::getIMEI(void)
{
    DEBUG_PRINT("Modem IMEI");
    _timeout = 350;
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

bool NBIoT::setNBIoTDetails(void)
{
    DEBUG_PRINT("NBIoT Details");
    _timeout = 350;
    _response = sendATCommOK("AT+QCFG=\"nb1/bandprior\",08\r\n");
    _response = sendATCommOK("AT+QCFG=\"band\",F,0,A0E189F\r\n");
    _response = sendATCommOK("AT+QCFG=\"nwscanseq\",0301,1\r\n");
    _response = sendATCommOK("AT+QCFG=\"nwscanmode\",3,1\r\n");
    _response = sendATCommOK("AT+QCFG=\"iotopmode\",1,1\r\n");

    return _response;
}

char *NBIoT::getSignalQuality(void)
{
    DEBUG_PRINT("Modem signal quality");
    _timeout = 350;
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
    _timeout = 350;
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

/**************************************************************
 * TCP/IP functions
 * ************************************************************/

bool NBIoT::setTCPAPN(void)
{
    DEBUG_PRINT("Modem APN");
    _timeout = 10000;
    _response = sendATCommOK("AT+QICSGP=1,1,\"nbiot.vodacom.za\",\"\",\"\",0\r\n");
    return _response;
}

bool NBIoT::deactTCPContext(void)
{
    DEBUG_PRINT("Deactivating PDP context");
    _timeout = 40000;
    _response = sendATCommOK("AT+QIDEACT=1\r\n");
    return _response;
}

bool NBIoT::actTCPContext(void)
{
    DEBUG_PRINT("Activating PDP context");
    _timeout = 150000;
    _response = sendATCommOK("AT+QIACT=1\r\n");
    BG96_serial->write("AT+QIACT?\r\n");
    _response = readATCommResp("+QIACT: 1,1,1,");
    return _response;
}

bool NBIoT::closeTCPConnection(void)
{
    DEBUG_PRINT("CLOSE TCP");
    _timeout = 10000;
    _response = sendATCommOK("AT+QICLOSE=1\r\n");
    return _response;
}

bool NBIoT::openTCPConnection(void)
{
    DEBUG_PRINT("OPEN TCP");
    _timeout = 350;
    _response = sendATCommOK("AT+QIDNSCFG=1,\"8.8.8.8\"\r\n");
    delay(1000);
    _timeout = 150000;
    BG96_serial->write("AT+QIOPEN=1,1,\"TCP\",\"thingcola.hellothing.com\",30001,0,1\r\n");
    _response = readATCommResp("OK\r\n\r\n+QIOPEN: 1,0\r\n");
    return _response;
}

bool NBIoT::sendTCPData(char *data)
{
    DEBUG_PRINT("Send data");
    _timeout = 5000;
    BG96_serial->write("AT+QISEND=1\r\n");
    _response = readATCommResp(">");
    _timeout = 10000;
    BG96_serial->write(data);
    BG96_serial->write(0x1A);
    _response = readATCommResp("SEND OK\r\n\r\n+QIURC: \"recv\",1,4\r\nOK");
    return _response;
}