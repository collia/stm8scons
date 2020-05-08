#!/bin/bash

#build docker image run in docker directory:
# docker build -t stm_operate .   

FLASH=0
DEBUG=0
GDB=0
PRJ=.
CLEAN=0

#STM8PLIBPATH=$(pwd)/STM8S_StdPeriph_Lib/Libraries/STM8S_StdPeriph_Driver/

while getopts cdgufp: option 
do 
 case "${option}" 
 in
     u) echo "Usage:
 -p <progect> - directory
 -f - flash to board
 -c - clean the project
 -u - usage"
        exit 0
        ;;
     p) PRJ=${OPTARG};;
     c) CLEAN=1;;
     f) FLASH=1;;
     *) echo "Wrong option. Use -u for usage info"
        exit 0
        ;;
 esac 
done

if [ "$CLEAN" = "1" ]; then
    echo "Cleaning..."
    docker run --rm=true -v "$(pwd)/$PRJ:/home/user/project" \
           stm8_operate "scons -c"
    exit 0
fi

echo "Building..."
docker run --rm=true -v "$(pwd)/$PRJ:/home/user/project"  \
       stm8_operate "scons --debug=includes"

if [ "$?" != "0" ]; then
	echo Compilation failed
	exit $?
fi

if [ "$FLASH" = "1" ]; then
    echo "Flashing..."
    echo     docker run --rm=true -v "$(pwd)/$PRJ:/home/user/project"  \
       -ti --privileged -v /dev/bus/usb:/dev/bus/usb \
       stm8_operate "stm8flash -c stlinkv2 -p stm8s003f3 -w $(pwd)/$PRJ/main.ihx "
    docker run --rm=true -v "$(pwd)/$PRJ:/home/user/project"  \
       -ti --privileged -v /dev/bus/usb:/dev/bus/usb \
       stm8_operate "stm8flash -c stlinkv2 -p stm8s003f3 -w /home/user/project/main.ihx "
fi
