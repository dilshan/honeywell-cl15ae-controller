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

#ifndef GLOBALDEF_H
#define	GLOBALDEF_H

#define BTN_POWER   0x01
#define BTN_SPEED   0x02
#define BTN_SWING   0x04
#define BTN_COOL    0x08
#define BTN_TIMER   0x10

struct sysConfig
{
    unsigned char speed;
    unsigned char timer;
    unsigned char flag;
};

typedef struct sysConfig sysConfig;

#define CONFIG_FLAG_POWER   0x01
#define CONFIG_FLAG_COOL    0x02
#define CONFIG_FLAG_SWING   0x04
#define CONFIG_FLAG_SLEEP   0x08

#define OUT_COOLER_MOTOR    0x01
#define OUT_OSC_MOTOR       0x02
#define OUT_LOW_MOTOR       0x04
#define OUT_MID_MOTOR       0x08
#define OUT_HIGH_MOTOR      0x10

#define BUTTON_IGNORE_LIMIT   10

enum MotorState
{
    MS_IDLE,
    MS_MAIN_MOTOR,
    MS_COOLER,
    MS_SWING
};

typedef enum MotorState MotorState;

#endif	/* GLOBALDEF_H */

