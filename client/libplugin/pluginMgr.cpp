// pluginMgr.cpp: implementation of the pluginMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pluginMgr.h"
#include "PluginMainFrame.h"
#include "macro.h"

using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static PluginMgr* gInstance = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int IPlugIn::curruId = -1;

PluginMgr::PluginMgr()
{

}

PluginMgr::~PluginMgr()
{

}

void PluginMgr::LoadAll(const char* path)
{
	int min = WM_PLUG_FIRST;
	int max = WM_PLUG_FIRST;

	WIN32_FIND_DATAA wfd;
	char pattern[MAX_PATH];
	sprintf(pattern, "%s*.DLL", path);
	HANDLE handle = FindFirstFileA(pattern, &wfd);
	if (handle == INVALID_HANDLE_VALUE)
		return;

	//遍历目录下的所有DLL形式插件
	do
	{
		if (!strcmp(wfd.cFileName, ".") || !strcmp(wfd.cFileName, ".."))
			continue;

		if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;
		
		DllWrapper* wrapper = new DllWrapper();
		bool bret = wrapper->LoadLibrary(path, wfd.cFileName);
		if (bret)
		{
			min = max;
			max += WM_PLUG_EACH; 
			IPlugProxy& proxy = wrapper->GetPlugIn();
			// 设置客户接口参数
			if (proxy)
			{
				proxy->SetMinResId(min);
				proxy->SetMaxResId(max);
				proxy->OnInitalize();
			}
			// 设置服务接口参数
			IServiceProxy& service = wrapper->GetService();
			if (service)
				service->OnInitalize();
			m_dllList.push_back(wrapper);
		}
		else
			delete wrapper;
	}while(FindNextFileA(handle,&wfd));
}

void PluginMgr::FreeAll()
{
	for(list<DllWrapper*>::iterator it = m_dllList.begin(); it != m_dllList.end(); it++)
	{
		DllWrapper* wrapper = *it;
		if (wrapper)
		{
			wrapper->FreeLibrary();
			delete wrapper;
			wrapper = NULL;
		}
	}
}

int PluginMgr::GetCount()
{
	return m_dllList.size();
}

void PluginMgr::TraversalAll(void (*f)(void*, DllWrapper*), void* param)
{
	for(list<DllWrapper*>::iterator it = m_dllList.begin(); it != m_dllList.end(); it++)
	{
		DllWrapper* wrapper = *it;
		if(wrapper && f)
		{
			f(param, wrapper);
		}
	}
}


void PluginMgr::OnCommand(UINT uId)
{
	//遍历所有的插件，找到与uID相匹配的
	for(list<DllWrapper*>::iterator it = m_dllList.begin(); it != m_dllList.end(); it++)
	{
		__try
		{
			DllWrapper* wrapper = *it;
			IPlugProxy& proxy = wrapper->GetPlugIn();
			if (wrapper && proxy)
			{
				if (uId >= proxy->GetMinResId() && uId <= proxy->GetMaxResId())
				{
					IPlugIn::curruId = uId;
					proxy->OnCommand(uId);
					break;
				}
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			KDebug("OnCommand exception");
		}
	}
}

void PluginMgr::OnCommandUI(CCmdUI* pCmdUI)
{
	PluginMainFrame* pMain = (PluginMainFrame*)ISite::GetMainFrameWnd();
	for(list<DllWrapper*>::iterator it = m_dllList.begin(); it != m_dllList.end(); it++)
	{
		__try
		{
			DllWrapper* wrapper = *it;
			IPlugProxy& proxy = wrapper->GetPlugIn();
			if (wrapper && proxy)
			{

				UINT uId = pCmdUI->m_nID;
				if(uId >= proxy->GetMinResId() && uId <= proxy->GetMaxResId())
				{
					// 框架处理ui
					pMain->OnCommandUI(pCmdUI);
					proxy->OnCommandUI(pCmdUI);
				}
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			KDebug("OnCommandUI exception");
		}
	}
}

PluginMgr* PluginMgr::GetInstance()
{
	if (gInstance)
		return gInstance;
	else
	{
		gInstance = new PluginMgr();
		return gInstance;
	}
}

void PluginMgr::FreeInstance()
{
	if (gInstance)
	{
		delete gInstance;
		gInstance = NULL;
	}
}

void PluginMgr::OnNotify(UINT notifyId, WPARAM wparam, LPARAM lparam)
{
	for(list<DllWrapper*>::iterator it = m_dllList.begin(); it != m_dllList.end(); it++)
	{
		__try
		{
			DllWrapper* wrapper = *it;
			IPlugProxy& proxy = wrapper->GetPlugIn();
			if (wrapper && proxy)
			{
				proxy->OnNotify(notifyId, wparam, lparam);
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			KDebug("OnNotify exception");
		}
	}
}

BOOL PluginMgr::OnService(UINT serviceId, WPARAM wparam, LPARAM lparam)
{
	bool flag = false;
	for(list<DllWrapper*>::iterator it = m_dllList.begin(); it != m_dllList.end(); it++)
	{
		__try
		{
			DllWrapper* wrapper = *it;
			IServiceProxy& service = wrapper->GetService();
			if (service)
			{
				if(service->OnService(serviceId,wparam,lparam))
					flag = true;
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			KDebug("OnService exception");
		}
	}
	return flag;
}
