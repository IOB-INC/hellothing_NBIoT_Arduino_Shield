/*
    hellothing_BG96_NBIoT.cpp

    NB-IoT library for the hellothing Arduino shield
    with the Quectel BG96 modem.

    Created 1 April 2019

    This project is licensed under the terms of the MIT license.
    For more information refer to the license file in the Github repository.
*/

#include "hellothing_BG96_NBIoT.h"

NBIoT::NBIoT(Access_technology_t tech)
{
    MDM_serial = new SoftwareSerial(MDM_RX, MDM_TX);
    _access_tech = tech;
}

NBIoT::NBIoT(Access_technology_t tech, attr *attributes, uint8_t attributes_size)
{
    MDM_serial = new SoftwareSerial(MDM_RX, MDM_TX);
    _attributes = attributes;
    _attributes_size = attributes_size;
    _access_tech = tech;
}

bool NBIoT::sendATCmdResp()
{
    flushBuffer();

    MDM_serial->write(_at_cmd);

    return readModemResp();
}

bool NBIoT::readModemResp(void)
{
    uint32_t start = millis();

    while ((MDM_serial->available() < strlen(_at_resp)) && (millis() - start < _timeout))
    {
        delay(500);
    }

    byte size = MDM_serial->readBytes(_buff, RESPONSE_BUFFER_SIZE);
    _buff[size] = '\0';

    DEBUG_SERIAL(_buff);

    _pt = strstr(_buff, "{");
    if (_pt)
    {
        if (!deserializeJson(JsonDoc, _pt))
        {
            _json_key = JsonDoc["k"];
            for (int i = 0; i < _attributes_size; i++)
            {
                if (!strcmp(_attributes[i].key, _json_key))
                {
                    _attributes[i].value = JsonDoc["v"];

                    if (_attributes[i].type == DIGITAL)
                    {
                        digitalWrite(_attributes[i].pin, _attributes[i].value);
                    }
                    else if (_attributes[i].type == DIGITAL_INVERT)
                    {
                        digitalWrite(_attributes[i].pin, !_attributes[i].value);
                    }

                    JsonDoc.clear();
                    JsonObject outputs = JsonDoc.createNestedObject("outputs");
                    outputs[_attributes[i].key] = _attributes[i].value;

                    serializeJson(JsonDoc, _input_buff);
                    JsonDoc.clear();

                    return sendData(_input_buff);
                }
            }
        }
        return false;
    }
    else if (strstr(_buff, _at_resp))
    {
        return true;
    }
    return false;
}

void NBIoT::flushBuffer(void)
{
    if (MDM_serial->available())
    {
        byte size = MDM_serial->readBytes(_buff, RESPONSE_BUFFER_SIZE);
        // Add the final 0 to end the C string
        _buff[size] = '\0';

        DEBUG_SERIAL(_buff);
    }
}

void NBIoT::setAtCmd(const char *memstring)
{
    strcpy_P(_at_cmd, (char *)CF(memstring));
}

void NBIoT::setAtResp(const char *memstring)
{
    strcpy_P(_at_resp, (char *)CF(memstring));
}

/**************************************************************
 * Power management functions
 * ************************************************************/

void NBIoT::modemPowerUp(void)
{
    DEBUG_SERIAL(F("Modem powering up"));
    digitalWrite(MDM_PWR_EN, HIGH);
    for (int i = 0; i < 10; i++)
    {
        Serial.print(". ");
        delay(1000);
    }
    Serial.println();
}

void NBIoT::modemPowerDown(void)
{
    DEBUG_SERIAL(F("Modem powering down"));
    _timeout = DEFAULT_TIMEOUT;
    setAtCmd(QPOWD);
    setAtResp(OK);
    MDM_serial->write(_at_cmd);
    delay(500);
    digitalWrite(MDM_PWR_EN, LOW);
}

void NBIoT::modemReset(void)
{
    DEBUG_SERIAL(F("Modem resetting"));
    modemPowerDown();
    delay(1000);
    modemPowerUp();
}

/**************************************************************
 * SIM functions
 * ************************************************************/

char *NBIoT::getICCID(void)
{
    DEBUG_SERIAL(F("SIM ICCID"));
    _timeout = DEFAULT_TIMEOUT;
    setAtCmd(GET_CCID);
    setAtResp(OK);
    _response = sendATCmdResp();
    if (sscanf(_buff, "\r\n+QCCID: %s\r\nOK\r\n", _return_buff))
    {
        return _return_buff;
    }
    else
    {
        return _error;
    }
}

