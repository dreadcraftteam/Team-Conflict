//========= Copyright DREADCRAFT TEAM, All rights reserved. ==============//
// 
// NextBotRoam.cpp
// A roam action.
// 
//========================================================================//

#include "cbase.h"
#include "NextBotRoam.h"
#include "nav_mesh.h"

//-----------------------------------------------------------------------------------------------------
// The Roam Bot
//-----------------------------------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS(roam_bot, CNextBotRoam);

#ifndef TF_DLL
PRECACHE_REGISTER(roam_bot);
#endif

//-----------------------------------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------------------------------
CNextBotRoam::CNextBotRoam()
{
	ALLOCATE_INTENTION_INTERFACE(CNextBotRoam);

	m_locomotor = new NextBotGroundLocomotion(this);
}

//-----------------------------------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------------------------------
CNextBotRoam::~CNextBotRoam()
{
	DEALLOCATE_INTENTION_INTERFACE;

	if (m_locomotor)
		delete m_locomotor;
}

//-----------------------------------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------------------------------
void CNextBotRoam::Precache()
{
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------------------------------
void CNextBotRoam::Spawn(void)
{
	BaseClass::Spawn();
}

//---------------------------------------------------------------------------------------------
// 
// The Simple Bot behaviors
//
// For use with TheNavMesh->ForAllAreas()
// Find the Nth area in the sequence
// 
//---------------------------------------------------------------------------------------------

class SelectNthAreaFunctor
{
public:
	SelectNthAreaFunctor(int count)
	{
		m_count = count;
		m_area = NULL;
	}

	bool operator() (CNavArea* area)
	{
		m_area = area;
		return (m_count-- > 0);
	}

	int m_count;
	CNavArea* m_area;
};


//---------------------------------------------------------------------------------------------
//
// This action causes the bot to pick a random nav area in the mesh and move to it, then
// pick another, etc.
// Actions usually each have their own .cpp/.h file and are organized into folders since there
// are often many of them.
//
//---------------------------------------------------------------------------------------------

class CNextBotRoamRoam : public Action< CNextBotRoam >
{
public:
	//----------------------------------------------------------------------------------
	// OnStart is called once when the Action first becomes active
	//----------------------------------------------------------------------------------
	virtual ActionResult< CNextBotRoam >	OnStart(CNextBotRoam* me, Action< CNextBotRoam >* priorAction)
	{
		// smooth out the bot's path following by moving toward a point farther down the path
		m_path.SetMinLookAheadDistance(300.0f);

		return Continue();
	}


	//-----------------------------------------------------------------------------------------
	// Update is called repeatedly (usually once per server frame) while the Action is active
	//-----------------------------------------------------------------------------------------
	virtual ActionResult< CNextBotRoam >	Update(CNextBotRoam* me, float interval)
	{
		if (m_path.IsValid() && !m_timer.IsElapsed())
		{
			// PathFollower::Update() moves the bot along the path using the bot's ILocomotion and IBody interfaces
			m_path.Update(me);
		}
		else
		{
			SelectNthAreaFunctor pick(RandomInt(0, TheNavMesh->GetNavAreaCount() - 1));
			TheNavMesh->ForAllAreas(pick);

			if (pick.m_area)
			{
				CNextBotRoamPathCost cost(me);
				m_path.Compute(me, pick.m_area->GetCenter(), cost);
			}

			// follow this path for a random duration (or until we reach the end)
			m_timer.Start(RandomFloat(5.0f, 10.0f));
		}

		return Continue();
	}


	//----------------------------------------------------------------------------------------------------------------
	// this is an event handler - many more are available (see declaration of Action< Actor > in NextBotBehavior.h)
	//----------------------------------------------------------------------------------------------------------------
	virtual EventDesiredResult< CNextBotRoam > OnStuck(CNextBotRoam* me)
	{
		// we are stuck trying to follow the current path - invalidate it so a new one is chosen
		m_path.Invalidate();

		return TryContinue();
	}


	virtual const char* GetName(void) const { return "Roam"; }		// return name of this action

private:
	PathFollower m_path;
	CountdownTimer m_timer;
};


//---------------------------------------------------------------------------------------------
//
// Instantiate the bot's Intention interface and start the initial Action (CNextBotRoamRoam in
// this case)
//
//---------------------------------------------------------------------------------------------
IMPLEMENT_INTENTION_INTERFACE(CNextBotRoam, CNextBotRoamRoam)
