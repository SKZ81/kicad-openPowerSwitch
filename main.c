#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/delay_basic.h>

// Network
#include <enc28j60.h>
#include <ip_arp_udp_tcp.h>
#include <net.h> // net.h really ??????? better naming required here, please !
// Indeed, I disagree to "include <net.h>" to import *ONLY* packet offsets macros


// read/write to UART using FILE*
#include <stdio.h>
#include "avr_uart.h"

char response[12] = "ACK_ACK_ACK";

static uint8_t mac_address[6] = {0x22,0x22,0x22,0x10,0x00,0x33};
static uint8_t local_host[4]  = {192,168,1,191};
// static uint8_t remote_host[4] = {192,168,1,155};
static uint8_t gateway[4]     = {192,168,1,1};
static uint16_t local_port    = 2000;
//static uint16_t remote_port   = 5000;

// RAM buffer to receive / prepare the packet.
// NB : should be read / prepared directly in ENC28J60 SRAM
#define BUFFER_SIZE 1024 // should be large enough for this time (NB : half the SRAM of an atmega328p / Arduino UNO)
uint8_t buf[BUFFER_SIZE];


static void network_init (void) {
    _delay_loop_1(0);
    printf("enc28j60Init(mac_address);\n");
    enc28j60Init(mac_address);
    printf("enc28j60clkout(2);\n");
    enc28j60clkout(2);
    _delay_loop_1(0);
    printf("enc28j60PhyWrite(PHLCON,0x476);\n");
    enc28j60PhyWrite(PHLCON,0x476);
    _delay_loop_1(0);
    printf("init_ip_arp_udp_tcp(mac_address,local_host,local_port);\n");
    init_ip_arp_udp_tcp(mac_address,local_host,local_port);
    printf("client_set_gwip(gateway);\n");
    client_set_gwip(gateway);
    printf("Wait for link UP.");
    while(!enc28j60linkup()) {
        printf(".");
    }
    printf(" OK.\n");
}


inline
char HEX_DIGIT(uint8_t val) {
    if (val>=0 && val<10) {
        return '0' + val;
    } else if (val>=10 && val < 16) {
        return 'A' + val - 10;
    }
    return '?';
}


inline
void HEXDUMP(uint8_t *buf, uint16_t len, uint16_t size) {
    char ascii[9] = {0,0,0,0,0,0,0,0,0};
    uint16_t count = 0;

    while(count < len && count < size) {
        ascii[0]=0;ascii[1]=0;ascii[2]=0;ascii[3]=0;ascii[4]=0;ascii[5]=0;ascii[6]=0;ascii[7]=0;ascii[8]=0;
        for(int i=0; i<8; i++) {
            if (count < size) { // still < ??
                printf("%c%c ", HEX_DIGIT(buf[count]>>4), HEX_DIGIT(buf[count] & 0x0F));
                if (buf[count]>31 && buf[count]<128) {
                    ascii[i] = buf[count];
                } else {
                    ascii[i] = '?';
                }
            } else { // no more data to dump
                printf("-- ");
                ascii[i] = '-';
            }
            count++;
        }
        printf("\t\t%s\n", ascii);
    }
}

int main(void) {

    avr_uart_init();

    stdout = &avr_uart_output;
    stdin  = &avr_uart_input_echo;

    printf("ENC28J60 UDP echo server\n");

    network_init();
    // infite loop
    do {
        if (enc28j60hasRxPkt()) {
            // wait and read packet
            // NB : as MAC configured, the packet is either our own, or a broadcast
            enc28j60PacketReceive(BUFFER_SIZE, buf);

            printf("Got a packet : ");
            uint16_t size = buf[UDP_LEN_H_P]<<8 | buf[UDP_LEN_L_P];
            if (size > BUFFER_SIZE - ETH_HEADER_LEN) {
                printf("ETH buffer overflow (received : %d > max : %d), drop packet\n", size, (BUFFER_SIZE-ETH_HEADER_LEN) );
            } else {
                HEXDUMP(buf, BUFFER_SIZE, size);

                // data to reply is packet content
                //char data[] = buf + ETH_HEADER_LEN + IP_HEADER_LEN + UDP_HEADER_LEN;

                make_udp_reply_from_request(buf, response, 11, local_port);

                enc28j60PacketSend(size, buf);

                printf("Sent Reply!\n");
            }
        }
        _delay_ms(10);
    } while (1);

    return (0);

}
