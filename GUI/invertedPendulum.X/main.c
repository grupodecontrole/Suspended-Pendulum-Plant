/*
 * File:   main.c
 * Author: Julio Cesar Ferreira Lima
 * Project: Pendulum Inverted
 * Created on May 18, 2018, 1:20 AM
 */

// PIC16F877A Configuration Bit Settings
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)


#define _XTAL_FREQ 20000000

// Load the xc.h library, globally,  to set the frequency configurations.  
#include <xc.h>

// Load the usart_pic16.h library, locally,  to set and use the USART configurations.  
#include "usart_pic16.h"

// Load the stdlib.h library, globally,  to use the functions.  
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Declare prototypes.
void help(void);
void getData(void);
void choice(void);
void step(int duty, unsigned long int time);
void run(int duty);
void stop(void);
void set_pwm(int percent);
void status(void);
unsigned int sampling (void);
void acquire(void);
void adReadA0(void);
void main(void);
void sys(int duty);

// Define PWM duty-cycle.
int pwm=0;

// Define counters.
int count=0;
int max_counters=0;

// Define state of step;
char stepEnable = 0;

// Define state of acquirement;
char acquirement = 1;

// Declare the buffer;
uint8_t buffer = 0;
char strBuffer[25];

// Declare the content received;
char content[25];

void init(void){
    // Define bits IO.
    TRISA = 0b00000001;
    PORTA = 0b00000000;
    TRISB = 0b00000001;
    PORTB = 0b00000000;
    TRISC = 0b10000000;
    PORTC = 0b00000000;
    TRISD = 0b00000000;
    PORTD = 0b00000000;
    
    // A/D.
    ADCON0 = 0b10000001;    
    ADCON1 = 0b10001110;
    
    // Extern Interruption 
    OPTION_REG = 0b00000111;
    INTCON = 0b10100000;
    // Timer Zero
    TMR0 = 236;
    
    // Timer Two.
    CCPR1L = 0b00000000;
    CCP1CON = 0b00001100;    
    CCP2CON = 0b00001100;
    PR2 = 255;    
    T2CON = 0b00000111;
}

void interrupt ISR(void)
{
    if (RCIE && RCIF) {
        USARTHandleRxInt();
        return;
    }
    
    if(INTCONbits.TMR0IF){
        INTCONbits.TMR0IF = 0;
        count++;
        if(count>=max_counters && stepEnable){
            CCPR1L = 0b00000000;    
            CCPR2L = 0b00000000;   
            CCP1CONbits.CCP1Y = 0b00000000;
            CCP1CONbits.CCP1X = 0b00000000;
            CCP2CONbits.CCP2Y = 0b00000000;
            CCP2CONbits.CCP2X = 0b00000000;
            acquirement = 0;
            stepEnable = 0;
        }
    }
    
    if(INTCONbits.INTF){
        INTCONbits.INTF = 0;
        acquire();
    }
}

void main(void) {
    // Initialize USART with baud rate 9600.
    USARTInit(19200);
    // Initialize registers.
    init();
    // Defines of PWM duty-cycle.
    pwm=0;
    // Declare the buffer;
    buffer = 0;
    // Stop motor;
    stop();
    // Plot the head.
    help();
    USARTWriteString(">");
    // Infinity loop.
    while(1)
    {   
        // Receive data.
        getData();  
    }   
    return;
}

