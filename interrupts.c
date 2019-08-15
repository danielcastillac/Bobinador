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
#define _XTAL_FREQ 8000000

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/
char palabra[20]; // Buffer variable for reception
char send[5]; // Buffer for transmission
unsigned int n = 0; // Receive char counter
char recibi = 0;
unsigned int overflow = 0; // TMR0 overflow counter
unsigned int ADC_value_press;
unsigned int ADC_value_dist;
extern bool MOT_1; // Move motor 1 flag
extern bool MOT_2; // Move motor 2 flag
extern bool MOT_3; // Move motor 3 flag
extern bool MOT_4; // Move motor 4 flag
extern bool DIR_1; // Direction of motor 1
extern bool DIR_2; // Direction of motor 2
extern bool DIR_3; // Direction of motor 3
extern bool DIR_4; // Direction of motor 4
unsigned int MOT_1_count = 0; // Motor 1 cycle count
unsigned int MOT_2_count = 0; // Motor 2 cycle count
unsigned int MOT_3_count = 0; // Motor 3 cycle count
unsigned int MOT_4_count = 0; // Motor 4 cycle count
unsigned int count_1 = 5; // Number of half -periods motor 1
unsigned int count_2 = 5; // Number of half -periods motor 2
unsigned int count_3 = 5; // Number of half -periods motor 3
unsigned int count_4 = 20; // Number of half -periods motor 4

/* High-priority service */

void interrupt high_isr(void) {
    if (INTCONbits.TMR0IF) {
        /* Timer0 ISR for motor control time-step */
        INTCONbits.TMR0IF = 0; // Restart TMR0 interrupt flag
        TMR0 = 250; // Time-step (MAX 255 8 bits, 65535 16 bits)
        MOT_1_count++;
        MOT_2_count++;
        MOT_3_count++;
        MOT_4_count++;

        if (MOT_1) {
            if (MOT_1_count == count_1) {
                LATAbits.LA3 = !PORTAbits.RA3;
                MOT_1_count = 0;
            }
        }
        if (MOT_2) {
            if (MOT_2_count == count_2) {
                LATAbits.LA5 = !PORTAbits.RA5;
                MOT_2_count = 0;
            }
        }
        if (MOT_3) {
            if (MOT_3_count == count_3) {
                LATCbits.LC1 = !PORTCbits.RC1;
                MOT_3_count = 0;
            }
        }
        if (MOT_4) {
            if (MOT_4_count == count_4) {
                LATBbits.LB6 = !PORTBbits.RB6;
                MOT_4_count = 0;
            }
        }

    } else if (PIR1bits.RCIF) {
        /* Recieve bluetooth ISR */
        PIR1bits.RCIF = 0; // Restart Recieve interrupt flag
        palabra[n] = RCREG; // Save recieve char in buffer variable
        n++;
        if (RCREG == '\n') {
            TXREG = palabra[0]; // Retransmit to check connection
            while (TXIF == 0);
            recibi = 1;
            n = 0;
        }
    } else if (PIR1bits.ADIF) {
        /* A/D converter ISR */
        PIR1bits.ADIF = 0; // Restart A/D flag
        if (ADCON0bits.CHS == 0b0000) {
            ADC_value_press = ADRES;
        } else if (ADCON0bits.CHS == 0b0001) {
            ADC_value_dist = ADRES;
        }

        //        __delay_ms(10); // REVISAR SI PONIENDOLO EXISTEN PROBLEMAS
        //        ADCON0bits.CHS = !ADCON0bits.CHS; // Change channel

    } else if (INTCON3bits.INT1IF) {
        /* Limit switch 1 ISR */
        INTCON3bits.INT1IF = 0;
        // PARAR MOVIMIENTO EN LA DIRECTION CUANDO SE RECIBA BANDERA
        //        MOT_3 = false;
        DIR_3 = !DIR_3;

    } else if (INTCON3bits.INT2IF) {
        /* Limit switch 2 ISR */
        INTCON3bits.INT2IF = 0;
        // PARAR MOVIMIENTO EN LA DIRECTION CUANDO SE RECIBA BANDERA
        //        MOT_3 = false;
        DIR_3 = !DIR_3;

    }
}