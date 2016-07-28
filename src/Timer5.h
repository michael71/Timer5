/*
 * Copyright (c) 2016
   Michael Blank
 
 * library to allow for Timer5 Interrupts 
 *  for SAMD processors, Arduino Zero and Arduino MKR 1000
 *
 *  based on the AudioZero library
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */
	
#ifndef TIMER5_H
#define TIMER5_H

#include "Arduino.h"

typedef void(*voidFuncPtr)(void);

class Timer5Class{
public:

	Timer5Class() {};
	void begin(uint16_t value);
	void start(void) ;
	void end();
    void attachInterrupt(voidFuncPtr);

private:
	void tcConfigure(uint16_t value);
	bool tcIsSyncing(void);
	void tcStartCounter(void);
	void tcReset(void);
	void tcEnable(void);
	void tcDisable(void);
  
};

extern Timer5Class MyTimer5;
#endif
