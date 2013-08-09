# Microsoft Developer Studio Project File - Name="elevate" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=elevate - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "elevate.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "elevate.mak" CFG="elevate - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "elevate - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "elevate - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "elevate - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\build\debug\elevate\"
# PROP BASE Intermediate_Dir "..\..\build\debug\elevate\"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\build\debug\elevate\"
# PROP Intermediate_Dir "..\..\build\debug\elevate\"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE CPP /nologo /MTd /W3 /GX /Od /I "../../common" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_UNICODE" /GZ PRECOMP_VC7_TOBEREMOVED /c
# ADD CPP /nologo /MTd /W3 /GX /Od /I "../../common" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_UNICODE" /GZ PRECOMP_VC7_TOBEREMOVED /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib LzmaStatic.lib comctl32.lib shell32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"mainCRTStartup" /subsystem:console /debug /machine:IX86 /out:"..\..\bin\debug\$(ProjectName).exe" /pdbtype:sept /libpath:"../../libs"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib LzmaStatic.lib comctl32.lib shell32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"mainCRTStartup" /subsystem:console /debug /machine:IX86 /out:"..\..\bin\debug\$(ProjectName).exe" /pdbtype:sept /libpath:"../../libs"

!ELSEIF  "$(CFG)" == "elevate - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\build\release\elevate\"
# PROP BASE Intermediate_Dir "..\..\build\release\elevate\"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\build\release\elevate\"
# PROP Intermediate_Dir "..\..\build\release\elevate\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE CPP /nologo /MT /W3 /GX /Os /Ob2 /Gy /I "../../common" /D "WIN32" /D "NDEBUG" /D "_UNICODE" PRECOMP_VC7_TOBEREMOVED /c
# ADD CPP /nologo /MT /W3 /GX /Os /Ob2 /Gy /I "../../common" /D "WIN32" /D "NDEBUG" /D "_UNICODE" PRECOMP_VC7_TOBEREMOVED /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib LzmaStatic.lib comctl32.lib shell32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"mainCRTStartup" /subsystem:windows /machine:IX86 /out:"..\..\bin\$(ProjectName).exe" /pdbtype:sept /libpath:"../../libs" /opt:noref /opt:noicf
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib LzmaStatic.lib comctl32.lib shell32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"mainCRTStartup" /subsystem:windows /machine:IX86 /out:"..\..\bin\elevate.exe" /pdbtype:sept /libpath:"../../libs" /opt:noref /opt:noicf

!ENDIF 

# Begin Target

# Name "elevate - Win32 Debug"
# Name "elevate - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cc;cxx;def;odl;idl;hpj;bat;asm;asmx"
# Begin Source File

SOURCE=.\elevate.cpp
# End Source File
# Begin Source File

SOURCE=.\Win7Elevate_Inject.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;inc;xsd"
# Begin Source File

SOURCE=.\elevate.h
# End Source File
# Begin Source File

SOURCE=.\Inject.h
# End Source File
# Begin Source File

SOURCE=.\Win7Elevate_Inject.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
