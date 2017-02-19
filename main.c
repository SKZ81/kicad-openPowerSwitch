#include <avr/io.h>
#include <avr/interrupt.h>

// read/write to UART using FILE*
#include <stdio.h>
#include "avr_uart.h"



int main(void) {

    avr_uart_init();
    stdout = &avr_uart_output;
    stdin  = &avr_uart_input;

//     char input;

    while(1) {
//         puts("Hello world!\n");
//         input = getchar();
//         printf("You wrote %c\n", input);
    }

    return 0;
}
