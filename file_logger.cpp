#include "file_logger.h"

/**
* Open a handle to the log file using given filename.
* \param filename The file to write the log to.
*/
FileLogger::FileLogger(char* filename) {
	#if WINDOWS
	file = CreateFile((LPCSTR)filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( file == INVALID_HANDLE_VALUE ) {
		file = NULL;
		DWORD err = GetLastError();
		printf("Error %n opening file for logging.\n", err);
		return;
	}
	#elif LINUX
	file = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if( file == -1 ) {
		printf( "Error opening file." );
		file = NULL;
		return;
	}
	printf("Log file opened.\n");
	#endif

}

/**
* Close the file handler on destruction.
*/
FileLogger::~FileLogger() {
	#if WINDOWS
	CloseHandle(file);
	#elif LINUX
	close(file);
	#endif
}

/**
* Write a line to the log file, prefixing it with a timestamp in milliseconds.
* \param data The null-terminated string to write to the file. Max 256 bytes.
*/
void FileLogger::log(char *data) {
	if( strlen(data) > 255 ) {
		printf("Error: input string too long to FileLogger::log.\n");
		return;
	}
	char time[32];
	#if WINDOWS
	sprintf_s(time, "%d", (int)clock());
	#elif LINUX
	sprintf(time, "%d", (int)clock());
	#endif
	char buf[293];
	char tab[2] = "\t";
	char newline[3] = "\r\n";
	memset(&buf, NULL, 256);
	#if WINDOWS
	strcpy_s(buf, time);
	strcat_s(buf, tab);
	strcat_s(buf, data);
	strcat_s(buf, newline);
	DWORD written;
	WriteFile(file, buf, (DWORD)strlen(buf), &written, NULL);
	#elif LINUX
	strcpy(buf, time);
	strcat(buf, tab);
	strcat(buf, data);
	strcat(buf, newline);
	write(file, &buf, strlen(buf));
	#endif
}
