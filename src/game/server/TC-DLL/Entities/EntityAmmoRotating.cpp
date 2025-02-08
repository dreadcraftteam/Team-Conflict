#include "cbase.h"
#include "items.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define AMMO_DECAL "example_model_material" // DONT TOUCH THIS! 
#define AMMO_MODEL "models/items/boxmrounds.mdl"
#define AMMO_MIN_HEIGHT 50
int AmmoPickupDecalIndex; // set by CAmmoRotating::Precache()

#define SF_SUPPRESS_PICKUP_DECAL	0x00000002

//-----------------------------------------------------------------------------
// Rotating health kit. Heals the player when picked up.
//-----------------------------------------------------------------------------
class CAmmoRotating : public CItem
{
	DECLARE_CLASS( CAmmoRotating, CItem );
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();
public:

	CAmmoRotating();

	void	Spawn();
	void	Activate();
	void	Precache();

	bool	MyTouch( CBasePlayer *pPlayer );

	CBaseEntity*	Respawn();
	void			Materialize();

	int	m_iAmmoToGive;
	float m_fRespawnTime;
	CNetworkVar(bool, m_bRespawning);

private:
	Vector MdlTop;
};

LINK_ENTITY_TO_CLASS(item_ammo_rotating, CAmmoRotating );

PRECACHE_REGISTER(item_ammo_rotating);

BEGIN_DATADESC( CAmmoRotating )
	DEFINE_KEYFIELD(m_iAmmoToGive, FIELD_INTEGER, "GiveAmmo"),
	DEFINE_KEYFIELD( m_fRespawnTime, FIELD_FLOAT, "RespawnTime"),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CAmmoRotating, DT_AmmoRotating )
	SendPropBool( SENDINFO( m_bRespawning )),
	SendPropExclude( "DT_BaseEntity", "m_angRotation" ),
END_SEND_TABLE()

CAmmoRotating::CAmmoRotating()
{
	if ( m_iAmmoToGive <= 0 )
		m_iAmmoToGive = 25;

	if ( m_fRespawnTime <= 0 )
		m_fRespawnTime = 20;
}

void CAmmoRotating::Spawn()
{
	// CItem is designed for Vphys objects, so we need to undo a couple of things its spawn() does
	Vector OriginalLocation = GetAbsOrigin();
		BaseClass::Spawn();
	VPhysicsDestroyObject();
	SetAbsOrigin(OriginalLocation);

	UseClientSideAnimation();
	SetModel(AMMO_MODEL);

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

void CAmmoRotating::Activate()
{
	BaseClass::Activate();

	// Ensure minimum distance above a standable surfare
	trace_t tr;
	UTIL_TraceLine(MdlTop,MdlTop + Vector(0,0,-AMMO_MIN_HEIGHT),MASK_PLAYERSOLID,this,COLLISION_GROUP_NONE,&tr); // measuring from MdlTop
	if(tr.DidHit())
	{
		if ( !HasSpawnFlags( SF_SUPPRESS_PICKUP_DECAL ) )
			engine->StaticDecal(tr.endpos, AmmoPickupDecalIndex,0,0,false); // mark the location of the pickup
		SetAbsOrigin( GetAbsOrigin() + ( Vector(0,0, AMMO_MIN_HEIGHT*(1-tr.fraction)) ) );
	}
}

void CAmmoRotating::Precache()
{
	PrecacheModel(AMMO_MODEL );
	PrecacheScriptSound( "HL2Player.PickupWeapon" );
	PrecacheScriptSound( "AlyxEmp.Charge" );
	AmmoPickupDecalIndex = UTIL_PrecacheDecal(AMMO_DECAL, true );
}

// Called from CItem::ItemTouch()
bool CAmmoRotating::MyTouch( CBasePlayer *pPlayer )
{
	pPlayer->GiveAmmo(m_iAmmoToGive, "Pistol");
	pPlayer->GiveAmmo(m_iAmmoToGive, "AR2");
	pPlayer->GiveAmmo(m_iAmmoToGive, "AR2AltFire");
	pPlayer->GiveAmmo(m_iAmmoToGive, "SMG1");
	pPlayer->GiveAmmo(m_iAmmoToGive, "smg1_grenade");
	pPlayer->GiveAmmo(m_iAmmoToGive, "Buckshot");
	pPlayer->GiveAmmo(m_iAmmoToGive, "357");
	pPlayer->GiveAmmo(m_iAmmoToGive, "rpg_round");
	pPlayer->GiveAmmo(m_iAmmoToGive, "grenade");
	pPlayer->GiveAmmo(m_iAmmoToGive, "slam");
		
	CSingleUserRecipientFilter PlayerFilter( pPlayer );
	PlayerFilter.MakeReliable();

	UserMessageBegin( PlayerFilter, "ItemPickup" );
	WRITE_STRING( GetClassname() );
	MessageEnd();
	EmitSound( PlayerFilter, pPlayer->entindex(), "HL2Player.PickupWeapon" ); // this should be done by the HUD really

	Respawn();
	return true;
}

// Disappear
CBaseEntity* CAmmoRotating::Respawn()
{
	SetTouch(NULL);
	m_bRespawning = true;
	
	SetThink ( &CAmmoRotating::Materialize );
	SetNextThink( gpGlobals->curtime + m_fRespawnTime );

	return this;
}

// Reappear
void CAmmoRotating::Materialize()
{
	EmitSound("AlyxEmp.Charge");
	m_bRespawning = false;
	SetTouch(&CItem::ItemTouch);
}