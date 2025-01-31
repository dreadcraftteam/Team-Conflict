//========= Copyright Valve Corporation, All rights reserved. ============//
//
// 
//
//========================================================================//

#include "cbase.h"
#include "NextBotBase.h"
#include "nav_mesh.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS(nb_bot, CNextBotBase);

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CNextBotBase::CNextBotBase()
{
	ALLOCATE_INTENTION_INTERFACE(CNextBotBase);

	m_locomotor = new NextBotGroundLocomotion(this);

	// default weapons
	smg1 = Weapon_Create("weapon_smg1");
	shotgun = Weapon_Create("weapon_shotgun");
	pistol = Weapon_Create("weapon_pistol");
	crowbar = Weapon_Create("weapon_crowbar");
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CNextBotBase::~CNextBotBase()
{
	DEALLOCATE_INTENTION_INTERFACE;

	if (m_locomotor)
		delete m_locomotor;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNextBotBase::Precache()
{
	BaseClass::Precache();

	PrecacheModel("models/kleiner.mdl");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CBaseCombatWeapon* CNextBotBase::GetBestWeapon()
{
	int num = RandomInt(1, 4);

	switch (num)
	{
	case 1:
		return smg1;
	case 2:
		return shotgun;
	case 3:
		return pistol;
	case 4:
		return crowbar;
	default:
		return crowbar;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNextBotBase::Spawn(void)
{
	BaseClass::Spawn();

	// wtf??
	Weapon_Equip(GetBestWeapon());
	Weapon_SetActivity(Weapon_TranslateActivity(ACT_HL2MP_RUN), 0);

	SetModel("models/kleiner.mdl");
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
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


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CNextBotBaseRoam : public Action< CNextBotBase >
{
public:
	//-----------------------------------------------------------------------------
	// Purpose: 
	//-----------------------------------------------------------------------------
	virtual ActionResult< CNextBotBase >	OnStart(CNextBotBase* me, Action< CNextBotBase >* priorAction)
	{
		// smooth out the bot's path following by moving toward a point farther down the path
		m_path.SetMinLookAheadDistance(300.0f);

		return Continue();
	}


	//-----------------------------------------------------------------------------
	// Purpose: 
	//-----------------------------------------------------------------------------
	virtual ActionResult< CNextBotBase >	Update(CNextBotBase* me, float interval)
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
				CSimpleBotPathCost cost(me);
				m_path.Compute(me, pick.m_area->GetCenter(), cost);
			}

			// follow this path for a random duration (or until we reach the end)
			m_timer.Start(RandomFloat(5.0f, 10.0f));
		}

		return Continue();
	}


	//-----------------------------------------------------------------------------
	// Purpose: 
	//-----------------------------------------------------------------------------
	virtual EventDesiredResult< CNextBotBase > OnStuck(CNextBotBase* me)
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


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
IMPLEMENT_INTENTION_INTERFACE(CNextBotBase, CNextBotBaseRoam)
