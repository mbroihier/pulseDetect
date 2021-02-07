#include <thread>
#include "../include/HW.h"
static int numberOfInterrupts = 0;
static void interruptHandler() {
  numberOfInterrupts++;
};
int main(int argc, char ** argv) {
  /*
   This program demonstrates the use of wiringPi to monitor a pin and count the pulses that occur on that pin.
   The pin, physical pin 40, is attached to a hardware based PWM at physical pin 12.  This program sets the
   frequency of the PWM to 1.14 Hz (period 877 ms) and the duty cycle to 50%.  Half the time the pulse will be
   high, half low.
  
   Two types of pulse detectors are demonstrated.  The first is interrupt driven.  An interrupt is attached to
   the rising edge of the signal on physical pin 40.  That handler simply increments a counter.  The second example
   polls on a background thread every 25 msec.  When the signal is high for a sufficient amount of time, the pulse
   count in incremented.  Note that this thread is used to terminate the program by either of two means - either 100
   pulses are detected, or a watchdog timer expires.  This means that the print statement should always say that the
   polling routine's count should be 100 unless the watchdog causes the termination.

   Copyright (c) 2021
   Mark Broihier
  */
  HW hardwareInterface;
  int numberOfPulses = 0;
  int highestHighCount = 0;
  int highestLowCount = 0;
  auto pulseDetector2 = [&numberOfPulses, &hardwareInterface, &highestHighCount, &highestLowCount]() {
                          int const THRESHOLD = 6;
                          int const EXIT_WHEN_VALID = 100;
                          int const EXIT_WHEN_INVALID = 1000;
                          int levelHighCount = 0;
                          int levelLowCount = 0;
                          int watchdog = 0;
                          struct timeval tv = {0, 25000}; // check every 25 milliseconds
                          while (numberOfPulses < EXIT_WHEN_VALID) {
                            select(0, NULL, NULL, NULL, &tv);  // wait and give up control
                            tv = {0, 25000}; // reset timeout                        
                            if (hardwareInterface.signalIn() == 1) {
                              levelLowCount = 0;
                              levelHighCount++;
                              if (levelHighCount > highestHighCount) {
                                highestHighCount = levelHighCount;
                              }
                            } else {
                              levelHighCount = 0;
                              levelLowCount++;
                              if (levelLowCount > highestLowCount) {
                                highestLowCount = levelLowCount;
                              }
                            }
                            if (levelHighCount == THRESHOLD) {
                              numberOfPulses++;
                              watchdog = 0;
                            }
                            if (watchdog++ > EXIT_WHEN_INVALID) return;  // if physical pins 12 and 40 are not connected
                                                                         // this stops the thread
                          }
                          hardwareInterface.turnOffPWMPin();
                          return;
                        };
  std::thread background(pulseDetector2);
  hardwareInterface.setInterruptHandler(interruptHandler);
  fprintf(stdout, "Setup PWM for 1.14 Hz 50%% duty cycle\n");
  hardwareInterface.setupPWMPin(4095, 4096, 0.50);  // clock (divisor), range, duty cycle
  background.join();  // wait for background thread to terminate
  fprintf(stdout, "The interrupt handler observed %d zero to one transitions/pulses\n", numberOfInterrupts);
  fprintf(stdout, "The polling routine observed %d pulses\n", numberOfPulses);
  fprintf(stdout, "Saw as many as %d counts while the level was high and %d counts while the level was low\n",
          highestHighCount, highestLowCount);
  return 0;
}
