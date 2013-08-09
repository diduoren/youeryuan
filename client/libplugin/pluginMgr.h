// pluginMgr.h: interface for the pluginMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLUGINMGR_H__B7EC75A8_A9E7_4C17_9E10_98C5B9BC50D4__INCLUDED_)
#define AFX_PLUGINMGR_H__B7EC75A8_A9E7_4C17_9E10_98C5B9BC50D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "iPlug.h"
#include <list>

// 插件管理类，该类主要负责导入所有插件，并管理所有插件
class LIBPLUG_API PluginMgr  
{
public:
	virtual ~PluginMgr();

	// 服务
	BOOL OnService(UINT notifyId, WPARAM wparam, LPARAM lparam);
	// 通知
	void OnNotify(UINT serviceId,WPARAM wparam,LPARAM lparam);
	
	//工具栏点击响应
	void OnCommand(UINT);
	void OnCommandUI(CCmdUI*);
	
	// 导入所有插件
	void LoadAll(const char* path);

	// 释放所有插件
	void FreeAll();
	
	//遍历所有插件
	void TraversalAll(void (*f)(void*, DllWrapper*), void* param);
	
	// 得到插件的个数
	int	GetCount();

	//	得到实例
	static PluginMgr* GetInstance();
	// 释放实例
	static void FreeInstance();

private:
	PluginMgr();
	
	// 文件列表
	std::list<DllWrapper*> m_dllList;

	friend class PluginMainFrame;
};

#endif // !defined(AFX_PLUGINMGR_H__B7EC75A8_A9E7_4C17_9E10_98C5B9BC50D4__INCLUDED_)
