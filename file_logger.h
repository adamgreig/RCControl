#pragma once
#include <ctime>
#include "stdafx.h"

/**
* Log a given string to the filename given at construction.
* The string is logged with a timestamp obtained from clock()
* which should provide appropriate resolution on most C++
* implementations, but may need adaptation for specific platforms.
*/
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
