//#include "shareddefs.h"
//#include "cbase.h"
//
//class CGenericToken : public CGEWeaponMelee
//{
//public:
//#ifdef GAME_DLL
//	// Function call to set this entitie's glow
//	virtual void SetGlow(bool state, Color glowColor = Color(255, 255, 255));
//#else
//	// This is called after we receive and process a network data packet
//	virtual void PostDataUpdate(DataUpdateType_t updateType);
//#endif
//
//private:
//#ifdef CLIENT_DLL
//	CEntGlowEffect* m_pEntGlowEffect;
//	bool m_bClientGlow;
//#endif
//	CNetworkVar(bool, m_bEnableGlow);
//	CNetworkVar(color32, m_GlowColor);
//};