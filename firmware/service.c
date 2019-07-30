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

void mainService()
{
    unsigned char mainButtonState = 0x00;
    unsigned char lastButtonState = 0x00;
    
    commitConfig(MS_MAIN_MOTOR);
    
    while(!((mainConfig.flag) & CONFIG_FLAG_SLEEP))
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
        
        // Check for POWER button event.
        if(((mainButtonState & BTN_POWER) == 0x00) && ((lastButtonState & BTN_POWER) == BTN_POWER))
        {
            // Shutdown the system
            buttonIgnore = 1;
            mainConfig.flag = mainConfig.flag & (~(CONFIG_FLAG_POWER | CONFIG_FLAG_SLEEP));
            shutdownSystem();
            break;
        }
        
        // Check for SPEED button event.
        if(((mainButtonState & BTN_SPEED) == 0x00) && ((lastButtonState & BTN_SPEED) == BTN_SPEED))
        {
            mainConfig.speed = mainConfig.speed << 1;
            if(mainConfig.speed > 4)
            {
                mainConfig.speed = 1;
            }
            
            commitConfig(MS_MAIN_MOTOR);
            writeEEPROM(&mainConfig);
            buttonIgnore = 1;
        }
        
        // Check for TIMER button event.
        if(((mainButtonState & BTN_TIMER) == 0x00) && ((lastButtonState & BTN_TIMER) == BTN_TIMER))
        {
            if(mainConfig.timer == 0)
            {
                mainConfig.timer = 1;
            }
            else
            {
                mainConfig.timer = mainConfig.timer << 1;
                if(mainConfig.timer > 8)
                {
                    mainConfig.timer = 0;
                }
            }
            
            enableSleepTimer(mainConfig.timer);
            buttonIgnore = 1;
        }
        
        // Check for COOL button event.
        if(((mainButtonState & BTN_COOL) == 0x00) && ((lastButtonState & BTN_COOL) == BTN_COOL))
        {
            if((mainConfig.flag & CONFIG_FLAG_COOL) == CONFIG_FLAG_COOL)
            {
                mainConfig.flag = mainConfig.flag & (~CONFIG_FLAG_COOL);
            }
            else
            {
                mainConfig.flag = mainConfig.flag | CONFIG_FLAG_COOL;
            }
            
            commitConfig(MS_COOLER);
            writeEEPROM(&mainConfig);
            buttonIgnore = 1;
        }
        
        // Check for SWING button event.
        if(((mainButtonState & BTN_SWING) == 0x00) && ((lastButtonState & BTN_SWING) == BTN_SWING))
        {
            if((mainConfig.flag & CONFIG_FLAG_SWING) == CONFIG_FLAG_SWING)
            {
                mainConfig.flag = mainConfig.flag & (~CONFIG_FLAG_SWING);
            }
            else
            {
                mainConfig.flag = mainConfig.flag | CONFIG_FLAG_SWING;
            }
            
            commitConfig(MS_SWING);
            writeEEPROM(&mainConfig);
            buttonIgnore = 1;
        }
        
        // Update front panel indicators with current system configuration.
        updateIndicators(&mainConfig);
        lastButtonState = mainButtonState;
        __delay_ms(5);
    }
    
    if((mainConfig.flag) & CONFIG_FLAG_SLEEP)
    {
        PORTB = 0x60;
        TRISB = 0x00;
        
        sleepService();
    }
}

void sleepService()
{
    unsigned char mainButtonState = 0x00;
    unsigned char lastButtonState = 0x00;
    
    buttonIgnore = 0;
    
    while(1)
    {
        scanButtonState(&mainButtonState);
        
        // Check for POWER button event.
        if(((mainButtonState & BTN_POWER) == 0x00) && ((lastButtonState & BTN_POWER) == BTN_POWER))
        {
            mainConfig.flag = mainConfig.flag & (~(CONFIG_FLAG_POWER | CONFIG_FLAG_SLEEP));
            shutdownSystem();
            buttonIgnore = 1;
            break;
        }
        
        TRISB = 0x00;
        PORTB = PORTB & 0x80;
        PORTB = PORTB | 0x48;
        __delay_ms(1);
        PORTB = PORTB & 0x80;
        
        lastButtonState = mainButtonState;
        __delay_ms(5);
    }
}

void __interrupt() mainISR()
{
    if((INTCONbits.TMR0IE) && (INTCONbits.TMR0IF)) 
    {
        if((mainConfig.flag & CONFIG_FLAG_POWER) == 0x00)
        {
            // Shutdown all the motors in the system.
            PORTA = 0x00;
            sysState = MS_IDLE;
            INTCONbits.TMR0IE = 0;
        }
        else
        {
            // Handle main motor (speed) state.
            if(sysState == MS_MAIN_MOTOR)
            {
                PORTA = (PORTA & 0xE3) | (mainConfig.speed << 2);
            }
            
            // Handle cooler pump state.
            if(sysState == MS_COOLER)
            {
                PORTA = PORTA & 0xFE;
                if(mainConfig.flag & CONFIG_FLAG_COOL)
                {
                    PORTA = PORTA | 0x01;
                }
            }
            
            // Handle swing motor state.
            if(sysState == MS_SWING)
            {
                PORTA = PORTA & 0xFD;
                if(mainConfig.flag & CONFIG_FLAG_SWING)
                {
                    PORTA = PORTA | 0x02;
                }
            }
            
            // Move to next available state.
            intrMoveToNextState(sysState);
        }
    }
    else if((PIE1bits.TMR1IE) && (PIR1bits.TMR1IF))
    {
        timerVal++;
        
        if(timerVal >= timerTarget)
        {
            mainConfig.flag = mainConfig.flag | CONFIG_FLAG_SLEEP;
            PORTA = 0x00;
            T1CON = 0x00;
            PIE1bits.TMR1IE = 0;
        }
        else
        {
            TMR1H = 0x0B;
            TMR1L = 0xDC;
            PIE1bits.TMR1IE = 1;
        }
        
        PIR1bits.TMR1IF = 0;
    }
}

void intrMoveToNextState(const MotorState execState)
{
    switch(execState)
    {
        case MS_IDLE:
            INTCONbits.TMR0IE = 0;
            INTCONbits.TMR0IF = 0;
            return;
        case MS_MAIN_MOTOR:
            sysState = MS_COOLER;
            break;
        case MS_COOLER:
            sysState = MS_SWING;
            break;
        case MS_SWING:
            sysState = MS_IDLE;
            break;
    }
    
    // Switch system to specified state in 50ms.
    TMR0 = 157;
    INTCONbits.TMR0IE = 1;
    INTCONbits.TMR0IF = 0;
}