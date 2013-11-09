LIB_PATH = ./BBBio_lib/
LIBRARIES = BBBio

all : libBBBio.a

libBBBio.a : ${LIB_PATH}BBBiolib.c
	gcc -c ${LIB_PATH}BBBiolib.c -o ${LIB_PATH}BBBiolib.o
	ar -rs ${LIB_PATH}libBBBio.a ${LIB_PATH}BBBiolib.o
	cp ${LIB_PATH}libBBBio.a ./


LED : Demo_LED/LED.c libBBBio.a
	gcc -o LED Demo_LED/LED.c -L ${LIB_PATH} -lBBBio

ADT7301 : Demo_ADT7301/ADT7301.c libBBBio.a
	gcc -o ADT7301 Demo_ADT7301/ADT7301.c -L ${LIB_PATH} -lBBBio

.PHONY: clean
clean :
	rm -rf ${LIB_PATH}*.o ${LIB_PATH}libBBBio.a libBBBio.a LED ADT7301

