#pragma once

#include <SDL_Keycode.h>

#include "CrUtilities.h"
#include "CrMath.h"

typedef Func<void(bool)> KeyActivatorFunction;

//Actual function storage
class KeyActivator
{
	friend class CrKeySystem;

	template<bool bButtonDown>
	void Call()
	{

		if (bLastActivated != bButtonDown)
		{
			bLastActivated = bButtonDown;
			Func(bButtonDown);
		}
	}
	KeyActivatorFunction Func;

	//What state this has been called in last. If you try to set the same state twice, it will ignore the second one.
	bool bLastActivated = false;

	KeyActivator(KeyActivatorFunction&& InFunc) : Func(InFunc), bLastActivated(false) {}
};

class KeySubscriber
{
	friend class CrKeySystem;
	uint32_t RequiredKeycodes = 0;
	uint32_t CurrentNumKeycodes = 0;

	SP<KeyActivator> Activator;
};


//Untested - maybe bugs here.
class CrKeySystem
{
	template<bool Input>
	void InternalProcess(Array<WP<KeySubscriber>>& KeySubArr);

	SP<KeySubscriber> BindToKey_Internal(SDL_Keycode Keycode, SP<KeyActivator> InActivator);
	SP<KeySubscriber> BindToKeys_Internal(const Array<SDL_Keycode>& Keycodes, SP<KeyActivator> InActivator);

	void DoRemovals(Array<WP<KeySubscriber>>& KeySubArr);

	Map<SDL_Keycode, Array<WP<KeySubscriber>>> KeyToKeySubscribers;

public:
	//return shared pointers to the key subscribers.
	//When they are destroyed, our weak pointers will be removed.
	//When all the KeySubscribers that own a KeyActivator are destroyed, the key activator will be destroyed as well.
	SP<KeySubscriber> BindToKey(SDL_Keycode Keycode, KeyActivatorFunction InFunc);
	SP<KeySubscriber> BindToKeys(const Array<SDL_Keycode>& Keycodes, KeyActivatorFunction InFunc);

	//Subscribe to keys using an existing KeySubscriber/function. Use Remove() on KeySubscriber to remove the binding. Returns the new KeySubscriber binding.
	SP<KeySubscriber> BindToKey(SDL_Keycode Keycode, const SP<KeySubscriber>& InExisting);
	SP<KeySubscriber> BindToKeys(Array<SDL_Keycode> Keycodes, const SP<KeySubscriber>& InExisting);

	void Process(const union SDL_Event& Event);
};