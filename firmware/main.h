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

#ifndef MAIN_H
#define	MAIN_H

#include "globaldef.h"

#define _XTAL_FREQ 1000000

// CONFIG
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTRC oscillator; port I/O function on both RA6/OSC2/CLKO pin and RA7/OSC1/CLKI pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = ON       // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is MCLR)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB3/PGM pin has digital I/O function, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CCPMX = RB2      // CCP1 Pin Selection bit (CCP1 function on RB2)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)       

#include <xc.h>

__EEPROM_DATA(0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

sysConfig mainConfig;
volatile MotorState sysState;

unsigned char buttonIgnore;
unsigned short timerTarget;
volatile unsigned short timerVal;

void initSystem(void);
void mainService(void);
void shutdownSystem(void);
void sleepService(void);

void activateBuzzer(void);
void scanButtonState(unsigned char *buttonState);
void updateIndicators(const sysConfig *config);
void commitConfig(const MotorState execState);
void enableSleepTimer(unsigned char timeSlot);

void intrMoveToNextState(const MotorState execState);

#endif	/* MAIN_H */

