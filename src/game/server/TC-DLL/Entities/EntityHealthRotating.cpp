//========= Copyright DREADCRAFT TEAM, All rights reserved. ============//
//
// Purpose: Rotating Pickup Entity
//
//======================================================================//

#include "cbase.h"
#include "items.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define HEALTH_MODEL "models/items/healthkit.mdl"
#define HEALTH_MIN_HEIGHT 50

#define SF_SUPPRESS_PICKUP_DECAL	0x00000002

//-----------------------------------------------------------------------------
// Rotating health kit. Heals the player when picked up.
//-----------------------------------------------------------------------------
class CHealthRotating : public CItem
{
	DECLARE_CLASS( CHealthRotating, CItem );
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();
public:

	CHealthRotating();

	void	Spawn();
	void	Activate();
	void	Precache();

	bool	MyTouch( CBasePlayer *pPlayer );

	CBaseEntity*	Respawn();
	void			Materialize();

	int	m_iHealthToGive;
	float m_fRespawnTime;
	CNetworkVar(bool, m_bRespawning);

private:
	Vector MdlTop;
};

LINK_ENTITY_TO_CLASS(item_health_rotating, CHealthRotating );

PRECACHE_REGISTER(item_health_rotating);

BEGIN_DATADESC( CHealthRotating )
	DEFINE_KEYFIELD( m_iHealthToGive, FIELD_INTEGER, "GiveHealth"),
	DEFINE_KEYFIELD( m_fRespawnTime, FIELD_FLOAT, "RespawnTime"),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CHealthRotating, DT_HealthRotating )
	SendPropBool( SENDINFO( m_bRespawning )),
	SendPropExclude( "DT_BaseEntity", "m_angRotation" ),
END_SEND_TABLE()

CHealthRotating::CHealthRotating()
{
	if ( m_iHealthToGive <= 0 )
		m_iHealthToGive = 25;

	if ( m_fRespawnTime <= 0 )
		m_fRespawnTime = 20;
}

void CHealthRotating::Spawn()
{
	// CItem is designed for Vphys objects, so we need to undo a couple of things its spawn() does
	Vector OriginalLocation = GetAbsOrigin();
		BaseClass::Spawn();
	VPhysicsDestroyObject();
	SetAbsOrigin(OriginalLocation);

	UseClientSideAnimation();
	SetModel(HEALTH_MODEL);

	SetMoveType(MOVETYPE_NONE);

	// Grab the highest point on the model before we change the bounding box
	MdlTop = GetAbsOrigin();
	MdlTop.z += GetModelPtr()->hull_max().z;
			
	SetSolid(SOLID_NONE);
	CollisionProp()->UseTriggerBounds(true,6); // Reign in the volume added to the trigger collision box
	Vector OBBSize = Vector(CollisionProp()->OBBSize().Length() / 2); // need to use length as the model will be rotated at 45 degrees on clients
	SetSize(-OBBSize,OBBSize); // Resize the bounding box
	
	AddEffects(EF_NOSHADOW);	
}

void CHealthRotating::Activate()
{
	BaseClass::Activate();

	// Ensure minimum distance above a standable surfare
	trace_t tr;
	UTIL_TraceLine(MdlTop,MdlTop + Vector(0,0,-HEALTH_MIN_HEIGHT),MASK_PLAYERSOLID,this,COLLISION_GROUP_NONE,&tr); // measuring from MdlTop
	if(tr.DidHit())
	{
		if ( !HasSpawnFlags( SF_SUPPRESS_PICKUP_DECAL ) )
		SetAbsOrigin( GetAbsOrigin() + ( Vector(0,0, HEALTH_MIN_HEIGHT*(1-tr.fraction)) ) );
	}
}

void CHealthRotating::Precache()
{
	PrecacheModel(HEALTH_MODEL );
	PrecacheScriptSound( "HL2Player.PickupWeapon" );
	PrecacheScriptSound( "AlyxEmp.Charge" );
}

// Called from CItem::ItemTouch()
bool CHealthRotating::MyTouch( CBasePlayer *pPlayer )
{
	if ( pPlayer && pPlayer->GetHealth() < pPlayer->GetMaxHealth() ) 
	{
		pPlayer->TakeHealth( m_iHealthToGive, DMG_GENERIC );
		
		CSingleUserRecipientFilter PlayerFilter( pPlayer );
		PlayerFilter.MakeReliable();

		UserMessageBegin( PlayerFilter, "ItemPickup" );
		WRITE_STRING( GetClassname() );
		MessageEnd();
		EmitSound( PlayerFilter, pPlayer->entindex(), "HL2Player.PickupWeapon" ); // this should be done by the HUD really

		Respawn();
		return true;
	}

	return false;
}

// Disappear
CBaseEntity* CHealthRotating::Respawn()
{
	SetTouch(NULL);
	m_bRespawning = true;
	
	SetThink ( &CHealthRotating::Materialize );
	SetNextThink( gpGlobals->curtime + m_fRespawnTime );

	return this;
}

// Reappear
void CHealthRotating::Materialize()
{
	EmitSound("AlyxEmp.Charge");
	m_bRespawning = false;
	SetTouch(&CItem::ItemTouch);
}