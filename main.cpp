#include <avr/interrupt.h>

#include <iopins.h>
#include <util/delay.h>

#include "Clock.h"

template <class Pin, int delay = 300>
class Switch {
public:
	Switch() : on(false), shortTime(0) {
		Pin::SetDirRead();
		Pin::Set(); // pull up
	}
	bool isOn() {
		if (Pin::IsSet()) {
			shortTime = 0;
			on = false;
			return false;
		}
		// pin is shorted to zero
		if (on)
			return true;
		Clock::clock_t now = Clock::millis();
		if (shortTime == 0) {
			shortTime = now;
			return false;
		}
		if (Clock::delta(now, shortTime) > delay) {
			on = true;
			return true;
		}

		return false;
	}
private:
	bool on;
	Clock::clock_t shortTime;
};

typedef IO::Pb5 Fan;
typedef IO::Pb4 Heater;
typedef IO::Pb3 Led;

typedef Switch<IO::Pb2> Button;

const Clock::clock_t COOL_DELAY=60000; // 1 minute

int main(void)
{
	Fan::SetDirWrite();
	Fan::Clear();
	Heater::SetDirWrite();
	Heater::Clear();
	Led::SetDirWrite();
	Led::Clear();

	sei();

	for (int i = 0; i < 5; i++) {
		Led::Set();
		_delay_ms(100);
		Led::Clear();
		_delay_ms(100);
	}
	Clock::start();

	Button button;
	Clock::clock_t start;
	enum {
		IDLE,
		ON,
		COOLING
	} state = IDLE;

	for(;;)
	{
		bool on = button.isOn();
		switch (state) {
		case IDLE:
			if (on) {
				state = ON;
				Fan::Set();
				Heater::Set();
				Led::Set();
			}
			break;
		case ON:
			if (!on) {
				state = COOLING;
				Heater::Clear();
				Led::Clear();
				start = Clock::millis();
			}
			break;
		case COOLING:
			if (on) {
				state = ON;
				Fan::Set();
				Heater::Set();
				Led::Set();
			} else {
				Clock::clock_t now = Clock::millis();
				if (Clock::delta(now, start) > COOL_DELAY) {
					state = IDLE;
					Fan::Clear();
					Heater::Clear();
					Led::Clear();
				}
			}
			break;
		}
	}
}

extern "C" void __cxa_pure_virtual()
{
  cli();
  for (;;);
}
