BBBiolib
=======

Beaglebone black I/O library , using Memory mapped I/O

Simple C I/O library of Beaglebone balck

	V1 	October 2013 - shabaz (iolib) : create basic library 

	V2 	October 2013 - shabaz (iolib) : fix some BUG

	V2.1	November.7 2013 - VagetableAvenger (BBBlib) : add some comment and modify function name

	V2.2	November.10 2013 - VagetableAvenger (BBBlib) : add GPIO Enable/Disable function (GPIO 1~3)

	V2.3	November.19 2013 - VagetableAvenger (BBBlib) : add GPIO Enable/Disable function (GPIO 0)

	V2.4	November.23 2013 - VagetableAvenger (BBBlib) : add whole GPIO control function (I/O and direction set)

=============================================================================================

this library support simple I/O for beaglebone black ,using C .

in Demo_* directory include some demo using this library ,each circuit layout and document in file directory .

Demo List :

	LED demo :

		using LED and Switch to control basic I/O .

		# make LED

	LED_GPIO demo :

		Switch on DIP swtich , the corresponding LED will be glittered .

		this demo express how to using BBBIO_GPIO_ prefix funcion to control whole GPIO .

		# make LED_GPIO


	ADT7301 demo :

		using ADT7301 IC to get temperature data , an simple demo for SPI

		# make ADT7301

	Seven-Segment Array Display demo :

		using F5648RS Seven-Segment to display number . this demo is shows a method about how to enable GPIO2 .

		# make SEVEN_SCAN

		NOTE : please confirm your HDMI display is disable or it will take some error .

		HOT to Disable HDMI ? 

			http://www.logicsupply.com/blog/2013/07/18/disabling-the-beaglebone-black-hdmi-cape/ 

	GPIO Status demo :

		it's an utility for show GPIO clock module status of  GPIO1 / GPIO2 / GPIO3 .

		# make GPIO_STATUS

	Ultrasonic Range sensor 28015 Demo:

		simple demo fo Ultrasonic Sensor 28015 , include 28015 datasheet.

		# make 28015



Hotw to use :

	Build libBBBio.a :

		# make
	
	Build Demos : (ex : ADT7301 demo)

		# make ADT7301
