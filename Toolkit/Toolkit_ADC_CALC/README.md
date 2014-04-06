Toolkit_ADC_CALC
================

ADC Argument calculator of Beaglebone Black I/O Library .

================

**Introduction :**

	This toolkit is a simple calculator of ADC module argument in BBBio library,

	it calculate the Clock Divider , Open Delay , Sample Delay and Sample Average for ADC module ,

	and list all "Feasible Solution" of these four value to achieve the request frequency .

================

**Compiling ADC_CALC**

	#make ADC_CALC

================

**Using ADC_CALC**

	# ./ADC_CALC -f [request frequency] -t [frequency tolerance]

================

**Description**	

	-f {request frequency}

		Sample rate/frequency for ADC module .

	-t {frequency tolerance}
	
		Feasible tolerance of sample rate .

================

**Example**

	For normal Audio sample rate 44100 Hz , and feasible tolerance is +- 30

	./ADC_CALC -f 44100 -t 30
