#include "CrKeySystem.h"
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
				if (Elem->Activator)
				{
					Elem->Activator->Call<Input>();
				}
			}
		}
		else
		{
			//Try to call before decrementing.
			if (Elem->CurrentNumKeycodes == Elem->RequiredKeycodes)
			{
				if (Elem->Activator)
				{
					Elem->Activator->Call<Input>();
				}
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

	if (InActivator)
	{
		KS->Activator = InActivator;
	}
	KS->RequiredKeycodes = 1;

	auto Found = Keycode_KeyToKeySubscribers.find(Keycode);

	if (Found == Keycode_KeyToKeySubscribers.end())
	{
		Array<WP<KeySubscriber>> New{ WP<KeySubscriber>(KS) };
		Keycode_KeyToKeySubscribers.emplace(Keycode, New);
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

	if (InActivator)
	{
		KS->Activator = InActivator;
	}
	KS->RequiredKeycodes = Keycodes.size();

	for (auto& Keycode : Keycodes)
	{
		auto Found = Keycode_KeyToKeySubscribers.find(Keycode);
		if (Found == Keycode_KeyToKeySubscribers.end())
		{
			Array<WP<KeySubscriber>> New{ WP<KeySubscriber>(KS) };
			Keycode_KeyToKeySubscribers.emplace(Keycode, New);
		}
		else
		{
			Found->second.push_back(KS);
		}
	}
	return KS;
}

SP<KeySubscriber> CrKeySystem::BindToKey_Internal(SDL_Scancode Keycode, SP<KeyActivator> InActivator)
{
	SP<KeySubscriber> KS = MkSP<KeySubscriber>();

	if (InActivator)
	{
		KS->Activator = InActivator;
	}
	KS->RequiredKeycodes = 1;

	auto Found = Scancode_KeyToKeySubscribers.find(Keycode);

	if (Found == Scancode_KeyToKeySubscribers.end())
	{
		Array<WP<KeySubscriber>> New{ WP<KeySubscriber>(KS) };
		Scancode_KeyToKeySubscribers.emplace(Keycode, New);
	}
	else
	{
		Found->second.push_back(KS);
	}
	return KS;
}

SP<KeySubscriber> CrKeySystem::BindToKeys_Internal(const Array<SDL_Scancode>& Keycodes, SP<KeyActivator> InActivator)
{
	SP<KeySubscriber> KS = MkSP<KeySubscriber>();

	if (InActivator)
	{
		KS->Activator = InActivator;
	}
	KS->RequiredKeycodes = Keycodes.size();

	for (auto& Keycode : Keycodes)
	{
		auto Found = Scancode_KeyToKeySubscribers.find(Keycode);
		if (Found == Scancode_KeyToKeySubscribers.end())
		{
			Array<WP<KeySubscriber>> New{ WP<KeySubscriber>(KS) };
			Scancode_KeyToKeySubscribers.emplace(Keycode, New);
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

//Yeah I know this is ugly af copy/paste but I don't want to refactor it right now.

SP<KeySubscriber> CrKeySystem::BindToKey(SDL_Keycode Keycode)
{
	return BindToKey_Internal(Keycode, nullptr);
}

SP<KeySubscriber> CrKeySystem::BindToKeys(const Array<SDL_Keycode>& Keycodes)
{
	return BindToKeys_Internal(Keycodes, nullptr);
}

SP<KeySubscriber> CrKeySystem::BindToKey(SDL_Keycode Keycode, KeyActivatorFunction&& InFunc)
{
	return BindToKey_Internal(Keycode, MkSP<KeyActivator>(KeyActivator(std::move(InFunc))));
}

SP<KeySubscriber> CrKeySystem::BindToKeys(const Array<SDL_Keycode>& Keycodes, KeyActivatorFunction&& InFunc)
{
	return BindToKeys_Internal(Keycodes, MkSP<KeyActivator>(KeyActivator(std::move(InFunc))));
}

SP<KeySubscriber> CrKeySystem::BindToKey(SDL_Keycode Keycode, const SP<KeySubscriber>& InExisting)
{
	return BindToKey_Internal(Keycode, InExisting->Activator);
}

SP<KeySubscriber> CrKeySystem::BindToKeys(const Array<SDL_Keycode>& Keycodes, const SP<KeySubscriber>& InExisting)
{
	return BindToKeys_Internal(Keycodes, InExisting->Activator);
}


SP<KeySubscriber> CrKeySystem::BindToKey(SDL_Scancode Scancode)
{
	return BindToKey_Internal(Scancode, nullptr);
}

SP<KeySubscriber> CrKeySystem::BindToKeys(const Array<SDL_Scancode>& Scancodes)
{
	return BindToKeys_Internal(Scancodes, nullptr);
}

SP<KeySubscriber> CrKeySystem::BindToKey(SDL_Scancode Scancode, KeyActivatorFunction&& InFunc)
{
	return BindToKey_Internal(Scancode, MkSP<KeyActivator>(KeyActivator(std::move(InFunc))));
}

SP<KeySubscriber> CrKeySystem::BindToKeys(const Array<SDL_Scancode>& Scancodes, KeyActivatorFunction&& InFunc)
{
	return BindToKeys_Internal(Scancodes, MkSP<KeyActivator>(KeyActivator(std::move(InFunc))));
}

SP<KeySubscriber> CrKeySystem::BindToKey(SDL_Scancode Scancode, const SP<KeySubscriber>& InExisting)
{
	return BindToKey_Internal(Scancode, InExisting->Activator);
}

SP<KeySubscriber> CrKeySystem::BindToKeys(const Array<SDL_Scancode>& Scancodes, const SP<KeySubscriber>& InExisting)
{
	return BindToKeys_Internal(Scancodes, InExisting->Activator);
}


void CrKeySystem::Process(const SDL_Event& Event)
{
	if (Event.type == SDL_KEYDOWN || Event.type == SDL_KEYUP)
	{
		auto C = Keycode_KeyToKeySubscribers.find(Event.key.keysym.sym);
		auto S = Scancode_KeyToKeySubscribers.find(Event.key.keysym.scancode);
		bool bCodeVal = !(C == Keycode_KeyToKeySubscribers.end());
		bool bScanVal = !(S == Scancode_KeyToKeySubscribers.end());
		if (bCodeVal)
		{
			DoRemovals(C->second);

			if (Event.type == SDL_KEYDOWN)
			{
				InternalProcess<true>(C->second);
			}
			else if (Event.type == SDL_KEYUP)
			{
				InternalProcess<false>(C->second);
			}
		}
		if (bScanVal)
		{
			DoRemovals(S->second);

			if (Event.type == SDL_KEYDOWN)
			{
				InternalProcess<true>(S->second);
			}
			else if (Event.type == SDL_KEYUP)
			{
				InternalProcess<false>(S->second);
			}
		}
	}
}
