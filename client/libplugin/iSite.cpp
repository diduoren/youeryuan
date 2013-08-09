#include "stdafx.h"
#include "iPlug.h"
#include "PluginApp.h"
#include "PluginMainFrame.h"
#include "pluginMgr.h"
#include "MyProgress.h"
#include "SEU_QQwry.h"

struct TrojanInfo ISite::m_current;
bool ISite::m_bIsQQwryExist = false;
SEU_QQwry* ISite::m_QQwry;


void ISite::AddDocTemplate(CMultiDocTemplate * temp)
{
	PluginApp *pApp = static_cast<PluginApp*>(AfxGetApp());
	pApp->AddDocTemplate(temp);
}

CFrameWnd* ISite::CreateFrameWnd(CMultiDocTemplate* pTpl, CRuntimeClass* pClass, const char* title)
{
	CDocument* pDoc = NULL;
	POSITION pos = pTpl->GetFirstDocPosition();
	if (pos == NULL)
	{
		pDoc = pTpl->OpenDocumentFile(NULL);
		pDoc->SetTitle(title);
	}
	else
		pDoc =  pTpl->GetNextDoc(pos);
	
	if (pDoc == NULL)
		return NULL;

	CFrameWnd* pFrame = NULL;
	pos = pDoc->GetFirstViewPosition();
	while(pos != NULL)
	{
		CView* pView = pDoc->GetNextView(pos);
		if(pView->IsKindOf(pClass))
		{
			pFrame = (CFrameWnd*)(pView->GetParentFrame());
			pFrame->ActivateFrame(SW_SHOWNORMAL);
			return pFrame;
		}
	}
	pFrame = pTpl->CreateNewFrame(pDoc, NULL);
	pTpl->InitialUpdateFrame(pFrame, NULL);
	
	return pFrame;
}

CWnd* ISite::GetMainFrameWnd()
{
	PluginApp *pApp = static_cast<PluginApp*>(AfxGetApp());
	return pApp->GetMainWnd();
}

BOOL ISite::Service(UINT serviceId,WPARAM wparam,LPARAM lparam)
{
	return PluginMgr::GetInstance()->OnService(serviceId,wparam,lparam);
}

void ISite::Notify(UINT notifyId,WPARAM wparam,LPARAM lparam)
{
	PluginMgr::GetInstance()->OnNotify(notifyId,wparam,lparam);
}


struct TrojanInfo* ISite::GetSelectedTarget()
{
	return &m_current;
}

void ISite::SetStatus(int nIndex, const char* fmt, ...)
{
	if(nIndex < 0 || nIndex > 3)	return;

	PluginApp *pApp = static_cast<PluginApp*>(AfxGetApp());
	PluginMainFrame* frame = (PluginMainFrame*)pApp->GetMainWnd();
	char msg[4096] = {0};
	va_list args;

	va_start(args, fmt);
	vsprintf(msg, fmt, args);
	va_end(args);
	frame->m_wndStatusBar.SetPaneText(nIndex, msg);
}
	
void ISite::SetCurrentTarget(const struct TrojanInfo* info)
{
	static struct TrojanInfo zeroInfo = {0};
	if(info == NULL)
		info = &zeroInfo;
	memcpy(&m_current, info, sizeof(*info));
}


int ISite::SendCmd(unsigned short cmd, void* s, char* body /* = NULL */, int n /* = 0 */)
{
	DispatchData data;
	memset(&data, 0, sizeof(data));
	data.s = s;
	data.sendData.cmd = cmd;
	data.sendData.body = body;
	data.sendData.dataLen = n;

	return ISite::Service(ZCM_SEND_MSG, (WPARAM)&data, NULL);
}

void ISite::ShowDockPane(UINT uId)
{
	PluginApp *pApp = static_cast<PluginApp*>(AfxGetApp());
	PluginMainFrame* frame = (PluginMainFrame*)pApp->GetMainWnd();
	if (frame->m_paneMgr->IsPaneClosed(uId))
		frame->m_paneMgr->ShowPane(uId);
	else
		frame->m_paneMgr->ClosePane(uId);
}

