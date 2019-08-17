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
bool DIR_1 = 1; // Direction of motor 1
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
unsigned int length = 9250;
unsigned int turns = 4000;
unsigned int speed;
unsigned int PWM_duty = 50;

bool busy_flag = false;
bool zero_flag = true;
bool finish = false;

/* i.e. uint8_t <variable_name>; */
extern unsigned int overflow;
extern char recibi;
extern char palabra[20];
extern unsigned int n;
extern unsigned int ADC_value_press;
//extern unsigned int ADC_value_dist;
extern unsigned int count_1;
extern unsigned int count_2;
extern unsigned int count_3;
extern unsigned int count_4;
extern char send[7];
extern unsigned int mot_3_steps;
extern unsigned int mot_4_steps;
unsigned int mot_4_step_count;
bool enable = 0; // Motor 1 on/2 off by default

/******************************************************************************/
/* Main Program                                                               */
/* Function prototypes */
//int ADC_get(char channel);
void trans_Char(char out);
unsigned int mot_3_step_count(unsigned int l, unsigned int ms);
//unsigned long int real_turns(unsigned int turn);
void send_String(const char *out);
void zero_mark();
/******************************************************************************/

void main(void) {
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    InitApp();
    while (1) {

        CCPR1L = PWM_duty; // Update duty cycle for LED strip

        LATAbits.LA2 = DIR_1; // Set motor 1 direction
        LATAbits.LA4 = DIR_2; // Set motor 2 direction
        LATCbits.LC0 = DIR_3; // Set motor 3 direction
        LATBbits.LB7 = DIR_4; // Set motor 4 direction
        LATBbits.LB5 = enable;
        LATBbits.LB4 = !enable;

        //        if (busy_flag) {
        //            TXREG = 'C'; // Transmit -Currently working- flag to mobile app
        //            while (TXIF == 0);
        //        }


        if (recibi == 1) {
            /* Bluetooth reception routine */
            recibi = 0; // Turn down reception flag
            //            MOT_1 = 1;

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
            } else if (palabra[0] == 'C') {
                // Switch between Mot1/Mot2 turn on/off
                enable = !enable;
            } else if ((palabra[0] == 'D') && (zero_flag)) { // && !busy_flag
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
                    // Stop cart
                    MOT_3 = false;
                } else if (palabra[1] == '3') {
                    // Mark zero
                    if (MOT_3 == 0) {
                        // Only mark zero if the cart isn't moving
                        zero_mark();
                    }

                }

            /* FOR DEBUGGING ONLY !!!!!!! */
            } else if (palabra[0] == 'M') {
                // Turn on/off given motor
                if (palabra[1] == '1') {
                    MOT_1 = !MOT_1;
                } else if (palabra[1] == '2') {
                    MOT_2 = !MOT_2;
                } else if (palabra[1] == '3') {
                    MOT_3 = !MOT_3;
                }
            }


        } else if (GODONE == 0) {
            /* Restart ADC data gattering */
            __delay_ms(4); // Wait to next conversion
            //            ADCON0bits.CHS = !ADCON0bits.CHS; // Change channel
            GODONE = 1;
            //            send[0] = 'A';
            //            send[1] = '0' + (mot_3_steps / 1000);
            //            send[2] = '0' + ((mot_3_steps % 1000) / 100);
            //            send[3] = '0' + (((mot_3_steps % 1000) % 100) / 10);
            //            send[4] = '0' + ((((mot_3_steps % 1000) % 100) / 10) % 10);
            //            
            //            send[5] = '\n';
            //            send_String(send);
        }

        //        if (mot_3_steps == 10) { // mot_3_step_count(length, 1)
        //            // Reached end of given winding
        //            MOT_3 = 0;
        ////            DIR_3 = !DIR_3;
        //            mot_3_steps = 0;
        //        }

        //        if (mot_4_steps == mot_4_step_count && busy_flag) {
        //            // When reached numbers of steps, stop pressure motor
        //            MOT_4 = 0;
        //        }

        //        send[0] = '0' + ((mot_3_steps % 1000) / 100);
        //        send[1] = '0' + (((mot_3_steps % 1000) % 100) / 10);
        //        send[2] = '0' + ((((mot_3_steps % 1000) % 100) / 10) % 10);
        //        send[3] = '\n';
        //        send_String(send);

        


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

unsigned int mot_3_step_count(unsigned int l, unsigned int ms) {
    // lenght: 5 numbers, in mm, multiplied by 100, ex: 50,33 mm= 05033
    return (ms * l) / 2;
}

//unsigned long int real_turns(unsigned int turn) {
//    return (turn*7437)/1000;
//}

void zero_mark() {
    DIR_3 = 1; // Start to the right
    T1CONbits.TMR1ON = 0; // Disable timer1 (marking zero)
    T0CONbits.TMR0ON = 1; // Enable timer0 (winding control)
    zero_flag = false;
    MOT_1 = 1;
    MOT_3 = 1;
}
