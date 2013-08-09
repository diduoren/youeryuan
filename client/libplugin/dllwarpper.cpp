#include "stdafx.h"
#include "iPlug.h"

typedef IPlugIn* (*GETPLUGINTERFACE)();
typedef IService* (*GETSERVICEINTERFACE)();

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DllWrapper::DllWrapper()
{
}

DllWrapper::~DllWrapper()
{

}

const char*	DllWrapper::GetFileName() const
{
	return	m_strFile;
}

HINSTANCE DllWrapper::GetInstance() const
{
	return	m_hInstance;
}

IPlugProxy& DllWrapper::GetPlugIn()
{
	return	m_plugProxy;
}

IServiceProxy& DllWrapper::GetService()
{
	return	m_serviceProxy;
}

const char* DllWrapper::GetFilePath() const
{
	return m_strPath;
}

bool DllWrapper::LoadLibrary(const char* path, const char* file)
{
	char fileName[MAX_PATH];
	strcpy(m_strPath, path);
	strcpy(m_strFile, file);
	sprintf(fileName, "%s%s", m_strPath, m_strFile);

	m_hInstance = ::LoadLibraryA(fileName);
	if (m_hInstance)
	{
		// 获得插件接口函数指针
		GETPLUGINTERFACE pfn = (GETPLUGINTERFACE)GetProcAddress(m_hInstance,"GetIPlugIn");
		if (pfn)
		{
			IPlugIn* plug = pfn();
			if (plug)
			{
				m_plugProxy.Attach(plug);
			}
		}
		
		// 获得平台接口的指针
		GETSERVICEINTERFACE pfn2 = (GETSERVICEINTERFACE)GetProcAddress(m_hInstance,"GetIService");
		if (pfn2)
		{
			IService* psite = pfn2();
			if (psite)
			{
				m_serviceProxy.Attach(psite);
			}
		}
		if (m_plugProxy || m_serviceProxy)
			return true;
	}
	::FreeLibrary(m_hInstance);
	return false;
}

bool DllWrapper::FreeLibrary()
{
	// 释放插件接口
	if (m_plugProxy)
	{
		m_plugProxy->OnUninitalize();
		delete m_plugProxy.Detach();
	}

	// 释放平台接口
	if (m_serviceProxy)
	{
		m_serviceProxy->OnUninitalize();
		delete m_serviceProxy.Detach();
	}

	::FreeLibrary(m_hInstance);
	return true;
}

UINT DllWrapper::GetImage(CImageList & il, UINT nID)
{
	CBitmap bmp;
	HBITMAP hBitmap = LoadBitmap(m_hInstance,MAKEINTRESOURCE(nID));
	CBitmap* pBmp = bmp.FromHandle(hBitmap);

	int ret = il.Add(pBmp,CXTPImageManager::GetBitmapMaskColor(*pBmp, CPoint(0, 0)));

	return il.GetImageCount();
}