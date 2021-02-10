# pulseDetect

This repository has C++ and python examples of using wiringPi.  A hardware based PWM is setup on a PI using wiringPi.  This pulse is generated on physical pin 12 and pin 12 needs to be wired to physical pin 40.  Two software pulse detectors monitor pin 40 and count when the signal goes high.  After 100 pulses are detected, a short report is written.

Installation (assuming a Debian/Raspbian buster or better Linux distribution)

  1)  sudo apt-get update
  2)  sudo apt-get upgrade
  3)  sudo apt-get install git
  4)  sudo apt-get install build-essentials
  5)  sudo apt-get install cmake
  6)  sudo apt-get install wiringpi
  7)  git clone https://github.com/mbroihier/pulseDetect
      - cd pulseDetect
      - mkdir build
      - cd build
      - cmake ..
      - make
  8)  run pulseDetect
      - sudo ./pulseDetect
  9)  If you want to run the python script (implements the same thing)
      - cd pulseDetect
      - sudo pip3 install wiringpi
      - sudo ./pulseDetect.py
