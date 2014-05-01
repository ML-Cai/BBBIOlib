
BBBiolib
=======

BBBIO include some device tree overlays example for linux kernel 3.8.13 .

==============================================================================

**Compile each overlays**

	cd overlay

	./build.sh

**Apply overlay**

	cp {overlay file} /lib/firmware/

	echo {overlay's part-number} >> /sys/devices/bone_capemgr.*/slots

**Overlays list**

	1. EHRPWM0~3 overlay

		echo BBBIO-EHRPWM >> /sys/devices/bone_capemgr.*/slots

**Reference from**

	https://learn.adafruit.com/introduction-to-the-beaglebone-black-device-tree/overview

