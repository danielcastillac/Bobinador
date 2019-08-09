/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
#include <xc.h>        /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>       /* HiTech General Include File */
#elif defined(__18CXX)
#include <p18cxxx.h>   /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#endif

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#define _XTAL_FREQ 8000000

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
unsigned int motor1 = 1; // Cycle number for motor 1
unsigned int motor2; // Cycle number for motor 2
bool RA0state = false; // Current state of STEP pin for motor 1
bool DIR_1 = 0; // Direction of motor 1

/* i.e. uint8_t <variable_name>; */
extern unsigned int overflow;
extern char recibi;
extern char palabra[20];
extern unsigned int n;
extern unsigned int ADC_value;

/******************************************************************************/
/* Main Program                                                               */

/******************************************************************************/

void main(void) {
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    InitApp();

    while (1) {

        LATAbits.LA1 = DIR_1; // Set motor 1 direction

        if (recibi == 1) {
            /* Bluetooth reception routine */
            recibi = 0;
            switch (palabra[n]) {
                    /* Selection for specific set of instructions */
                case 49:

                    DIR_1 = 1;
                    break;
                case 50:

                    DIR_1 = 0;
                    break;
            }
        } else if (GODONE == 0) {
            /* Restart ADC data gattering */
            GODONE = 1;
        }
    }
}