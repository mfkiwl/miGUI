﻿#include "framework/mgf.h"
#include "framework/BackendGDI.h"
#include "framework/BackendOpenGL.h"
#include "framework/SystemWindow.h"
#include "framework/Window.h"
#include "framework/Rectangle.h"
#include "framework/Font.h"
#include "framework/Text.h"
#include "framework/Button.h"
#include "framework/Icons.h"
#include "framework/TextInput.h"
#include "framework/ListBox.h"
#include "framework/Table.h"
#include "framework/TextBuffer.h"
#include "framework/Popup.h"
#include "framework/AudioEngine.h"

#include "ap.h"
#include "playlist.h"

#include <list>
#include <filesystem>

AP_application* g_app = 0;

#define AP_PLAYLISTAREASIZE 180
#define AP_CONTROLAREASIZE 70

class WindowMain : public mgf::SystemWindow
{
public:
	WindowMain(int windowFlags, const mgPoint& windowPosition, const mgPoint& windowSize);
	virtual ~WindowMain() {}
	virtual bool OnClose() override;
	virtual void OnSize() override;
};

WindowMain::WindowMain(int windowFlags, const mgPoint& windowPosition, const mgPoint& windowSize)
	:
	mgf::SystemWindow(windowFlags, windowPosition, windowSize)
{
	GetSizeMinimum()->x = 700;
	GetSizeMinimum()->y = 500;
}

bool WindowMain::OnClose()
{
	return MessageBoxA(
		GetOSData()->hWnd,
		"Close app",
		"Are you sure?",
		MB_ICONQUESTION | MB_YESNO)
		== IDYES ? 1 : 0;
}

void WindowMain::OnSize()
{
	AP_application* app = static_cast<AP_application*>(GetUserData());

	if (!app->m_guiWindow)
		return;
	mgf::SystemWindow::OnSize();
	auto & sz = GetSize();
	app->m_guiWindow->SetSize(sz.x, sz.y);
	
	if(app->m_playlistArea)
		app->m_playlistArea->SetRect(0, 0, AP_PLAYLISTAREASIZE, sz.y - AP_CONTROLAREASIZE);
	if(app->m_controlArea)
		app->m_controlArea->SetRect(0, sz.y - AP_CONTROLAREASIZE, sz.x, sz.y);
	if(app->m_tracklistArea)
		app->m_tracklistArea->SetRect(AP_PLAYLISTAREASIZE, 0, sz.x, sz.y - AP_CONTROLAREASIZE);

	if (app->m_listboxPlaylist)
		app->m_listboxPlaylist->SetRect(5, 25, AP_PLAYLISTAREASIZE, sz.y - AP_CONTROLAREASIZE);
	if (app->m_tableTracklist)
		app->m_tableTracklist->SetRect(AP_PLAYLISTAREASIZE, 0, sz.x, sz.y - AP_CONTROLAREASIZE);
}

//void window_onDropFiles(uint32_t num, uint32_t i, const wchar_t* fn, int x, int y)
//{
//	g_data.playlistMgr->AddTrackToEditPlaylist(fn);
//}

AP_application::~AP_application()
{
}

