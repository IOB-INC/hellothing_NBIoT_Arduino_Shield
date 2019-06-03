# hellothing NB-IoT Arduino Shield

This is a library for the hellothing NB-IoT Arduino shield. For more information about the setup of the device and the library click [here](https://fastcomm.com/development-kit/).

# Examples

- [Temperature sensor](https://github.com/Fastcomm/hellothing_NBIoT_Arduino_Shield/blob/master/examples/BG96_NBIoT_Temperature_Example/BG96_NBIoT_Temperature_Example.ino) -
  This is a basic example to display basic TCP/IP communication with the onboard temperature sensor.

- [Leak detector](https://github.com/Fastcomm/hellothing_NBIoT_Arduino_Shield/blob/master/examples/BG96_NBIoT_Leak_Detector_Example/BG96_NBIoT_Leak_Detector_Example.ino) -
  This is a use case to build remote leak detection unit with the option of system control eg. A geyser with a flow valve.
  \*\* External hardware needed.

# Pinout

- A0 - Temperature sensor
- A1 - Modem current
- D2 - Button
- D3 - Accelerometer interrupt
- D8 - Modem 1.8V ok
- D9 - Modem enable
- D10 - Test LED 0
- D11 - Test LED 1
- D12 - Modem Rx
- D13 - Modem Tx

# Power

The shield is powered through the Arduino.
The Arduino can be powered as normal either by the USB cable or an external power supply(7-12V).
