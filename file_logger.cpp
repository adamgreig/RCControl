#include "file_logger.h"

/**
* Open a handle to the log file using given filename.
* \param filename The file to write the log to.
*/
FileLogger::FileLogger(char* filename) {
	file = CreateFile((LPCSTR)filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( file == INVALID_HANDLE_VALUE ) {
		file = NULL;
		DWORD err = GetLastError();
		cout << "Error " << err << " opening file for logging." << endl;
		return;
	}

}

/**
* Close the file handler on destruction.
*/
FileLogger::~FileLogger() {
	CloseHandle(file);
}

/**
* Write a line to the log file, prefixing it with a timestamp in milliseconds.
* \param data The null-terminated string to write to the file. Max 256 bytes.
*/
void FileLogger::log(char *data) {
	if( strlen(data) > 255 ) {
		cout << "Error: input string too long to FileLogger::log." << endl;
		return;
	}
	char time[32];
	sprintf_s(time, "%d", (int)clock());
	char buf[293];
	char tab[2] = "\t";
	char newline[3] = "\r\n";
	memset(&buf, NULL, 256);
	strcpy_s(buf, time);
	strcat_s(buf, tab);
	strcat_s(buf, data);
	strcat_s(buf, newline);
	DWORD written;
	WriteFile(file, buf, (DWORD)strlen(buf), &written, NULL);
}
