// TlntPlugin.h: interface for the TlntPlugin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TLNTPLUGIN_H__251A997E_54F1_44A7_837A_FAF70E7A6A2F__INCLUDED_)
#define AFX_TLNTPLUGIN_H__251A997E_54F1_44A7_837A_FAF70E7A6A2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "iPlug.h"

class TlntPlugin : public IPlugIn  
{
public:
	TlntPlugin();
	virtual ~TlntPlugin();

	// 插件初始化
	virtual BOOL	OnInitalize();
	// 卸载插件
	virtual BOOL	OnUninitalize();
	
	// ************************************
	// 创建文档模板
	virtual void OnCreateDocTemplate();

	// 创建框架菜单
	virtual void OnCreateFrameMenu(CMenu* pMainMenu);

	// 创建框架工具条
	virtual	void OnCreateFrameToolBar(ToolBarData*,UINT& count);

	// 创建停靠窗口
	virtual void OnCreateDockPane(DockPane* ,UINT& count);

	// 菜单、工具条操作
	virtual void	OnCommand(UINT resId);
	virtual void	OnCommandUI(CCmdUI* pCmdUI);
	// 收到通知，通知号，参数1，参数2
	virtual void	OnNotify(UINT,WPARAM,LPARAM);

private:
	CMultiDocTemplate* m_docTemp;
};

#endif // !defined(AFX_TLNTPLUGIN_H__251A997E_54F1_44A7_837A_FAF70E7A6A2F__INCLUDED_)
