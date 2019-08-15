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
bool DIR_2 = 0; // Direction of motor 2
bool DIR_3 = 0; // Direction of motor 3
bool DIR_4 = 0; // Direction of motor 4
bool MOT_1 = 0; // Move motor 1 flag
bool MOT_2 = 0; // Move motor 2 flag
bool MOT_3 = 0; // Move motor 3 flag
bool MOT_4 = 0; // Move motor 4 flag
/* Parameters */
unsigned int caliber;
unsigned int diameter;
unsigned int length;
unsigned int turns;
unsigned int speed;
unsigned int PWM_duty = 50;

bool busy_flag = false;

/* i.e. uint8_t <variable_name>; */
extern unsigned int overflow;
extern char recibi;
extern char palabra[20];
extern unsigned int n;
extern unsigned int ADC_value_press;
extern unsigned int ADC_value_dist;
extern unsigned int count_1;
extern unsigned int count_2;
extern unsigned int count_3;
extern unsigned int count_4;
extern char send[5];

/******************************************************************************/
/* Main Program                                                               */
/* Function prototypes */
//int ADC_get(char channel);
void trans_Char(char out);
void send_String(const char *out);

/******************************************************************************/

void main(void) {
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    InitApp();

    while (1) {
        MOT_1 = 1;
        MOT_2 = 1;
        MOT_4 = 1;

        CCPR1L = PWM_duty; // Update duty cycle for LED strip

        LATAbits.LA2 = DIR_1; // Set motor 1 direction
        LATAbits.LA4 = DIR_2; // Set motor 2 direction
        LATCbits.LC0 = DIR_3; // Set motor 3 direction
        LATBbits.LB7 = DIR_4; // Set motor 4 direction

        //        if (busy_flag) {
        //            TXREG = 'C'; // Transmit -Currently working- flag to mobile app
        //            while (TXIF == 0);
        //        }

        if (recibi == 1) {
            /* Bluetooth reception routine */
            recibi = 0; // Turn down reception flag

            if (palabra[0] == 'B') {
                // Its controlling LED intensity
                PWM_duty = ((palabra[1] - 48) * 10) + ((palabra[2] - 48));
            } else if (palabra[0] == 'A') {
                // Its trasmiting the parameters
                caliber = ((palabra[1] - 48) * 10) + ((palabra[2] - 48)); // 2 digits
                diameter = ((palabra[3] - 48) * 1000) + ((palabra[4] - 48) * 100) + ((palabra[5] - 48) * 10) + ((palabra[6] - 48)); // 4 digits (int not float)
                length = ((palabra[7] - 48) * 10000) + ((palabra[8] - 48) * 1000) + ((palabra[9] - 48) * 100) + ((palabra[10] - 48) * 10) + ((palabra[11] - 48)); // 5 digits (int not float)
                turns = ((palabra[12] - 48) * 1000) + ((palabra[13] - 48) * 100) + ((palabra[14] - 48) * 10) + ((palabra[15] - 48)); // 4 digits
                speed = palabra[15]; // 1 digit: 1: low; 2: medium, 3: high

                busy_flag = true; // Machine is currently working
            } else if ((palabra[0] == 'D')) { // && !busy_flag
                // Move cart manually to mark zero
                if (palabra[1] == '0') {
                    // Move cart right
                    MOT_3 = true;
                    DIR_3 = false;
                } else if (palabra[1] == '1') {
                    // Move cart left
                    MOT_3 = true;
                    DIR_3 = true;
                } else if (palabra[1] == '2') {
                    ; // Mark zero
                    MOT_3 = false;
                }

            } else if (palabra[0] == 'E') {
                // Move pressure motor
                if (palabra[1] == '0') {
                    MOT_4 = !MOT_4;
                } else if (palabra[1] == '1') {
                    DIR_4 = !DIR_4;
                }
            }


        } else if (GODONE == 0) {
            /* Restart ADC data gattering */
            __delay_ms(10); // Wait to next conversion
            ADCON0bits.CHS = !ADCON0bits.CHS; // Change channel
            GODONE = 1;

            send[0] = '0' + (ADC_value_press / 1000);
            send[1] = '0' + ((ADC_value_press % 1000) / 100);
            send[2] = '0' + (((ADC_value_press % 1000) % 100) / 10);
            send[3] = '0' + ((((ADC_value_press % 1000) % 100) / 10) % 10);
            send_String(send);
        }

    }
}

/* Function declarations */

void trans_Char(char out) {
    while (TXIF == 0);
    TXREG = out;
}

void send_String(const char *out) {
    while (*out != '\0') {
        trans_Char(*out);
        out++;
    }
}

//int ADC_get(char channel) {
//    if (channel !ADCON0bits.CHS) { //diferente
//        __delay_ms(10);
//        ADCON0bits.CHS = channel;
//    }
//    if (GODONE == 0) {
//        GODONE = 1;
//    }
//    return ADRES;
//}