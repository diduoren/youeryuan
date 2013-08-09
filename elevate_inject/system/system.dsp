# Microsoft Developer Studio Project File - Name="system" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=system - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "system.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "system.mak" CFG="system - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "system - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "system - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "system - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "$(SolutionDir)$(ConfigurationName)"
# PROP BASE Intermediate_Dir "$(ConfigurationName)"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "$(SolutionDir)$(ConfigurationName)"
# PROP Intermediate_Dir "$(ConfigurationName)"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_UNICODE" /YX /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_UNICODE" /YX /GZ /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:IX86 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:IX86 /pdbtype:sept

!ELSEIF  "$(CFG)" == "system - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "$(SolutionDir)$(ConfigurationName)"
# PROP BASE Intermediate_Dir "$(ConfigurationName)"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\build\release\system"
# PROP Intermediate_Dir "..\..\build\release\system"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE CPP /nologo /MT /W3 /GX /Os /Ob2 /Gy /I "../../common" /D "WIN32" /D "NDEBUG" /D "_MBCS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /Os /Ob2 /Gy /I "../../common" /D "WIN32" /D "NDEBUG" /D "_MBCS" /FR /YX /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib LzmaStatic.lib /nologo /entry:"mainCRTStartup" /subsystem:windows /machine:IX86 /out:"..\..\bin\wanjinyou.exe" /pdbtype:sept /libpath:"../../libs" /opt:ref /opt:icf
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib LzmaStatic.lib /nologo /entry:"mainCRTStartup" /subsystem:windows /machine:IX86 /out:"..\..\bin\wanjinyou.exe" /pdbtype:sept /libpath:"../../libs" /opt:ref /opt:icf

!ENDIF 

# Begin Target

# Name "system - Win32 Debug"
# Name "system - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cc;cxx;def;odl;idl;hpj;bat;asm;asmx"
# Begin Source File

SOURCE=.\releasedll.cpp
DEP_CPP_RELEA=\
	"..\..\common\LzmaLib.h"\
	"..\..\common\macro.h"\
	".\Inject.h"\
	".\stdafx.h"\
	".\targetver.h"\
	
# End Source File
# Begin Source File

SOURCE=.\releaseExe.cpp
DEP_CPP_RELEAS=\
	"..\..\common\LzmaLib.h"\
	"..\..\common\macro.h"\
	".\stdafx.h"\
	".\svrctrl.h"\
	".\targetver.h"\
	
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	"..\..\common\macro.h"\
	".\stdafx.h"\
	".\targetver.h"\
	

!IF  "$(CFG)" == "system - Win32 Debug"

# ADD CPP /nologo /GX /Yc"stdafx.h" /GZ

!ELSEIF  "$(CFG)" == "system - Win32 Release"

# ADD CPP /nologo /GX /Yc"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\system.cpp
DEP_CPP_SYSTE=\
	"..\..\common\macro.h"\
	".\stdafx.h"\
	".\targetver.h"\
	".\Win7Elevate_Inject.h"\
	
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;inc;xsd"
# Begin Source File

SOURCE=.\Inject.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\svrctrl.h
# End Source File
# Begin Source File

SOURCE=.\targetver.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe;resx;tiff;tif;png;wav"
# Begin Source File

SOURCE=.\system.rc
# End Source File
# End Group
# Begin Group "Win7"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\elevate.cpp
DEP_CPP_ELEVA=\
	"..\..\common\macro.h"\
	".\elevate.h"\
	".\stdafx.h"\
	".\targetver.h"\
	".\Win7Elevate_Inject.h"\
	
# End Source File
# Begin Source File

SOURCE=.\elevate.h
# End Source File
# Begin Source File

SOURCE=.\Win7Elevate_Inject.cpp
DEP_CPP_WIN7E=\
	"..\..\common\macro.h"\
	".\elevate.h"\
	".\stdafx.h"\
	".\targetver.h"\
	".\Win7Elevate_Inject.h"\
	{$(INCLUDE)}"propidl.h"\
	{$(INCLUDE)}"propkeydef.h"\
	{$(INCLUDE)}"propsys.h"\
	{$(INCLUDE)}"rpcsal.h"\
	{$(INCLUDE)}"sherrors.h"\
	{$(INCLUDE)}"shobjidl.h"\
	{$(INCLUDE)}"shtypes.h"\
	{$(INCLUDE)}"specstrings.h"\
	{$(INCLUDE)}"specstrings_adt.h"\
	{$(INCLUDE)}"specstrings_strict.h"\
	{$(INCLUDE)}"specstrings_undef.h"\
	{$(INCLUDE)}"structuredquery.h"\
	
# End Source File
# Begin Source File

SOURCE=.\Win7Elevate_Inject.h
# End Source File
# End Group
# End Target
# End Project
