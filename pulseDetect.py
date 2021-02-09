#!/usr/bin/python3
import os
import select
import sys
import threading
import wiringpi

class HW():
    '''
    Class that creates a hardware interface to demonstate pulse detectors
    '''
    def __init__(self):
        '''
        Initialize wiringPi and objects needed
        '''
        if os.getuid() == 0:
            wiringpi.wiringPiSetupGpio()
            self.PWMOutputPin = 18
            self.SignalInputPin = 21
            wiringpi.pinMode(self.PWMOutputPin, wiringpi.PWM_OUTPUT)
            wiringpi.pinMode(self.SignalInputPin, wiringpi.INPUT)
        else:
            print("This object, build with the HW class, requires root privileges")
            sys.exit(-1)

    def signalIn(self):
        '''
        signalIn - read the current signal level needed by the pulse detectors
        '''
        return (wiringpi.digitalRead(self.SignalInputPin))

    def setInterruptHandler(self, interruptHandler ):
        '''
        setInterruptHandler - connect a handler to the signal input
        '''
        wiringpi.wiringPiISR(self.SignalInputPin, wiringpi.INT_EDGE_RISING, interruptHandler)

    def turnOffPWMPin(self):
        '''
        turnOffPWMPin - zero the PWM setting to disable pulses
        '''
        wiringpi.pwmWrite(self.PWMOutputPin, 0)

    def setupPWMPin(self, clock, range_, dutyCycle):
        '''
        setupPWMPin - setup the hardware for pulse generation and start the pulses
        '''
        wiringpi.pwmSetMode(wiringpi.PWM_MODE_MS)
        wiringpi.pwmSetClock(clock)
        wiringpi.pwmSetRange(range_)
        setting = int(range_ * dutyCycle)
        print("Settings for PWM0 are {} clock divisor, {} range, and {} setting".format(clock, range_, setting))
        wiringpi.pwmWrite(self.PWMOutputPin, setting)

class pulseDetector2(threading.Thread):
    def __init__(self, hardwareInterface, sharedVariables):
        '''
        init - constructor - sets up shared objects
        '''
        super(pulseDetector2, self).__init__()
        self.hardwareInterface = hardwareInterface
        self.sharedVariables = sharedVariables

    def run(self):
        '''
        run - thread that polls the input signal in background to count pulses
        '''
        THRESHOLD = 6
        EXIT_WHEN_VALID = 100
        EXIT_WHEN_INVALID = 1000
        levelHighCount = 0
        levelLowCount = 0
        watchdog = 0
        while self.sharedVariables['numberOfPulses'] < EXIT_WHEN_VALID:
            select.select([], [], [], 0.025)
            if self.hardwareInterface.signalIn() == 1:
                levelLowCount = 0
                levelHighCount += 1
                if levelHighCount > self.sharedVariables['highestHighCount']:
                    self.sharedVariables['highestHighCount'] = levelHighCount
            else:
                levelHighCount = 0
                levelLowCount += 1
                if levelLowCount > self.sharedVariables['highestLowCount']:
                    self.sharedVariables['highestLowCount'] = levelLowCount
            if levelHighCount == THRESHOLD:
                self.sharedVariables['numberOfPulses'] += 1
                watchdog = 0
            watchdog += 1
            if watchdog > EXIT_WHEN_INVALID:
                break
        self.hardwareInterface.turnOffPWMPin()

numberOfInterrupts = 0

def interruptHandler():
    global numberOfInterrupts
    numberOfInterrupts += 1
    
def main():
    '''
    Main program for pulseDetect
    '''
    global numberOfInterrupts
    hardwareInterface = HW()
    sharedVariables = {'numberOfPulses' : 0, 'highestHighCount' : 0, 'highestLowCount' : 0 }
    background = pulseDetector2(hardwareInterface, sharedVariables)
    background.start()
    hardwareInterface.setInterruptHandler(interruptHandler)
    print("Setup PWM for 1.14 Hz 50% duty cycle")
    hardwareInterface.setupPWMPin(4095, 4096, 0.50)
    background.join()
    print("The interrup handler observed {} zero to one transitions/pulses".format(numberOfInterrupts))
    print("The polling routine observed {} pulses".format(sharedVariables['numberOfPulses']))
    print("The polling routine saw as many as {} counts while the level was high and {} counts while the level was low".
          format(sharedVariables['highestHighCount'], sharedVariables['highestLowCount']))
if __name__ == '__main__':
    main()
