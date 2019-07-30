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

#include <xc.h>

#include "memory.h"

void writeEEPROM(const sysConfig *config)
{
    unsigned char configFlag = config->flag & (~(CONFIG_FLAG_SLEEP | CONFIG_FLAG_POWER));
    
    // Write changes in main motor speed.
    if(eeprom_read(0) != config->speed)
    {
        eeprom_write(0, config->speed);
    }
    
    // Write changes in flags.
    if(eeprom_read(1) != configFlag)
    {
        eeprom_write(1, configFlag);
    }
}

void readEEPROM(sysConfig *config)
{
    config->speed = eeprom_read(0);
    config->flag = eeprom_read(1);
    config->timer = 0;
    
    // Ensure speed is in valid limits [1 to 4].
    if((config->speed == 0) || (config->speed > 4))
    {
        config->speed = 1;
    }
}