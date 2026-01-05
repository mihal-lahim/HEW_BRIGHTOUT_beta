

#ifndef INPUT_DEVICE_H
#define INPUT_DEVICE_H

#include <Windows.h>
#include "GameObject.h"

// “ü—ÍğŒ—ñ‹“Œ^
enum class InputCondition
{
	NONE,
	STARTED,
	PERFOMED,
	CANCELED,
};

typedef WORD InputKey;

class InputDevice : public GameObject
{
public:
	InputDevice() = default;
	virtual ~InputDevice() = default;
	// –ˆƒtƒŒ[ƒ€ŒÄ‚Ô
	virtual void PreUpdate() = 0;
	// “ü—Í’l‚ğæ“¾
	virtual float GetInputValue(InputKey input, InputCondition inputCondition) = 0;
};



#endif
