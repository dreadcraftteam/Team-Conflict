//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: HUD Target ID element
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "c_hl2mp_player.h"
#include "c_playerresource.h"
#include "vgui_entitypanel.h"
#include "iclientmode.h"
#include "vgui/ILocalize.h"
#include "hl2mp_gamerules.h"
#include "c_team.h"
#include <vgui_controls/AnimationController.h>
#include "c_baseplayer.h"
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar tc_class;

//-----------------------------------------------------------------------------
// Purpose: Displays current ammunition level
//-----------------------------------------------------------------------------
class CClassHud : public vgui::Panel, public CHudElement
{
	DECLARE_CLASS_SIMPLE(CClassHud, vgui::Panel);

public:
	CClassHud(const char* pElementName);
	void Reset();

	virtual void PerformLayout();

protected:
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme);
	virtual void OnThink();

private:
	vgui::HFont m_hFont;
	Color		m_bgColor;

	vgui::Label* m_pWarmupLabel;	// "Warmup Mode"

	vgui::Label* m_pBackground;		// black box

	bool m_bSuitAuxPowerUsed;

	CPanelAnimationVarAliasType(int, m_iTextX, "text_xpos", "8", "proportional_int");
	CPanelAnimationVarAliasType(int, m_iTextY, "text_ypos", "8", "proportional_int");


};

DECLARE_HUDELEMENT(CClassHud);

const char* classes[] =
{
	"Unnasigned"
	"Commander",
	"Supporter",
	"Builder",
	"Rasher",
	"Soldier",
	"Flamer",
	"Sniper",
	"Medic"
};

const char* GetHUDClassName()
{
	int index = tc_class.GetInt();

	return classes[index];
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CClassHud::CClassHud(const char* pElementName) : BaseClass(NULL, "ClassDisplay"), CHudElement(pElementName)
{
	vgui::Panel* pParent = g_pClientMode->GetViewport();
	SetParent(pParent);
	SetVisible(false);
	SetAlpha(255);

	m_pBackground = new vgui::Label(this, "Background", "");

	m_pWarmupLabel = new vgui::Label(this, "RoundState_warmup", "test label");
	m_pWarmupLabel->SetPaintBackgroundEnabled(false);
	m_pWarmupLabel->SetPaintBorderEnabled(false);
	m_pWarmupLabel->SizeToContents();
	m_pWarmupLabel->SetContentAlignment(vgui::Label::a_west);
	m_pWarmupLabel->SetFgColor(GetFgColor());

	m_bSuitAuxPowerUsed = false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CClassHud::Reset()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CClassHud::ApplySchemeSettings(vgui::IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	SetFgColor(Color(0, 0, 0, 0));	//GetSchemeColor("RoundStateFg", pScheme) );
	m_hFont = pScheme->GetFont("Default", true);

	m_pBackground->SetBgColor(GetSchemeColor("BgColor", pScheme));
	m_pBackground->SetPaintBackgroundType(2);

	SetAlpha(255);
	SetBgColor(Color(0, 0, 0, 0));
	SetPaintBackgroundType(0);
}

//-----------------------------------------------------------------------------
// Purpose: Resizes the label
//-----------------------------------------------------------------------------
void CClassHud::PerformLayout()
{

	BaseClass::PerformLayout();

	int wide, tall;
	GetSize(wide, tall);

	// find the widest line
	int labelWide = m_pWarmupLabel->GetWide();

	// find the total height
	int fontTall = vgui::surface()->GetFontTall(m_hFont);
	int labelTall = fontTall;

	labelWide += m_iTextX * 2;
	labelTall += m_iTextY * 2;

	m_pBackground->SetBounds(0, 0, labelWide, labelTall);

	int xOffset = (labelWide - m_pWarmupLabel->GetWide()) / 2;
	m_pWarmupLabel->SetPos(0 + xOffset, 0 + m_iTextY);
}

//-----------------------------------------------------------------------------
// Purpose: Updates the label color each frame
//-----------------------------------------------------------------------------
void CClassHud::OnThink()
{
	SetVisible(false);

	C_BaseHLPlayer* pLocalPlayer = (C_BaseHLPlayer*)C_BasePlayer::GetLocalPlayer();

	if (pLocalPlayer == NULL)
		return;

	if (HL2MPRules()->IsTeamplay() == false)
		return;

	if (pLocalPlayer->IsAlive() == false)
		return;

	if (pLocalPlayer->m_HL2Local.m_flSuitPower < 100)
	{
		if (m_bSuitAuxPowerUsed == false)
		{
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("FadeOutTeamLine");
			m_bSuitAuxPowerUsed = true;
		}
	}
	else
	{
		if (m_bSuitAuxPowerUsed == true)
		{
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("FadeInTeamLine");
			m_bSuitAuxPowerUsed = false;
		}
	}

	int iTeamNumber = pLocalPlayer->GetTeamNumber();
	Color c = GameResources()->GetTeamColor(iTeamNumber);

	wchar_t string1[1024];
	C_Team* pTeam = GetGlobalTeam(iTeamNumber);

	if (pTeam)
	{
		wchar_t TeamName[64];
		g_pVGuiLocalize->ConvertANSIToUnicode(pTeam->Get_Name(), TeamName, sizeof(TeamName));

		g_pVGuiLocalize->ConstructString(string1, sizeof(string1), g_pVGuiLocalize->Find("WELL"), 1, TeamName);

		m_pBackground->SetFgColor(GetFgColor());
		m_pWarmupLabel->SetFgColor(c);

		m_pWarmupLabel->SetText(string1);
		m_pWarmupLabel->SetVisible(true);

		m_pWarmupLabel->SizeToContents();

		SetVisible(true);
	}

	InvalidateLayout();
}