bool AP_application::Init(backend_type bt)
{
	g_app = this;

	m_framework = mgf::InitFramework();
	m_sysWindow = new WindowMain(MGWS_OVERLAPPEDWINDOW,
		mgPoint(MGCW_USEDEFAULT, 0),
		mgPoint(MGCW_USEDEFAULT, 0));
	m_sysWindow->SetUserData(this);
	
	m_style = m_framework->GetNewStyle(1);
	m_style.listItemBG1.setAsIntegerARGB(0xFFBFDDFF);
	m_style.listItemBG2.setAsIntegerARGB(0xFFBFDDFF);
	m_style.listItemHoverBG.setAsIntegerARGB(0xFFA8D2FF);

	m_audio = new mgf::AudioEngine;
	m_backend = 0;
	switch (bt)
	{
	case backend_type::Gdi:
		m_backend = new mgf::BackendGDI;
		break;
	case backend_type::OpenGLOld:
	{
		mgf::BackendOpenGLParams oglParams;
		m_backend = new mgf::BackendOpenGL(oglParams);
	}break;
	default:
		break;
	}
	
	m_context = m_framework->CreateContext(m_sysWindow, m_backend);
	m_fontImpact = m_context->GetBackend()->CreateFont(L"Impact", 20, true, false);
	m_popupFont = m_context->GetBackend()->CreateFont(L"Arial", 9, false, false);
	m_listboxFont = m_context->GetBackend()->CreateFont(L"Arial", 9, false, false);
	m_context->SetDefaultPopupFont(m_popupFont);

	m_sysWindow->SetVisible(true);
	
	m_guiWindow = new mgf::Window(m_context);
	m_guiWindow->SetTitle(L"Window", 0);
	m_guiWindow->SetWithCloseButton(false);
	m_guiWindow->SetWithCollapseButton(false);
	m_guiWindow->SetWithTitlebar(false);
	//m_guiWindow->SetDrawBG(false);
	m_guiWindow->SetCanMove(false);
	m_guiWindow->SetCanResize(false);
	m_guiWindow->SetCanToTop(false);
	m_guiWindow->SetSize(
		m_context->GetSystemWindow()->GetSize().x,
		m_context->GetSystemWindow()->GetSize().y);
	/*g_data.playlistArea = guiWindow.m_data->AddRectangle();
	g_data.playlistArea->SetColor(0xFF77ADFF);

	g_data.controlArea = guiWindow.m_data->AddRectangle();
	g_data.controlArea->SetColor(0xFFBAC5C6);
		
	g_data.tracklistArea = guiWindow.m_data->AddRectangle();
	g_data.tracklistArea->SetColor(0xFFE1EEEF);*/
	//g_data.buttonNewPlaylist = guiWindow.m_data->AddButton();
	//g_data.buttonNewPlaylist->SetPositionAndSize(0, 5, 180, 18); //SetPositionAndSize is more comfortable
	//g_data.buttonNewPlaylist->SetDrawBG(false);
	//g_data.buttonNewPlaylist->onReleaseLMB = Playlist_BTN_newPL_onRelease;
	
	m_icons = m_framework->CreateIcons("../data/ap/icons.png", m_context->GetBackend());
	if (m_icons)
	{
		int iconID1 = m_icons->Add(0, 0, 180, 17);
		int iconID2 = m_icons->Add(0, 19, 180, 17);
		//g_data.buttonNewPlaylist->SetIcons(icons.m_data, iconID1, iconID2, iconID1, iconID1);
	}
	
	//auto window2 = context.m_data->CreateWindow();
	/*auto textInput = guiWindow.m_data->AddTextInput(fontImpact.m_data);
	textInput->SetRect(50, 200, 500, 250);
	textInput->SetText(L"Hello world");*/
	
	///*	g_data.listboxPlaylist = guiWindow.m_data->AddListBox(listboxFont.m_data);
	//	g_data.listboxPlaylist->SetRect(50, 200, 200, 300);
	//	g_data.listboxPlaylist->SetItemHeight(listboxFont.m_data->GetMaxSize().y);
	//	g_data.listboxPlaylist->SetDrawBG(false);
	//	g_data.listboxPlaylist->CanEdit(true);
	//	g_data.listboxPlaylist->SetDrawItemBG(true);
	//	g_data.listboxPlaylist->onTextInputCharEnter = Playlist_LB_onTextInputCharEnter;
	//	g_data.listboxPlaylist->onTextInputEndEdit = Playlist_LB_onTextInputEndEdit;
	//	g_data.listboxPlaylist->SetTextInputCharLimit(100);*/
		//g_data.listboxPlaylist->SetData(listboxData, 10);
		
	//	g_data.tableTracklist = guiWindow.m_data->AddTable(5, listboxFont.m_data);// initialization in PlayListManager
	
	m_playlistMgr = new PlayListManager();
	
	if(m_listboxPlaylist)
		m_listboxPlaylist->SetUserStyle(&m_style);
	
	// also rebuild all gui
	m_context->GetSystemWindow()->OnSize();
	return true;
}

void AP_application::Run()
{
	bool sleep = true;

	while (m_framework->Run())
	{
		if (sleep)
			Sleep(1);

		/*if (context.m_data->m_input->mouseButtonFlags1 & MG_MBFL_RMBDOWN)
		{
			testPopup->Show(context.m_data->m_input->mousePosition.x, context.m_data->m_input->mousePosition.y);
		}*/

		m_framework->DrawAll();
	}
}