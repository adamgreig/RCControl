#include "file_logger.h"

///Open the log file handler
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
	file = open(filename, O_WRONLY | O_CREAT | O_TRUNC);
	printf("Log file opened.\n");
	#endif

}

///Close the file handler
FileLogger::~FileLogger() {
	#if WINDOWS
	CloseHandle(file);
	#elif LINUX
	close(file);
	#endif
}

///Write data to the log file, prefixing it with a timestamp in milliseconds
void FileLogger::log(char *data) {
	char time[32];
	#if WINDOWS
	sprintf_s(time, "%d", clock());
	#elif LINUX
	sprintf(time, "%d", clock());
	#endif
	char buf[256];
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
