
all : BBBIO-EHRPWM-00A0.dts BBBIO-SPI1-00A0.dts
	dtc -O dtb -o BBBIO-EHRPWM-00A0.dtbo -b 0 -@ BBBIO-EHRPWM-00A0.dts
	dtc -O dtb -o BBBIO-SPI1-00A0.dtbo -b 0 -@ BBBIO-SPI1-00A0.dts

install : all BBBIO-EHRPWM-00A0.dtbo BBBIO-SPI1-00A0.dtbo
	cp BBBIO-EHRPWM-00A0.dtbo /lib/firmware/
	cp BBBIO-SPI1-00A0.dtbo /lib/firmware/

clean :
	rm BBBIO-EHRPWM-00A0.dtbo BBBIO-SPI1-00A0.dtbo
