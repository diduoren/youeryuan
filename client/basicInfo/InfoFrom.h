#pragma once


#include "resource.h"
#include "TreeListCtrl.h"

// InfoFrom form view

class InfoFrom : public CFormView
{
	DECLARE_DYNCREATE(InfoFrom)

public:
	InfoFrom();           // protected constructor used by dynamic creation
	virtual ~InfoFrom();

	void InsertData(void* data, void* info);
	void OnQueryFailed(unsigned short cmd, unsigned short resp);
	HTREEITEM InsertPair(const char* name, const char* value, HTREEITEM parent = NULL);
	void SortTree(int nCol, BOOL bAscending, HTREEITEM hParent);

public:
	enum { IDD = IDD_FORMVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:
	CListCtrl	m_listCtrl;
	CTreeListCtrl m_treeList;
	BOOL		m_init;

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};


