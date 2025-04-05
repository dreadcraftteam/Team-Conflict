//========= Copyright Valve Corporation, All rights reserved. ============//
//
// 
//
//========================================================================//

#ifndef SIMPLE_BOT_H
#define SIMPLE_BOT_H

#include "tc/NextBot/NextBot.h"
#include "tc/NextBot/NextBotBehavior.h"
#include "tc/NextBot/NextBotGroundLocomotion.h"
#include "tc/NextBot/Path/NextBotPathFollow.h"


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CNextBotBase : public NextBotCombatCharacter
{
public:
	DECLARE_CLASS(CNextBotBase, NextBotCombatCharacter);

	CNextBotBase();
	virtual ~CNextBotBase();

	virtual void Precache();
	virtual void Spawn(void);

	virtual CBaseCombatWeapon* GetBestWeapon();

	// default weapons
	CBaseCombatWeapon* smg1, * shotgun, * pistol, * crowbar;

	// INextBot
	DECLARE_INTENTION_INTERFACE(CNextBotBase)
	virtual NextBotGroundLocomotion* GetLocomotionInterface(void) const { return m_locomotor; }

private:
	NextBotGroundLocomotion* m_locomotor;
};


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CSimpleBotPathCost : public IPathCost
{
public:
	CSimpleBotPathCost(CNextBotBase* me)
	{
		m_me = me;
	}

	// return the cost (weighted distance between) of moving from "fromArea" to "area", or -1 if the move is not allowed
	virtual float operator()(CNavArea* area, CNavArea* fromArea, const CNavLadder* ladder, const CFuncElevator* elevator, float length) const
	{
		if (fromArea == NULL)
		{
			// first area in path, no cost
			return 0.0f;
		}
		else
		{
			if (!m_me->GetLocomotionInterface()->IsAreaTraversable(area))
			{
				// our locomotor says we can't move here
				return -1.0f;
			}

			// compute distance traveled along path so far
			float dist;

			if (ladder)
			{
				dist = ladder->m_length;
			}
			else if (length > 0.0)
			{
				// optimization to avoid recomputing length
				dist = length;
			}
			else
			{
				dist = (area->GetCenter() - fromArea->GetCenter()).Length();
			}

			float cost = dist + fromArea->GetCostSoFar();

			// check height change
			float deltaZ = fromArea->ComputeAdjacentConnectionHeightChange(area);
			if (deltaZ >= m_me->GetLocomotionInterface()->GetStepHeight())
			{
				if (deltaZ >= m_me->GetLocomotionInterface()->GetMaxJumpHeight())
				{
					// too high to reach
					return -1.0f;
				}

				// jumping is slower than flat ground
				const float jumpPenalty = 5.0f;
				cost += jumpPenalty * dist;
			}
			else if (deltaZ < -m_me->GetLocomotionInterface()->GetDeathDropHeight())
			{
				// too far to drop
				return -1.0f;
			}

			return cost;
		}
	}

	CNextBotBase* m_me;
};


#endif // SIMPLE_BOT_H
