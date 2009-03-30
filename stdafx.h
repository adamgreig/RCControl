// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#if WINDOWS
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <tchar.h>
#elif LINUX
#include <unistd.h>
#include <linux/types.h>
#endif

#include <stdio.h>


// TODO: reference additional headers your program requires here
