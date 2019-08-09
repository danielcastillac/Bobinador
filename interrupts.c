/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#elif defined(__18CXX)
#include <p18cxxx.h>    /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#endif

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/
char palabra[20];
unsigned int n = 0;
char recibi = 0;
unsigned int overflow = 0; // TMR0 overflow counter

/* High-priority service */

void interrupt high_isr(void) {
    if (INTCONbits.TMR0IF) {
        /* Timer0 ISR for motor control time-step */
        INTCONbits.TMR0IF = 0; // Restart TMR0 interrupt flag
        overflow++;
        LATAbits.LA0 = !PORTAbits.RA0; // Invert state
        TMR0 = 50;
    } else if (PIR1bits.RCIF) {
        /* Recieve ISR */
        PIR1bits.RCIF = 0; // Restart Recieve interrupt flag
        palabra[n] = RCREG; // Save recieve char in buffer variable
        n++;
        if (RCREG == '\r') {
            TXREG = palabra[0]; // Retransmit to check connection
            while (TXIF == 0);
            recibi = 1;
            n = 0;
        }
    }
}