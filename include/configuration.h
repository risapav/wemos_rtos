#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

// #define ARDUINO_OTA
#ifndef PROGMEM
#define PROGMEM ICACHE_RODATA_ATTR
#endif
const char string_0[] PROGMEM = "\nDemo program, (c)Risapav 2019\n";
const char string_1[] PROGMEM = "\nSDK version:%s\n";
const char string_2[] PROGMEM = "\nESP8266 chip ID:0x%x\n";
const char string_3[] PROGMEM = "String 3";
const char string_4[] PROGMEM = "String 4";
const char string_5[] PROGMEM = "String 5";

// Then set up a table to refer to your strings.
const char *const string_table[] PROGMEM = {string_0, string_1, string_2,
                                            string_3, string_4, string_5};

#endif
