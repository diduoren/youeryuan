// PluginApp.cpp: implementation of the PluginApp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PluginApp.h"
#include "pluginMgr.h"
#include "DlgRegister.h"
#include "SEU_QQwry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(PluginApp, CWinApp)
	ON_COMMAND_RANGE(WM_PLUG_FIRST, WM_PLUG_LAST, OnPlugCmd)
	ON_UPDATE_COMMAND_UI_RANGE(WM_PLUG_FIRST, WM_PLUG_LAST, OnPlugCmdUI)
END_MESSAGE_MAP()

PluginApp::PluginApp()
{

}

PluginApp::~PluginApp()
{

}

static void GetApplicationPath(char* appPath, int n)
{
	char filename[MAX_PATH], drive[_MAX_DRIVE], path[MAX_PATH];
	GetModuleFileNameA(AfxGetInstanceHandle(), filename, MAX_PATH);
	
	_splitpath(filename, drive, path, NULL, NULL);
	sprintf(appPath, "%s%s", drive, path);
}

static char iniFileName[256] = {0};
	
BOOL PluginApp::InitInstance()
{
	GetModuleFileNameA(NULL,iniFileName, 250);
	char *p = strrchr(iniFileName, '.');
	*p = 0;
	strcat(iniFileName,".ini");
	
	HANDLE	hFile = CreateFile("QQwry.dat", 0, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{ 
		ISite::m_bIsQQwryExist = true;
		ISite::m_QQwry = new SEU_QQwry;
		ISite::m_QQwry->SetPath("QQWry.Dat");
	}
	else
	{
		ISite::m_bIsQQwryExist = false;
	}
	CloseHandle(hFile);

	return TRUE;
}

char* PluginApp::GetCFGFile(){return iniFileName;}


void PluginApp::OnPlugCmd(UINT id)
{
	PluginMgr::GetInstance()->OnCommand(id);
}

void PluginApp::OnPlugCmdUI(CCmdUI* pCmdUI)
{
	PluginMgr::GetInstance()->OnCommandUI(pCmdUI);
}

void PluginApp::LoadAllPlugins()
{
	char path[MAX_PATH];
	GetApplicationPath(path, MAX_PATH);
	strcat(path, "plugins\\");

	PluginMgr::GetInstance()->LoadAll(path);
}

void PluginApp::UnloadAllPlugins()
{
	PluginMgr::GetInstance()->FreeAll();
	PluginMgr::FreeInstance();
}

static void CreateDocTemplate(void* p, DllWrapper* wrapper)
{
	IPlugProxy& proxy = wrapper->GetPlugIn();
	if(proxy)
		proxy->OnCreateDocTemplate();
}

void PluginApp::RegisterAllDocTemplates()
{
	PluginMgr::GetInstance()->TraversalAll(CreateDocTemplate, NULL);
}

static DWORD dwMask=87654321;  //运算子，随便设定
static DWORD dwKey=0x20080808;  //密钥码，设成自己好记、别人不知的

static BOOL IsRegisterOK_()
{
	DWORD dwVolumeSerialNumber,dwUserID,dwDecodeNumber; 
	
	CString strUserID,strRegCode;
	char str[200];
	int str_len=200;

	// 将硬盘序列号与运算子异或后做为用户号
    GetVolumeInformation("C:\\",NULL,NULL,&dwVolumeSerialNumber,NULL,NULL,NULL,NULL);  
	dwUserID=dwVolumeSerialNumber^dwMask;  
	strUserID.Format("%d",dwUserID);

	// 正确的注册码应为16进制形式=(10进制的dwUserID转16进制后)^(16进制的密钥码)
	// 开发者可利用科学型计算器，根据用户报知的用户号直接算出注册码返回给用户

    // 先直接从操作系统的win.ini读取注册码
	const char* soft = "CtrlWin7";
	const char* usrId = "UserID";
	const char* regCode = "RegCode";
	const char* cfgFile = PluginApp::GetCFGFile();

    if(GetPrivateProfileString(soft, regCode, "", str, str_len, cfgFile) != 0 )
    {
        strRegCode=str;
		
		// 验证注册码
		dwDecodeNumber=strtoul(strRegCode,NULL,16);  
        dwDecodeNumber^=dwKey;  //解密钥
		dwDecodeNumber^=dwMask;  //还原硬盘序列号
		if(dwDecodeNumber==dwVolumeSerialNumber)  //注册成功
	    	return TRUE;
	}

    // 如果上面读取的注册码不对或没有，再通过对话框取注册码
	for(;;)
	{
		CDlgRegister dlg;
		dlg.m_strUserID=strUserID;

		if(dlg.DoModal()==IDOK)
		{
			strRegCode=dlg.m_strRegCode;

			// 验证注册码
			dwDecodeNumber=strtoul(strRegCode,NULL,16);  
            dwDecodeNumber^=dwKey;  //解密钥
			dwDecodeNumber^=dwMask;  //还原硬盘序列号

			if(dwDecodeNumber==dwVolumeSerialNumber)  //注册成功
			{
				// 将通过的注册信息保存到操作系统的win.ini，以备以后直接读取
				WritePrivateProfileString(soft, usrId, strUserID, cfgFile);
				WritePrivateProfileString(soft, regCode, strRegCode, cfgFile);

	    		return TRUE;
			}
     		else
			    AfxMessageBox("注册码错误！ 请重试输入或向开发者索取本机注册码！");
		}
		else
			break; 
	}

    return FALSE;  //注册失败
}

BOOL PluginApp::IsRegisterOK() 
{
	return IsRegisterOK_();    
}