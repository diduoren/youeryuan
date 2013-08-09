#include "StdAfx.h"
#include "ScreenPlugin.h"
#include "ScreenForm.h"
#include "macro.h"
#include "conn.h"
#include "Buffer.h"

#include <set>
#include <process.h>

using namespace std;

conn lastS = 0;
conn currS = 0;
bool willClose = false;
DispatchData gParam;
ScreenForm*	gView;
CFrameWnd* gFrame;
set<conn>	gSets;

static void CloseOthers()
{
	for(set<conn>::iterator i = gSets.begin(); i != gSets.end(); i++)
	{
		__try
		{
			if(*i != currS)
				ISite::SendCmd(CONTROL_CLOSE_SCREEN, *i, NULL, 0);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			KDebug("CloseOthers exception");
		}
	}
}

ScreenPlugin::ScreenPlugin()
{
	strcpy(m_plugInfo.sAuthor, "greatyao");
	strcpy(m_plugInfo.sName, "屏幕监控");
	strcpy(m_plugInfo.sDesc, "远程计算机屏幕查看");
	m_plugInfo.uReserved = 0x0100;
}

ScreenPlugin::~ScreenPlugin()
{
}

void ScreenPlugin:: OnCreateDocTemplate()
{
	m_docTemp = new CMultiDocTemplate(
		GetMinResId() + 1,
		RUNTIME_CLASS(PluginDoc),
		RUNTIME_CLASS(CMDIChildWnd),
		RUNTIME_CLASS(ScreenForm));
	ISite::AddDocTemplate(m_docTemp);
}


void ScreenPlugin::OnCreateFrameMenu(CMenu* pMenu)
{
	pMenu->GetSubMenu(2)->AppendMenu(MF_STRING, GetMinResId() + 2, "屏幕监控");
}

void ScreenPlugin::OnCreateFrameToolBar(ToolBarData* pArr,UINT& count)
{
	pArr->uIconId = IDB_BITMAPSCREEN;
	pArr->uId = GetMinResId() + 2;
	strcpy(pArr->sName, "屏幕监控");	
	strcpy(pArr->sTip, "远程计算机屏幕查看");	

	count = 1;
}

void ScreenPlugin::OnCommand(UINT uId)
{
	if (uId == GetMinResId() + 2)
	{
		gFrame = ISite::CreateFrameWnd(m_docTemp, RUNTIME_CLASS(ScreenForm), "屏幕监控");
		willClose = false;
		CloseOthers();
		if(currS)
			ISite::SendCmd(CONTROL_MONITOR_SCREEN, currS, NULL, 0);
	}
}

void ScreenPlugin::OnCreateDockPane(DockPane* pArr,UINT& count)
{	
	count = 0;
}

void ScreenPlugin::OnNotify(UINT id, WPARAM wp, LPARAM lp)
{
	if(wp == NULL)	return;

	ScreenForm* pView = NULL;
	POSITION posdoc = m_docTemp->GetFirstDocPosition();
	while(posdoc != NULL)
	{
		CDocument* pdoc = m_docTemp->GetNextDoc(posdoc);
		POSITION posview = pdoc->GetFirstViewPosition();
		pView = (ScreenForm*)(pdoc->GetNextView(posview));
	}

	if(willClose == true)	return;

	if(id == ZCM_SEL_OBJ)//选择了新的目标机
	{
		STANDARDDATA* pSD = (STANDARDDATA*)wp;
		if(currS == pSD->s)	return;
		
		lastS = currS;
		currS = pSD->s;
		gSets.insert(currS);

		if( pView)
		{
			//关闭前面的视频
			CloseOthers();
			//打开现在用户的视频
			if(IsCurrentWorking())
				ISite::SendCmd(CONTROL_MONITOR_SCREEN, currS, NULL, 0);
		}
	}
	else if(id == ZCM_WM_SCREEN)
	{
		gView = pView;
		memcpy(&gParam, (void*)wp, sizeof(gParam));
		_beginthreadex(NULL, 0, screenThread, NULL, 0, NULL);
	}
	else if(id == ZCM_WM_OFFLINE)
	{
		STANDARDDATA* pSD = (STANDARDDATA*)wp;
		gSets.erase(pSD->s);
	}
	else if(IsCurrentWorking() == false)//当前插件不在前端	
		return;
	else if(id == ZCM_RECV_MSG)//收到目标机的发送回来的消息
	{
		DispatchData* data = (DispatchData*)wp;
		control_header* header = data->recvData.header;
		unsigned short cmd = header->command;
		if(cmd < CONTROL_MONITOR_SCREEN || cmd > CONTROL_CLOSE_SCREEN)
			return;
		pView->ProcessData(wp);
	}
}

