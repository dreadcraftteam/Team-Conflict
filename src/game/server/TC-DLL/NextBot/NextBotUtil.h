//========= Copyright Valve Corporation, All rights reserved. ============//
//
// 
//
//========================================================================//

#ifndef _NEXT_BOT_UTIL_H_
#define _NEXT_BOT_UTIL_H_

#include "NextBotLocomotionInterface.h"
#include "nav_area.h"
#include "nav_mesh.h"
#include "nav_pathfind.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class INextBotEntityFilter
{
public:
	// return true if the given entity passes this filter
	virtual bool IsAllowed( CBaseEntity *entity ) const = 0;
};


// trace filter callback functions. needed for use with the querycache/optimization functionality
bool VisionTraceFilterFunction( IHandleEntity *pServerEntity, int contentsMask );
bool IgnoreActorsTraceFilterFunction( IHandleEntity *pServerEntity, int contentsMask );


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class NextBotTraceFilterIgnoreActors : public CTraceFilterSimple
{
public:
	NextBotTraceFilterIgnoreActors( const IHandleEntity *passentity, int collisionGroup ) : CTraceFilterSimple( passentity, collisionGroup, IgnoreActorsTraceFilterFunction )
	{
	}
};


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class NextBotVisionTraceFilter : public CTraceFilterSimple
{
public:
	NextBotVisionTraceFilter( const IHandleEntity *passentity, int collisionGroup )	: CTraceFilterSimple( passentity, collisionGroup, VisionTraceFilterFunction )
	{
	}
};


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class NextBotTraceFilterIgnoreNextBots : public CTraceFilterSimple
{
public:
	NextBotTraceFilterIgnoreNextBots( const IHandleEntity *passentity, int collisionGroup )
		: CTraceFilterSimple( passentity, collisionGroup )
	{
	}

	virtual bool ShouldHitEntity( IHandleEntity *pServerEntity, int contentsMask )
	{
		if ( CTraceFilterSimple::ShouldHitEntity( pServerEntity, contentsMask ) )
		{
			CBaseEntity *entity = EntityFromEntityHandle( pServerEntity );
#ifdef TERROR
			CBasePlayer *player = ToBasePlayer( entity );
			if ( player && player->IsGhost() )
				return false;
#endif // TERROR

			return ( entity->MyNextBotPointer() == NULL );
		}
		return false;
	}
};


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class NextBotTraceFilter : public CTraceFilterSimple
{
public:
	NextBotTraceFilter( const IHandleEntity *passentity, int collisionGroup )
		: CTraceFilterSimple( passentity, collisionGroup )
	{
		CBaseEntity *entity = const_cast<CBaseEntity *>(EntityFromEntityHandle( passentity ));
		m_passBot = entity->MyNextBotPointer();
	}

	virtual bool ShouldHitEntity( IHandleEntity *pServerEntity, int contentsMask )
	{
		if ( CTraceFilterSimple::ShouldHitEntity( pServerEntity, contentsMask ) )
		{
			CBaseEntity *entity = EntityFromEntityHandle( pServerEntity );
#ifdef TERROR
			CBasePlayer *player = ToBasePlayer( entity );
			if ( player && player->IsGhost() )
				return false;
#endif // TERROR

			// Skip players on the same team - they're not solid to us, and we'll avoid them
			if ( entity->IsPlayer() && m_passBot && m_passBot->GetEntity() &&
				m_passBot->GetEntity()->GetTeamNumber() == entity->GetTeamNumber() )
				return false;

			INextBot *bot = entity->MyNextBotPointer();
			
			return ( !bot || bot->IsAbleToBlockMovementOf( m_passBot ) );
		}
		return false;
	}
	
	const INextBot *m_passBot;
};


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class NextBotTraceFilterOnlyActors : public CTraceFilterSimple
{
public:
	NextBotTraceFilterOnlyActors( const IHandleEntity *passentity, int collisionGroup )
		: CTraceFilterSimple( passentity, collisionGroup )
	{
	}

	virtual TraceType_t	GetTraceType() const
	{
		return TRACE_ENTITIES_ONLY;
	}

	virtual bool ShouldHitEntity( IHandleEntity *pServerEntity, int contentsMask )
	{
		if ( CTraceFilterSimple::ShouldHitEntity( pServerEntity, contentsMask ) )
		{
			CBaseEntity *entity = EntityFromEntityHandle( pServerEntity );

#ifdef TERROR
			CBasePlayer *player = ToBasePlayer( entity );
			if ( player && player->IsGhost() )
				return false;
#endif // TERROR

			return ( entity->MyNextBotPointer() || entity->IsPlayer() );
		}
		return false;
	}
};


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class NextBotTraversableTraceFilter : public CTraceFilterSimple
{
public:
	NextBotTraversableTraceFilter( INextBot *bot, ILocomotion::TraverseWhenType when = ILocomotion::EVENTUALLY ) : CTraceFilterSimple( bot->GetEntity(), COLLISION_GROUP_NONE )
	{
		m_bot = bot;
		m_when = when;
	}

	virtual bool ShouldHitEntity( IHandleEntity *pServerEntity, int contentsMask )
	{
		CBaseEntity *entity = EntityFromEntityHandle( pServerEntity );

		if ( m_bot->IsSelf( entity ) )
		{
			return false;
		}

		if ( CTraceFilterSimple::ShouldHitEntity( pServerEntity, contentsMask ) )
		{
			return !m_bot->GetLocomotionInterface()->IsEntityTraversable( entity, m_when );
		}

		return false;
	}

private:
	INextBot *m_bot;
	ILocomotion::TraverseWhenType m_when;
};


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
inline CBaseEntity *SelectClosestEntityByTravelDistance( INextBot *me, const CUtlVector< CBaseEntity * > &candidateEntities, CNavArea *startArea, float travelRange )
{
	// collect nearby walkable areas within travelRange
	CUtlVector< CNavArea * > nearbyAreaVector;
	CollectSurroundingAreas( &nearbyAreaVector, startArea, travelRange, me->GetLocomotionInterface()->GetStepHeight(), me->GetLocomotionInterface()->GetDeathDropHeight() );

	// find closest entity in the collected area set
	CBaseEntity *closeEntity = NULL;
	float closeTravelRange = FLT_MAX;

	for( int i=0; i<candidateEntities.Count(); ++i )
	{
		CBaseEntity *candidate = candidateEntities[i];

		CNavArea *area = TheNavMesh->GetNearestNavArea( candidate, GETNAVAREA_CHECK_LOS, 500.0f );

		if ( area && area->IsMarked() && area->GetCostSoFar() < closeTravelRange )
		{
			closeEntity = candidate;
			closeTravelRange = area->GetCostSoFar();
		}
	}

	return closeEntity;
}


