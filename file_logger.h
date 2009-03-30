#pragma once
#define WINDOWS 1

#if WINDOWS
#include <windows.h>
#endif

#include <ctime>
#include <stdio.h>

///Log a string to a persistent log file
class FileLogger
{
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
