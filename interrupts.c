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

unsigned int overflow = 0; // TMR0 overflow counter

/* High-priority service */

void interrupt high_isr(void) {
    if(INTCONbits.TMR0IF) {
        /* Timer0 ISR for motor control time-step */
        INTCONbits.TMR0IF = 0; // Restart TMR0 interrupt flag
        T0CONbits.TMR0ON = 1; // Enable timer0
        overflow++;
        }
}