#ifdef OBSOLETE // WTF?
//--------------------------------------------------------------------------------------------
/**
 * Trace filter that skips "traversable" entities, but hits other Actors.
 * Used for obstacle avoidance.
 */
class NextBotMovementAvoidanceTraceFilter : public CTraceFilterSimple
{
public:
	NextBotMovementAvoidanceTraceFilter( INextBot *bot ) : CTraceFilterSimple( bot->GetEntity(), COLLISION_GROUP_NONE )
	{
		m_bot = bot;
	}

	virtual bool ShouldHitEntity( IHandleEntity *pServerEntity, int contentsMask )
	{
		CBaseEntity *entity = EntityFromEntityHandle( pServerEntity );

#ifdef TERROR
		CBasePlayer *player = ToBasePlayer( entity );
		if ( player && player->IsGhost() )
			return false;
#endif // TERROR

		if ( m_bot->IsSelf( entity ) )
		{
			return false;
		}

		if ( CTraceFilterSimple::ShouldHitEntity( pServerEntity, contentsMask ) )
		{
			return !m_bot->GetLocomotionInterface()->IsEntityTraversable( entity, ILocomotion::IMMEDIATELY );
		}

		return false;
	}

private:
	INextBot *m_bot;
};
#endif


#endif // _NEXT_BOT_UTIL_H_
