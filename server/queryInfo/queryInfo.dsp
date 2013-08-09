# Microsoft Developer Studio Project File - Name="queryInfo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=queryInfo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "queryInfo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "queryInfo.mak" CFG="queryInfo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "queryInfo - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "queryInfo - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "queryInfo - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\build\debug\qinfo"
# PROP BASE Intermediate_Dir "..\..\build\debug\qinfo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\build\debug\qinfo"
# PROP Intermediate_Dir "..\..\build\debug\qinfo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "QUERYINFO_EXPORTS" /D "_UNICODE" /GZ PRECOMP_VC7_TOBEREMOVED /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "QUERYINFO_EXPORTS" /D "_UNICODE" /GZ PRECOMP_VC7_TOBEREMOVED /c
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib Netapi32.lib Rpcrt4.lib Iphlpapi.lib /nologo /subsystem:windows /dll /debug /machine:IX86 /out:"..\..\bin\debug\$(ProjectName).dll" /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib Netapi32.lib Rpcrt4.lib Iphlpapi.lib /nologo /subsystem:windows /dll /debug /machine:IX86 /out:"..\..\bin\debug\$(ProjectName).dll" /pdbtype:sept

!ELSEIF  "$(CFG)" == "queryInfo - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\build\release\qinfo"
# PROP BASE Intermediate_Dir "..\..\build\release\qinfo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\build\release\qinfo"
# PROP Intermediate_Dir "..\..\build\release\qinfo"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /Oi /Os /Gy /I "../../common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "QUERYINFO_EXPORTS" /D "_UNICODE" PRECOMP_VC7_TOBEREMOVED /c
# ADD CPP /nologo /MT /W3 /GX /Oi /Os /Gy /I "../../common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "QUERYINFO_EXPORTS" /D "_UNICODE" PRECOMP_VC7_TOBEREMOVED /c
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib Iphlpapi.lib /nologo /subsystem:windows /dll /debug /machine:IX86 /def:"queryInfo.def" /out:"..\..\bin\svrdll\$(ProjectName).dll" /pdbtype:sept /opt:ref /opt:icf
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib Iphlpapi.lib /nologo /subsystem:windows /dll /debug /machine:IX86 /def:"queryInfo.def" /out:"..\..\bin\svrdll\queryinfo.dll" /pdbtype:sept /opt:ref /opt:icf

!ENDIF 

# Begin Target

# Name "queryInfo - Win32 Debug"
# Name "queryInfo - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cc;cxx;def;odl;idl;hpj;bat;asm;asmx"
# Begin Source File

SOURCE=.\queryInfo.cpp
DEP_CPP_QUERY=\
	"..\..\common\conn.h"\
	"..\..\common\macro.h"\
	"..\..\common\svrdll.h"\
	".\queryInfo.h"\
	
# End Source File
# Begin Source File

SOURCE=.\queryInfo.def
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;inc;xsd"
# Begin Source File

SOURCE=.\queryInfo.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe;resx;tiff;tif;png;wav"
# End Group
# End Target
# End Project