void help(void){
    //Write intro text
    USARTWriteLine("********************************************************************************************");
    USARTWriteLine("        Welcome to the Inverted Pendulum Plant!");
    USARTWriteLine("        To operate the system, type on options in below."); 
    USARTGotoNewLine();    
    USARTWriteLine("        Example 0:");
    USARTWriteLine("        If you want give a step with 100% of duty-cycle, until you type anything, type:");
    USARTWriteLine("        step()");
    USARTGotoNewLine();    
    USARTWriteLine("        Example 1:");
    USARTWriteLine("        If you want give a step with 100% of duty-cycle, for 200 milliseconds, type:");
    USARTWriteLine("        step(200)");
    USARTGotoNewLine();    
    USARTWriteLine("        Example 2:");
    USARTWriteLine("        If you want give a step with 50% of duty-cycle,for 300 milliseconds, type:");
    USARTWriteLine("        step(50,300)");
    USARTGotoNewLine();    
    USARTWriteLine("        Example 3:");
    USARTWriteLine("        If you want operate the system continually with 100% of duty-cycle, type:");
    USARTWriteLine("        run()     ");
    USARTGotoNewLine(); 
    USARTWriteLine("        Example 4:");
    USARTWriteLine("        If you want to identify the system parsing a RBS sinal, type:");
    USARTWriteLine("        sys(max_duty_cicle)");
    USARTGotoNewLine(); 
    USARTWriteLine("        Example 5:");
    USARTWriteLine("        If you want a help, type:");
    USARTWriteLine("        help()    ");
    USARTGotoNewLine(); 
    USARTWriteLine("        Example 6:");
    USARTWriteLine("        If you want stop the system, type:");
    USARTWriteLine("        stop()    ");
    USARTGotoNewLine(); 
    USARTWriteLine("        Example 7:");
    USARTWriteLine("        If you want acquire the data obtained by sensors of system, type:");
    USARTWriteLine("        acquire() ");
    USARTWriteLine("        Example 8:");
    USARTWriteLine("        If you want to know the status of system, type:");
    USARTWriteLine("        status()  ");
    USARTGotoNewLine(); 
    USARTWriteLine("        Example 9:");
    USARTWriteLine("        If you want a help, type:");
    USARTWriteLine("        help()    ");
    USARTWriteLine("*********************************************************************************************");
    USARTGotoNewLine();    
    USARTGotoNewLine();
}

void getData(void){
    //Get the amount of data waiting in USART queue.
    uint8_t n= USARTDataAvailable();
    //If we have some data.
    if(n!=0){
        char data = USARTReadData();
        //If enter pressed.
        if(data == '\r'){
            choice();
            //Reset variables.
            buffer = 0;
            for(int i=0 ; i<25;i++){
                content[i] = '\0';
            }
        }
        else{
        //Storage data.
        content[buffer] = data;
        //Plot received data.
        USARTWriteChar(content[buffer]);
        //Increment buffer.
        buffer++;
        }
    }                        
}

void choice(void){
    if(memcmp(content, "step(", 5) == 0){
        char duty[10];
        char time[10];
        int firstP = strcspn(content, "(");
        int secondP = strcspn(content, ",");
        int thirdP = strcspn(content, ")");
        for(int i = 0 ; i<strlen(content); i++){
          if(i < secondP - firstP - 1){
            duty[i] = content[firstP+i+1];
          }
          else if (i<strlen(duty)){
            duty[i] = '\0';
          }
          if(i < thirdP - secondP - 1){
            time[i] = content[secondP+i+1];
          }
          else if (i<strlen(time)){
            time[i] = '\0';
          }
        }
        int duty_i = atoi(duty);        
        int time_1 = atoi(time);
        if(*time == NULL){
            time_1 = duty_i;
            duty_i = 100;
        }
        step(duty_i,time_1);
    }
    else if(memcmp(content, "run(", 4) == 0){
        char duty[10];
        int firstP = strcspn(content, "(");
        int secondP = strcspn(content, ")");

        for(int i = 0 ; i<strlen(content); i++){
          if(i < secondP - firstP - 1){
            duty[i] = content[firstP+i+1];
          }
          else if (i<strlen(duty)){
            duty[i] = '\0';
          }
        }
        run(atoi(duty));
    }
    else if(memcmp(content, "sys(", 4) == 0){
        char duty[10];
        int firstP = strcspn(content, "(");
        int secondP = strcspn(content, ")");

        for(int i = 0 ; i<strlen(content); i++){
          if(i < secondP - firstP - 1){
            duty[i] = content[firstP+i+1];
          }
          else if (i<strlen(duty)){
            duty[i] = '\0';
          }
        }
        sys(atoi(duty));
    }
    else if(memcmp(content, "stop()", 6) == 0){
        stop();
    }
    else if(memcmp(content, "status()", 8) == 0){
        status();
    }
    else if(memcmp(content, "acquire()", 8) == 0){
        acquire();
    }
    else if(memcmp(content, "help()", 6) == 0){
        help();
    }
    else{
        USARTWriteLine("You typed something wrong. ");
    }
    USARTGotoNewLine();
    USARTWriteString(">");
}

