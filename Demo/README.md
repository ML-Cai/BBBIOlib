BBBiolib/Demo
==============

Beaglebone Black I/O Library Demo Manual.

============================================================================================

**LED demo**

	using LED and Switch to control basic I/O .

	# make LED


**LED_GPIO demo**

	Switch on DIP swtich , the corresponding LED will be glittered .

	this demo express how to using BBBIO_GPIO_ prefix funcion to control whole GPIO .

		# make LED_GPIO


**ADT7301 demo**

	using ADT7301 IC to get temperature data , an simple demo for SPI , using AM335x McSPI module

		# make ADT7301


**Seven-Segment Array Display demo**

	Using F5648RS Seven-Segment to display number . this demo is shows a method about how to enable GPIO2 .

		# make SEVEN_SCAN

	**NOTE** : please confirm your HDMI display is disable or it will take some error .

	**HOT to Disable HDMI ? **

		http://www.logicsupply.com/blog/2013/07/18/disabling-the-beaglebone-black-hdmi-cape/ 

**Debouncing Demo**

	This demo show an simple demo for Buttom Debouncing , using AM335x's debouncing unit.

	it display push count once per 2s , you can comment **DEBOUNCING** to compare different

		# make DEBOUNCING

		
**4x4 keypad Demo**

	simple keypad scanner programming for 4x4 keypad, it's an example for BBBIO_GPIO_ prefix function .

	this demo express how to using BBBIO_GPIO_ prefix funcion to control whole GPIO .

		# make 4x4keypad


**PWM Demo**

	Express how to using the BBBIO_PWMSS_ prefix function , this demo generate 50HZ output to epwm0A and epwm0B ,

	and the duty of epwm0A is 50% , 25% for epwm0B , run 10s .

		# make PWM


**Servo Motor Demo**

	This demo express how to control Servo Motor , using ePWM module in beaglebone black .

	using ePWM0A for signal .

	Demo Video : http://www.youtube.com/watch?v=xbuMTBIEgEc&list=HL1386612017&feature=mh_lolz

		# make SMOTOR


**ADC Demo**

	Simple ADC reader ,using on-board 1.8v and AGnd as sample input .

	note : MAX Voltage on AIN0 ~ 7 is 1.8V !!!

		# make ADC


**ADC with Arduino Microphone Demo**

	ADC sampleing of Arduino Microphone module , this demo also shows the application of ADC_CALC toolkit.

		# make ADC_VOICE
