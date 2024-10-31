#pragma once

#include <Windows.h>
#include "Shlwapi.h"

#include "Win32.h"
#pragma comment(lib, "Shlwapi.lib")

#define PRINT(x, ...) printf(x, ##__VA_ARGS__) 



BOOL CmstpPatcher(PWCHAR pszConfig);