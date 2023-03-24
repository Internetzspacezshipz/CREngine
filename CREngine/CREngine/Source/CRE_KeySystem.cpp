#include "CRE_KeySystem.hpp"
#include <SDL_events.h>

KeySubscriber_wp CRE_KeySystem::BindToKey(SDL_Keycode Keycode, KeyActivatorFunction InFunc)
{
	return BindToKey(Keycode, std::make_shared<KeyActivator>(KeyActivator(std::move(InFunc))));
}

KeySubscriber_wp CRE_KeySystem::BindToKeys(const Array<SDL_Keycode>& Keycodes, KeyActivatorFunction InFunc)
{
	return BindToKeys(Keycodes, std::make_shared<KeyActivator>(KeyActivator(std::move(InFunc))));
}

KeySubscriber_wp CRE_KeySystem::BindToKey(SDL_Keycode Keycode, KeyActivator_sp InActivator)
{
	KeySubscriber_sp KS = std::make_shared<KeySubscriber>();

	KS->Activator = InActivator;
	KS->RequiredKeycodes = 1;

	auto Found = KeyToKeySubscribers.find(Keycode);
	if (Found == KeyToKeySubscribers.end())
	{
		std::vector<KeySubscriber_sp> New{ KS };
		KeyToKeySubscribers.emplace(Keycode, New);
	}
	else
	{
		Found->second.push_back(KS);
	}
	return KS;
}

KeySubscriber_wp CRE_KeySystem::BindToKeys(const Array<SDL_Keycode>& Keycodes, KeyActivator_sp InActivator)
{
	KeySubscriber_sp KS = std::make_shared<KeySubscriber>();

	KS->Activator = InActivator;
	KS->RequiredKeycodes = Keycodes.size();

	for (auto& Keycode : Keycodes)
	{
		auto Found = KeyToKeySubscribers.find(Keycode);
		if (Found == KeyToKeySubscribers.end())
		{
			std::vector<KeySubscriber_sp> New{ KS };
			KeyToKeySubscribers.emplace(Keycode, New);
		}
		else
		{
			Found->second.push_back(KS);
		}
	}
	return KS;
}


KeySubscriber_wp CRE_KeySystem::BindToKey(SDL_Keycode Keycode, KeySubscriber_wp InExisting)
{
	return BindToKey(Keycode, InExisting.lock()->Activator);
}

KeySubscriber_wp CRE_KeySystem::BindToKeys(Array<SDL_Keycode> Keycodes, KeySubscriber_wp InExisting)
{
	return BindToKeys(Keycodes, InExisting.lock()->Activator);
}


void CRE_KeySystem::Process(const SDL_Event& Event)
{
	if (Event.type == SDL_KEYDOWN || Event.type == SDL_KEYUP)
	{
		auto V = KeyToKeySubscribers.find(Event.key.keysym.sym);

		if (V == KeyToKeySubscribers.end())
		{
			//return if empty.
			return;
		}

		if (Event.type == SDL_KEYDOWN)
		{
			InternalProcess<true>(V->second);
		}
		else if (Event.type == SDL_KEYUP)
		{
			InternalProcess<false>(V->second);
		}

		DoRemovals(V->second);
	}
}

void CRE_KeySystem::DoRemovals(Array<KeySubscriber_sp>& KeySubArr)
{
	RemoveByPredicate(KeySubArr, [](KeySubscriber_sp Item) { return Item->bWantsRemoval; });
}

void KeySubscriber::Remove()
{
	bWantsRemoval = true;
}
