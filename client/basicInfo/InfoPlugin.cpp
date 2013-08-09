#include "StdAfx.h"
#include "InfoPlugin.h"
#include "InfoFrom.h"
#include "macro.h"
#include "conn.h"

conn lastS = 0;
conn currS = 0;

InfoPlugin::InfoPlugin()
{
	strcpy(m_plugInfo.sAuthor, "greatyao");
	strcpy(m_plugInfo.sName, "系统信息");
	strcpy(m_plugInfo.sDesc, "远程计算机基本信息");
	m_plugInfo.uReserved = 0x0100;
}

InfoPlugin::~InfoPlugin()
{
}

void InfoPlugin:: OnCreateDocTemplate()
{
	m_docTemp = new CMultiDocTemplate(
		GetMinResId() + 1,
		RUNTIME_CLASS(PluginDoc),
		RUNTIME_CLASS(CMDIChildWnd),
		RUNTIME_CLASS(InfoFrom));
	ISite::AddDocTemplate(m_docTemp);
}


void InfoPlugin::OnCreateFrameMenu(CMenu* pMenu)
{
	pMenu->GetSubMenu(2)->AppendMenu(MF_STRING,GetMinResId() + 2, "系统信息");
}

void InfoPlugin::OnCreateFrameToolBar(ToolBarData* pArr,UINT& count)
{
	pArr->uIconId = IDB_BMP_BASICINFO;
	pArr->uId = GetMinResId() + 2;
	strcpy(pArr->sName, "系统信息");	
	strcpy(pArr->sTip, "远程计算机基本信息");	

	count = 1;
}

void InfoPlugin::OnCommand(UINT uId)
{
	if (uId == GetMinResId() + 2)
	{
		ISite::CreateFrameWnd(m_docTemp, RUNTIME_CLASS(InfoFrom), "系统基本信息");
		
		if(lastS == 0 || lastS != currS)
		//if(g_LastSTData.s == 0 || g_LastSTData.s != g_STData.s)
			ISite::SendCmd(CONTROL_QUERY_INFO, currS, NULL, 0);
	}
}

void InfoPlugin::OnCreateDockPane(DockPane* pArr,UINT& count)
{	
	count = 0;
}

void InfoPlugin::OnNotify(UINT id, WPARAM wp, LPARAM lp)
{
	if(wp == NULL)	return;

	InfoFrom* pView = NULL;
	POSITION posdoc = m_docTemp->GetFirstDocPosition();
	if(posdoc)
	{
		CDocument* pdoc = m_docTemp->GetNextDoc(posdoc);
		POSITION posview = pdoc->GetFirstViewPosition();
		pView = (InfoFrom*)(pdoc->GetNextView(posview));
	}

	if(id == ZCM_SEL_OBJ)//选择了新的目标机
	{
		STANDARDDATA* pSD = (STANDARDDATA*)wp;
		if(currS == pSD->s)	return;
		
		lastS = currS;
		currS = pSD->s;
		if( pView)
		{
			if(IsCurrentWorking())
				ISite::SendCmd(CONTROL_QUERY_INFO, currS, NULL, 0);
		}
	}
	else if(IsCurrentWorking() == false)	//当前插件不在前端
		return;
	else if(id == ZCM_WM_OFFLINE)//目标机下线	   
	{
		STANDARDDATA* pSD = (STANDARDDATA*)wp;
		if(currS == pSD->s)
		{
			currS = 0;
		}
	}
	else if(id == ZCM_RECV_MSG)//收到目标机的发送回来的消息
	{
		DispatchData* data = (DispatchData*)wp;
		void* s = data->s;
		control_header* header = data->recvData.header;
		unsigned short cmd = header->command;
		unsigned short resp = header->response;

		if(cmd != CONTROL_QUERY_INFO)
			return;

		if(resp != 0)	pView->OnQueryFailed(cmd, resp);
		else			
		{
			struct TrojanInfo info;
			ISite::Service(ZCM_GET_OBJ, (WPARAM)s, (LPARAM)&info);
			pView->InsertData(data->recvData.body, &info);
		}
	}
}

BOOL InfoPlugin::OnInitalize()
{
	return TRUE;
}

BOOL InfoPlugin::OnUninitalize()
{
	return TRUE;
}

void InfoPlugin::OnCommandUI(CCmdUI* pCmdUI)
{

}
