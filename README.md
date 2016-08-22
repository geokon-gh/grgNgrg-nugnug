## A UDP to LED program for the Due

This sketch supports multiple pins and multiple input ports for driving an arbitrary number of LEDs. B/c we're using the FastLED library and it's parallel-output functionality we are limited to it's pre-defined pin values

uint16_t available_pins[] = {
         25,
         26,
         27,
         28,
         14,
         15,
         29,
         11};

However this is the fastest solution we've been able to find (Faster than normal FastLED routines and faster than Adafruit's Neopixel library)

Variables and how to set them is all described at the top of the .ino file
