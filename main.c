#include <avr/io.h>
#include <avr/interrupt.h>

// read/write to UART using FILE*
#include <stdio.h>
#include "avr_uart.h"

void setup_relays() {
    DDRC |=  0b000111110;
    PORTC &= 0b111000001;
}

#define RELAY_MIN 1
#define RELAY_MAX 5
uint8_t setRelayState(int8_t relay, uint8_t state) {
    if ((relay < RELAY_MIN) || (relay > RELAY_MAX)) {
        return 0;
    }

    if (state) {
        // our model of relay is active low, so reset bit for ACTIVE state
        PORTC &= ~_BV(relay);
    } else {
        // set bit for STANDBY state
        PORTC |= _BV(relay);
    }
    return 1;
}

uint8_t getRelayState(int relay, uint8_t */*inout*/ state) {
    if ((relay < RELAY_MIN) || (relay > RELAY_MAX)) {
        return 0;
    }
    *state = PORTC & _BV(relay);
    return 1;
}

int main(void) {

    avr_uart_init();
    setup_relays();

    stdout = &avr_uart_output;
    stdin  = &avr_uart_input_echo;

    int relay=0, state=0;
    printf("openPowerSwitch v0.1\n");
    while(1) {
        printf("> ");
        fscanf(stdin, "%i %i", &relay, &state);
        printf("\nSwitch relay #%i %s : %s\n", relay, state==0?"off":"on", setRelayState(relay, !(state==0))?"OK":"ERROR");
    }

    return 0;
}
