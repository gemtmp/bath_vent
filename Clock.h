/*
 * Clock.h
 *
 *  Created on: Feb 3, 2013
 *      Author: gem
 */

#ifndef CLOCK_H_
#define CLOCK_H_


class Clock {
public:
	typedef uint16_t clock_t;
	static const clock_t MAX_CLOCK = 0xFFFF;
	typedef void (*func)();

	static void start();
	static clock_t millis();

	// delta in mills between a and b
	static inline clock_t delta(clock_t a, clock_t b) {
		if (a >= b)
			return a - b;
		else
			return MAX_CLOCK - b + a;
	}
};


#endif /* CLOCK_H_ */
