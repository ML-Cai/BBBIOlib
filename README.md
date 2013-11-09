BBBiolib
=======

Beaglebone black I/O library , using Memory mapped I/O

Simple C I/O library of Beaglebone balck

	V1 	October 2013 - shabaz (iolib) : create basic library 

	V2 	October 2013 - shabaz (iolib) : fix some BUG

	V2.1	November 2013 - VagetableAvenger (BBBlib) : add some comment and modify function name

=============================================================================================

this library support simple I/O for beaglebone black ,using C .

in Demo_* directory include some demo using this library ,each circuit layout and document in file directory .

Demo List :

	LED demo :

		using LED and Switch to control basic I/O

	ADT7301 demo :

		using ADT7301 IC to get temperature data , an simple demo for SPI


Hotw to use :

	Build libBBBio.a :

		make
	
	Build Demos : (ex : ADT7301 demo)

		make ADT7301
