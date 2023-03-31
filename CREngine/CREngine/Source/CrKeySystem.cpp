#include "CrKeySystem.hpp"
#include <SDL_events.h>

template<bool Input>
void CrKeySystem::InternalProcess(Array<WP<KeySubscriber>>& KeySubArr)
{
	for (auto Weak : KeySubArr)
	{
		auto Elem = Weak.lock();

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

//Actual adding implementation
SP<KeySubscriber> CrKeySystem::BindToKey_Internal(SDL_Keycode Keycode, SP<KeyActivator> InActivator)
{
	SP<KeySubscriber> KS = MkSP<KeySubscriber>();

	KS->Activator = InActivator;
	KS->RequiredKeycodes = 1;

	auto Found = KeyToKeySubscribers.find(Keycode);

	if (Found == KeyToKeySubscribers.end())
	{
		Array<WP<KeySubscriber>> New{ WP<KeySubscriber>(KS) };
		KeyToKeySubscribers.emplace(Keycode, New);
	}
	else
	{
		Found->second.push_back(KS);
	}
	return KS;
}

SP<KeySubscriber> CrKeySystem::BindToKeys_Internal(const Array<SDL_Keycode>& Keycodes, SP<KeyActivator> InActivator)
{
	SP<KeySubscriber> KS = MkSP<KeySubscriber>();

	KS->Activator = InActivator;
	KS->RequiredKeycodes = Keycodes.size();

	for (auto& Keycode : Keycodes)
	{
		auto Found = KeyToKeySubscribers.find(Keycode);
		if (Found == KeyToKeySubscribers.end())
		{
			Array<WP<KeySubscriber>> New{ WP<KeySubscriber>(KS) };
			KeyToKeySubscribers.emplace(Keycode, New);
		}
		else
		{
			Found->second.push_back(KS);
		}
	}
	return KS;
}

void CrKeySystem::DoRemovals(Array<WP<KeySubscriber>>& KeySubArr)
{
	RemoveByPredicate(KeySubArr, [](WP<KeySubscriber> Item) { return Item.expired(); });
}



SP<KeySubscriber> CrKeySystem::BindToKey(SDL_Keycode Keycode, KeyActivatorFunction InFunc)
{
	return BindToKey_Internal(Keycode, MkSP<KeyActivator>(KeyActivator(std::move(InFunc))));
}

SP<KeySubscriber> CrKeySystem::BindToKeys(const Array<SDL_Keycode>& Keycodes, KeyActivatorFunction InFunc)
{
	return BindToKeys_Internal(Keycodes, MkSP<KeyActivator>(KeyActivator(std::move(InFunc))));
}


SP<KeySubscriber> CrKeySystem::BindToKey(SDL_Keycode Keycode, const SP<KeySubscriber>& InExisting)
{
	return BindToKey_Internal(Keycode, InExisting->Activator);
}

SP<KeySubscriber> CrKeySystem::BindToKeys(Array<SDL_Keycode> Keycodes, const SP<KeySubscriber>& InExisting)
{
	return BindToKeys_Internal(Keycodes, InExisting->Activator);
}


void CrKeySystem::Process(const SDL_Event& Event)
{
	if (Event.type == SDL_KEYDOWN || Event.type == SDL_KEYUP)
	{
		auto V = KeyToKeySubscribers.find(Event.key.keysym.sym);

		if (V == KeyToKeySubscribers.end())
		{
			//return if empty.
			return;
		}

		DoRemovals(V->second);

		if (Event.type == SDL_KEYDOWN)
		{
			InternalProcess<true>(V->second);
		}
		else if (Event.type == SDL_KEYUP)
		{
			InternalProcess<false>(V->second);
		}
	}
}
