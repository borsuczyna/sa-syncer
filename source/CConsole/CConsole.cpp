#include "../stdafx.h"

void CConsole::Init()
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);

	SetConsoleTitle("SA Syncer");
	Print("Starting SA Syncer");
}

void CConsole::Print(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    char timeStr[9];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", timeinfo);

    char buffer[256];
    vsprintf(buffer, format, args);
    std::string message = buffer;

    printf("[%s] %s", timeStr, message.c_str());
    va_end(args);
    printf("\n");
}