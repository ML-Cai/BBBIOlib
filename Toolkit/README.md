BBBiolib/Toolkit
================

Beaglebone Black I/O Library Toolkit Manual.

=============================================================================================


**GPIO CLK Status toolkit**
	This toolkit show the GPIO clock module status of  GPIO0 /GPIO1 / GPIO2 / GPIO3 . 

	Help for check GPIO clock module enable or not .

	if GPIO clock is disable ,access pin in that will cause **"bus error"** message .

	# make GPIO_STATUS


**Expansion Header toolkit**
	This toolkit show the "current" pins status of Expansion Header P8 and P9 ,

	include Pin mode , pull-high/low .... etc .

	# make EP_STATUS


**ADC Argument Calculation toolkit**
	This toolkit is a simple computer of ADC clock argument calculation ,

	it calculate the Clock Divider / Open Delay / Sample Average for ADC module ,

	and list all "Feasible Solution" of these three value to achieve the request frequency .

	# make ADC_CALC

	# ./ADC_CALC -f {request frequency} -t {frequency tolerance}
