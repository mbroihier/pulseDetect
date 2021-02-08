/*
  HW - hardware interface - setups up BCM pin 18 as a PWM output and pin 21 is an input that can have an interrupt 
  attached to it.  It is assumed that pin 18 (physical 12) is wired to 21 (physical 40).
 
  Copyright (c) 2021
  Mark Broihier
 
 */
#include "../include/HW.h"
void HW::setupPWMPin(int clock, int range, float dutyCycle) {
  /*
   Setup a PWM on physical pin 12 - this will be a hardware clocked PWM
  */
  pwmSetMode(PWM_MODE_MS);
  pwmSetClock(clock);
  pwmSetRange(range);
  int setting = range * dutyCycle;
  fprintf(stdout, "Settings for PWM0 are %d clock divisor, %d range, and %d setting\n", clock, range, setting);
  pwmWrite(PWMOutputPin, setting);
};
void HW::turnOffPWMPin() {
  /*
   Turn off PWM by setting duty cycle to 0
  */
  pwmWrite(PWMOutputPin, 0);
};
void HW::setInterruptHandler(void functionPtr()) {
  /*
   Attach an interrupt handler to a pin
  */
  wiringPiISR(SignalInputPin, INT_EDGE_RISING, functionPtr);
};
int HW::signalIn() {
  return (digitalRead(SignalInputPin));
}
HW::HW(void) {
  /*
   Initialize wiringPi and define pins to be used
  */
  uid_t uid = getuid();
  if (uid == 0) {
    wiringPiSetupGpio();  // set wiring to BCM
    PWMOutputPin = 18;    // physical pin 12
    SignalInputPin = 21;  // physical pin 40
    pinMode(PWMOutputPin, PWM_OUTPUT);
    pinMode(SignalInputPin, INPUT);
  } else {
    fprintf(stdout, "This object, built with the HW class, requires root privilege\n");
    exit(-1);
  }
};
HW::~HW(void) {
};  
