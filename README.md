# A collection of component drivers for esp32 board
A collection of library/drivers for esp32 to interface with external components with examples. Provides abstraction layer for easy device initialization and reading while internally taking care of pin initialization and event handling.

## Components:

#### DHT22: Humidity and temperature sensor
Handles the device serial communication and timing internally providing a simple read function to read sensor data.
#### CSMS: Capacitative soil moisture sensor
Initializes an ADC pin for reading. Works with any other analog device.
#### WLS : Water level sensor
Same as CSMS. Initializes an ADC pin for reading. Works with any other analog device.
#### WIFI
Handles device configuration for wifi connection internally. Credentials can be configured in `Kconfig` or the header `wifi.h` itself.
#### Clock: Query web for current time
Handles NTP Server query to get current UNIX time. Board should be connected to internet first.
#### LED, Relay
Handles GPIO pin configuration for input and output.
#### Websocket client
Handles device configuration as a websocket client and allows easy sending of message over websocket.