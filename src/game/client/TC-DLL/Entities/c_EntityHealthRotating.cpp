#include "cbase.h"
#include "dlight.h"
#include "r_efx.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define ITEM_ROTATION_RATE	( 360.0f / 4.0f )
#define PICKUP_MIN_HEIGHT 50

//-----------------------------------------------------------------------------
// Rotating health kit. Heals the player when picked up.
//-----------------------------------------------------------------------------
class C_HealthRotating : public C_BaseAnimating
{
	DECLARE_CLASS(C_HealthRotating, C_BaseAnimating);
	DECLARE_CLIENTCLASS();
public:
	C_HealthRotating() {
		ClientRotAng = QAngle(45, 0, 0);
		m_bRespawning = m_bRespawning_Cache = false;
	}
	void Spawn() { ClientThink(); }

	bool IsRespawning();
	void ClientThink();
	void PostDataUpdate(DataUpdateType_t updateType);
	bool ShouldDraw();

	bool	m_bRespawning;
	bool	m_bRespawning_Cache;

private:
	QAngle		ClientRotAng; // m_angRotation is stomped sometimes (CItem returning the ent to spawn position?)
};

LINK_ENTITY_TO_CLASS(item_health_rotating, C_HealthRotating);

IMPLEMENT_CLIENTCLASS_DT(C_HealthRotating, DT_HealthRotating, CHealthRotating)
RecvPropBool(RECVINFO(m_bRespawning)),
END_RECV_TABLE()

inline bool C_HealthRotating::IsRespawning()
{
	return m_bRespawning;
}

void C_HealthRotating::ClientThink()
{
	if (IsAbsQueriesValid())
	{
		// Rotate
		ClientRotAng.y += ITEM_ROTATION_RATE * gpGlobals->frametime;
		if (ClientRotAng.y >= 360)
			ClientRotAng.y -= 360;

		SetAbsAngles(ClientRotAng);
	}

	SetNextClientThink(CLIENT_THINK_ALWAYS);
}

void C_HealthRotating::PostDataUpdate(DataUpdateType_t updateType)
{
	if (m_bRespawning_Cache != m_bRespawning)
	{
		// Appear/disappear
		UpdateVisibility();
		ClientRotAng.y = 0;
		m_bRespawning_Cache = m_bRespawning;
	}

	return BaseClass::PostDataUpdate(updateType);
}

bool C_HealthRotating::ShouldDraw()
{
	return !IsRespawning() && BaseClass::ShouldDraw();
}