#pragma once

#include <SDL_Keycode.h>

#include "CRE_Utilities.hpp"
#include "CRE_Math.hpp"

typedef Func<void(bool)> KeyActivatorFunction;

//Actual function storage
class KeyActivator
{
	friend class CRE_KeySystem;

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

typedef std::shared_ptr<KeyActivator> KeyActivator_sp;
typedef std::weak_ptr<KeyActivator> KeyActivator_wp;

class KeySubscriber
{
	friend class CRE_KeySystem;

	bool bWantsRemoval = false;
	uint32_t RequiredKeycodes = 0;
	uint32_t CurrentNumKeycodes = 0;

	KeyActivator_sp Activator;

public:

	void Remove();
};


typedef std::shared_ptr<KeySubscriber> KeySubscriber_sp;
typedef std::weak_ptr<KeySubscriber> KeySubscriber_wp;

//Untested - maybe bugs here.
class CRE_KeySystem
{
public:
	//return weak pointers to the key subscribers. Use Remove() on KeySubscriber to remove the binding.
	KeySubscriber_wp BindToKey(SDL_Keycode Keycode, KeyActivatorFunction InFunc);
	KeySubscriber_wp BindToKeys(const Array<SDL_Keycode>& Keycodes, KeyActivatorFunction InFunc);

	KeySubscriber_wp BindToKey(SDL_Keycode Keycode, KeyActivator_sp InActivator);
	KeySubscriber_wp BindToKeys(const Array<SDL_Keycode>& Keycodes, KeyActivator_sp InActivator);

	//Subscribe to keys using an existing KeySubscriber/function. Use Remove() on KeySubscriber to remove the binding. Returns the new KeySubscriber binding.
	KeySubscriber_wp BindToKey(SDL_Keycode Keycode, KeySubscriber_wp InExisting);
	KeySubscriber_wp BindToKeys(Array<SDL_Keycode> Keycodes, KeySubscriber_wp InExisting);

	void Process(const union SDL_Event& Event);

private:

	template<bool Input>
	void InternalProcess(Array<KeySubscriber_sp>& KeySubArr)
	{
		for (auto& Elem : KeySubArr)
		{
			//Lovely constexpr if... So nice.
			//Add 1 if we're pressing down on the key, subtract one if it is being released.
			if constexpr (Input)
			{
				Elem->CurrentNumKeycodes++;
				//Try to call after incrementing.
				if (Elem->CurrentNumKeycodes == Elem->RequiredKeycodes)
				{
					Elem->Activator->Call<Input>();
				}
			}
			else
			{
				//Try to call before decrementing.
				if (Elem->CurrentNumKeycodes == Elem->RequiredKeycodes)
				{
					Elem->Activator->Call<Input>();
				}
				Elem->CurrentNumKeycodes--;
			}
			//Ensure we never go over the number of required keycodes.
			Elem->CurrentNumKeycodes = std::min(Elem->CurrentNumKeycodes, Elem->RequiredKeycodes);
		}
	}

	void DoRemovals(Array<KeySubscriber_sp>& KeySubArr);

	Map<SDL_Keycode, Array<KeySubscriber_sp>> KeyToKeySubscribers;
};