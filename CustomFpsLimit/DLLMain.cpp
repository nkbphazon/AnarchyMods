#include <Windows.h>

#include "ModUtils.h"

using namespace ModUtils;
using namespace mINI;

float maximumFramerate = 100;

void ReadConfig()
{
	INIFile config(GetModFolderPath() + "\\config.ini");
	INIStructure ini;

	if (config.read(ini))
	{
		maximumFramerate = stof(ini["custom_fps_limit"]["limit"]);
	}
	else
	{
		ini["custom_fps_limit"]["limit"] = "100";
		config.write(ini, true);
	}
	Log("Maximum framerate: ", maximumFramerate);
}

DWORD WINAPI MainThread(LPVOID lpParam)
{
	if (GetCurrentProcessName() != "anarchyonline.exe") {
		CloseLog();
		return 0;
	}

	Log("Activating custom fps limit ...");

	Log("ProcessName=", GetCurrentProcessName());
	Log("IsNewEngine=", IsNewEngine());

	ReadConfig();

	HMODULE afcm = GetModuleHandleA("AFCM.dll");
	if (afcm == 0) {
		Log("Couldn't get address of AFCM");
	}
	else {
		Log("Getting address of MaximumFramerate variable");
		float* maxFramerate = (float*)GetProcAddress(afcm, "?m_vMaximumFramerate@Timer_t@@2MA");
		if (maxFramerate == 0) {
			Log("Couldn't get address of MaximumFramerate");
		}
		else {
			*maxFramerate = maximumFramerate;
			Log("Successfully updated maximum framerate");
		}
	}

	CloseLog();
	return 0;
}

BOOL WINAPI DllMain(HINSTANCE module, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
		CreateThread(0, 0, &MainThread, 0, 0, NULL);
	}
	return 1;
}