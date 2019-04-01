# wemos_rtos
some tests provided on WEMOS HW with esp8266-rtos-sdk framework using ATOM PlatformIO

## Použitá literatúra
 - https://circuits4you.com/2018/02/03/esp8266-nodemcu-adc-analog-value-on-dial-gauge/
 - https://github.com/tttapa/ESP8266

## Nastavenie dosky WeMOS Mega

- nastavenie komunikácie medzi jednotlivými časťami procesorovej dosky
 - https://robu.in/product/wemos-mega-wifi-r3-atmega2560nodemcu-esp8266-32mb-memory-usb-ttl-ch340g-compatible-for-arduino-mega/
 - https://www.adrirobot.it/wemos_mega/wemos_mega.htm
 - https://media.readthedocs.org/pdf/arduino-esp8266/2.4.0-rc1/arduino-esp8266.pdf

| | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | RXD/TXD |
|-|-|-|-|-|-|-|-|-|-|
| All modules work independent | --- | --- | --- | --- | --- | --- | --- | --- | 0 |
| ATmega2560 <-> ESP8266 | ON | ON | --- | --- | --- | --- | --- | --- | 0 |
| USB <-> ATmega2560 (upload sketch) | --- | --- | ON | ON | --- | --- | --- | --- | 0 |
| USB <-> ESP8266 (upload sketch) | --- | --- | --- | --- | ON | ON | ON | --- | 0 |
| USB <-> ESP8266 (connect) | --- | --- | --- | --- | ON | ON | --- | --- | 0 |
| USB <-> ATmega2560 <-> ESP8266 | ON | ON | ON | ON | --- | --- | --- | --- | 3 |