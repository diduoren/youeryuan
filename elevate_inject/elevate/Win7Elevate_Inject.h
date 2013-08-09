#pragma once

void AttemptOperation( bool bInject, bool bElevate, DWORD dwPid, const wchar_t *szProcName,
						  const wchar_t *szCmd, const wchar_t *szArgs, const wchar_t *szDir,
						  const wchar_t *szPathToOurDll);
