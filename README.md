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

=============================================================================================

this library support simple I/O for beaglebone black ,using C .

in Demo_* directory include some demo using this library ,each circuit layout and document in file directory .


**Hotw to use :**

        Build libBBBio.a :

                # make

        Build Demos : (ex : ADT7301 demo)

                # make ADT7301

**Toolkit List :**

        **GPIO CLK Status toolkit :**

                it's an utility to show GPIO clock module status of  GPIO0 /GPIO1 / GPIO2 / GPIO3 . 

		Help for check GPIO clock module enable or not .
	
		if GPIO clock is disable ,access pin in that will cause **"bus error"** message .

		# make GPIO_STATUS

        **Expansion Header toolkit :**

                it shows the "current" status of Expansion Header P8 and P9 ,

		include Pin mode , pull-high/low .... etc .

                # make EP_STATUS


**Dts : **

In BBB , the default pin mux mode is 7 , as GPIO mode , 

if your device tree doesn't change the pin mux mode , the ePWM / SPI / I2C .. is unused .

I mapped CONTROL_MODULE register via mmap and modify config register in offset 800h~E00h , 

but it has no effect , i conjecture this action must operat in kernel mode.

and i also modify u-boot mux file , but it has no effect too.

so i try to using dtsi file to setting the pinmux mode .
	
**Reference from this nice blogger  :**

	http://blog.pignology.net/2013/05/getting-uart2-devttyo1-working-on.html

	
	**support Dts list :**
		
		**am335x-boneblack_epwmss0.dtsi : **

			enable epwmss0 / ehhrpwm0 , change P9_21/P9_22 pin mux as ehhrpwm0A/ehhrpwm0B .

**Demo List :**

	**LED demo :**

		using LED and Switch to control basic I/O .

		# make LED

	**LED_GPIO demo :**

		Switch on DIP swtich , the corresponding LED will be glittered .

		this demo express how to using BBBIO_GPIO_ prefix funcion to control whole GPIO .

		# make LED_GPIO


	**ADT7301 demo :**

		using ADT7301 IC to get temperature data , an simple demo for SPI

		# make ADT7301

	**Seven-Segment Array Display demo :**

		using F5648RS Seven-Segment to display number . this demo is shows a method about how to enable GPIO2 .

		# make SEVEN_SCAN

		**NOTE** : please confirm your HDMI display is disable or it will take some error .

		**HOT to Disable HDMI ? **

			http://www.logicsupply.com/blog/2013/07/18/disabling-the-beaglebone-black-hdmi-cape/ 

	**Ultrasonic Range sensor 28015 Demo:**

		simple demo fo Ultrasonic Sensor 28015 , include 28015 datasheet.

		# make 28015

	**Debouncing Demo :**

		this demo show an simple demo for Buttom Debouncing , using AM335x's debouncing unit.

		 it display push count once per 2s , you can comment **DEBOUNCING** to compare different

		# make DEBOUNCING
		
	**4x4 keypad Demo :**

		simple keypad scanner programming for 4x4 keypad, it's an example for BBBIO_GPIO_ prefix function .

		this demo express how to using BBBIO_GPIO_ prefix funcion to control whole GPIO .

		# make 4x4keypad
