//========= Copyright DREADCRAFT TEAM, All rights reserved. ==============//
// 
// NextBotRoam.h
// A roam action.
// 
//========================================================================//

#ifndef SIMPLE_BOT_H
#define SIMPLE_BOT_H

#include "TC-DLL/NextBot/NextBot.h"
#include "TC-DLL/NextBot/NextBotBehavior.h"
#include "TC-DLL/NextBot/NextBotGroundLocomotion.h"
#include "TC-DLL/NextBot/Path/NextBotPathFollow.h"


//----------------------------------------------------------------------------
//
// The Roam Bot
//
//----------------------------------------------------------------------------
class CNextBotRoam : public NextBotCombatCharacter
{
public:
	DECLARE_CLASS(CNextBotRoam, NextBotCombatCharacter);

	CNextBotRoam();
	virtual ~CNextBotRoam();

	virtual void Precache();
	virtual void Spawn(void);

	// INextBot
	DECLARE_INTENTION_INTERFACE(CNextBotRoam)
	virtual NextBotGroundLocomotion* GetLocomotionInterface(void) const { return m_locomotor; }

private:
	NextBotGroundLocomotion* m_locomotor;
};


//--------------------------------------------------------------------------------------------------------------
//
// Functor used with the A* algorithm of NavAreaBuildPath() to determine the "cost" of moving from one area to
// another.
// "Cost" is generally the weighted distance between the centers of the areas. If you want the bot
// to avoid an area/ladder/elevator, increase the cost.  If you want to disallow an area/ladder/elevator,
// return -1.
//
//--------------------------------------------------------------------------------------------------------------
class CNextBotRoamPathCost : public IPathCost
{
public:
	CNextBotRoamPathCost(CNextBotRoam* me)
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

	CNextBotRoam* m_me;
};


#endif // ROAM_BOT_H
