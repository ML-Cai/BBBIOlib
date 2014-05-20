BBBiolib
=======

Beaglebone black I/O library , using Memory mapped I/O

Simple C I/O library of Beaglebone balck

	V1 	October 2013 - shabaz (iolib) : create basic library 

	V2 	October 2013 - shabaz (iolib) : fix some BUG

	V2.1	November.7 2013 - add some comment and modify function name

	V2.2	November.10 2013 - add GPIO Enable/Disable function (GPIO 1~3)

	V2.3	November.19 2013 - add GPIO Enable/Disable function (GPIO 0)

	V2.4	November.23 2013 - add whole GPIO control function (I/O and direction set)

	V2.5	December.7  2013 - change directory architecture ,add Demo/ and Toolkit/ directory .

	V2.6	December.9  2013 - add simple ePWM module control function in am335x

	V2.7	December.20 2013 - add simple McSPI control function .

	V2.8	March.14 2014 - add ADC application of Arduino Microphone module ,
				and ADC argument calculation toolkit .

=============================================================================================

this library support simple I/O for beaglebone black ,using C .

Demo Directory include some demo basic this library ,each circuit layout ,picture and document in file directory .


**Hotw to use :**

        Build libBBBio.a :

                # make

        Build Demos : (ex : ADT7301 demo)

                # make ADT7301


**Toolkit List :**

	1. GPIO CLK Status toolkit

	2. Expansion Header toolkit

	3. ADC Argument Calculation toolkit



**Demo List :**

	1. LED demo

	2. LED_GPIO demo

	3. ADT7301 demo

	4. Seven-Segment Array Display demo

	5. Debouncing Demo

	6. 4x4 keypad Demo

	7. PWM Demo , [using BBBIO-EHRPWM overlay]

	8. Servo Motor Demo , [using BBBIO-EHRPWM overlay]

	9. ADC Demo

	10. ADC with Arduino Microphone Demo


**overlays**

BBBIO include some device tree overlays example for linux kernel 3.8.13 .

compile each overlays:

	cd overlay

	./build.sh

apply overlay :

	cp {OVERLAY} /lib/firmware/

	echo {OVERLAY's part-number} >> /sys/devices/bone_capemgr.*/slots

overlays list :

	1. EHRPWM0~3 overlay

		echo BBBIO-EHRPWM >> /sys/devices/bone_capemgr.*/slots

Reference from :

	https://learn.adafruit.com/introduction-to-the-beaglebone-black-device-tree/overview



**External Reference**

Bulldog :

	Bulldog is a GPIO API for the Beaglebone Black written in Java.

	github : https://github.com/Datenheld/Bulldog
