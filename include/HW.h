/*
 * SPI Commands for SD cards
 *
 * Copyright (c) 2020
 * Mark Broihier
 *
 * This class is based on work by Denis Krasutski https://github.com/Krasutski-zz/sdcard_spi_driver
 */
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>

class HW {
private:
  int PWMOutputPin;
  int SignalInputPin;
public:
  void setupPWMPin(int clock, int range, float dutycycle);
  void turnOffPWMPin();
  void setInterruptHandler(void functionPtr());
  int signalIn();
  HW(void);
  ~HW(void);
};
