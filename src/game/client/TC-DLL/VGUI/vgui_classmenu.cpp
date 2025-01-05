//The following include files are necessary to allow your MyPanel.cpp to compile.
#include "cbase.h"

#if 0
#include "vgui_classmenu.h"
using namespace vgui;
#include <vgui/IVGui.h>

#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/RichText.h>
#include "vgui/ISurface.h"
#include "basemodelpanel.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//CClassMenu class: Tutorial example class
class CClassMenu : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CClassMenu, vgui::Frame);
	//CMyPanel : This Class / vgui::Frame : BaseClass

	CClassMenu(vgui::VPANEL parent); 	// Constructor
	~CClassMenu() {};				// Destructor

protected:
	//VGUI overrides:
	virtual void OnTick();
	virtual void OnCommand(const char* pcCommand);

private:
	//Other used VGUI control Elements:
	Button* m_pButton1;
	Button* m_pButton2;
	Button* m_pButton3;
	Button* m_pButton4;
	Button* m_pButton5;
	Button* m_pButton6;
	Button* m_pButton7;
	Button* m_pButton8;
	CModelPanel* m_pClassPreview;
};

// Constuctor: Initializes the Panel
CClassMenu::CClassMenu(vgui::VPANEL parent)
	: BaseClass(NULL, "TCClassMenu")
{
	SetParent(parent);

	SetScheme("ClientScheme");

	SetSizeable(false);
	SetMoveable(false);
	SetCloseButtonVisible(false);
	SetMinimizeButtonVisible(false);

	SetTitle("", false);
	
	int screenWidth, screenHeight;
	vgui::surface()->GetScreenSize(screenWidth, screenHeight);
	int posX = (screenWidth - 850) / 2;
	int posY = (screenHeight - 700) / 2;
	SetPos(posX, posY);

	SetSize(850, 700);

	m_pButton1 = new Button(this, "Button1", "Commander", this, "class_commander");
	m_pButton1->SetPos(40, 60);
	m_pButton1->SetSize(300, 35);
	m_pButton1->SetContentAlignment(vgui::Button::a_center);

	m_pButton2 = new Button(this, "Button2", "Suppoter", this, "class_supporter");
	m_pButton2->SetPos(40, 110);
	m_pButton2->SetSize(300, 35);
	m_pButton2->SetContentAlignment(vgui::Button::a_center);

	m_pButton3 = new Button(this, "Button3", "Builder", this, "class_builder");
	m_pButton3->SetPos(40, 160);
	m_pButton3->SetSize(300, 35);
	m_pButton3->SetContentAlignment(vgui::Button::a_center);

	m_pButton4 = new Button(this, "Button4", "Rasher", this, "class_rasher");
	m_pButton4->SetPos(40, 210);
	m_pButton4->SetSize(300, 35);
	m_pButton4->SetContentAlignment(vgui::Button::a_center);

	m_pButton5 = new Button(this, "Button5", "Soldier", this, "class_soldier");
	m_pButton5->SetPos(40, 260);
	m_pButton5->SetSize(300, 35);
	m_pButton5->SetContentAlignment(vgui::Button::a_center);

	m_pButton6 = new Button(this, "Button6", "Flamer", this, "class_flamer");
	m_pButton6->SetPos(40, 310);
	m_pButton6->SetSize(300, 35);
	m_pButton6->SetContentAlignment(vgui::Button::a_center);

	m_pButton7 = new Button(this, "Button7", "Sniper", this, "class_sniper");
	m_pButton7->SetPos(40, 360);
	m_pButton7->SetSize(300, 35);
	m_pButton7->SetContentAlignment(vgui::Button::a_center);

	m_pButton8 = new Button(this, "Button8", "Medic", this, "class_medic");
	m_pButton8->SetPos(40, 410);
	m_pButton8->SetSize(300, 35);
	m_pButton8->SetContentAlignment(vgui::Button::a_center);

	m_pClassPreview = new CModelPanel(this, "ClassPreview");
	m_pClassPreview->SetPos(380, 60);
	m_pClassPreview->SetSize(415, 480);
	m_pClassPreview->DeleteModelData();

	ivgui()->AddTickSignal(GetVPanel());

	InvalidateLayout();
}

//Class: CClassMenuInterface Class. Used for construction.
class CClassMenuInterface : public TCClassMenu
{
private:
	CClassMenu* TCClassMenu;
public:
	CClassMenuInterface()
	{
		TCClassMenu = NULL;
	}
	void Create(vgui::VPANEL parent)
	{
		TCClassMenu = new CClassMenu(parent);
	}
	void Destroy()
	{
		if (TCClassMenu)
		{
			TCClassMenu->SetParent((vgui::Panel*)NULL);
			delete TCClassMenu;
		}
	}
	void Activate(void)
	{
		if (TCClassMenu)
		{
			TCClassMenu->Activate();
		}
	}
};
static CClassMenuInterface g_TCClassMenu;
TCClassMenu* classmenu = (TCClassMenu*)&g_TCClassMenu;

ConVar cl_classmenu("cl_classmenu", "0", FCVAR_CLIENTDLL, "Sets the state of classMenu <state>");

void CClassMenu::OnTick()
{
	BaseClass::OnTick();
	SetVisible(cl_classmenu.GetBool());
}

CON_COMMAND(OpenClassMenu, "Opens class menu")
{
	cl_classmenu.SetValue(!cl_classmenu.GetBool());
	classmenu->Activate();
};

void CClassMenu::OnCommand(const char* pcCommand)
{
	BaseClass::OnCommand(pcCommand);

	if (!Q_stricmp(pcCommand, "turnoff"))
		cl_classmenu.SetValue(0);
}
#endif