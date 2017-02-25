#include "Queue.h"

#if !defined(F_CPU)
    #warning "F_CPU not defined in makefile - now defined in softuart.h"
    #define F_CPU 16000000UL

#endif

#define SOFTUART_BAUD_RATE      1200
#define SOFTUART_CHANNELS       1

/****************************************************************************/
// These are the definitions for Software UART Channel 1
#if SOFTUART_CHANNELS > 0
#define SOFTUART_RXPIN_1      PIND
#define SOFTUART_RXDDR_1      DDRD
#define SOFTUART_RXBIT_1      PD2

#define SOFTUART_TXPORT_1     PORTD
#define SOFTUART_TXDDR_1      DDRD
#define SOFTUART_TXBIT_1      PD3
#endif

/****************************************************************************/
// These are the definitions for Software UART Channel 2
#if SOFTUART_CHANNELS > 1
#define SOFTUART_RXPIN_2      PIND
#define SOFTUART_RXDDR_2      DDRD
#define SOFTUART_RXBIT_2      PD4

#define SOFTUART_TXPORT_2     PORTD
#define SOFTUART_TXDDR_2      DDRD
#define SOFTUART_TXBIT_2      PD5
#endif

/****************************************************************************/
// These are the definitions for Software UART Channel 3
#if SOFTUART_CHANNELS > 2
#define SOFTUART_RXPIN_3      PIND
#define SOFTUART_RXDDR_3      DDRD
#define SOFTUART_RXBIT_3      PD6

#define SOFTUART_TXPORT_3     PORTD
#define SOFTUART_TXDDR_3      DDRD
#define SOFTUART_TXBIT_3      PD7
#endif

/****************************************************************************/
// These are the definitions for Software UART Channel 4
#if SOFTUART_CHANNELS > 3
#define SOFTUART_RXPIN_4      PIND
#define SOFTUART_RXDDR_4      DDRD
#define SOFTUART_RXBIT_4      PD8

#define SOFTUART_TXPORT_4     PORTD
#define SOFTUART_TXDDR_4      DDRD
#define SOFTUART_TXBIT_4      PD9
#endif


#define SOFTUART_T_COMP_LABEL      TIMER0_COMPA_vect
#define SOFTUART_T_COMP_REG        OCR0A
#define SOFTUART_T_CONTR_REGA      TCCR0A
#define SOFTUART_T_CONTR_REGB      TCCR0B
#define SOFTUART_T_CNT_REG         TCNT0
#define SOFTUART_T_INTCTL_REG      TIMSK0
#define SOFTUART_CMPINT_EN_MASK    (1 << OCIE0A)
#define SOFTUART_CTC_MASKA         (1 << WGM01)
#define SOFTUART_CTC_MASKB         (0)

/* "A timer interrupt must be set to interrupt at three times
   the required baud rate." */
#define SOFTUART_PRESCALE (64)
// #define SOFTUART_PRESCALE (1)

#if (SOFTUART_PRESCALE == 8)
    #define SOFTUART_PRESC_MASKA         (0)
    #define SOFTUART_PRESC_MASKB         (1 << CS01)
#elif (SOFTUART_PRESCALE==1)
    #define SOFTUART_PRESC_MASKA         (0)
    #define SOFTUART_PRESC_MASKB         (1 << CS00)
#elif (SOFTUART_PRESCALE==64)
    #define SOFTUART_PRESC_MASKA         (0)
    #define SOFTUART_PRESC_MASKB         (1 << CS01)|(1 << CS00)
#else
    #error "prescale unsupported"
#endif


#define SOFTUART_TIMERTOP ( F_CPU/SOFTUART_PRESCALE/SOFTUART_BAUD_RATE/3 - 1)

#if (SOFTUART_TIMERTOP > 0xff)
    #warning "Check SOFTUART_TIMERTOP: increase prescaler, lower F_CPU or use a 16 bit timer"
#endif

#define SOFTUART_IN_BUF_SIZE     32
#define SOFTUART_OUT_BUF_SIZE    8

typedef struct softuartRX_t{
  char           inbuf[SOFTUART_IN_BUF_SIZE];
  unsigned char  qin;
  unsigned char  qout;
  unsigned char  flag_rx_off;
  unsigned char  flag_rx_ready;
} softuartRX;

typedef struct softuartTX_t{
  unsigned char  flag_tx_busy;
  unsigned char  timer_tx_ctr;
  unsigned char  bits_left_in_tx;
  unsigned short internal_tx_buffer; /* ! mt: was type uchar - this was wrong */

  unsigned char flag_ok_to_pop;
  Queue tx_buffer;
} softuartTX;

typedef struct softuartISR_t{
  unsigned char flag_rx_waiting_for_stop_bit; // = SU_FALSE;
  unsigned char rx_mask;

  unsigned char timer_rx_ctr;
  unsigned char bits_left_in_rx;
  unsigned char internal_rx_buffer;
  int tx_byte;
} softuartISR;

typedef struct softUART_t{
  softuartRX rx;
  softuartTX tx;
  softuartISR isr;
  uint32_t baud;
  uint8_t type;
  uint8_t id;
} softUART;

// Init the Software Uart
void softuart_init(void);

// Clears the contents of the input buffer.
void softuart_flush_input_buffer( int i ) ;

// Tests whether an input character has been received.
unsigned char softuart_kbhit( int i );

// Reads a character from the input buffer, waiting if necessary.
char softuart_getchar( int i );

// To check if transmitter is busy
unsigned char softuart_transmit_busy( int i );

// Writes a character to the serial port.
void softuart_putchar( const char, int i );

// Turns on the receive function.
void softuart_turn_rx_on( int i );

// Turns off the receive function.
void softuart_turn_rx_off( int i );

// Write a NULL-terminated string from RAM to the serial port
void softuart_puts( const char *s, int i );

// Write a NULL-terminated string from program-space (flash)
// to the serial port. example: softuart_puts_p(PSTR("test"))
void softuart_puts_p( const char *prg_s, int i );

// Helper-Macro - "automatically" inserts PSTR
// when used: include avr/pgmspace.h before this include-file
#define softuart_puts_P(s___) softuart_puts_p(PSTR(s___))
