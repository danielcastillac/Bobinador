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
bool MOT_4 = 1; // Move motor 4 flag
/* Parameters */
unsigned int caliber;
unsigned int length = 9250;
unsigned int turns = 4000;
unsigned int PWM_duty = 50;

bool param_flag = false; // If it has recieved parameters
bool zero_flag = true; // If it is in marking zero mode (default mode)
bool finish_flag = false; // If machine has finished winding
bool enable = 0; // Motor 1 on/ 2 off by default
bool winding = 0; // Currently windiing flag
bool move_4 = 0; // Move motor 4 flag

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
extern unsigned int count_1;
extern unsigned int count_3;
unsigned int mot_4_step_count;
unsigned int steps_4 = 0;

/******************************************************************************/
/* Function prototypes */
//int ADC_get(char channel);
void trans_Char(char out);
unsigned int mot_3_step_count(unsigned int l, unsigned int ms);
//unsigned long int real_turns(unsigned int turn);
void send_String(const char *out);
void zero_mark();
void unwind();
void finish();
void reset();
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
        LATBbits.LB5 = enable; // Enable mot_1 by default
        LATBbits.LB4 = !enable; // Disable mot_2 by default

        if (recibi == 1) {
            /* Bluetooth reception routine */
            recibi = 0; // Turn down reception flag

            if (palabra[0] == 'A') {
                // Its trasmiting the parameters
                caliber = ((palabra[1] - 48) * 10) + ((palabra[2] - 48)); // 2 digits
                length = ((palabra[3] - 48) * 10000) + ((palabra[4] - 48) * 1000) + ((palabra[5] - 48) * 100) + ((palabra[6] - 48) * 10) + ((palabra[7] - 48)); // 5 digits (int not float)
                turns = ((palabra[8] - 48) * 1000) + ((palabra[9] - 48) * 100) + ((palabra[10] - 48) * 10) + ((palabra[11] - 48)); // 4 digits

                param_flag = true; // Machine has parameters
            } else if (palabra[0] == 'B') {
                // Its controlling LED intensity
                PWM_duty = ((palabra[1] - 48) * 10) + ((palabra[2] - 48));
            }  else if ((palabra[0] == 'D') && (zero_flag)) { // && !param_flag
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
                    if ((MOT_3 == 0) && (param_flag)) {
                        // Only mark zero if the cart isn't moving and it has recieved parameters
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
            } else if (palabra[0] == 'N') {
                // Switch between Mot1/Mot2 turn on/off
                enable = !enable;
            } else if (palabra[0] == 'W') {
                // Unwind routine if necessary
                unwind();
            } else if (palabra[0] == 'R') {
                reset();
            } else if (palabra[0] == 'E') {
                if (palabra[1] == 'D') {
                    // Move up N steps
                    DIR_4 = true;
                    steps_4 = ((palabra[2] - 48) * 10) + ((palabra[3] - 48));
                    trans_Char(palabra[3]);
                    move_4 = true;
                } else if (palabra[1] == 'I') {
                    // Move down N steps
                    DIR_4 = false;
                    steps_4 = ((palabra[2] - 48) * 10) + ((palabra[3] - 48));
                    trans_Char(palabra[3]);
                    move_4 = true;
                }
                
            }

        } else if (GODONE == 0) {
            /* Restart ADC data gattering */
            __delay_ms(4); // Wait to next conversion
            //            ADCON0bits.CHS = !ADCON0bits.CHS; // Change channel
            GODONE = 1;
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

unsigned int mot_3_step_count(unsigned int l, unsigned int ms) {
    // lenght: 5 numbers, in mm, multiplied by 100, ex: 50,33 mm= 05033
    return (ms * l) / 2;
}

void zero_mark() {
    // Post-zero marked routine
    DIR_3 = 1; // Start to the right
    T1CONbits.TMR1ON = 0; // Disable timer1 (marking zero)
    T0CONbits.TMR0ON = 1; // Enable timer0 (winding control)
    zero_flag = false; // Exit zero marking mode
    winding = true; // Enter winding mode
    MOT_1 = true;
    MOT_3 = true;
}

//unsigned long int real_turns(unsigned int turn) {
//    return (turn*7437)/1000;
//}

void unwind() {
    // Unwind current winding
    enable = !enable; // Disable Motor 1, Enable Motor 2
    T1CONbits.TMR1ON = 1; // Enable timer1 (marking zero)
    T0CONbits.TMR0ON = 0; // Disable timer0 (winding control)
    MOT_3 = 0;
    MOT_2 = 1;
}

void finish() {
    // When finished winding, routine
    T1CONbits.TMR1ON = 1; // Disable timer1 (marking zero)
    T0CONbits.TMR0ON = 0; // Enable timer0 (winding control)
    MOT_1 = MOT_3 = 0; // Shutdown motors
    winding = false; // Exit winding mode
    finish_flag = true;
}

void reset() {
    #asm 
    reset
    #endasm
}