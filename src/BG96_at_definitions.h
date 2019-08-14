#include <avr/pgmspace.h>

#define CF(s) ((const __FlashStringHelper *)s)

const char OK[] PROGMEM = "OK\r\n";
const char ERROR[] PROGMEM = "ERROR";
const char QPOWD[] PROGMEM = "AT+QPOWD\r\n";

const char SET_IPR[] PROGMEM = "AT+IPR=19200\r\n";
const char GET_IPR[] PROGMEM = "AT+IPR?\r\n";
const char RESP_IPR[] PROGMEM = "+IPR: 19200\r\n";

const char SET_ECHO_OFF[] PROGMEM = "ATE0\r\n";
const char SET_ECHO_ON[] PROGMEM = "ATE1\r\n";
const char SET_CMEE[] PROGMEM = "AT+CMEE=2\r\n";

const char GET_CCID[] PROGMEM = "AT+QCCID\r\n";
const char GET_CIMI[] PROGMEM = "AT+CIMI\r\n";
const char GET_GSN[] PROGMEM = "AT+GSN\r\n";

const char SET_QCFG_BANDPRIOR[] PROGMEM = "AT+QCFG=\"nb1/bandprior\",08\r\n";
const char SET_QCFG_BAND_EDGE[] PROGMEM = "AT+QCFG=\"band\",%s,0,0\r\n";
const char SET_QCFG_BAND_NB[] PROGMEM = "AT+QCFG=\"band\",F,0,%s\r\n";
const char SET_QCFG_NWSCANSEQ[] PROGMEM = "AT+QCFG=\"nwscanseq\",0301,1\r\n";
const char SET_QCFG_NWSCANMODE_EDGE[] PROGMEM = "AT+QCFG=\"nwscanmode\",1,1\r\n";
const char SET_QCFG_NWSCANMODE_NB[] PROGMEM = "AT+QCFG=\"nwscanmode\",3,1\r\n";
const char SET_QCFG_IOTOPMODE[] PROGMEM = "AT+QCFG=\"iotopmode\",1,1\r\n";

const char GET_QCSQ[] PROGMEM = "AT+QCSQ\r\n";
const char GET_QNWINFO[] PROGMEM = "AT+QNWINFO\r\n";
const char SET_CGREG[] PROGMEM = "AT+CGREG=2\r\n";
const char SET_CEREG[] PROGMEM = "AT+CEREG=2\r\n";
const char GET_CGREG[] PROGMEM = "AT+CGREG?\r\n";
const char GET_CEREG[] PROGMEM = "AT+CEREG?\r\n";
const char SET_CGATT[] PROGMEM = "AT+CGATT=1\r\n";
const char GET_CGATT[] PROGMEM = "AT+CGATT?\r\n";
const char RESP_CGATT[] PROGMEM = "+CGATT: 1";
const char GET_COPS_ALL[] PROGMEM = "AT+COPS=?\r\n";
const char GET_COPS[] PROGMEM = "AT+COPS?\r\n";
const char SET_COPS[] PROGMEM = "AT+COPS=1,2,\"%s\"\r\n";

const char SET_QICSGP[] PROGMEM = "AT+QICSGP=1,1,\"%s\",\"\",\"\",0\r\n";
const char SET_QIDEACT[] PROGMEM = "AT+QIDEACT=1\r\n";
const char SET_QIACT[] PROGMEM = "AT+QIACT=1\r\n";
const char GET_QIACT[] PROGMEM = "AT+QIACT?\r\n";
const char RESP_QIACT[] PROGMEM = "+QIACT: 1,1,1,";
const char SET_QICLOSE[] PROGMEM = "AT+QICLOSE=1\r\n";
const char SET_QIDNSCFG[] PROGMEM = "AT+QIDNSCFG=1,\"%s\"\r\n";
const char SET_QIOPEN[] PROGMEM = "AT+QIOPEN=1,1,\"TCP\",\"%s\",%s,0,1\r\n";
const char RESP_QIOPEN[] PROGMEM = "OK\r\n\r\n+QIOPEN: 1,0\r\n";
const char SET_QISEND[] PROGMEM = "AT+QISEND=1\r\n";
const char RESP_QISEND[] PROGMEM = ">";
const char RESP_QISEND_DATA[] PROGMEM = "SEND OK\r\n\r\n+QIURC: \"recv\",1,";