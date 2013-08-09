// ProcessForm.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ProcessForm.h"
#include "ProcessPlugin.h"
#include "macro.h"

// ProcessForm

IMPLEMENT_DYNCREATE(ProcessForm, CFormView)

ProcessForm::ProcessForm()
	: CFormView(ProcessForm::IDD)
{
}

ProcessForm::~ProcessForm()
{
}

void ProcessForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

	//DDX_Control(pDX, IDC_UP_DIR, m_btnUpDir);
}

BEGIN_MESSAGE_MAP(ProcessForm, CFormView)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, OnRclickItem)
	ON_COMMAND(ID_KILL_PROCESS, OnKillProcess)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, OnLvnColumnclickList1)
END_MESSAGE_MAP()


// ProcessForm diagnostics

#ifdef _DEBUG
void ProcessForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void ProcessForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void ProcessForm::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(&rect);
	int W = rect.Width(), H = rect.Height();

	//重新排列各个控件的相对关系
	CWnd* pWnd = this->GetDlgItem(IDC_LIST1);
	if( pWnd != NULL)
		pWnd->MoveWindow(0, 0, W, H);

}

void ProcessForm::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	
	m_listCtrl.Create(//WS_VISIBLE|LVS_REPORT|LVS_EDITLABELS, 
		WS_CHILD|WS_TABSTOP|WS_VISIBLE|WM_VSCROLL,
		CRect(100, 100, 300, 300), this, IDC_LIST1);
	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EDITLABELS);

	m_listCtrl.InsertColumn(0, "映像名称", LVCFMT_LEFT, 240);
	m_listCtrl.InsertColumn(1, "PID", LVCFMT_LEFT, 100);
}

void ProcessForm::InsertData(void* body, int dataLen, int seq)
{
	process_info* pros = (process_info*)body;
	if(dataLen < 0)	return;

	if(seq == 0 || seq == 1)
	{
		m_listCtrl.DeleteAllItems();
	}
	if(dataLen < 0)	return;
		
	int nProcs = dataLen / sizeof(process_info);

	for(int i = 0; i < nProcs; i++)
	{
		int nItem = m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), pros[i].name);
		if(nItem == -1)	continue;
	
		CString pid;
		pid.Format("%d", pros[i].id);
		m_listCtrl.SetItemText(nItem, 1, pid);
	}
}

void ProcessForm::OnRclickItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	if(m_listCtrl.GetSelectedCount() <= 0)
		return;
	
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, ID_KILL_PROCESS,"杀死进程");

	CPoint pt;
	GetCursorPos(&pt);
	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
	menu.DestroyMenu();
}

void ProcessForm::OnKillProcess()
{
	if(m_listCtrl.GetSelectedCount() <= 0)
		return;

	int nItem = m_listCtrl.GetNextItem(-1, LVNI_SELECTED);

	char *lpTips = "警告: 终止进程会导致不希望发生的结果，\n"
		"包括数据丢失和系统不稳定。在被终止前，\n"
		"进程将没有机会保存其状态和数据。";
	
	CString title;
	title.Format("是否要结束%s？\n\n%s",  m_listCtrl.GetItemText(nItem, 0), lpTips);	
	
	if (::MessageBox(m_hWnd, title, "进程结束警告", MB_YESNO|MB_ICONQUESTION) == IDNO)
		return;

	CString str = m_listCtrl.GetItemText(nItem, 1);
	int pid = atoi(str);
	ProcessPlugin::KillProcess(pid);
}

void ProcessForm::OnOperationFailed(unsigned short cmd, unsigned short resp)
{
	CString errMsg;
	if(resp == ERR_NOTHISMODULE)
		errMsg = "木马服务端不支持该模块，请上传相应的插件";
	else if(resp == ERR_INVPARA)
		errMsg = "参数不正确，不合法的命令";
	else if(resp == ERR_NOTFIND)
		errMsg = "进程不存在";
	else if(resp == ERR_DENIED)
		errMsg = "没有权限操作";
	else
		errMsg = "未知错误";
	MessageBox(errMsg, "进程管理", MB_OK|MB_ICONHAND);
}

void ProcessForm::ProcessData(WPARAM wp)
{
	DispatchData* data = (DispatchData*)wp;
	control_header* header = data->recvData.header;
	unsigned short cmd = header->command;
	unsigned short resp = header->response;
	int dataLen = header->dataLen;
	int seq = header->seq;
	void* body = data->recvData.body;
	
	if(cmd == CONTROL_ENUM_PROCS)
	{
		if(resp != 0)	OnOperationFailed(cmd, resp);
		else			InsertData(body, dataLen, seq);
	}
	else if(resp != 0)	OnOperationFailed(cmd, resp);
}

static bool isNum = false;

static int CALLBACK  MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	if(isNum == false)
	{
		CString &lp1 = *((CString *)lParam1);
		CString &lp2 = *((CString *)lParam2);
		int &sort = *(int *)lParamSort;
		if (sort == 0)	return lp1.CompareNoCase(lp2);
		else			return lp2.CompareNoCase(lp1);
	}
	 else
	 {
		 int lp1 = atoi(*((CString *)lParam1));
		 int lp2 = atoi(*((CString *)lParam2));
		 int &sort = *(int *)lParamSort;
		 if (sort == 0)	return lp1 < lp2;
		 else			return lp2 < lp1;
	 }
}

void ProcessForm::OnLvnColumnclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
     LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
     int Length = m_listCtrl.GetItemCount();
     CArray<CString,CString> ItemData;
     ItemData.SetSize(Length);

     for (int i = 0; i < Length; i++)
     {
         ItemData[i] = m_listCtrl.GetItemText(i,pNMLV->iSubItem);
         m_listCtrl.SetItemData(i,(DWORD_PTR)&ItemData[i]);//设置排序关键字
	 }

     static int sort = 0;
     static int SubItem = 0;

     if (SubItem != pNMLV->iSubItem)
     {
         sort = 0;
         SubItem = pNMLV->iSubItem;
     }
     else
     {
         sort = 1 - sort;
     }

	 if(SubItem == 1) isNum = true;
	 else			isNum = false;

     m_listCtrl.SortItems(MyCompareProc,(DWORD_PTR)&sort);//排序
     *pResult = 0;
}