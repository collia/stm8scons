# Description 
Environment for stm8 tools for build and device flashing.

Also  included coulple demo projects:
- blinky - blinks led on GPIO B5
- ssd1306 - test for driver for screen 192x64, based on ssd1306. Very simplified version of driver from project https://github.com/adafruit/Adafruit_SSD1306

# License
Files in this repository are under two licenses MIT and MCD-ST Liberty SW License Agreement V2. License for each file are written in file header.

# How to build docker image
All tools are incapsulated in docker image. 
In directory `docker` run command
```shell
docker build -t stm8_operate .
```
# How to build project
All projects are placed in src directory - directory is project.
To run clean build in `src` directory call:
```shell
/build.sh -p <projectname> -c
```
For project building:
```shell
/build.sh -p <projectname>
```
For example:
```
/build.sh -p ssd1306 -c
```

# How to flash project
All those examples were tested with STM8S003F3P6 board ([Shop](https://arduino.ua/prod3766-plata-razrabotchika-stm8s003f3p6)) and stlink2.

In `src` directory call
```shell
/build.sh -p ssd1306 -f
``` 
This will rebuild project and flash it.
