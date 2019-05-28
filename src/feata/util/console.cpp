#include <cstdio>
#include "util/console.hpp"
#include "defs/arch.hpp"


#if defined(FEATA_SYSTEM_WIN)

static bool AllocConsoleWin();
static bool FreeConsoleWin();

#elif defined(FEATA_SYSTEM_UNIX)

static bool AllocConsoleUnix();
static bool FreeConsoleUnix();

static FILE* gConsoleHandle = nullptr;

#endif

namespace util::console
{
	bool AllocConsole()
	{
		bool res = false;

#if defined(FEATA_SYSTEM_WIN)
		if((res = AllocConsoleWin()))
		{
			freopen("CONIN$", "r", stdin);
			freopen("CONOUT$", "w", stdout);
			freopen("CONOUT$", "w", stderr);
		}
#elif defined(FEATA_SYSTEM_UNIX)
		res = AllocConsoleUnix();
#endif
		return res;
	}

	bool FreeConsole()
	{
#if defined(FEATA_SYSTEM_WIN)
		return FreeConsoleWin();
#elif defined(FEATA_SYSTEM_UNIX)
		return FreeConsoleUnix();
#else
		return false;
#endif
	}
}

#if defined(FEATA_SYSTEM_WIN)

#include <Windows.h>

bool AllocConsoleWin()
{
	return AllocConsole();
}

bool FreeConsoleWin()
{
	return FreeConsole();
}

#elif defined(FEATA_SYSTEM_UNIX)

#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>


bool AllocConsoleUnix()     // still does not work
{
    const std::string pname = "";// tempnam(nullptr, nullptr);
    if(pname.empty())
        return false;

    if(mkfifo(pname.c_str(), 0777))
        return false;

    if(fork() == 0)
    {
        setsid();
        execlp("x-terminal-emulator", "-hold", "-e", "cat", pname.c_str(), nullptr);
    }

    gConsoleHandle = freopen(pname.c_str(), "w", stdout);
    if(!gConsoleHandle)
        return false;

    return true;
}

bool FreeConsoleUnix()
{
    if(gConsoleHandle)
    {
        fclose(gConsoleHandle);
        gConsoleHandle = nullptr;
        return true;
    }

	return false;
}

#endif
