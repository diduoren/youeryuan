// DockPaneWnd.h : main header file for the SAMPLEAPP application
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// ©1998-2005 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(__DOCKPANEWND_H__)
#define __DOCKPANEWND_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Resource.h"

#define WM_LIST_CLEAR	WM_USER+1001

typedef struct tagNMCTRLDATA
{
	NMHDR	hdr;
	int		iStatus;
	PVOID	pData;
}NM_CTRLDATA;

template<class BASE_CLASS>
class CDockPaneWnd : public BASE_CLASS
{
protected:

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_NCPAINT:
		case WM_PAINT:
			{
				BASE_CLASS::WindowProc(message, wParam, lParam);

				CWindowDC dc(this);

				CRect rc;
				GetWindowRect(&rc);

				int cx = rc.Width();
				int cy = rc.Height();

				const COLORREF clrFrame =  GetSysColor(COLOR_3DSHADOW);
				dc.Draw3dRect(0, 0, cx, cy, clrFrame, clrFrame);

				break;
			}
			
		case WM_RBUTTONDOWN:
			{
				
				Invalidate();
				CWnd* pOwner = GetOwner();
				
				NM_CTRLDATA nmlist;
				
				nmlist.hdr.code = WM_LIST_CLEAR;
				nmlist.hdr.idFrom = GetDlgCtrlID();						
				nmlist.hdr.hwndFrom = m_hWnd;
				nmlist.iStatus = 0;
				nmlist.pData = NULL;
				
				pOwner->SendMessage( WM_NOTIFY, nmlist.hdr.idFrom, (LPARAM)&nmlist );		
				break;
			}
		case WM_LBUTTONDOWN:
			Invalidate();
			break;
		}

		return BASE_CLASS::WindowProc(message, wParam, lParam);
	}
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__DOCKPANEWND_H__)
