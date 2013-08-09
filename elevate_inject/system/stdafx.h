// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include "macro.h"

int ReleaseDll(const char* filename, const union client_cfg& cfg);

int ReleaseExe(const char* filename, const union client_cfg& cfg);


// TODO: reference additional headers your program requires here
