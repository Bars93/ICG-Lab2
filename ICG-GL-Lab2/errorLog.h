#ifndef ERRORLOG_H
#define ERRORLOG_H
#ifdef _WIN32 
#include <Windows.h>
#endif
#include <string>
#include <vector>

class errorLog
{
	std::string lastError;
	std::vector<std::string> Errors;
	HWND hWnd;
	int Error;
	unsigned int errorCount;
public:
	inline unsigned int getErrorCount() const { return errorCount; };
	errorLog(void);
	~errorLog(void);
};

#endif /* ERRORLOG_H */
