#include <avr/interrupt.h>

#include <iopins.h>

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
		if ((now > shortTime ? now - shortTime : shortTime - now) > delay) {
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

typedef Switch<IO::Pb2> Button;

const Clock::clock_t COOL_DELAY=60000; // 1 minute

int main(void)
{
	Fan::SetDirWrite();
	Fan::Clear();
	Heater::SetDirWrite();
	Heater::Clear();

	sei();

	Clock::start();

	Button button;
	Clock::clock_t start = 0;
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
			}
			break;
		case ON:
			if (!on) {
				state = COOLING;
				Heater::Clear();
				start = Clock::millis();
			}
			break;
		case COOLING:
			if (on) {
				state = ON;
				Fan::Set();
				Heater::Set();
			} else {
				Clock::clock_t now = Clock::millis();
				if ((now > start ? now - start : start - now) > COOL_DELAY) {
					state = IDLE;
					Fan::Clear();
					Heater::Clear();
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
