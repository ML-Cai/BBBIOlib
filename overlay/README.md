
BBBiolib
=======

BBBIO include some device tree overlays example for linux kernel 3.8.13 .

==============================================================================

**Compile each overlays**

	#cd overlay

	#make

**Install overlays**

	#cd overlay

	#make install

**Apply overlay**

	echo {overlay's part-number} >> /sys/devices/bone_capemgr.*/slots

**Clean overlay**

	#cd overlay

	#make clean

**Overlays list**

	1. EHRPWM0~3 overlay

		#echo BBBIO-EHRPWM >> /sys/devices/bone_capemgr.*/slots

	2. SPI1 overlay

		#echo BBBIO-SPI1 >> /sys/devices/bone_capemgr.*/slots

**Reference from**

	https://learn.adafruit.com/introduction-to-the-beaglebone-black-device-tree/overview

