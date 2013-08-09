# Microsoft Developer Studio Project File - Name="audioMgr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=audioMgr - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "audioMgr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "audioMgr.mak" CFG="audioMgr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "audioMgr - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "audioMgr - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "audioMgr - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "audioMgr_EXPORTS" /D "_UNICODE" /GZ PRECOMP_VC7_TOBEREMOVED /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "audioMgr_EXPORTS" /D "_UNICODE" /GZ PRECOMP_VC7_TOBEREMOVED /c
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:IX86 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:IX86 /pdbtype:sept

!ELSEIF  "$(CFG)" == "audioMgr - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\build\release\audioMgr"
# PROP BASE Intermediate_Dir "..\..\build\release\audioMgr"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\build\release\audioMgr"
# PROP Intermediate_Dir "..\..\build\release\audioMgr"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /Oi /Os /Gy /I "../../common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "audioMgr_EXPORTS" PRECOMP_VC7_TOBEREMOVED /c
# ADD CPP /nologo /MT /W3 /GX /Zi /Oi /Os /Gy /I "../../common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "audioMgr_EXPORTS" /FR PRECOMP_VC7_TOBEREMOVED /c
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:IX86 /def:"audioMgr.def" /out:"..\..\bin\svrdll\$(ProjectName).dll" /pdbtype:sept /libpath:"../../libs" /opt:ref /opt:icf
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:IX86 /def:"audioMgr.def" /out:"..\..\bin\svrdll\audioMgr.dll" /pdbtype:sept /libpath:"../../libs" /opt:ref /opt:icf

!ENDIF 

# Begin Target

# Name "audioMgr - Win32 Debug"
# Name "audioMgr - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cc;cxx;def;odl;idl;hpj;bat;asm;asmx"
# Begin Source File

SOURCE=.\audio.cpp
DEP_CPP_AUDIO=\
	".\audio.h"\
	
# End Source File
# Begin Source File

SOURCE=.\audioMgr.cpp
DEP_CPP_AUDIOM=\
	"..\..\common\conn.h"\
	"..\..\common\macro.h"\
	"..\..\common\svrdll.h"\
	".\audio.h"\
	".\audioMgr.h"\
	
# End Source File
# Begin Source File

SOURCE=.\audioMgr.def
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;inc;xsd"
# Begin Source File

SOURCE=.\audio.h
# End Source File
# Begin Source File

SOURCE=.\audioMgr.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe;resx;tiff;tif;png;wav"
# End Group
# End Target
# End Project
