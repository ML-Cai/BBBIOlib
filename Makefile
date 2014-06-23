
LIB_PATH = ./BBBio_lib/
DEMO_PATH = ./Demo/
TOOLKIT_PATH = ./Toolkit/
LAB_PATH = ./Lab/


LIBRARIES = BBBio

all : libBBBio.a

libBBBio.a : ${LIB_PATH}BBBiolib.c ${LIB_PATH}BBBiolib.h BBBiolib_PWMSS.o BBBiolib_McSPI.o BBBiolib_ADCTSC.o
	gcc -c ${LIB_PATH}BBBiolib.c -o ${LIB_PATH}BBBiolib.o
	ar -rs ${LIB_PATH}libBBBio.a ${LIB_PATH}BBBiolib.o ${LIB_PATH}BBBiolib_PWMSS.o ${LIB_PATH}BBBiolib_McSPI.o ${LIB_PATH}BBBiolib_ADCTSC.o
	cp ${LIB_PATH}libBBBio.a ./

BBBiolib_PWMSS.o : ${LIB_PATH}BBBiolib_PWMSS.c ${LIB_PATH}BBBiolib_PWMSS.h
	gcc -c ${LIB_PATH}BBBiolib_PWMSS.c -o ${LIB_PATH}BBBiolib_PWMSS.o

BBBiolib_McSPI.o : ${LIB_PATH}BBBiolib_McSPI.c ${LIB_PATH}BBBiolib_PWMSS.h
	gcc -c ${LIB_PATH}BBBiolib_McSPI.c -o ${LIB_PATH}BBBiolib_McSPI.o

BBBiolib_ADCTSC.o : ${LIB_PATH}BBBiolib_ADCTSC.c ${LIB_PATH}BBBiolib_ADCTSC.h
	gcc -c ${LIB_PATH}BBBiolib_ADCTSC.c -o ${LIB_PATH}BBBiolib_ADCTSC.o


install : libBBBio.a 
	

#---------------------------------------------------
# Demo
#---------------------------------------------------

LED : ${DEMO_PATH}Demo_LED/LED.c libBBBio.a
	gcc -o LED ${DEMO_PATH}Demo_LED/LED.c -L ${LIB_PATH} -lBBBio

ADT7301 : ${DEMO_PATH}Demo_ADT7301/ADT7301.c libBBBio.a
	gcc -o ADT7301 ${DEMO_PATH}Demo_ADT7301/ADT7301.c -L ${LIB_PATH} -lBBBio

ADXL345 :  ${DEMO_PATH}Demo_ADXL345/ADXL345.c libBBBio.a
	gcc -o ADXL345 ${DEMO_PATH}Demo_ADXL345/ADXL345.c -L ${LIB_PATH} -lBBBio
ADXL345_NET :  ${DEMO_PATH}Demo_ADXL345/ADXL345_net.c libBBBio.a
	gcc -o ADXL345 ${DEMO_PATH}Demo_ADXL345/ADXL345_net.c -L ${LIB_PATH} -lBBBio

L3G4200D : ${DEMO_PATH}Demo_L3G4200D/L3G4200D.c libBBBio.a
	gcc -o L3G4200D ${DEMO_PATH}Demo_L3G4200D/L3G4200D.c -L ${LIB_PATH} -lBBBio

SEVEN_SCAN : ${DEMO_PATH}Demo_SevenScan/SevenScan.c libBBBio.a
	gcc -o SevenScan ${DEMO_PATH}Demo_SevenScan/SevenScan.c  -L ${LIB_PATH} -lBBBio

SMOTOR : ${DEMO_PATH}Demo_ServoMotor/ServoMotor.c libBBBio.a
	gcc -o SMOTOR ${DEMO_PATH}Demo_ServoMotor/ServoMotor.c -L ${LIB_PATH} -lBBBio

