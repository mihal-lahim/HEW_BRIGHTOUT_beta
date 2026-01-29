
#ifndef DEBUG_COUNTER_H
#define DEBUG_COUNTER_H


#define DCOUNTER(limit) \
	static int count = 0; \
	++count; \
	if (count >= limit) \
	throw 0 \






#endif