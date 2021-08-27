# OpenShock
```
  ____                ______            __  
 / __ \___  ___ ___  / __/ /  ___  ____/ /__
/ /_/ / _ \/ -_) _ \_\ \/ _ \/ _ \/ __/  '_/
\____/ .__/\__/_//_/___/_//_/\___/\__/_/\_\ 
    /_/                                     
```
Simple shock measurement device consisting in 5 main components in the initial prototype:
- Arduino Nano as microcontroller
- Accelerometer
- RTC
- SDLoggger
- Battery Pack

I highly recommend modding the Arduino Nano to consume less power, or even better, using only the ATMEGA with Arduino bootloader. With a standard 12V7Ah and a custom regulator is easy to acchieve more than 15 days record time @ almost constant 350Hz Sample Rate - 500Hz is doable with DELAY flag set at 0, at cost of an increased current draw. 

The project requirements were heavily influenced by the Railway industry and the device was tested for measurement of shocks between railcars on a low natural frequency axis. The main hypothesis is that with a standard MEMs acceloremeter, one could measure impacts reasonably well, which seems to correlate.


- [X] Arduino code 1.0.0 ready
- [ ] Software for graphing the results
- [ ] SRS Module