LED_GPIO : ${DEMO_PATH}Demo_LED_GPIO/LED_GPIO.c libBBBio.a
	gcc -o LED_GPIO ${DEMO_PATH}Demo_LED_GPIO/LED_GPIO.c -L ${LIB_PATH} -lBBBio -pthread

DEBOUNCING : ${DEMO_PATH}Demo_Debouncing/Debouncing.c libBBBio.a
	gcc -o Debouncing ${DEMO_PATH}Demo_Debouncing/Debouncing.c -L ${LIB_PATH} -lBBBio

4x4keypad : ${DEMO_PATH}Demo_4x4keypad/4x4keypad.c libBBBio.a
	gcc -o 4x4keypad ${DEMO_PATH}Demo_4x4keypad/4x4keypad.c -L ${LIB_PATH} -lBBBio

PWM : ${DEMO_PATH}Demo_PWM/PWM.c libBBBio.a
	gcc -o PWM ${DEMO_PATH}Demo_PWM/PWM.c -L ${LIB_PATH} -lBBBio

BMOTOR : ${DEMO_PATH}Demo_BrushlessMotor/BrushlessMotor.c libBBBio.a
	 gcc -o BMOTOR ${DEMO_PATH}Demo_BrushlessMotor/BrushlessMotor.c -L ${LIB_PATH} -lBBBio

ADC : ${DEMO_PATH}Demo_ADC/ADC.c libBBBio.a
	gcc -o ADC ${DEMO_PATH}Demo_ADC/ADC.c -L ${LIB_PATH} -lBBBio -lm

ADC_VOICE : ${DEMO_PATH}Demo_ADC/ADC_voice.c libBBBio.a
	gcc -o ADC_VOICE ${DEMO_PATH}Demo_ADC/ADC_voice.c -L ${LIB_PATH} -lBBBio -lm -pthread -O3


DAC_VOICE : ${DEMO_PATH}Demo_DAC/DAC_voice.c libBBBio.a
	gcc -o DAC_VOICE ${DEMO_PATH}Demo_DAC/DAC_voice.c -L ${LIB_PATH} -lBBBio -lm -pthread -O3

#---------------------------------------------------
# toolkit 
#---------------------------------------------------

GPIO_STATUS : ${TOOLKIT_PATH}Toolkit_GPIO_CLK_Status/GPIO_status.c libBBBio.a
	gcc -o GPIO_CLK_status ${TOOLKIT_PATH}Toolkit_GPIO_CLK_Status/GPIO_status.c  -L ${LIB_PATH} -lBBBio

EP_STATUS : ${TOOLKIT_PATH}Toolkit_EP_Status/EP_status.c libBBBio.a
	gcc -o EP_status ${TOOLKIT_PATH}Toolkit_EP_Status/EP_status.c -L ${LIB_PATH} -lBBBio


ADC_CALC : ${TOOLKIT_PATH}Toolkit_ADC_CALC/ADC_CALC.c
	gcc -o ADC_CALC ${TOOLKIT_PATH}Toolkit_ADC_CALC/ADC_CALC.c


#---------------------------------------------------
# Lab
#---------------------------------------------------

RA : ${LAB_PATH}Lab_Robot_Arm/Robot_Arm.c libBBBio.a
	gcc -o RA  ${LAB_PATH}Lab_Robot_Arm/Robot_Arm.c -L ${LIB_PATH} -lBBBio -lm

VD : ${LAB_PATH}Voice_Door/voice_door.cpp libBBBio.a
	g++ -o VD ${LAB_PATH}Voice_Door/voice_door.cpp -L ${LIB_PATH} -lBBBio -lfftw3 -lm -pthread -O3


.PHONY: clean
clean :
	rm -rf ${LIB_PATH}*.o ${LIB_PATH}libBBBio.a libBBBio.a LED ADT7301 GPIO_CLK_status SevenScan Ultrasonic28015 TMP SMOTOR LED_GPIO Debouncing 4x4keypad EP_status PWM RA ADXL345 ADC ADC_CALC L3G4200D