void ISite::ShowDockPane(UINT uId,BOOL bShow)
{
	PluginApp *pApp = static_cast<PluginApp*>(AfxGetApp());
	PluginMainFrame* main = (PluginMainFrame*)pApp->GetMainWnd();

	if (bShow)
		main->m_paneMgr->ShowPane(uId);
	else
		main->m_paneMgr->ClosePane(uId);
}

void ISite::WriteLog(const char* fmt, ...)
{
	char log[4096] = {0};

	va_list args;
	va_start(args, fmt);
	int n = vsprintf(log, fmt, args);
	va_end(args);

	ISite::Service(ZCM_WM_WRITE_LOG, (WPARAM)log, NULL);
}

CString ISite::GetIPAddrPort(const char* ip, int port)
{
	CString text;
	text.Format("%s:%d", ip, port);
	return text;
}

CString ISite::GetRealAddr(const struct TrojanInfo* info)
{
	CString addr;
	if(m_bIsQQwryExist)
		addr = m_QQwry->IPtoAdd(info->trojanip);
	else
		addr = "";

	return addr;
}

void ISite::OnNotifyProgress(WPARAM wp, LPARAM lp)
{
	ISite::GetMainFrameWnd()->SendMessage(ZCM_PROGRESS, wp, lp);
}

CWnd* ISite::CreateProgressDlg(int isUpOrDown)
{
	StructProgressNotify structDlg;
	structDlg.id = CREATE_DLG;
	structDlg.data = isUpOrDown;
	ISite::OnNotifyProgress((WPARAM)&structDlg, (LPARAM)&structDlg);
	return structDlg.dlg;
}

void ISite::DestoryDlg(CWnd* dlg)
{
	StructProgressNotify structDlg = {DESTROY_DLG, dlg, 0};
	ISite::OnNotifyProgress((WPARAM)&structDlg, (LPARAM)&structDlg);
}

void ISite::SetSource(CWnd* dlg,const char* src)
{
	StructProgressNotify structDlg = {INIT_SRC, dlg, (UINT_PTR)src};
	ISite::OnNotifyProgress((WPARAM)&structDlg, (LPARAM)&structDlg);
}
	

void ISite::SetDestination(CWnd* dlg,const char* dst)
{
	StructProgressNotify structDlg = {INIT_DST, dlg, (UINT_PTR)dst};
	ISite::OnNotifyProgress((WPARAM)&structDlg, (LPARAM)&structDlg);
}

void ISite::SetSubDir(CWnd* dlg,const char* dir)
{
	StructProgressNotify structDlg = {SET_DIR, dlg, (UINT_PTR)dir};
	ISite::OnNotifyProgress((WPARAM)&structDlg, (LPARAM)&structDlg);
}

void ISite::SetSize(CWnd* dlg, __int64 nowSize)
{
	StructProgressNotify structDlg = {SET_SIZE, dlg, (UINT_PTR)&nowSize};
	ISite::OnNotifyProgress((WPARAM)&structDlg, (LPARAM)&structDlg);
}

void ISite::SetTransFile(CWnd* dlg, const char* name)
{
	StructProgressNotify structDlg = {SET_NAME, dlg, (UINT_PTR)name};
	ISite::OnNotifyProgress((WPARAM)&structDlg, (LPARAM)&structDlg);
}

void ISite::SetProgress(CWnd* dlg, __int64 nowSize)
{
	StructProgressNotify structDlg = {SET_PROGRESS, dlg, (UINT_PTR)&nowSize};
	ISite::OnNotifyProgress((WPARAM)&structDlg, (LPARAM)&structDlg);
}

void ISite::SetSpeed(CWnd* dlg, __int64 speed, int isTime)
{
	StructProgressNotify structDlg = {SET_SPEED, dlg, (UINT_PTR)&speed};
	if(isTime)	structDlg.id = SET_TIME;

	ISite::OnNotifyProgress((WPARAM)&structDlg, (LPARAM)&structDlg);
}