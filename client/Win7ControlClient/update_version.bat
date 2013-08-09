@ECHO OFF
SETLOCAL

set /p var=0<build.txt
set /a var+=1
echo %var% > build.txt

ECHO #define VER_MAJOR 1 > version_parsed.h
ECHO #define VER_MINOR 1 >> version_parsed.h
ECHO #define VER_BUGFIX 0 >> version_parsed.h
ECHO #define VER_BUILDNUM %var% >> version_parsed.h
ECHO #define VER_COMPANY "公安部第三研究所"  >> version_parsed.h
ECHO #define VER_PRODUCT	"网络远程监控管理控制端"  >> version_parsed.h
ECHO #define DO_STRINGIFY(x) #x >> version_parsed.h
ECHO #define STRINGIFY(x) DO_STRINGIFY(x) >> version_parsed.h
ENDLOCAL
EXIT /B
