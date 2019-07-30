//*******************************************************************************
//* Copyright (c) 2019 Dilshan R Jayakody.
//*
//* Permission is hereby granted, free of charge, to any person obtaining 
//* a copy of this software and associated documentation files (the "Software"), 
//* to deal in the Software without restriction, including without limitation 
//* the rights to use, copy, modify, merge, publish, distribute, sublicense, 
//* and/or sell copies of the Software, and to permit persons to whom the 
//* Software is furnished to do so, subject to the following conditions:
//*
//* The above copyright notice and this permission notice shall be included in 
//* all copies or substantial portions of the Software.
//*
//* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
//* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
//* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
//* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
//* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
//* THE SOFTWARE.
//*******************************************************************************

#include "main.h"
#include "memory.h"

int main() 
{
    unsigned char mainButtonState = 0x00;
    unsigned char lastButtonState = 0x00;
    
    // Initialize global variables.
    mainConfig.flag = 0;
    mainConfig.speed = 1;
    mainConfig.timer = 0;
    
    timerVal = 0;
    timerTarget = 0;
    buttonIgnore = 0;
    
    sysState = MS_IDLE;
    
    // Start Initialize the system.
    initSystem();
    readEEPROM(&mainConfig);
    activateBuzzer();
    
    // Idle service loop.
    while(1)
    {
        if(buttonIgnore == 0)
        {
            scanButtonState(&mainButtonState);
        }
        else
        {
            // Ignore switch bouncing.
            if((++buttonIgnore) == BUTTON_IGNORE_LIMIT)
            {
                buttonIgnore = 0;
            }
        }
        
        if(((mainButtonState & BTN_POWER) == 0x00) && ((lastButtonState & BTN_POWER) == BTN_POWER))
        {
            // Turn on the system.
            buttonIgnore = 1;
            activateBuzzer();
            mainConfig.flag = mainConfig.flag | CONFIG_FLAG_POWER;
            
            __delay_ms(10);
            mainService();
            
            readEEPROM(&mainConfig);
            mainButtonState = 0x00;
        }
        
        lastButtonState = mainButtonState;
        __delay_ms(10);
    }
    
    return 0;
}

void updateIndicators(const sysConfig *config)
{
    TRISB = 0x00;
    
    // Update speed indicators.
    PORTB = PORTB & 0x80;
    PORTB = PORTB | (config->speed) | 0x40;
    __delay_ms(1);
    
    // Update timer indicators.
    PORTB = PORTB & 0x80;
    PORTB = PORTB | (config->timer) | 0x20;
    __delay_ms(1);
    
    // Update cool indicator.
    PORTB = PORTB & 0x80;
    if((config->flag) & CONFIG_FLAG_COOL)
    {
        PORTB = PORTB | 0x50;
    }
    __delay_ms(1);

    // Update swing indicator.
    PORTB = PORTB & 0x80;
    if((config->flag) & CONFIG_FLAG_SWING)
    {
        PORTB = PORTB | 0x30;
    }
    __delay_ms(1);
    
    PORTB = PORTB & 0x80;
}

void shutdownSystem()
{    
    // Shutdown indicators.
    PORTB = 0x60;
    TRISB = 0x00;
    
    // Shutdown all motors in the system.
    PORTA = 0x00;
}

void scanButtonState(unsigned char *buttonState)
{
    unsigned char tempButtonState = 0x00;
    
    PORTB = 0x60;
    TRISB = 0x1F;
    
    // Check state of the POWER button.
    if(!(PORTBbits.RB0))
    {
        tempButtonState = tempButtonState | BTN_POWER;
    }
    
    // Check state of the SPEED button.
    if(!(PORTBbits.RB1))
    {
        tempButtonState = tempButtonState | BTN_SPEED;
    }
    
    // Check state of the SWING button.
    if(!(PORTBbits.RB2))
    {
        tempButtonState = tempButtonState | BTN_SWING;
    }
    
    // Check state of the COOL button.
    if(!(PORTBbits.RB3))
    {
        tempButtonState = tempButtonState | BTN_COOL;
    }
    
    // Check state of the TIMER button.
    if(!(PORTBbits.RB4))
    {
        tempButtonState = tempButtonState | BTN_TIMER;
    }
    
    PORTB = 0x60;
    TRISB = 0x00;
    
    *buttonState = tempButtonState;
}

void activateBuzzer()
{
    unsigned char bellCounter = 254;
    TRISB = TRISB & 0x7F;
    
    while((--bellCounter) > 0)
    {
        // Generate 2kHz waveform on PORTB7.
        PORTBbits.RB7 = (bellCounter % 2);
        __delay_us(200);
    }  
    
    // Reset PORTB7 and shutdown buzzer.
    PORTBbits.RB7 = 0;
}

void commitConfig(const MotorState execState)
{
    sysState = execState;
    
    // Switch system to specified state in 100ms.
    TMR0 = 60;
    INTCONbits.TMR0IE = 1;
    INTCONbits.TMR0IF = 0;
}

void enableSleepTimer(unsigned char timeSlot)
{
    if(timeSlot > 0)
    {
        // Enable sleep timer with 2sec interval.
        timerTarget = 900 * timeSlot;
        timerVal = 0;

        PIR1bits.TMR1IF = 0;
        PIE1bits.TMR1IE = 1;

        T1CON = 0x31;
        TMR1H = 0x0B;
        TMR1L = 0xDC;
    }
    else
    {
        // Disable sleep timer and clear flags.
        T1CON = 0x00;
        PIR1bits.TMR1IF = 0;
        PIE1bits.TMR1IE = 0;
        
        timerVal = 0;
        timerTarget = 0;
        
        mainConfig.flag = mainConfig.flag & (~CONFIG_FLAG_SLEEP);
    }
}

void initSystem()
{
    // Setup MCU clock to 1MHz.
    OSCCON = 0x40;
    
    // Configure MCU peripherals and system registers.
    ADCON0 = 0x00;
    ADCON1 = 0x06;
    CCP1CON = 0x00;
    SSPCON = 0x05;
    OPTION_REG = 0x46;
    INTCON = 0xC0;
    T1CON = 0x00;
    
    // Set I/O port values to default state.
    TRISB = 0x00;
    PORTB = 0x60;
    
    TRISA = 0x00;
    PORTA = 0x00;
}
