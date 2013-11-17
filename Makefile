
LIB_PATH = ./BBBio_lib/
LIBRARIES = BBBio

all : libBBBio.a

libBBBio.a : ${LIB_PATH}BBBiolib.c ${LIB_PATH}BBBiolib.h
	gcc -c ${LIB_PATH}BBBiolib.c -o ${LIB_PATH}BBBiolib.o
	ar -rs ${LIB_PATH}libBBBio.a ${LIB_PATH}BBBiolib.o
	cp ${LIB_PATH}libBBBio.a ./


LED : Demo_LED/LED.c libBBBio.a
	gcc -o LED Demo_LED/LED.c -L ${LIB_PATH} -lBBBio

ADT7301 : Demo_ADT7301/ADT7301.c libBBBio.a
	gcc -o ADT7301 Demo_ADT7301/ADT7301.c -L ${LIB_PATH} -lBBBio

SEVEN_SCAN : Demo_SevenScan/SevenScan.c libBBBio.a
	gcc -o SevenScan Demo_SevenScan/SevenScan.c  -L ${LIB_PATH} -lBBBio

GPIO_STATUS : Demo_GPIO_Status/GPIO_status.c libBBBio.a
	gcc -o GPIO_status Demo_GPIO_Status/GPIO_status.c  -L ${LIB_PATH} -lBBBio


28015 :  Demo_Ultrasonic28015/Ultrasonic28015.c libBBBio.a
	gcc -o Ultrasonic28015 Demo_Ultrasonic28015/Ultrasonic28015.c -L ${LIB_PATH} -lBBBio


TMP : Demo_tmp/tmp.c libBBBio.a
	gcc -o TMP Demo_tmp/tmp.c  -L ${LIB_PATH} -lBBBio


.PHONY: clean
clean :
	rm -rf ${LIB_PATH}*.o ${LIB_PATH}libBBBio.a libBBBio.a LED ADT7301 GPIO_status SevenScan Ultrasonic28015 TMP

