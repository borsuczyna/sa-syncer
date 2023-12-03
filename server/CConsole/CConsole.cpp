#include "../stdafx.h"

void CConsole::Init()
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);

	SetConsoleTitle(L"SA Syncer");
}

void CConsole::Print(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	time_t now = time(0);
	struct tm* timeinfo = localtime(&now);
	char timeStr[9];
	strftime(timeStr, sizeof(timeStr), "%H:%M:%S", timeinfo);

	printf("[%s] ", timeStr);
	vprintf(format, args);
	va_end(args);
	printf("\n");
}