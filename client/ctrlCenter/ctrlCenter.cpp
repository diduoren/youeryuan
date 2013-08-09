// ctrlCenter.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "MasterPlugin.h"
#include "MasterService.h"

IPlugIn*	g_Plugin;
IService*	g_Service;

static AFX_EXTENSION_MODULE CtrlCenterDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("CTRLCENTER.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(CtrlCenterDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(CtrlCenterDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("CTRLCENTER.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(CtrlCenterDLL);
	}
	return 1;   // ok
}

extern "C" IPlugIn* GetIPlugIn()
{
	g_Plugin = new MasterPlugin();
	return g_Plugin;
}

extern "C" IService* GetIService()
{
	g_Service = new MasterService();
	return g_Service;
}
