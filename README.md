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

	V2.8	March.14 2014 - add ADC application of Arduino Microphone module ,and ADC argument calculation toolkit .

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

	5. Ultrasonic Range sensor 28015 Demo

	6. Debouncing Demo

	7. 4x4 keypad Demo

	8. PWM Demo

	9. Servo Motor Demo

	10. ADC Demo

	11. ADC with Arduino Microphone Demo



**Dts :**

In Linux kernel 3.8 ,when BBB power on ,the default pin mux mode is 7 (as GPIO mode) ,

if your device tree doesn't change the pin mux mode , the ePWM / SPI / I2C .. is unused .

the register control pin mux mode is CONTROL_MODULE , but access this register need to be in privilege mode,

because that , BBBio doesn't control ths register .

so i try to using dtsi file to setting the pinmux mode .

**Reference from this nice blogger  :**

	http://blog.pignology.net/2013/05/getting-uart2-devttyo1-working-on.html

accroding this blog , i add some dts file for pin mux .

**Dts list :**

	am335x-boneblack_epwmss_all.dtsi :

		enable epwmss0~2 / ehhrpwm0~2 , if you just nedd some of tham ,

		please remove unecessary part of epwmss and ehhrpwm .

		if epwmss is disable , ehhrpwm is disable too.



