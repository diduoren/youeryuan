// PluginMainFrame.h: interface for the PluginMainFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLUGINMAINFRAME_H__6C382AC5_B22D_41D4_B2CC_BED096730856__INCLUDED_)
#define AFX_PLUGINMAINFRAME_H__6C382AC5_B22D_41D4_B2CC_BED096730856__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "iPlug.h"
#include "TreePropSheet.h"

class LIBPLUG_API PluginMainFrame :  public CXTPMDIFrameWnd 
{
	DECLARE_DYNAMIC(PluginMainFrame)

public:
	PluginMainFrame();
	virtual ~PluginMainFrame();

// Operations
public:
	//初始化所有的菜单栏
	CXTPMenuBar* RegisterAllFrameMenu(LPCTSTR name,UINT UID);
	
	//初始化所有的工具条
	CXTPToolBar* RegisterAllFrameToolBar(LPCTSTR name, UINT UID = 0);
	
	//创建停靠窗口栏
	void RegisterAllDockPane(CXTPDockingPaneManager*, CXTPDockingPane* left = NULL,
							CXTPDockingPane* bottom= NULL, CXTPDockingPane* right= NULL );
	
	//加载工具条所对应的图标文件
	CImageList* RegisterAllImage(int cx,int cy);

	void OnCommandUI(CCmdUI*);

private:

// Overrides
	//virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnExitApp();
	afx_msg LRESULT OnSystemTray(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnProgressDlgNotify(WPARAM wP, LPARAM lP);

	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	CMenu	m_defaultMenu;
	CImageList	m_imgList;

	CXTPDockingPaneManager* m_paneMgr;
	CTreePropSheet m_sheet;

	NOTIFYICONDATA  m_nid;

	friend class ISite;
};

#endif // !defined(AFX_PLUGINMAINFRAME_H__6C382AC5_B22D_41D4_B2CC_BED096730856__INCLUDED_)
