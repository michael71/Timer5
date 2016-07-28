/*
  Timer5 library for Arduino Zero and MKR1000 
  (only for SAMD arch.)
  Copyright (c) 2016 Michael Blank, OpenSX. All right reserved.

  based on the code of the AudioZero library by:
  Arturo Guadalupi <a.guadalupi@arduino.cc>
  Angelo Scialabba <a.scialabba@arduino.cc>
  Claudio Indellicati <c.indellicati@arduino.cc> <bitron.it@gmail.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include "Timer5.h"

/*Global variables*/
voidFuncPtr Timer_callBack = NULL;

void Timer5Class::begin(uint16_t value) {
	
	/*Modules configuration */

	tcConfigure(value);
}

void Timer5Class::end() {
	tcDisable();
	tcReset();

}

/*void AudioZeroClass::prepare(int volume){
//Not Implemented yet
}*/

void Timer5Class::start(void) {
  
      tcStartCounter();
}



/**
 * Configures the TC to generate events at the sample frequency.
 *
 * Configures the TC in Frequency Generation mode, with an event output once
 * each time the audio sample frequency period expires.
 */
 void Timer5Class::tcConfigure(uint16_t rate)  
{
	// Enable GCLK for TCC2 and TC5 (timer counter input clock)
	GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK1 | GCLK_CLKCTRL_ID(GCM_TC4_TC5)) ;
	//------------                                                  (GCLK1=32kHz)  was   0 = 48Mhz
	while (GCLK->STATUS.bit.SYNCBUSY);

	tcReset();

	// Set Timer counter Mode to 16 bits
	TC5->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16;

	// Set TC5 mode as match frequency
	TC5->COUNT16.CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;

  // prescaler DIV1 (=no prescaler div)
	TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1 | TC_CTRLA_ENABLE;

	TC5->COUNT16.CC[0].reg = (uint16_t) (32768/rate -1); //value; // (SystemCoreClock / sampleRate - 1);
	while (tcIsSyncing());
	
	// Configure interrupt request
	NVIC_DisableIRQ(TC5_IRQn);
	NVIC_ClearPendingIRQ(TC5_IRQn);
	NVIC_SetPriority(TC5_IRQn, 0);
	NVIC_EnableIRQ(TC5_IRQn);

	// Enable the TC5 interrupt request
	TC5->COUNT16.INTENSET.bit.MC0 = 1;
	while (tcIsSyncing());
}	


bool Timer5Class::tcIsSyncing()
{
  return TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY;
}

void Timer5Class::tcStartCounter()
{
  // Enable TC

  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
  while (tcIsSyncing());
}

void Timer5Class::tcReset()
{
  // Reset TCx
  TC5->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
  while (tcIsSyncing());
  while (TC5->COUNT16.CTRLA.bit.SWRST);
}

void Timer5Class::tcDisable()
{
  // Disable TC5
  TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
  while (tcIsSyncing());
}

void Timer5Class::attachInterrupt(voidFuncPtr callback)
{
  Timer_callBack = callback;
}

Timer5Class MyTimer5;

#ifdef __cplusplus
extern "C" {
#endif

void IRQ_HandlerTimer5 (void)
{

    // Clear the interrupt
    TC5->COUNT16.INTFLAG.bit.MC0 = 1;

    if (Timer_callBack != NULL) {
      Timer_callBack();
    }  
  
}

void TC5_Handler (void) __attribute__ ((weak, alias("IRQ_HandlerTimer5")));

#ifdef __cplusplus
}
#endif