void set_pwm(int percent){
    pwm = (percent*10.23);
    CCPR1L = pwm>>2;    
    CCPR2L = pwm>>2;   
    CCP1CONbits.CCP1Y = pwm & 0b00000001;
    CCP1CONbits.CCP1X = pwm & 0b00000010;
    CCP2CONbits.CCP2Y = pwm & 0b00000001;
    CCP2CONbits.CCP2X = pwm & 0b00000010;
}

void step(int duty, unsigned long int time){
    // Turn up the motor if the value was a percentage.
    if(duty>100){
        duty=0;
    }
    set_pwm(duty);
    
    sprintf(strBuffer,"%d",duty);
    USARTWriteLine(strBuffer);
    sprintf(strBuffer,"%d",time);
    USARTWriteLine(strBuffer);
    
    if(time == 0){
        // Wait for it.
        acquire();
        stop();
    }
    else{
        stepEnable = 1;
        count = 0;
        max_counters = time;
        // Wait for it.
        acquire();
        USARTWriteLine("If you want stop the system, type:");        
        USARTWriteLine("stop()");
    }
}


void run(int duty){
    // Turn up the motor if the value was a percentage.
    if(duty>100){
        duty=0;
    }
    else if(duty == 0){
        // Wait for it.
        duty = 100;
    }
    set_pwm(duty);
    // Wait for it.
    acquire();
    USARTWriteLine("If you want stop the system, type:");        
    USARTWriteLine("stop()");
}

void status(void){
    char output[25];
    for(int i=0 ; i<25;i++){
        output[i] = '\0';    
    }
    float aux = (float)(pwm)/1023; 
    int duty = (int) (aux*100);
    strcpy(output, "The duty-cycle of PWM is: ");
    sprintf(strBuffer,"%d",duty);
    strcat(output, strBuffer);    
    strcat(output, "%");    
    USARTWriteLine(output);
    USARTGotoNewLine();
    adReadA0();
}

void stop(void){
    set_pwm(0);
    stepEnable = 0;
        acquirement = 0;
}

unsigned int sampling (void){
    __delay_us(100);  
    ADCON0bits.GO_nDONE = 1;
    while(ADCON0bits.GO_nDONE){}
    return (int)(ADRESH << 8) + (ADRESL);
}

void acquire(void){
    USARTGotoNewLine();
    acquirement = 1;
    while(USARTDataAvailable() == 0 &&acquirement){
        unsigned int valueAD = sampling();
        if(valueAD<=1023 && valueAD >= 0){
            USARTWriteString("The analog value is: ");
            sprintf(strBuffer,"%d",valueAD);
            USARTWriteString(strBuffer);
            USARTGotoNewLine();
        }        
    }
}

void adReadA0(void){
    unsigned int valueAD = sampling();
    if(valueAD<=1023 && valueAD >= 0){
        USARTWriteString("The analog value is: ");
        sprintf(strBuffer,"%d",valueAD);
        USARTWriteString(strBuffer);
    }
}

void sys(int duty){
    acquirement = 1;
    unsigned int valueAD = sampling();       
    unsigned int max_duty = duty;
    while(USARTDataAvailable() == 0 && acquirement){
        srand((unsigned) valueAD*TMR0 );
        duty = rand()% max_duty;
        if(duty>max_duty){
            duty=max_duty;
        }
        set_pwm(duty);
        PR2 = rand()%255;
        USARTWriteString("Analog: ");
        sprintf(strBuffer,"%d",valueAD);
        USARTWriteString(strBuffer);
        USARTWriteString(", Duty: ");
        sprintf(strBuffer,"%d",duty);
        USARTWriteString(strBuffer);
        USARTWriteString(", PR2: ");
        sprintf(strBuffer,"%d",PR2);
        USARTWriteString(strBuffer);
        USARTGotoNewLine();
    }
    USARTWriteLine("If you want stop the system, type:");        
    USARTWriteLine("stop()");
}