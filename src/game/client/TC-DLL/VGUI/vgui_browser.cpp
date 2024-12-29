#include "cbase.h"
#include "vgui_browser.h"

// browser
CHTMLBrowser::CHTMLBrowser(vgui::VPANEL parent)
    : BaseClass(nullptr, "CHTMLBrowser")
{
    SetParent(parent);

    SetParent(parent);
    SetSize(800, 600);
    int screenWidth, screenHeight;
    vgui::surface()->GetScreenSize(screenWidth, screenHeight);
    int posX = (screenWidth - 800) / 2;
    int posY = (screenHeight - 600) / 2;
    SetPos(posX, posY);
    SetTitle("Browser", true);
    SetSizeable(false);
    SetMoveable(true);
    SetVisible(true);

    m_pHTML = new vgui::HTML(this, "HTMLPanel");
    m_pHTML->SetSize(800, 600);
    m_pHTML->SetPos(0, 60);

    m_pURLInput = new vgui::TextEntry(this, "URLInput");
    m_pURLInput->SetSize(500, 24);
    m_pURLInput->SetPos(10, 30);

    m_pGoButton = new vgui::Button(this, "GoButton", "Go", this, "Go");
    m_pGoButton->SetSize(50, 24);
    m_pGoButton->SetPos(520, 30);

    m_pBackButton = new vgui::Button(this, "BackButton", "Back", this, "Back");
    m_pBackButton->SetSize(50, 24);
    m_pBackButton->SetPos(580, 30);

    m_pForwardButton = new vgui::Button(this, "ForwardButton", "Forward", this, "Forward");
    m_pForwardButton->SetSize(60, 24);
    m_pForwardButton->SetPos(640, 30);

    m_pReloadButton = new vgui::Button(this, "ReloadButton", "Reload", this, "Reload");
    m_pReloadButton->SetSize(60, 24);
    m_pReloadButton->SetPos(710, 30);
}

CHTMLBrowser::~CHTMLBrowser()
{
}

void CHTMLBrowser::OpenURL(const char* url)
{
    m_pHTML->OpenURL(url, false);
    Q_strncpy(m_szCurrentURL, url, sizeof(m_szCurrentURL));
    UpdateURLField();
}

CHTMLBrowser* g_pHTMLBrowser = nullptr;

void ShowHTMLBrowser(vgui::VPANEL parent)
{
    if (!g_pHTMLBrowser)
    {
        g_pHTMLBrowser = new CHTMLBrowser(parent);
    }

    g_pHTMLBrowser->SetVisible(true);
    g_pHTMLBrowser->MoveToFront();
    g_pHTMLBrowser->Activate();
}

void CHTMLBrowser::OnCommand(const char* command)
{
    if (!Q_stricmp(command, "Go"))
    {
        char url[1024];
        m_pURLInput->GetText(url, sizeof(url));
        m_pHTML->OpenURL(url, false);
    }
    else if (!Q_stricmp(command, "Back"))
    {
        m_pHTML->GoBack();
    }
    else if (!Q_stricmp(command, "Forward"))
    {
        m_pHTML->GoForward();
    }
    else if (!Q_stricmp(command, "Reload"))
    {
        m_pHTML->Refresh();
    }
    else
    {
        BaseClass::OnCommand(command);
    }
}

void CHTMLBrowser::UpdateURLField()
{
    m_pURLInput->SetText(m_szCurrentURL);
}

CON_COMMAND(OpenBrowser, "Opens the browser!")
{
    ShowHTMLBrowser(enginevgui->GetPanel(PANEL_GAMEUIDLL));
    g_pHTMLBrowser->OpenURL("https://www.google.com");
}