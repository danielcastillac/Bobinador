/******************************************************************************/
/* Files to Include                                                           */
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

#include "user.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void InitApp(void) {
    /* TODO Initialize User Ports/Peripherals/Project here */
    PORTA = 0;
    LATA = 0;
    PORTB = 0;
    LATB = 0;
    PORTC = 0;
    LATC = 0;
    /* Setup analog functionality */
    ADCON1 = 0b1110; // All ports digital except AN0
    TRISAbits.TRISA0 = 1; // A/D input pin
    ADCON0bits.CHS = 0; // A/D Channel 0
    ADCON2 = 0b10101100; // Right justified, 12 TAD, FOSC/4
    PIE1bits.ADIE = 1; // A/D Interrupt Enable bit
    PIR1bits.ADIF = 0; // A/D interrupt flag
    ADCON0bits.ADON = 1; // A/D converter enabled REVISAR
    /* Initialize peripherals */
    TRISAbits.RA1 = 0; // Motor 1, DIR output
    TRISAbits.RA2 = 0; // Motor 1, STEP output
    TRISAbits.RA3 = 0; // Motor 2, DIR output
    TRISAbits.RA4 = 0; // Motor 2, STEP output
    TRISCbits.RC0 = 0; // Motor 3, DIR output
    TRISCbits.RC1 = 0; // Motor 3, STEP output
    TRISBbits.RB7 = 0; // Motor 4, DIR output
    TRISBbits.RB6 = 0; // Motor 4, STEP output   
    /* Configure the IPEN bit (1=on) in RCON to turn on/off int priorities */
    RCONbits.IPEN = 0; // Disable interrupt priorities
    /* Enable interrupts */
    INTCONbits.GIE = 1; // Global interrupt enable
    INTCONbits.PEIE = 1; // Enable peripheral interrupts
    INTCONbits.TMR0IE = 1; // Enable timer0 interrupt
    INTCONbits.TMR0IF = 0; // Clear TMR0 interrupt flag
    INTCON2bits.TMR0IP = 1; // TMR0 interrupt High priority
    T0CONbits.T08BIT = 1; // TMR0: 16 bits
    T0CONbits.T0CS = 0; // Internal instruction cycle clock source
    T0CONbits.PSA = 0; // Prescaler is assigned
    T0CONbits.T0PS = 0b101; // Prescaler value 1:64
    TMR0 = 0x6; // Preload value
    // Finales de carrera
    INTCON3bits.INT1IE = 1; // Enable INT1
    INTCON3bits.INT2IE = 1; // Enable INT2
    INTCON3bits.INT1IF = 0; // Reset INT1 flag
    INTCON3bits.INT2IF = 0; // Reset INT2 flag    
    /* Bluetooth configuration */
    TRISCbits.RC6 = 0; // TX pin output
    TRISCbits.RC7 = 1; // RX pin input
    BRGH = 1; // High baud-rate
    BRG16 = 1;
    SPBRG = 207; // SPBRG=((_XTAL_FREQ/16)/Baud_rate) - 1;
    SPEN = 1; // Enable serial port pins
    SYNC = 0; // Asynchronous mode
    TXEN = 1; // Enable transmission
    CREN = 1; // Enable reception
    TX9 = 0; // 8-bit reception selected
    RX9 = 0; // 8-bit reception mode selected
    RCIE = 1; // Enable recieve interrupt bit
    TXIE = 0; // Disable transmit interrupt bit

}
