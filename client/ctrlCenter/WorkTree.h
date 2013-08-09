#if !defined(AFX_WORKTREE_H__24C825A0_2E7B_4A9B_92B3_DD56210F88C9__INCLUDED_)
#define AFX_WORKTREE_H__24C825A0_2E7B_4A9B_92B3_DD56210F88C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WorkTree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWorkTree window

#define WM_SEL_OBJ WM_USER+100
#define WM_DRAG_OBJ_FINISHED WM_USER+0x0101

#define ROOT_DATA 10
#define FIRST_GROUP_DATA 101
#define FIRST_OBJ_DATA 2001

class CWorkTree : public CTreeCtrl
{
// Construction
public:
	CWorkTree();

// Attributes
public:
//	BOOL m_bDragging;

// Operations
public:
	HTREEITEM FindItem(HTREEITEM item, unsigned int i);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkTree)
	//}}AFX_VIRTUAL

// Implementation
public:	
	char m_draginfo[255];
	LRESULT SendMsgForDragFinished( int message );
	virtual ~CWorkTree();

	// Generated message map functions
protected:
	UINT          m_TimerTicks;      //处理滚动的定时器所经过的时间
	UINT          m_nScrollTimerID;  //处理滚动的定时器
	CPoint        m_HoverPoint;      //鼠标位置
	UINT          m_nHoverTimerID;   //鼠标敏感定时器
	DWORD         m_dwDragStart;     //按下鼠标左键那一刻的时间
	BOOL          m_bDragging;       //标识是否正在拖动过程中
	CImageList*   m_pDragImage;      //拖动时显示的图象列表
	HTREEITEM     m_hItemDragS;      //被拖动的标签
	HTREEITEM     m_hItemDragD;      //接受拖动的标签
	//{{AFX_MSG(CWorkTree)
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	//afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	HTREEITEM CopyBranch(HTREEITEM htiBranch, HTREEITEM htiNewParent, HTREEITEM htiAfter);
	HTREEITEM CopyItem(HTREEITEM hItem, HTREEITEM htiNewParent, HTREEITEM htiAfter);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORKTREE_H__24C825A0_2E7B_4A9B_92B3_DD56210F88C9__INCLUDED_)
