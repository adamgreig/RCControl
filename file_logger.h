#pragma once

#if WINDOWS
#include <windows.h>
#elif LINUX
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#endif

#include <ctime>
#include <stdio.h>

///Log a string to a persistent log file
class FileLogger {
public:
	FileLogger(char* filename);
	~FileLogger();
	void log(char* data);
private:
#if WINDOWS
	HANDLE file;
#elif LINUX
	int file;
#endif
};
