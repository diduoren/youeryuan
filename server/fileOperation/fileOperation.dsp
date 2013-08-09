# Microsoft Developer Studio Project File - Name="fileOperation" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=fileOperation - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "fileOperation.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fileOperation.mak" CFG="fileOperation - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "fileOperation - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fileOperation - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "fileOperation - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\build\debug\fileop"
# PROP BASE Intermediate_Dir "..\..\build\debug\fileop"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\build\debug\fileop"
# PROP Intermediate_Dir "..\..\build\debug\fileop"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../common" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_USRDLL" /D "FILEOPERATION_EXPORTS" /D "_UNICODE" /GZ PRECOMP_VC7_TOBEREMOVED /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../common" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_USRDLL" /D "FILEOPERATION_EXPORTS" /D "_UNICODE" /GZ PRECOMP_VC7_TOBEREMOVED /c
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libconnD.lib ws2_32.lib /nologo /subsystem:console /dll /debug /machine:IX86 /def:"fileOperation.def" /out:"..\..\bin\debug\svrdll\$(ProjectName).dll" /pdbtype:sept /libpath:"../../libs"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libconnD.lib ws2_32.lib /nologo /subsystem:console /dll /debug /machine:IX86 /def:"fileOperation.def" /out:"..\..\bin\debug\svrdll\$(ProjectName).dll" /pdbtype:sept /libpath:"../../libs"

!ELSEIF  "$(CFG)" == "fileOperation - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\build\release\fileop"
# PROP BASE Intermediate_Dir "..\..\build\release\fileop"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\build\release\fileop"
# PROP Intermediate_Dir "..\..\build\release\fileop"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /Oi /Os /Ob2 /Gy /I "../../common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "FILEOPERATION_EXPORTS" /D "_UNICODE" PRECOMP_VC7_TOBEREMOVED /c
# ADD CPP /nologo /MT /W3 /GX /Oi /Os /Ob2 /Gy /I "../../common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "FILEOPERATION_EXPORTS" /D "_UNICODE" PRECOMP_VC7_TOBEREMOVED /c
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libconn2.lib ws2_32.lib /nologo /subsystem:windows /dll /debug /machine:IX86 /def:"fileOperation.def" /out:"..\..\bin\svrdll\$(ProjectName).dll" /pdbtype:sept /libpath:"../../libs" /opt:ref /opt:icf
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libconn2.lib ws2_32.lib /nologo /subsystem:windows /dll /debug /machine:IX86 /def:"fileOperation.def" /out:"..\..\bin\svrdll\fileOperation.dll" /pdbtype:sept /libpath:"../../libs" /opt:ref /opt:icf

!ENDIF 

# Begin Target

# Name "fileOperation - Win32 Debug"
# Name "fileOperation - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cc;cxx;def;odl;idl;hpj;bat;asm;asmx"
# Begin Source File

SOURCE=.\fileOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\fileOperation.def
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;inc;xsd"
# Begin Source File

SOURCE=.\fileOperation.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe;resx;tiff;tif;png;wav"
# End Group
# End Target
# End Project
