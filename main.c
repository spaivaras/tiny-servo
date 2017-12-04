#include <avr/io.h>
#include <util/delay.h> 
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#define MIN_SPEED 100                //1ms frame
#define MAX_SPEED 200                //2ms frame
#define EE_ADDRESS (uint8_t *)46     //Why not?
#define INC PB0
#define DEC PB4
#define SAVE_DELAY 150               //3 seconds

volatile uint16_t Tick = 0;	         // 100KHz pulse
volatile uint8_t sPulse = MIN_SPEED; // Servo pulse variable
volatile uint8_t highTick = 0;
volatile uint8_t lowTick = 0;
volatile uint8_t saveTick = 0;

uint8_t highState = (1<<PB0);
uint8_t lowState = (1<<PB4);

uint8_t highProcessed = 0;
uint8_t lowProcessed = 0;
uint8_t riseTo = MIN_SPEED;
uint8_t needSave = 0;

// _delay_ms() has limited amount
void delay_10_ms(uint16_t count)
{
	for (uint16_t i = 0; i < count; i++) {
		_delay_ms(10);
	}	
}

int main (void) 
{
	sei();

	// Manage pins
	DDRB |= (1<<PB3); // PB3 as output
	PORTB |= (1<<INC) | (1<<DEC); // Pullup on DEC and INC pins

	// Config time and space continium
	TCCR0A |= (1<<WGM01); // CTC mode
	TIMSK0 |= (1<<OCIE0A); // CTC interrupt
	OCR0A = 95;
	TCCR0B |= (1<<CS00); // Max speed

	if (!(PINB & (1<<INC))) {
		// While INC button held on boot max throtle - for calibration
		sPulse = MAX_SPEED;
		while (!(PINB & (1<<INC))) {}
		sPulse = MIN_SPEED;
	} else if (!(PINB & (1<<DEC))) {
		// If DEC button held on boot reset stored value to min
		eeprom_write_byte(EE_ADDRESS, riseTo);
	}

	// From SEI enabled delay seems to be 8 times longer because of interupts
	delay_10_ms(200);

	// Read and validate stored speed value
	riseTo = eeprom_read_byte(EE_ADDRESS);
	if (riseTo < MIN_SPEED || riseTo > MAX_SPEED) {
		riseTo = MIN_SPEED;
		eeprom_write_byte(EE_ADDRESS, riseTo);
	}

	// Slow increment of speed to stored/default value
	while (sPulse < riseTo) {
		sPulse += 1;
		_delay_ms(40);
	}

	// Main loop
	while (1) {
		// INC button debounce
		if ((PINB & (1<<INC)) != highState) {
			highState = PINB & (1<<INC);
			highTick = 0;
			if (highState > 0) {
				highProcessed = 0;
			}
		}
		// DEC button debounce
		if ((PINB & (1<<DEC)) != lowState) {
			lowState = PINB & (1<<DEC);
			lowTick = 0;
			if (lowState > 0) {
				lowProcessed = 0;
			}
		}

		// Increase/Decrease speed on button push (debounced)
		if (highState == 0 && highProcessed == 0 && highTick >= 5) {
			highProcessed = 1;
			sPulse += 2;
			needSave = 1;
			saveTick = 0;
		} else if (lowState == 0 && lowProcessed == 0 && lowTick >= 5) {
			lowProcessed = 1;
			sPulse -= 2;
			needSave = 1;
			saveTick = 0;
		}

		// Store pending value to eeprom, throtle writes to save eeprom cycles
		if (needSave && saveTick >= SAVE_DELAY) {
			needSave = 0;
			eeprom_write_byte(EE_ADDRESS, sPulse);
		}
	};
}

// Tick interrupt
ISR(TIM0_COMPA_vect)
{
	// Full 20ms frame manage ticks, reset counter
	if(Tick >= 2000)
	   {
	      Tick = 0;
		  highTick++;
		  lowTick++;
		  saveTick++;
	   }

	// Manage high and low states of single frame
	Tick = Tick + 1;
	if(Tick <= sPulse) {
		PORTB |= (1<<PB3);
	} else {
		PORTB &= ~(1<<PB3);
	}
}
