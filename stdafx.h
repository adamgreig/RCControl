/**
* @file Common includes. Not pre-compiled.
*/

#pragma once

#if WINDOWS
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <tchar.h>
#include <windows.h>
#define sleep(x) Sleep(x)
#elif LINUX
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#define sleep(x) usleep(1000 * x)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdio>
using namespace std;
