// Why U still divide???
#define F_CPU 1200000UL

#include <avr/io.h>
#include <util/delay.h> 
#include <avr/interrupt.h>

volatile unsigned int Tick;	    // 100KHz pulse
volatile unsigned int sPulse;	// Servo pulse variable
volatile unsigned int Tick20ms;	// Servo frame variable


int main (void) 
{
	sei()

	// Manage pins
	DDRB |= (1<<PB3); // PB3 as output
	PORTB |= (1<<PB4) | (1<<PB0); // Pullup on PB4

	// Config time and space continium
	TCCR0A |= (1<<WGM01); // CTC mode
	TIMSK0 |= (1<<OCIE0A); // CTC interrupt
	OCR0A = 95;
	TCCR0B |= (1<<CS00); // Max speed
	
	// Temp vars
	Tick = 0;
	sPulse = 90;
	Tick_20ms = 0;

	// Main loop
	while (1) {
		if (PINB & (1<<PB0)) {
			if (PINB & (1<<PB4)) {
				sPulse = 190;
			} else {
				sPulse = 90;
			}
		} else {
			sPulse = 140;
		}
	};
}

// Tick interrupt
ISR(TIM0_COMPA_vect)
{
	if(Tick >= 2000) //20ms
	   {
	      Tick = 0;
		  Tick20ms = Tick20ms + 1;
	   }

	Tick = Tick + 1;

	if(Tick <= sPulse) {
		PORTB |= (1<<PB3);
	} else {
		PORTB &= ~(1<<PB3);
	}
}

