#include "CrTickSystem.h"
#include <time.h>


#define PRINT_NEW_CATEGORIES 1
#define PRINT_REMOVED_CATEGORIES 1

void CrTickSystem::DoMutations()
{
	//Do adds
	for (auto& ChangeTick : MutationQueue)
	{
		if (ChangeTick.bUsingAggregator)
		{
			if (ChangeTick.bAdd)
			{
				auto Category = ActiveDelayedTicks.find(ChangeTick.Number);
				if (Category == ActiveDelayedTicks.end())
				{
					//add a new category
					Category = ActiveDelayedTicks.emplace_hint(Category, ChangeTick.Number, Set<CrTick*>{});
					CrLOGD(PRINT_NEW_CATEGORIES, "DelayedTicks: category added: {}", ChangeTick.Number);
				}
				(*Category).second.emplace(ChangeTick.Tick);
			}
			else
			{
				auto Category = ActiveDelayedTicks.find(ChangeTick.Number);
				if ((*Category).second.size() == 1)
				{
					//Remove the whole category since it is now empty - we might as well not loop that again in the actual tick running function.
					ActiveDelayedTicks.erase(ChangeTick.Number);
					CrLOGD(PRINT_REMOVED_CATEGORIES, "DelayedTicks: category removed: {}", ChangeTick.Number);
				}
				else
				{
					(*Category).second.erase(ChangeTick.Tick);
				}
			}
		}
		else
		{
			if (ChangeTick.bAdd)
			{
				auto Category = ActiveImmediateTicks.find(ChangeTick.Number);
				if (Category == ActiveImmediateTicks.end())
				{
					//add a new category
					Category = ActiveImmediateTicks.emplace_hint(Category, ChangeTick.Number, Set<CrTick*>{});
					CrLOGD(PRINT_NEW_CATEGORIES, "ImmediateTicks: category added: {}", ChangeTick.Number);
				}
				(*Category).second.emplace(ChangeTick.Tick);
			}
			else
			{
				auto Category = ActiveImmediateTicks.find(ChangeTick.Number);
				if ((*Category).second.size() == 1)
				{
					//Remove the whole category since it is now empty - we might as well not loop that again in the actual tick running function.
					ActiveImmediateTicks.erase(ChangeTick.Number);
					CrLOGD(PRINT_REMOVED_CATEGORIES, "ImmediateTicks: category removed: {}", ChangeTick.Number);
				}
				else
				{
					(*Category).second.erase(ChangeTick.Tick);
				}
			}
		}
	}
	MutationQueue.clear();
}

CrTickSystem& CrTickSystem::Get()
{
	static CrTickSystem Singleton;
	return Singleton;
}

CrTickSystem::CrTickSystem()
{
	LastTime = std::chrono::steady_clock::now();
}

void CrTickSystem::RunTicks()
{
	//Alter the active ticks before we run them.
	DoMutations();

	auto NewTime = std::chrono::steady_clock::now();
	DeltaTime = static_cast<Seconds>(std::chrono::duration_cast<std::chrono::nanoseconds>(NewTime - LastTime).count())*1e-9;
	LastTime = NewTime;

	for (auto& Elem : ActiveImmediateTicks)
	{
		for (auto& TickInner : Elem.second)
		{
			TickInner->DoTick(DeltaTime);
		}
	}

	for (auto& Elem : ActiveDelayedTicks)
	{
		for (auto& TickInner : Elem.second)
		{
			const float NewAgg = TickInner->TickAggregator + DeltaTime;
			if (NewAgg >= TickInner->TimeBetweenTicks)
			{
				TickInner->DoTick(NewAgg);
				TickInner->TickAggregator = NewAgg - TickInner->TimeBetweenTicks;
			}
		}
	}
}

void CrTickSystem::AddTick(CrTick* Tick)
{
	MutationQueue.emplace_back(ChangeTickStatus{ Tick->Num, Tick, Tick->TimeBetweenTicks > 0.f, true });
}

void CrTickSystem::RemoveTick(CrTick* Tick)
{
	MutationQueue.emplace_back(ChangeTickStatus{ Tick->Num, Tick, Tick->TimeBetweenTicks > 0.f, false });
}

CrTick::~CrTick()
{
	Disable();
}

CrTick::CrTick(const CrID& ClassID) : CrTick(ClassID.GetNumber()) { }

CrTick::CrTick(const LocalityNumber& LNum)
{
	Num = LNum;
	Enable();
}

void CrTick::ChangeTickSettings(bool bInDisabled, Seconds NewTimeBetweenTicks)
{
	Disable();
	bDisabled = bInDisabled;
	if (bDisabled)
	{
		return;
	}
	TimeBetweenTicks = NewTimeBetweenTicks;
	TickAggregator = 0;
	Enable();
}

void CrTick::Disable()
{
	CrTickSystem::Get().RemoveTick(this);
}

void CrTick::Enable()
{
	CrTickSystem::Get().AddTick(this);
}
