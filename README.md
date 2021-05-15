# Arduino-watering-system
Arduino Auto-watering &amp; monitoring system

# Arduino automatic watering system Dev
## Functions
1. Indoor air temperature & humidity monitoring.
2. Plants soil moisture monitoring，support up to 4 sensors.
3. Real-time clock and display the day of the week.<sub>**①**</sub>
4. Automatic watering<sub>**②**</sub> based on the temperature & soil moisture & humidity,'s upper and lowwer limits.
5. Manualable watering whenever the user wants.
6. Temperatuare & weather information based on Internet real-time information.<sub>**③**</sub>
7. Records that record every time when the system waters plants, may include time, temperature etc.(not finished yet).

**①** 
*As the current version the time is updated every 20s, which means the time is actually not exactly correct, the error could up to 20s and time will stop going without updating information sent from ESP8266.*

**②** 
1. *Auto-watering only works when DHT11 sensor works properly.* 
2. *Adjust any of the parameter in Auto-watering settings to 0 to disable this function, this also means the Auto-watering function only works in the      enviroment that over 0 degrees(this function hasn't been added)*

**③** 
*Both ‘3’ & ‘6’ function needs connect to a ESP8266 device with given program.*

## Future outlook
1. Sending records information back to ESP8266 device periodically, the ESP8266 will send those information back to a database on PC via website.
2. Adding a outter clock that make the current clock function become real "real-time", the clock could keep going and the time information won't lost even the power goes down,the outter clock could be DS1306.
3. Increasing the number of sensors that monitoring the plants from 4 up to 8 sensors, or could adding or deleting the sensor numbers in times of need, considering that every sensor's concred needs a certain RAM for storage, there will be a upper limit for maxium avaliable sensors.
4. Adding 2 more snesors to each plant, one is temperature sensor for the soil for each plant, the other one is a Light Sensor, when the soil temperature is too high, the system will open a shield for the plant, which takes one more I/O port to contorl the shield. When the illumination is not enough in the end of the day, the system will turn on a light for the plant, which also takes a I/O port. Overall, this will make each plant takes 4 more I/O ports from 2 ports to 6 ports.
5. The information about the time of watering can be stored in a external SD card automatically.

## Hardware
1. Due to this project needs a large memory, and a mass of I/O ports, Arduino Mega 2560 could be a good choice, and an ESP8266 chip also needed.**①**
2. 128*64 OLED, I2C protocol supported.
3. 1 rotary encoder needs, better to have the encoder that has a switch on it.
4. DHT11 sensor.
5. 5v motor, as much as how many plants you wanna use this system to monitor. **②**
6. Soil moisture sensor, as much as how many motor used. **②**
7. Some OC relays, as much as how many motors you need, using relays can prevent the influence of motors. **③**

**①** *Wifi R3 + Mega2560 board is a Mega 2560 board with a ESP8266 chip on it, if you got this board then do not need other types of board.*

**②** *In current version, only 4 sensors and motor are allowed to add.*

**③** *Voltage would decrease when motor enables, this could influence the accuracy of moisture sensor.*

## Dev
### 1.03
Fixed the bug about the automatic watering, now the auto-watering can be used, but it relys on the DHT11 sensor, only avaliable when DHT11 is receiving data correctly.

### 1.02
Added 2 new functions
1. Will back to main menu if the user did not do anything in past 10s.
2. Will shutdown the back ground light if the user did not do anything in past 30s in home page or weather page.

### 1.01
Optimized the way of estimating if the connection between esp8266 and mega 2560 is connected.

### 1.0
Most functions are finished.

###2021-02-07 to 2021-03-31
Given up to use I2C portocol for transmission between esp8266 and mega 2560 as the esp8266 could not be set as I2C slave, tried both esp8266 and esp8266ex. SPI was also hard to use, serial port become the last choice, tried many times to fix the bug of serial transmission.

### 2021-02-03 
Added a new menu that display the weather

### 2021-01-14 to 2021-01-22
The bugs on the menu are fixed.

### 2021-01-11 to 2021-01-12
Rotary encoder, switch's debounce has been added. DHT11 sensoralso works will.

### 2021-01-10
The first time to write this project.
Only finished the code that been used to make the rotary encoder work(correctly count the number). Part of the Menu are finished for display, but not beable to use.