char *NBIoT::getIMSI(void)
{
    DEBUG_SERIAL(F("SIM IMSI"));
    _timeout = DEFAULT_TIMEOUT;
    setAtCmd(GET_CIMI);
    setAtResp(OK);
    _response = sendATCmdResp();
    if (sscanf(_buff, "\r\n%s\r\nOK\r\n", _return_buff))
    {
        return _return_buff;
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
    DEBUG_SERIAL(F("Baud setup"));
    _timeout = DEFAULT_TIMEOUT;

    strcpy(_error, "ERROR");

    uint8_t rep = 0;

    while (rep < 3)
    {
        MDM_serial->begin(115200);
        delay(500);
        setAtCmd(SET_IPR);
        MDM_serial->write(_at_cmd);
        delay(500);
        MDM_serial->end();

        delay(1000);

        MDM_serial->begin(19200);
        delay(500);
        setAtCmd(GET_IPR);
        setAtResp(RESP_IPR);
        if (sendATCmdResp())
        {
            setAtCmd(SET_ECHO_OFF);
            setAtResp(OK);
            if (sendATCmdResp())
            {
                setAtCmd(SET_CMEE);
                sendATCmdResp();
                if (_access_tech == EDGE)
                {
                    setAtCmd(SET_CGREG);
                }
                else
                {
                    setAtCmd(SET_CEREG);
                }
                sendATCmdResp();

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
        DEBUG_SERIAL(F("Retry baud setup"));
    }

    return false;
}

char *NBIoT::getIMEI(void)
{
    DEBUG_SERIAL(F("Modem IMEI"));
    _timeout = DEFAULT_TIMEOUT;
    setAtCmd(GET_GSN);
    setAtResp(OK);
    _response = sendATCmdResp();
    if (sscanf(_buff, "\r\n%s\r\nOK\r\n", _return_buff))
    {
        return _return_buff;
    }
    else
    {
        return _error;
    }
}

bool NBIoT::sendDeviceID(void)
{
    DEBUG_SERIAL(F("Send device IDs"));

    JsonObject comms = JsonDoc.createNestedObject("comms");
    comms["IMEI"] = imei;
    comms["ICCID"] = iccid;
    comms["IMSI"] = imsi;

    serializeJson(JsonDoc, _input_buff);
    JsonDoc.clear();

    return sendData(_input_buff);
}

/**************************************************************
 * Network functions
 * ************************************************************/

bool NBIoT::setExtConfig(const char *band)
{
    DEBUG_SERIAL(F("Extended Configuration Settings"));
    _timeout = DEFAULT_TIMEOUT;
    setAtResp(OK);

    if (_access_tech == EDGE)
    {
        setAtCmd(SET_QCFG_BAND_EDGE);
        sprintf(_input_buff, _at_cmd, band);
        strcpy(_at_cmd, _input_buff);
        _response = sendATCmdResp();
        setAtCmd(SET_QCFG_NWSCANMODE_EDGE);
        _response = sendATCmdResp();
    }
    else
    {
        setAtCmd(SET_QCFG_BAND_NB);
        sprintf(_input_buff, _at_cmd, band);
        strcpy(_at_cmd, _input_buff);
        _response = sendATCmdResp();
        setAtCmd(SET_QCFG_NWSCANMODE_NB);
        _response = sendATCmdResp();
        setAtCmd(SET_QCFG_IOTOPMODE);
        _response = sendATCmdResp();
    }

    setAtCmd(SET_QCFG_NWSCANSEQ);
    _response = sendATCmdResp();

    return _response;
}

char *NBIoT::getSignalQuality(void)
{
    DEBUG_SERIAL(F("Modem signal quality"));
    _timeout = DEFAULT_TIMEOUT;
    setAtCmd(GET_QCSQ);
    setAtResp(OK);
    if (sendATCmdResp())
    {
        _pt = strtok(_buff, ",");
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
    DEBUG_SERIAL(F("Current network"));
    _timeout = DEFAULT_TIMEOUT;
    setAtCmd(GET_QNWINFO);
    setAtResp(OK);
    if (sendATCmdResp())
    {
        _pt = strtok(_buff, "\"");
        _pt = strtok(NULL, "\"");
        return _pt;
    }
    else
    {
        return _error;
    }
}

bool NBIoT::sendSignalDetails(void)
{
    DEBUG_SERIAL(F("Send signal details"));

    JsonObject comms = JsonDoc.createNestedObject("comms");
    comms["SignalStrength"] = getSignalQuality();
    comms["CommsType"] = getServiceMode();

    serializeJson(JsonDoc, _input_buff);
    JsonDoc.clear();

    return sendData(_input_buff);
}

bool NBIoT::getNetworkReg(void)
{
    DEBUG_SERIAL(F("Network registration"));
    _timeout = DEFAULT_TIMEOUT;
    if (_access_tech == EDGE)
    {
        setAtCmd(GET_CGREG);
    }
    else
    {
        setAtCmd(GET_CEREG);
    }
    setAtResp(OK);
    return sendATCmdResp();
}

bool NBIoT::setNetworkAttach(void)
{
    DEBUG_SERIAL(F("Network attach"));
    _timeout = 140000;
    setAtCmd(SET_CGATT);
    setAtResp(OK);
    return sendATCmdResp();
}

bool NBIoT::getNetworkAttach(void)
{
    DEBUG_SERIAL(F("Network attach status"));
    _timeout = 140000;
    setAtCmd(GET_CGATT);
    setAtResp(RESP_CGATT);
    return sendATCmdResp();
}

bool NBIoT::getAvailableOperators(void)
{
    DEBUG_SERIAL(F("Available operators"));
    _timeout = 180000;
    setAtCmd(GET_COPS_ALL);
    setAtResp(OK);
    return sendATCmdResp();
}

bool NBIoT::getCurrentOperator(void)
{
    DEBUG_SERIAL(F("Current operator"));
    _timeout = 180000;
    setAtCmd(GET_COPS);
    setAtResp(OK);
    return sendATCmdResp();
}

bool NBIoT::setOperator(const char *oper)
{
    DEBUG_SERIAL(F("Operator selection"));
    _timeout = 180000;
    setAtCmd(SET_COPS);
    setAtResp(OK);
    sprintf(_input_buff, _at_cmd, oper);
    strcpy(_at_cmd, _input_buff);
    return sendATCmdResp();
}

/**************************************************************
 * TCP/IP functions
 * ************************************************************/

bool NBIoT::setAPN(const char *apn)
{
    DEBUG_SERIAL(F("Modem APN"));
    _timeout = 10000;
    setAtCmd(SET_QICSGP);
    setAtResp(OK);
    sprintf(_input_buff, _at_cmd, apn);
    strcpy(_at_cmd, _input_buff);
    return sendATCmdResp();
}

bool NBIoT::setDNS(const char *dns)
{
    DEBUG_SERIAL(F("DNS Server"));
    _timeout = 10000;
    setAtCmd(SET_QIDNSCFG);
    setAtResp(OK);
    sprintf(_input_buff, _at_cmd, dns);
    strcpy(_at_cmd, _input_buff);
    return sendATCmdResp();
}

bool NBIoT::deactContext(void)
{
    DEBUG_SERIAL(F("Deactivating PDP context"));
    _timeout = 40000;
    setAtCmd(SET_QIDEACT);
    setAtResp(OK);
    return sendATCmdResp();
}

bool NBIoT::actContext(void)
{
    DEBUG_SERIAL(F("Activating PDP context"));
    _timeout = 150000;

    setAtCmd(SET_QIACT);
    setAtResp(OK);
    sendATCmdResp();

    setAtCmd(GET_QIACT);
    setAtResp(RESP_QIACT);
    return sendATCmdResp();
}

bool NBIoT::closeConnection(void)
{
    DEBUG_SERIAL(F("Close TCP"));
    _timeout = 10000;
    setAtCmd(SET_QICLOSE);
    setAtResp(OK);
    _response = sendATCmdResp();

    if (_access_tech == EDGE)
    {
        return deactContext();
    }
    return _response;
}

bool NBIoT::openConnection(const char *domain, const char *port)
{
    uint8_t i;

    if (_access_tech == EDGE)
    {
        actContext();
    }

    _timeout = DEFAULT_TIMEOUT;
    setDNS("8.8.8.8");
    delay(1000);

    DEBUG_SERIAL(F("Open TCP"));
    _timeout = 150000;
    setAtCmd(SET_QIOPEN);
    setAtResp(RESP_QIOPEN);
    sprintf(_input_buff, _at_cmd, domain, port);
    strcpy(_at_cmd, _input_buff);

    for (i = 0; i < 3; i++)
    {
        if (sendATCmdResp())
        {
            return true;
        }
    }
    return false;
}

bool NBIoT::sendData(char *data)
{
    DEBUG_SERIAL(F("Send data"));
    _timeout = 5000;
    setAtCmd(SET_QISEND);
    setAtResp(RESP_QISEND);
    if (sendATCmdResp())
    {
        _timeout = 10000;
        MDM_serial->write(data);
        MDM_serial->write(0x1A);

        setAtResp(RESP_QISEND_DATA);
        return readModemResp();
    }

    return false;
}

/**************************************************************
 * Sensor functions
 * ************************************************************/

float NBIoT::getTemp(void)
{
    tempValue = analogRead(TEMP);
    tempValue = ((8.194 - sqrt(81.017156 - (0.02096 * tempValue))) / (-0.0052)) + 30.0;
    return tempValue;
}

bool NBIoT::registerOutputs(void)
{
    DEBUG_SERIAL(F("Register outputs"));

    char type[2];

    JsonObject reg_out = JsonDoc.createNestedObject("reg_out");

    for (int i = 0; i < _attributes_size; i++)
    {
        dtostrf(_attributes[i].type, 1, 0, type);
        reg_out[_attributes[i].key] = type;
    }

    serializeJson(JsonDoc, _input_buff);
    JsonDoc.clear();

    return sendData(_input_buff);
}