unsigned ScreenPlugin::screenThread(void* p)
{
	int nStatus = 0;
	conn ftpfd = gParam.s;
	int listenType = gParam.recvData.type;
	int imgSize = 20000;
	char* img = new char[20000];
	int (*myWrite)(conn, char*, char*, int, int) = gParam.WriteFunction;
	int (*myRead)(conn, char*, int, int, int, int) = gParam.readFunction;
	int flags = listenType == CONN_HTTP_SERVER ? 0 : 1;
	
	while(1)
	{
		__try
		{
			struct control_header header = {0};

			//首先读取屏幕控制信息
			int nRead = myRead(ftpfd, (char*)&header, sizeof(header), 0, 0, flags);
			//KDebug("Read %d hdr frame %d %x %d", nRead, header.reserved, header.command, header.dataLen);
			if(nRead < 0 || (nRead == 0 && listenType != CONN_HTTP_SERVER))
			{
				KDebug("Read from %p failed", ftpfd);
				nStatus = -1; 
				goto leave; 
			}

			if(!ISVALID_HEADER(header))
			{
				KDebug("screenThread ISVALID_HEADER = FALSE");
				continue;
			}

			if(header.dataLen > 0)
			{
				//然后读取屏幕图像数据
				nRead = myRead(ftpfd, img, header.dataLen, header.isCompressed, imgSize, flags);
				//KDebug("Read %d data", nRead);
				if(nRead > 0 || (nRead == 0 && listenType == CONN_HTTP_SERVER))	
					header.dataLen = nRead; 
				else
				{
					if(nRead <= -2)	
					{
						KDebug("Read data encrypt from %p failed", ftpfd);
						continue;
					}
					else	
					{
						KDebug("Connection %p lost", ftpfd);
						nStatus = -1;
						break;
					}
				}
			}

			//处理图像数据
			unsigned short cmd = header.command;
			unsigned short resp = header.response;
			int dataLen = header.dataLen;
			int seq = header.seq;
			static CBuffer recvBuf;

			if(resp != 0)
			{
				gView->OnOperationFailed(cmd, resp);
				continue;
			}

			if(seq == 1 && dataLen > 0)
				recvBuf.ClearBuffer() ;

			if(dataLen > 0)	recvBuf.Write((PBYTE)img, dataLen);
			else
			{
				if(cmd == CONTROL_INFO_SCREEN)
					gView->ResetScreen(&recvBuf);
				else if(cmd == CONTROL_FIRST_SCREEN)
				{
					gView->DrawFirstScreen(&recvBuf);
					gFrame->ActivateFrame(SW_MINIMIZE);
					gFrame->ActivateFrame(SW_MAXIMIZE|SW_NORMAL);
				}
				else if(cmd == CONTROL_NEXT_SCREEN)
					gView->DrawNextScreenRect(&recvBuf);
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			KDebug("screenThread exception");
		}
	}

leave:
	delete []img;
	return nStatus;
}

BOOL ScreenPlugin::OnInitalize()
{
	return TRUE;
}

BOOL ScreenPlugin::OnUninitalize()
{
	return TRUE;
}

void ScreenPlugin::OnCommandUI(CCmdUI* pCmdUI)
{

}

void ScreenPlugin::SendScreenCmd(int cmd)
{
	if(cmd == CONTROL_CLOSE_SCREEN)
		willClose = true;

	ISite::SendCmd(CONTROL_CLOSE_SCREEN, currS, NULL, 0);
}
