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
	uint32_t CurrentNumKeycodes = 0;//the number of key presses this currently has active. If more than 0, then it has at least 1 key being pressed.

	SP<KeyActivator> Activator;
public:
	bool IsPressed() const { return CurrentNumKeycodes >= RequiredKeycodes; }
	uint32_t GetCurrentNumKeycodes() const { return CurrentNumKeycodes; }
	uint32_t GetRequiredKeycodes() const { return RequiredKeycodes; }
};


//Untested - maybe bugs here.
class CrKeySystem
{
	template<bool Input>
	void InternalProcess(Array<WP<KeySubscriber>>& KeySubArr);

	SP<KeySubscriber> BindToKey_Internal(SDL_Keycode Keycode, SP<KeyActivator> InActivator);
	SP<KeySubscriber> BindToKeys_Internal(const Array<SDL_Keycode>& Keycodes, SP<KeyActivator> InActivator);

	SP<KeySubscriber> BindToKey_Internal(SDL_Scancode Keycode, SP<KeyActivator> InActivator);
	SP<KeySubscriber> BindToKeys_Internal(const Array<SDL_Scancode>& Keycodes, SP<KeyActivator> InActivator);

	void DoRemovals(Array<WP<KeySubscriber>>& KeySubArr);

	Map<SDL_Keycode, Array<WP<KeySubscriber>>> Keycode_KeyToKeySubscribers;
	Map<SDL_Scancode, Array<WP<KeySubscriber>>> Scancode_KeyToKeySubscribers;

public:
	//return shared pointers to the key subscribers.
	//When they are destroyed, our weak pointers will be removed.
	//When all the KeySubscribers that own a KeyActivator are destroyed, the key activator will be destroyed as well.

	[[nodiscard]] SP<KeySubscriber> BindToKey(SDL_Keycode Keycode);
	[[nodiscard]] SP<KeySubscriber> BindToKeys(const Array<SDL_Keycode>& Keycodes);

	[[nodiscard]] SP<KeySubscriber> BindToKey(SDL_Keycode Keycode, KeyActivatorFunction&& InFunc);
	[[nodiscard]] SP<KeySubscriber> BindToKeys(const Array<SDL_Keycode>& Keycodes, KeyActivatorFunction&& InFunc);

	//Subscribe to keys using an existing KeySubscriber/function. Use Remove() on KeySubscriber to remove the binding. Returns the new KeySubscriber binding.
	[[nodiscard]] SP<KeySubscriber> BindToKey(SDL_Keycode Keycode, const SP<KeySubscriber>& InExisting);
	[[nodiscard]] SP<KeySubscriber> BindToKeys(const Array<SDL_Keycode>& Keycodes, const SP<KeySubscriber>& InExisting);

	//Bind with no event, instead being pollable via KeySubscriber.
	[[nodiscard]] SP<KeySubscriber> BindToKey(SDL_Scancode Scancode);
	[[nodiscard]] SP<KeySubscriber> BindToKeys(const Array<SDL_Scancode>& Scancodes);
	//Scancodes instead of keycodes.
	[[nodiscard]] SP<KeySubscriber> BindToKey(SDL_Scancode Scancode, KeyActivatorFunction&& InFunc);
	[[nodiscard]] SP<KeySubscriber> BindToKeys(const Array<SDL_Scancode>& Scancodes, KeyActivatorFunction&& InFunc);
	//Subscribe to keys using an existing KeySubscriber/function. Use Remove() on KeySubscriber to remove the binding. Returns the new KeySubscriber binding.
	[[nodiscard]] SP<KeySubscriber> BindToKey(SDL_Scancode Scancode, const SP<KeySubscriber>& InExisting);
	[[nodiscard]] SP<KeySubscriber> BindToKeys(const Array<SDL_Scancode>& Scancodes, const SP<KeySubscriber>& InExisting);

	//HINT: Keycode is after keyboard mapping by OS or something, while Scancode is physical location instead (better for WASD and such)


	void Process(const union SDL_Event& Event);
};