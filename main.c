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
unsigned int caliber;
unsigned int diameter;
unsigned int length;
unsigned int turns;
unsigned int speed;
unsigned int PWM_duty;

/* i.e. uint8_t <variable_name>; */
extern unsigned int overflow;
extern char recibi;
extern char palabra[20];
extern unsigned int n;
extern unsigned int ADC_value;

/******************************************************************************/
/* Main Program                                                               */
/* Function prototypes */

/******************************************************************************/

void main(void) {
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    InitApp();

    while (1) {

        CCPR1L = PWM_duty;
        LATAbits.LA1 = DIR_1; // Set motor 1 direction

        if (recibi == 1) {
            /* Bluetooth reception routine */
            recibi = 0;

            if (palabra[0] == 'B') {
                // Its controlling LED intensity
                DIR_1 = !DIR_1;
                PWM_duty = ((palabra[1] - 48) * 10) + ((palabra[2] - 48));
            } else if (palabra[0] == 'A') {
                // Its trasmiting the parameters
                caliber = ((palabra[1] - 48) * 10) + ((palabra[2] - 48)); // 2 digits
                diameter = ((palabra[3] - 48) * 1000) + ((palabra[4] - 48) * 100) + ((palabra[5] - 48) * 10) + ((palabra[6] - 48)); // 4 digits (int not float)
                length = ((palabra[7] - 48) * 10000) + ((palabra[8] - 48) * 1000) + ((palabra[9] - 48) * 100) + ((palabra[10] - 48) * 10) + ((palabra[11] - 48)); // 5 digits (int not float)
                turns = ((palabra[12] - 48) * 1000) + ((palabra[13] - 48) * 100) + ((palabra[14] - 48) * 10) + ((palabra[15] - 48)); // 4 digits
                speed = palabra[15]; // 1 digit: 1: low; 2: medium, 3: high

//                if (caliber == 20 && diameter == 2000) {
//                    DIR_1 = 0;
//                }
                
            }


        } else if (GODONE == 0) {
            /* Restart ADC data gattering */
            GODONE = 1;
        }
    }
}
