# MICROCONTROLELR DEFINITIONS #
CC							= avr-gcc
BURNER						= avrdude
CC_AVR						= atmega328p
F_CPU						= 16000000UL
BURNER_AVR					= ATMEGA328P
BURNER_PROGRAMMER			= usbasp
COMPILER_PATH				= /dev/tty.usbmodem
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
SEARCHABLE_HEADER_FILE_DIRS	+= -I drivers/
# UART DRIVER
SEARCHABLE_HEADER_FILE_DIRS	+= -I drivers/uart_driver
# SOFTUART DRIVER
SEARCHABLE_HEADER_FILE_DIRS	+= -I drivers/softuart_driver
# BUFFER
SEARCHABLE_HEADER_FILE_DIRS += -I drivers/buffer
# I2C DRIVER
SEARCHABLE_HEADER_FILE_DIRS	+= -I drivers/i2c_driver
# ITG3200
SEARCHABLE_HEADER_FILE_DIRS	+= -I drivers/itg3200_gyro
# MAG
SEARCHABLE_HEADER_FILE_DIRS	+= -I drivers/hmc5883l_mag
# PWM #
SEARCHABLE_HEADER_FILE_DIRS	+= -I drivers/pwm
# ACCEL
SEARCHABLE_HEADER_FILE_DIRS	+= -I drivers/adxl345_accel


####################

# PROTOCOL HEADERS #
SEARCHABLE_HEADER_FILE_DIRS	+= -I protocols/
# UART #
SEARCHABLE_HEADER_FILE_DIRS	+= -I protocols/uart
# MAG #
SEARCHABLE_HEADER_FILE_DIRS	+= -I protocols/mag
# GYRO #
SEARCHABLE_HEADER_FILE_DIRS	+= -I protocols/gyro
# TEMP #
SEARCHABLE_HEADER_FILE_DIRS	+= -I protocols/temperature
# MOTORS #
SEARCHABLE_HEADER_FILE_DIRS	+= -I protocols/motors
# COMMUNICATION #
SEARCHABLE_HEADER_FILE_DIRS	+= -I protocols/com
# ACCEL #
SEARCHABLE_HEADER_FILE_DIRS	+= -I protocols/accel


# SYSTEM HEADERS #
SEARCHABLE_HEADER_FILE_DIRS	+= -I system/
# TICK #
SEARCHABLE_HEADER_FILE_DIRS	+= -I system/tick
# INIT #
SEARCHABLE_HEADER_FILE_DIRS	+= -I system/init
# LEDS #
SEARCHABLE_HEADER_FILE_DIRS	+= -I system/leds
####################

######################################################

####################
# DRIVER SOURCES #
# UART #
CSOURCE						+= drivers/uart_driver/uart_driver.c
# SOFTUART #
CSOURCE						+= drivers/softuart_driver/Queue.c
# SOFTUART #
CSOURCE						+= drivers/softuart_driver/softuart.c
# BUFFER #
CSOURCE						+= drivers/buffer/buffer.c
# I2C #
CSOURCE						+= drivers/i2c_driver/i2c_driver.c
# ITG3200 #
CSOURCE						+= drivers/itg3200_gyro/itg3200.c
# HMC5883L #
CSOURCE						+= drivers/hmc5883l_mag/hmc5883l_mag.c
# PWM #
CSOURCE						+= drivers/pwm/pwm_driver.c
# ADXL345 #
CSOURCE						+= drivers/adxl345_accel/adxl345.c
####################

####################
# PROTOCOL SOURCES #
# UART #
CSOURCE						+= protocols/uart/uart.c
# MAG #
CSOURCE						+= protocols/mag/mag.c
# GYRO #
CSOURCE						+= protocols/gyro/gyro.c
# TEMP #
CSOURCE						+= protocols/temperature/temperature.c
# MOTORS #
CSOURCE						+= protocols/motors/motors.c
# COMMUNICATION #
CSOURCE 					+= protocols/com/packet.c
# ACCEL #
CSOURCE						+= protocols/accel/accel.c
####################

####################
# SYSTEM #
# TICK #
CSOURCE						+= system/tick/system_tick.c
# INIT #
CSOURCE						+= system/init/system_init.c
# LEDS #
CSOURCE						+= system/leds/leds.c
####################
######################################################

C_STD						= gnu99

OBJECTS	 					= ./*.o
EXECUTABLE					= jay

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

  #${BURNER} -V -P ${COMPILER_PATH} -p ${BURNER_AVR} -c ${BURNER_PROGRAMMER} -e -b ${BAUDRATE} -U flash:w:${EXECUTABLE}.hex
	${BURNER} -V -p ${BURNER_AVR} -P ${COMPILER_PATH} -c ${BURNER_PROGRAMMER} -e -b ${BAUDRATE} -U flash:w:${EXECUTABLE}.hex

clean:
	rm *.o
	rm *.hex
	rm *.elf
