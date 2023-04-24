#pragma once

#include "CrTypes.h"
#include "CrID.h"

using Seconds = float;

//A number that should be different for each class of object that uses the ticking system ideally.
//This is mainly as an implementation since the processor will likely stay in the same cache location instead of jumping around for each different implementation of the tick function.
using LocalityNumber = uint32_t;

class CrTick
{
	friend class CrTickSystem;
	friend inline static void operator <=>(CrArchive& Arch, CrTick& ToSerialize);

	LocalityNumber Num = 0;

	Seconds TickAggregator = 0;

	//Individual settings for the tick...
	//These don't do anything unless TickMode is set to CrTickMode_MinTimeBetween
	Seconds TimeBetweenTicks = 0;

	//Set to determine 
	bool bDisabled = false;

public:
	virtual void DoTick(Seconds DeltaTime) = 0;
	virtual ~CrTick();
	//Pass in the class id in the constructor to use as our map key
	CrTick(const CrID& ClassID);
	CrTick(const LocalityNumber& ClassID);

	void ChangeTickSettings(bool bInDisabled, Seconds NewTimeBetweenTicks = 0.f);
	bool GetIsDisabled() const { return bDisabled; }
	Seconds GetTimeBetweenTicks() const { return TimeBetweenTicks; }

	void Disable();
	void Enable();

	//Deleted default constructor.
	CrTick() = delete;
};

inline static void operator <=>(CrArchive& Arch, CrTick& ToSerialize)
{
	if (!Arch.bSerializing)
	{
		Arch.SerializeSpecific<bool, false>(ToSerialize.bDisabled);
		Arch.SerializeSpecific<Seconds, false>(ToSerialize.TimeBetweenTicks);
	}
	else
	{
		Arch.SerializeSpecific<bool, true>(ToSerialize.bDisabled);
		Arch.SerializeSpecific<Seconds, true>(ToSerialize.TimeBetweenTicks);
	}
	//Update tick settings in both cases.
	ToSerialize.ChangeTickSettings(ToSerialize.bDisabled, ToSerialize.TimeBetweenTicks);
}

class CrTickSystem
{
	std::chrono::steady_clock::time_point LastTime;
	Seconds DeltaTime = 0.f;

	void DoMutations();

	struct ChangeTickStatus
	{
		LocalityNumber Number;
		CrTick* Tick;
		bool bUsingAggregator;
		bool bAdd;
	};

	Array<ChangeTickStatus> MutationQueue;

	//Active ticking items.
	Map<LocalityNumber, Set<CrTick*>> ActiveImmediateTicks;

	//Active ticking items that have specific time gaps that must be checked.
	Map<LocalityNumber, Set<CrTick*>> ActiveDelayedTicks;

public:
	static CrTickSystem& Get();
	CrTickSystem();

	void RunTicks();

	//Queues an item to be added before the next tick.
	//This will happen in the same order it was called in when in relation to other calls to AddTick and RemoveTick
	void AddTick(CrTick* Tick);

	//Queues an item to be removed before the next tick.
	//This will happen in the same order it was called in when in relation to other calls to RemoveTick and AddTick
	void RemoveTick(CrTick* Tick);
};
