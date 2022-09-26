// SpaceCadetPinball.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include "winmain.h"

#include <whb/proc.h>
#include <whb/log_cafe.h>
#include <whb/log_udp.h>
#include <whb/log.h>

#ifdef USE_ROMFS
#include <romfs-wiiu.h>
#endif

int MainActual(LPCSTR lpCmdLine)
{
	WHBProcInit();
	WHBLogCafeInit();
	WHBLogUdpInit();
	KPADInit();
	WPADEnableURCC(true);
	WPADEnableWiiRemote(true);
#ifdef USE_ROMFS
	romfsInit();
#endif


	// SDL init
	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO |
		SDL_INIT_EVENTS) < 0)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Could not initialize SDL2", SDL_GetError(), nullptr);
		return 1;
	}

	// Todo: get rid of restart to change resolution.
	int returnCode;
	do
	{
		returnCode = winmain::WinMain(lpCmdLine);
	}
	while (winmain::RestartRequested());

	SDL_Quit();

#ifdef USE_ROMFS
	romfsExit();
#endif
	KPADShutdown();
	WHBLogUdpDeinit();
	WHBLogCafeDeinit();
	WHBProcShutdown();

	return returnCode;
}

int main(int argc, char* argv[])
{
	std::string cmdLine;
	for (int i = 1; i < argc; i++)
		cmdLine += argv[i];

	return MainActual(cmdLine.c_str());
}

#if _WIN32
#include <windows.h>

// Windows subsystem main
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	return MainActual(lpCmdLine);
}

// fopen to _wfopen adapter, for UTF-8 paths
FILE* fopenu(const char* path, const char* opt)
{
	wchar_t* wideArgs[2]{};
	for (auto& arg : wideArgs)
	{
		auto src = wideArgs[0] ? opt : path;
		auto length = MultiByteToWideChar(CP_UTF8, 0, src, -1, nullptr, 0);
		arg = new wchar_t[length];
		MultiByteToWideChar(CP_UTF8, 0, src, -1, arg, length);
	}

	auto fileHandle = _wfopen(wideArgs[0], wideArgs[1]);
	for (auto arg : wideArgs)
		delete[] arg;

	return fileHandle;
}
#endif

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
