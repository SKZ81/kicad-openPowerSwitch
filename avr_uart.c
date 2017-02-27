#include <avr/io.h>
#include <stdio.h>


// ----------- UART parameters ----------

// NOTE : The compilation warning issued by setbaud.h is because the tolerance of baud rate actually acheived is over 102% the wanted 115200.
// But I always worked reliably with arduino boards ; from Linux as well as Windows, on multiple years, at this baud rate, then.. This warning is pretty safe to ignore.

// THOUGH, you still can override those value from the command line (cf makefiles).


#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef BAUD
#define BAUD 115200
#endif

// setbaud.h issues warnings, ;) but also defines UBRR*_VALUE, used in avr_uart_init
#include <util/setbaud.h>

/* http://www.cs.mun.ca/~rod/Winter2007/4723/notes/serial/serial.html */

void avr_uart_init(void) {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
}

void avr_uart_putchar(char c, FILE *stream) {
    if (c == '\n') {
        avr_uart_putchar('\r', stream);
    }
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
}

char avr_uart_getchar(FILE *stream) {
    loop_until_bit_is_set(UCSR0A, RXC0);
    return UDR0;
}


char avr_uart_getchar_echo(FILE *stream) {
    loop_until_bit_is_set(UCSR0A, RXC0);
    char c=UDR0;
    avr_uart_putchar(c, stream);
    return c;
}




// XXX TODO : FIX THIS warning:
// initialization from incompatible pointer type [-Wincompatible-pointer-types]

FILE avr_uart_output = FDEV_SETUP_STREAM(avr_uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE avr_uart_input = FDEV_SETUP_STREAM(NULL, avr_uart_getchar, _FDEV_SETUP_READ);
FILE avr_uart_input_echo = FDEV_SETUP_STREAM(avr_uart_putchar, avr_uart_getchar_echo, _FDEV_SETUP_RW);
