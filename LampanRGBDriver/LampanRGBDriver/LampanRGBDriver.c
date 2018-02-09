#include <avr/io.h>
#include <util/delay.h>


// Push button, any free digital pin will do
// check the readButton() method as I hard-coded PB0 there as well...
#define button PINB0

// port to drive transistor for warm white LEDs, I liked this better than
// using the RGB leds....it's gotta be a functional desk light after all!
#define WHITELEDS PINB1

// Global variables
uint8_t LEDoptions = 12;
uint8_t LEDmode = 0;
uint8_t redVal,greenVal,blueVal;
uint16_t i;
uint8_t fastFadeMode = 0;

// standard RGB colors
uint8_t white[] = {255,255,255};
uint8_t red[] = {255,0,0};
uint8_t green[] = {0,255.0};
uint8_t blue[] = {0,0,255};
uint8_t orange[] = {140,30,0};
uint8_t yellow[] = {255,255,0};
uint8_t pink[] = {240,142,203};
uint8_t purple[] = {139,0,139};
uint8_t black[] = {0,0,0};
uint8_t aqua[] = {51,204,204};

void setColor(uint8_t color[]){
	// green = OCR0A
	// blue = OCR1A
	// red = OCR1B
	OCR1A = color[0];
	OCR0A = color[1];
	OCR1B = color[2];
}

void readButton(){

	if (bit_is_clear(PINB, PB0))
	{
	        _delay_ms(240);
	        if (bit_is_clear(PINB, PB0)){
			 LEDmode++;
			 	if(LEDmode > LEDoptions-1) LEDmode = 0;
			}
	}
}

uint8_t maxValue(uint8_t value1, uint8_t value2){

	if(value1 > value2) return value1;
	else return value2;
}

void randomColors(){

  i += 1;      // Increment counter
  if (i < 255) // First phase of fades
  {
    redVal   -= 1; // Red down
    greenVal += 1; // Green up
    blueVal   = 1; // Blue low
  }
  else if (i < 509) // Second phase of fades
  {
    redVal    = 1; // Red low
    greenVal -= 1; // Green down
    blueVal  += 1; // Blue up
  } 
  else if (i < 763) // Third phase of fades
  {
    redVal  += 1; // Red up
    greenVal = 1; // Green low
    blueVal -= 1; // Blue down
  }
  else // Re-set the counter, and start the fades again
  {
    i = 1;
  }  
  
  uint8_t tmpColor[] = {redVal,greenVal,blueVal};
  setColor(tmpColor);
  
  if(fastFadeMode == 1) _delay_us(350);	// fade really fast, looks like a light-switch rave. woot!
  if(fastFadeMode == 0) _delay_ms(65);	// fade slowly if in slow mode

}

int main(){

	DDRB |= (1<<WHITELEDS) | (1<<PINB2) | (1<<PINB3) | (1<<PINB4) | (0<<button); // PortB pins for PWM output, PB1 for white leds

	// configure timer values and PWM for timer 0 8-bit non-inverted fast PWM mode
	// we'll only use one channel on this and two on Timer1 to get our 3 needed PWM channels

	// shut down any active PWM while setting up
	TCCR0A = 0;
	TCCR0B = 0;

	// configure PWM mode 3, Fast PWM
	TCCR0A |= (1<<WGM00) | (1<<WGM01);

	// set prescaler for timer0 to clk(i/o)/ 256
	TCCR0B |= (1<<CS02);
	
	TCCR0A |= (0<<COM0A0) | (1<<COM0A1);

	// NOW configure the timer1 settings with same values as above
	
	// shut down any active PWM while setting up
	TCCR1A = 0;
	TCCR1B = 0;
	
	// configure PWM mode 5, Fast PWM, 8-bit PWM
	TCCR1A |= (1<<WGM10);
	TCCR1B |= (1<<WGM12);

	// set prescaler for timer1 to clk(i/o)/256
	TCCR1B |= (1<<CS12);

	// compare modes for channel A and B
	TCCR1A |= (0<<COM1A0) | (1<<COM1A1) | (0<<COM1B0) | (1<<COM1B1);

	// loop forever
	while(1){
		
		switch(LEDmode){

		case 0:
			setColor(black);	// turn off RGB LEDs
			PORTB |= (1<<WHITELEDS);// turn on white LEDs
		break;

		case 1: 
			PORTB &= ~(1<<WHITELEDS); // turn off white LEDs
			setColor(red);	// set color to red
		break;

		case 2: 
			PORTB &= ~(1<<WHITELEDS); // turn off white LEDs
			setColor(green);	// set color to green
		break;
				
		case 3: 
			PORTB &= ~(1<<WHITELEDS); // turn off white LEDs
			setColor(blue);	// set color to blue
		break;

		case 4: 
			PORTB &= ~(1<<WHITELEDS); // turn off white LEDs
			setColor(orange);	// set color to orange
		break;

		case 5: 
			PORTB &= ~(1<<WHITELEDS); // turn off white LEDs	 
			setColor(yellow);	// set color to yellow
		break;

		case 6: 
			PORTB &= ~(1<<WHITELEDS);
			setColor(pink);	// set color to pink
		break;

		case 7: 
			PORTB &= ~(1<<WHITELEDS); // turn off white LEDs
			setColor(purple);	// set color to purple
		break;

		case 8: 
			PORTB &= ~(1<<WHITELEDS); // turn off white LEDs
			setColor(aqua);		// set color to aqua
		break;

		case 9: 
			PORTB &= ~(1<<WHITELEDS); // turn off white LEDs
			setColor(black);			// set color to black (off)
		break;

		case 10: 
			PORTB &= ~(1<<WHITELEDS); // turn off white LEDs
			fastFadeMode = 1;
			randomColors();	// cycle through random colors (fast mode)
		break;

		case 11: 
			PORTB &= ~(1<<WHITELEDS); // turn off white LEDs			
			fastFadeMode = 0;
			randomColors();	// cycle through random colors
		break;

		default: 
			PORTB |= (1<<WHITELEDS); // turn on white LEDs			
			setColor(black);	// turn off RGB LEDs and turn on white if we hit an unknown LEDmode value
		}

		readButton();
	}
}
