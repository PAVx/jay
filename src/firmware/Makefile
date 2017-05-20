# MICROCONTROLELR DEFINITIONS #
CC							= avr-gcc
BURNER						= avrdude
CC_AVR						= atmega328p
F_CPU						= 16000000UL
BURNER_AVR					= ATMEGA328P
BURNER_PROGRAMMER				= Arduino
COMPILER_PATH					= COM4 #/dev/tty.usb*
OBJECT_COPY					= avr-objcopy
BAUDRATE					= 115200
###########################################

# CC GLOBAL DEFINES
CC_GLOBAL_DEFINES 				= -D F_CPU=${F_CPU}

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
SEARCHABLE_HEADER_FILE_DIRS 	+= -I drivers/buffer
# I2C DRIVER
SEARCHABLE_HEADER_FILE_DIRS	+= -I drivers/i2c_driver
# ITG3200
SEARCHABLE_HEADER_FILE_DIRS	+= -I drivers/itg3200_gyro
# MAG
SEARCHABLE_HEADER_FILE_DIRS	+= -I drivers/hmc5883l_mag
# PWM
SEARCHABLE_HEADER_FILE_DIRS	+= -I drivers/pwm
# ACCEL
SEARCHABLE_HEADER_FILE_DIRS	+= -I drivers/adxl345_accel
# GPS
SEARCHABLE_HEADER_FILE_DIRS	+= -I drivers/nmea
# IR SENSOR
SEARCHABLE_HEADER_FILE_DIRS	+= -I drivers/d6t8l_ir
# ADC DRIVER
SEARCHABLE_HEADER_FILE_DIRS	+= -I drivers/adc

####################
# PROTOCOL HEADERS #
SEARCHABLE_HEADER_FILE_DIRS 	+= -I protocols/
# UART #
SEARCHABLE_HEADER_FILE_DIRS	+= -I protocols/uart
# MAG #
SEARCHABLE_HEADER_FILE_DIRS	+= -I protocols/mag
# GYRO #
SEARCHABLE_HEADER_FILE_DIRS	+= -I protocols/gyro
# TEMP #
SEARCHABLE_HEADER_FILE_DIRS	+= -I protocols/temperature
# MOTORS #
SEARCHABLE_HEADER_FILE_DIRS 	+= -I protocols/motors
# COMMUNICATION #
SEARCHABLE_HEADER_FILE_DIRS 	+= -I protocols/com
# ACCEL #
SEARCHABLE_HEADER_FILE_DIRS	+= -I protocols/accel
# GPS #
SEARCHABLE_HEADER_FILE_DIRS 	+= -I protocols/gps
# BATTERY #
SEARCHABLE_HEADER_FILE_DIRS 	+= -I protocols/battery

####################
# SYSTEM HEADERS #
SEARCHABLE_HEADER_FILE_DIRS	+= -I system/
# TICK #
SEARCHABLE_HEADER_FILE_DIRS	+= -I system/tick
# INIT #
SEARCHABLE_HEADER_FILE_DIRS	+= -I system/init
# LEDS #
SEARCHABLE_HEADER_FILE_DIRS	+= -I system/leds
# TYPES #
SEARCHABLE_HEADER_FILE_DIRS	+= -I system/types
# MEMORY #
SEARCHABLE_HEADER_FILE_DIRS 	+= -I system/memory/nonvolatile
####################

####################
# CONTROL HEADERS #
SEARCHABLE_HEADER_FILE_DIRS 	+= -I control/
# STATUS #
SEARCHABLE_HEADER_FILE_DIRS	+= -I control/status
# MANUAL CONTROL #
SEARCHABLE_HEADER_FILE_DIRS 	+= -I control/manual
# CONTROL PACKETS #
SEARCHABLE_HEADER_FILE_DIRS 	+= -I control/control_packets
# ANGLES #
SEARCHABLE_HEADER_FILE_DIRS 	+= -I control/angles
# ATTITUDE ADJUST #
SEARCHABLE_HEADER_FILE_DIRS 	+= -I control/attitude_adjust
# PID #
SEARCHABLE_HEADER_FILE_DIRS 	+= -I control/pid
# COMPLIMENTARY FILTER #
#SEARCHABLE_HEADER_FILE_DIRS	+= -I control/complimentary
# SENSOR FUSION #
#SEARCHABLE_HEADER_FILE_DIRS	+= -I control/sensor_fusion

######################################################

####################
# DRIVER SOURCES #
# UART #
CSOURCE						+= drivers/uart_driver/uart_driver.c
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
# NMEA #
CSOURCE						+= drivers/nmea/nmea.c
# ADC #
CSOURCE						+= drivers/adc/adc.c
# IR #
CSOURCE						+= drivers/d6t8l_ir/d6t8l.c
####################

####################
# PROTOCOL SOURCES #
# UART #
CSOURCE						+= protocols/uart/uart.c
# SOFTUART #
CSOURCE						+= drivers/softuart_driver/softuart.c
# MAG #
CSOURCE						+= protocols/mag/mag.c
# GYRO #
CSOURCE						+= protocols/gyro/gyro.c
# TEMP #
CSOURCE						+= protocols/temperature/temperature.c
# MOTORS #
CSOURCE						+= protocols/motors/motors.c
# COMMUNICATION #
CSOURCE						+= protocols/com/packet.c
# ACCEL #
CSOURCE						+= protocols/accel/accel.c
# GPS #
CSOURCE                    	+= protocols/gps/gps.c
# BATTERY #
CSOURCE                    	+= protocols/battery/battery.c
####################

####################
# SYSTEM #
# TICK #
CSOURCE						+= system/tick/system_tick.c
# INIT #
CSOURCE						+= system/init/system_init.c
# LEDS #
CSOURCE						+= system/leds/leds.c
# TYPES #
CSOURCE						+= system/types/types.c
# MEMORY #
CSOURCE						+= system/memory/nonvolatile/nvmem.c
####################

####################
# CONTROL #
# STATUS #
CSOURCE						+= control/status/status.c
# CONTROL PACKETS #
CSOURCE						+= control/control_packets/ctrl_pkt.c
# MANUAL CONTROL #
CSOURCE						+= control/manual/manual_control.c
# ANGLES #
CSOURCE						+= control/angles/angles.c
# ATTITUDE ADJUST #
CSOURCE						+= control/attitude_adjust/attitude_adjust.c
# PID #
CSOURCE						+= control/pid/pid.c
# COMPLIMENTARY FILTER #
#CSOURCE						+= control/complimentary/comp_filter.c
# SENSOR FUSION #
#CSOURCE						+= control/sensor_fusion/sensfusion.c

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
	${BURNER} -V -P ${COMPILER_PATH} -p ${BURNER_AVR} -c ${BURNER_PROGRAMMER} -e -b ${BAUDRATE} -U flash:w:${EXECUTABLE}.hex
	#${BURNER} -F -p ${BURNER_AVR} -c ${BURNER_PROGRAMMER} -e -b ${BAUDRATE} -U flash:w:${EXECUTABLE}.hex

fusebits328p:
	# sets fuse bits to use the external 16MHz XTAL on the Atmega 328P
	avrdude -p m328p -c usbasp -U lfuse:w:0xce:m

clean:
	rm *.elf *.hex *.o
