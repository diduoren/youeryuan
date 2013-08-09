                              // ConsoleEdit.cpp : implementation file
//

#include "stdafx.h"
#include "ConsoleEdit.h"
#include "Resource.h"
#include "macro.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConsoleEdit

CConsoleEdit::CConsoleEdit()
{
	m_clrText = RGB( 192, 192, 192 );
	m_clrBkgnd = RGB( 0, 0, 0 );
	m_brBkgnd.CreateSolidBrush(m_clrBkgnd);

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));       // zero out structure
	lf.lfWeight = FW_NORMAL;
	lf.lfCharSet = GB2312_CHARSET;//ANSI_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	strcpy(lf.lfFaceName, _T("FixedSys"));
	m_font.CreateFontIndirect(&lf);  

	m_pretitle = ">";
	memset(m_cmd,0,sizeof(m_cmd));
	m_ischinese = false;
	m_save_index = 0;
	m_can_input = true;
	
	m_bcopy = false;
}

CConsoleEdit::~CConsoleEdit()
{
}


BEGIN_MESSAGE_MAP(CConsoleEdit, CEdit)
	//{{AFX_MSG_MAP(CConsoleEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_TIMER()
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetfocus)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConsoleEdit message handlers


HBRUSH CConsoleEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	pDC->SetTextColor( m_clrText );		// text
	pDC->SetBkColor( m_clrBkgnd );		// text bkgnd

	return m_brBkgnd;					// ctl bkgnd
}

void CConsoleEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( nChar == 13 )
	{
		MoveToEnd();
		Refresh();
		CString allText;
		GetWindowText(allText);
		allText = allText.Mid(m_last_title_pos);
		strncpy(m_cmd,allText,
			allText.GetLength() > (sizeof(m_cmd)-1)
			?
			sizeof(m_cmd)-1 : allText.GetLength()
			);
		
		// 回车
		CEdit::OnChar(nChar, nRepCnt, nFlags);
		// 保存命令
		SaveCommand(m_cmd);
		// 发送给父窗口
		LRESULT result = SendMessageToParent(m_cmd,NM_CONSOLE_ENTER);
		// 清空命令
		memset(m_cmd,0,sizeof(m_cmd));
		// 根据父窗口返回值
		// 父窗口返回0,表示处理完毕，需要返回初始命令状态
		if (!result)
		{
			// 回车(空一行)
			CEdit::OnChar(nChar, nRepCnt, nFlags);
			// 添加初始命令提示
			AddTexts(m_pretitle);
		}
		return;
	}
	else
	{
		if (nChar == 8)
		{
			if (GetCurrentPosition() <= m_last_title_pos)
				return;
		}
		// 输入必须在m_last_title_pos之后
		if (GetCurrentPosition() < m_last_title_pos)
			MoveToEnd();

	}
/**************改用字符比较********************/
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

// 得到最后的位置
long CConsoleEdit::GetCurrentPosition()
{
	SetSel(-1,-1);
	int nstart,nstop;
	GetSel(nstart,nstop);
	return nstart;
}

// 移动到最后
void CConsoleEdit::MoveToEnd()
{
	m_nLength = SendMessage( WM_GETTEXTLENGTH );
	SetSel( m_nLength,m_nLength );
}

// 增加字符
// CEdit显示内容有限制，当超过该限制时，需要删除原来的内容，再增加内容
void CConsoleEdit::AddTexts(const char* str, int len)
{
	if(len == -1)	len = strlen(str);
	//KDebug("%d %d", m_nLength, len);
	if ((m_nLength + len) > MAX_DATA_NUM)
	{
		SetSel(0, len + 128, TRUE);
		ReplaceSel("");
		if (len > MAX_DATA_NUM)
		{
			CString result(str);
			result = result.Right(MAX_DATA_NUM);
			ReplaceSel(result);
			return;
		}
	}
	MoveToEnd();
	ReplaceSel(str);

	if (strcmp(str,m_pretitle)==0)
		m_last_title_pos = SendMessage( WM_GETTEXTLENGTH );
}

int CConsoleEdit::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	Clear();
	SetFont(&m_font);
	// 这个是为了显示的时候初始内容不被选中
	SetTimer(1,1,0);

	return 0;
}

void CConsoleEdit::AddTexts(char ch)
{
	char str[2];
	str[0] = ch;
	str[1] = '\0';
	AddTexts(str);
}


BOOL CConsoleEdit::Create(const RECT& rect, CWnd* pParentWnd, UINT nID )
{
	return CreateEx(WS_EX_CLIENTEDGE,_T("EDIT"),"",
			WS_CHILD | WS_VISIBLE | WS_VSCROLL 
			| ES_AUTOVSCROLL| ES_MULTILINE | ES_WANTRETURN,
			rect,pParentWnd,nID);
}

void CConsoleEdit::SaveCommand(const char *cmd)
{
	if (m_save_index > SAVE_CMD_NUM - 1)
	{
		m_save_index = 0;
	}
	if (strlen(cmd) !=0)
	{
		m_save_cmd[m_save_index] = cmd;
		m_save_index++;
	}
}

CString CConsoleEdit::GetCommand(BOOL bUp)
{
	if (bUp)
	{
		m_save_index--;
		if (m_save_index<0)
			m_save_index = SAVE_CMD_NUM - 1;
	}
	else
	{
		m_save_index++;
		if (m_save_index > SAVE_CMD_NUM - 1)
			m_save_index = 0;
	}
	return m_save_cmd[m_save_index];
}

void CConsoleEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (!m_can_input)
		return;

	// 向上的按键
	if (nChar == 38)
	{
		strcpy(m_cmd,GetCommand());
		ReplaceCommand(m_cmd);
		return;
	}
	// 向下
	if (nChar == 40)
	{
		strcpy(m_cmd,GetCommand(FALSE));
		ReplaceCommand(m_cmd);
		return;
	}

	// left
	if (nChar == 37)
	{
		int nPos = GetCurrentPosition();
		if (nPos <= m_last_title_pos)
			return;
	}
	// delete
	if (nChar == 46)
	{
		int nPos = GetCurrentPosition();
		if (nPos < m_last_title_pos)
			return;
	}
	/**************改用字符比较********************/
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CConsoleEdit::ReplaceCommand(const char* cmd)
{
	int nLen = SendMessage( WM_GETTEXTLENGTH );
	SetSel(m_last_title_pos,nLen);
	ReplaceSel(cmd);
}

LRESULT CConsoleEdit::SendMessageToParent(const char * szText, int message)
{
    if (!IsWindow(m_hWnd))
        return 0;

	NM_CONSOLEEDIT nmedit;
	memset(nmedit.szText,0,sizeof(nmedit.szText));
	strncpy(nmedit.szText,szText,(sizeof(nmedit.szText) -2) >strlen(szText)?strlen(szText):(sizeof(nmedit.szText)-2));
	nmedit.hdr.hwndFrom = m_hWnd;
	nmedit.hdr.idFrom = GetDlgCtrlID();
	nmedit.hdr.code = message;

    CWnd *pOwner = GetOwner();
    if (pOwner && IsWindow(pOwner->m_hWnd))
        return pOwner->SendMessage(WM_NOTIFY, nmedit.hdr.idFrom, (LPARAM)&nmedit);
    else
        return 0;
	return 0;
}

void CConsoleEdit::BeginInput()
{
	AddTexts(m_pretitle);
	m_can_input = true;
}


void CConsoleEdit::StopInput()
{
	m_can_input = false;
}

void CConsoleEdit::Clear()
{
	if (m_can_input)
	{
		SetWindowText("");
		AddTexts("超级终端 [版本 1.00]\r\n");
		AddTexts("(C) 版权所有 2011 Trimps Inc.\r\n");
		AddTexts("\r\n");
		AddTexts(m_pretitle);
	}
}


void CConsoleEdit::OnRButtonDown(UINT nFlags, CPoint point) 
{
	//加载右键菜单
	CMenu menu,*pmenu;
	RECT rect;
	GetWindowRect(&rect);
	VERIFY(menu.LoadMenu(IDR_MENU1));
	pmenu = menu.GetSubMenu(0);
	int nStart,nEnd;
	GetSel(nStart,nEnd);
	if(nStart == nEnd)
		pmenu->EnableMenuItem(IDR_MENUCOPY,MF_GRAYED | MF_DISABLED);
	if(!m_bcopy || nStart != nEnd)
		pmenu->EnableMenuItem(IDR_MENUPLASTER,MF_GRAYED | MF_DISABLED);
	int nMenuResult = pmenu->TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x+rect.left, point.y+rect.top, this);
	
	switch (nMenuResult)
	{
	case IDR_MENUCOPY:
		//复制
		OnMenuCopy();
		break;
	case IDR_MENUPLASTER:
		//粘贴
		OnMenuPlaster();
		break;
	default:
		break;
	}
}

// 这个是为了显示的时候初始内容不被选中
void CConsoleEdit::OnTimer(UINT nIDEvent) 
{
	MoveToEnd();
	CEdit::OnTimer(nIDEvent);
	KillTimer(1);
}

void CConsoleEdit::Refresh()
{
	m_nLength = SendMessage( WM_GETTEXTLENGTH );
	memset(m_cmd,0,sizeof(m_cmd));
	CString str;
	GetWindowText(str);

	int i = 0, pos = 0;
	while (i != -1)
	{
		i = str.Find(m_pretitle);
		if (i != -1)
		{
			pos += i;
			pos += m_pretitle.GetLength();
			str = str.Mid(i + m_pretitle.GetLength());
		}
		else
			break;
	}
	m_last_title_pos = pos;
}

void CConsoleEdit::SetWindowText(const char* str)
{
	if (m_can_input)
	{
		CEdit::SetWindowText(str);
	}
}

bool CConsoleEdit::GetCanInput()
{
	return m_can_input;
}

void CConsoleEdit::OnSetfocus() 
{
	BOOL bRet = ::CreateCaret(this->m_hWnd,
		LoadBitmap(g_hInstance,MAKEINTRESOURCE(IDB_CARET)),0,0);
	ShowCaret();
}

void CConsoleEdit::OnKillfocus() 
{
	DestroyCaret();
}

void CConsoleEdit::ClearText()
{
	if (m_can_input)
		SetWindowText("");
}

void CConsoleEdit::OnMenuCopy()
{
	int nStart,nEnd;
	GetSel(nStart,nEnd);
	if(nStart != nEnd)
	{
		Copy();
		MoveToEnd();
		m_bcopy = true;
	}
}

void CConsoleEdit::OnMenuPlaster()
{
	int nStart,nEnd;
	GetSel(nStart,nEnd);
	if(nStart == nEnd)
	{
		// 粘贴
		MoveToEnd();
		Paste();
		CString alltext;
		GetWindowText(alltext);
		MoveToEnd();
		// 得到命令
		alltext = alltext.Mid(m_last_title_pos,m_nLength-m_last_title_pos);
		strncpy(m_cmd,alltext,alltext.GetLength()>sizeof(m_cmd)?sizeof(m_cmd):alltext.GetLength());
	}
}
