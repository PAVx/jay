# MICROCONTROLELR DEFINITIONS #
CC							= avr-gcc
BURNER						= avrdude
CC_AVR						= atmega328p
F_CPU						= 16000000UL
BURNER_AVR					= ATMEGA328P
BURNER_PROGRAMMER			= usbasp
COMPILER_PATH				= /dev/tty.usbmodem*
OBJECT_COPY					= avr-objcopy
BAUDRATE					= 115200
###########################################

# CC GLOBAL DEFINES
CC_GLOBAL_DEFINES = -D F_CPU=${F_CPU}

###########################################
# MAIN SOURCE
CSOURCE						= main.c

####################
# DRIVER HEADERS #
# UART DRIVER
SEARCHABLE_HEADER_FILE_DIRS	+= -I drivers/uart_driver
# BUFFER
SEARCHABLE_HEADER_FILE_DIRS += -I drivers/buffer
# TWI
SEARCHABLE_HEADER_FILE_DIRS	+= -I drivers/twi
# I2C DRIVER
SEARCHABLE_HEADER_FILE_DIRS	+= -I drivers/i2c_driver
# CLOCK
SEARCHABLE_HEADER_FILE_DIRS	+= -I drivers/clock
# ITG3200
SEARCHABLE_HEADER_FILE_DIRS	+= -I drivers/itg3200_gyro

####################
# PROTOCOL HEADERS #
# UART #
SEARCHABLE_HEADER_FILE_DIRS	+= -I protocols/uart
# I2C #
SEARCHABLE_HEADER_FILE_DIRS	+= -I protocols/i2c
# GYRO #
SEARCHABLE_HEADER_FILE_DIRS	+= -I protocols/gyro
# TEMP #
SEARCHABLE_HEADER_FILE_DIRS	+= -I protocols/temperature

####################

######################################################

####################
# DRIVER SOURCES #
# UART #
CSOURCE						+= drivers/uart_driver/uart_driver.c
# BUFFER #
CSOURCE						+= drivers/buffer/buffer.c
# TWI #
CSOURCE						+= drivers/twi/twi.c
# CLOCK #
CSOURCE						+= drivers/clock/clock.c
# I2C #
CSOURCE						+= drivers/i2c_driver/i2c_driver.c
# ITG3200 #
CSOURCE						+= drivers/itg3200_gyro/itg3200.c
####################

####################
# PROTOCOL SOURCES #
# UART #
CSOURCE						+= protocols/uart/uart.c
# I2C #
CSOURCE						+= protocols/i2c/i2c.c
# GYRO #
CSOURCE						+= protocols/gyro/gyro.c
# TEMP #
CSOURCE						+= protocols/temperature/temperature.c
####################

######################################################

C_STD						= gnu99

OBJECTS	 					= ./*.o
EXECUTABLE					= Pigeon

########### COMPILER OPTIONS ##############
CFLAGS						+= -Wall
CFLAGS						+= -Werror
CFLAGS						+= -Wextra
CFLAGS						+= -Wimplicit

CFLAGS						+= -std=${C_STD}

# CC OPTIONS #
# OPTIMIZATION FLAG
CC_COMP_OPTIONS				+= -Os
# UC DEF
CC_COMP_OPTIONS				+= -mmcu=${CC_AVR}
# GLOBAL DEFINES FLAGS
CC_COMP_OPTIONS				+= ${CC_GLOBAL_DEFINES}
# CC WARNING FLAGS
CC_COMP_OPTIONS				+= ${CFLAGS}
# HEADER FILES
CC_COMP_OPTIONS				+= ${SEARCHABLE_HEADER_FILE_DIRS}
# SOURCE FILES
CC_COMP_OPTIONS				+= -c ${CSOURCE}


# CC LINKER OPTIONS #
CC_LN_OPTIONS				+= -mmcu=${CC_AVR}
CC_LN_OPTIONS				+= -Wl,-u,vfprintf
CC_LN_OPTIONS				+= -lm
CC_LN_OPTIONS				+= -lprintf_flt
CC_LN_OPTIONS				+= -o${EXECUTABLE}.elf
CC_LN_OPTIONS				+= ${OBJECTS}


all:
	${CC} ${CC_COMP_OPTIONS}
	${CC} ${CC_LN_OPTIONS}

	${OBJECT_COPY} -j .text -j .data -O ihex ${EXECUTABLE}.elf ${EXECUTABLE}.hex
	#${BURNER} -F -V -P ${COMPILER_PATH} -p ${BURNER_AVR} -c ${BURNER_PROGRAMMER} -e -b ${BAUDRATE} -U flash:w:${EXECUTABLE}.hex
	${BURNER} -F -p m328p -c usbasp -e -b ${BAUDRATE} -U flash:w:${EXECUTABLE}.hex 

eight:
	#${CC} -fno-tree-scev-cprop -ffunction-sections -fdata-sections -ffreestanding
	${CC} -Os -mmcu=atmega328p -DF_CPU=16000000UL ${CFLAGS} -c ${CSOURCE}
	${CC} -mmcu=atmega328p -Wl,-u,vfprintf -lprintf_flt -lm -o${EXECUTABLE}.elf ${OBJECTS}
	${OBJECT_COPY} -j .text -j .data -O ihex ${EXECUTABLE}.elf ${EXECUTABLE}.hex
	${BURNER} -F -p m328p -c usbasp -e -b ${BAUDRATE} -U flash:w:${EXECUTABLE}.hex
	rm -f ${OBJECTS} ${EXECUTABLE}.elf ${EXECUTABLE}.hex
