//========= Copyright DREADCRAFT TEAM, All rights reserved. ====================//
//
// Purpose:
//
//==============================================================================//

#include "GameUI/IGameUI.h"
#include "vgui_controls/Panel.h"
#include "vgui_controls/Button.h"
#include "vgui/ISurface.h"
#include "filesystem.h"

#include <vgui_controls/HTML.h>
#include "ienginevgui.h"

class CHTMLBrowser : public vgui::Frame
{
    DECLARE_CLASS_SIMPLE(CHTMLBrowser, vgui::Frame);

public:
    CHTMLBrowser(vgui::VPANEL parent);
    ~CHTMLBrowser() override;

    void OpenURL(const char* url);
    void OnCommand(const char* command) override;

private:
    vgui::HTML* m_pHTML;
    vgui::TextEntry* m_pURLInput;
    vgui::Button* m_pGoButton;
    vgui::Button* m_pBackButton;
    vgui::Button* m_pForwardButton;
    vgui::Button* m_pReloadButton;

    char m_szCurrentURL[1024];

    void UpdateURLField();
};