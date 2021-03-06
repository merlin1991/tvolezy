#define STRICT
#define NOCRYPT

#include <windows.h>
#include <string>
#include <sstream>
#include "tvolezy.h"
#include "volxp.h"
#include "volvista.h"

HWND hWnd;
HWND hWndParent;
LPCSTR className = "tVolEzyWndClass";
LPCSTR revID = "tVolEzy 2.0 by Tobbe";
TveSettings settings;
Volume *vol;

void __cdecl bangVolUp(HWND caller, const char *args);
void __cdecl bangVolDown(HWND caller, const char *args);
void __cdecl bangToggleMute(HWND caller, const char *args);
void readSettings();
void reportVolumeError();
void reportError(LPCSTR msg);
void volumeChanged(int newValue);
void muteChanged(bool newValue);
LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL WINAPI DllMain(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}

extern "C" int __cdecl initModuleEx(HWND parentWnd, HINSTANCE dllInst, LPCSTR szPath)
{
	hWndParent = parentWnd;

	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(osvi);

	GetVersionEx(&osvi);

	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT && osvi.dwMajorVersion >= 6)
	{
		vol = new VolVista(settings);
	}
	else
	{
		vol = new VolXP(settings);
	}

	vol->setVolChangedCallback(volumeChanged);
	vol->setMuteChangedCallback(muteChanged);

	readSettings();

	WNDCLASS wc;
	memset(&wc, 0, sizeof(wc));
	wc.lpfnWndProc = (WNDPROC)wndProc;
	wc.hInstance = dllInst;
	wc.lpszClassName = className;
	wc.style = CS_NOCLOSE;
	if (!RegisterClass(&wc))
	{
		reportError("Error registering tVolEzy window class");
		return 1;
	}

	hWnd = CreateWindowEx(WS_EX_TOOLWINDOW, className, "", WS_CHILD,
		0, 0, 0, 0, hWndParent, NULL, dllInst, NULL);
	if (hWnd == NULL)
	{
		reportError("Error creating tVolEzy window");
		UnregisterClass(className, dllInst);
		return 1;
	}

	// Register our bangs with LiteStep
	AddBangCommand("!tVolEzyUp", bangVolUp);
	AddBangCommand("!tVolEzyDown", bangVolDown);
	AddBangCommand("!tVolEzyToggleMute", bangToggleMute);

	// Register message for version info
	UINT msgs[] = {LM_GETREVID, LM_REFRESH, 0};
	SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)hWnd, (LPARAM)msgs);

	return 0;
}

void __cdecl bangVolUp(HWND caller, const char* args)
{
	int steps = 0;
	if (args && args[0] != '\0')
	{
		steps = atoi(args);
	}

	if (!vol->up(steps))
	{
		reportVolumeError();
	}
}

void __cdecl bangVolDown(HWND caller, const char* args)
{
	int steps = 0;
	if (args && args[0] != '\0')
	{
		steps = atoi(args);
	}

	if (!vol->down(steps))
	{
		reportVolumeError();
	}
}

void __cdecl bangToggleMute(HWND caller, const char* args)
{
	if(!vol->toggleMute())
	{
		reportVolumeError();
	}
}

void readSettings()
{
	settings.showErrors = GetRCBoolDef("tVolEzyShowErrors", TRUE) != FALSE;
	settings.unmuteOnVolUp = GetRCBoolDef("tVolEzyUnmuteOnVolUp", TRUE) != FALSE;
	settings.unmuteOnVolDown = GetRCBoolDef("tVolEzyUnmuteOnVolDown", FALSE) != FALSE;
	char buffer[1024];
	GetRCString("tVolEzyVolumeChangedCommand", buffer, "!none", 1024);
	settings.volumeChangedCommand = buffer;
	GetRCString("tVolEzyMuteChangedCommand", buffer, "!none", 1024);
	settings.muteChangedCommand = buffer;
}

void reportVolumeError()
{
	switch (vol->getError())
	{
		case Volume::ERROR_OPENMIXER:
			reportError("Could not open mixer");
			break;
		case Volume::ERROR_LINEINFO:
			reportError("Could not get line info");
			break;
		case Volume::ERROR_LINECONTROLS:
			reportError("Could not get line controls");
			break;
		case Volume::ERROR_CONTROLDETAILS:
			reportError("Could not get control details");
			break;
		case Volume::ERROR_SETDETAILS:
			reportError("Could not set control details");
			break;
		case Volume::ERROR_ACTIVATE:
			reportError("Could not activate endpoint device");
			break;
		case Volume::ERROR_GETDEFAULT:
			reportError("Could not get default endpoint device");
			break;
		case Volume::ERROR_GETMUTE:
			reportError("Could not get current mute status");
			break;
		case Volume::ERROR_GETVOL:
			reportError("Could not get current volume");
			break;
		case Volume::ERROR_INVALIDARG:
			reportError("The pActivationParams parameter must be NULL for \
				the specified interface; or pActivationParams points \
				to invalid data.");
			break;
		case Volume::ERROR_NOIFACE:
			reportError("The object does not support the requested interface type.");
			break;
		case Volume::ERROR_NOTFOUND:
			reportError("No device is available");
			break;
		case Volume::ERROR_OUTOFMEM:
			reportError("Out of memory");
			break;
		case Volume::ERROR_OUTOFRANGE:
			reportError("Parameter is out of range");
			break;
		case Volume::ERROR_PPDEVICENULL:
			reportError("ppDevice is NULL");
			break;
		case Volume::ERROR_PPINTERFACENULL:
			reportError("ppInterface is NULL");
			break;
		case Volume::ERROR_REMOVED:
			reportError("The audio endpoint device or the adapter device that \
				the endpoint device connects to has been removed");
			break;
		case Volume::ERROR_SETMUTE:
			reportError("Could not mute the volume");
			break;
		case Volume::ERROR_SETVOL:
			reportError("Could not set the volume");
			break;
		case Volume::ERROR_CALLBACK:
			reportError("Error while creating callback");
			break;
		default:
			break;
	}
}

void reportError(LPCSTR msg)
{
	if (settings.showErrors)
	{
		MessageBox(NULL, msg, "tVolEzy error", MB_OK | MB_ICONERROR);
	}
}

void volumeChanged(int newValue)
{
	std::string command = settings.volumeChangedCommand;
	std::string::size_type volTokenIndex = command.find("#VOLUME#", 0);
	std::string commandFirstPart = "";
	std::string commandLastPart = "";
	if (volTokenIndex != std::string::npos)
	{
		commandFirstPart = command.substr(0, volTokenIndex);
		commandLastPart = command.substr(volTokenIndex + 8);
	}

	std::ostringstream cmd;
	cmd << commandFirstPart << newValue << commandLastPart;

	LSExecute(NULL, cmd.str().c_str(), SW_HIDE);
}

void muteChanged(bool newValue)
{
	std::string command = settings.muteChangedCommand;
	std::string::size_type volTokenIndex = command.find("#MUTE#", 0);
	std::string commandFirstPart = "";
	std::string commandLastPart = "";
	if (volTokenIndex != std::string::npos)
	{
		commandFirstPart = command.substr(0, volTokenIndex);
		commandLastPart = command.substr(volTokenIndex + 6);
	}

	std::ostringstream cmd;

	cmd << commandFirstPart;

	if (newValue)
	{
		cmd << "muted";
	}
	else
	{
		cmd << "unmuted";
	}

	cmd << commandLastPart;

	LSExecute(NULL, cmd.str().c_str(), SW_HIDE);
}

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message){
		case LM_GETREVID:
			lstrcpy((LPSTR)lParam, revID);
			return strlen((LPTSTR)lParam);

		case LM_REFRESH:
			readSettings();
			return 0;

		case WM_DESTROY:
			hWnd = NULL;
			return 0;

		default:
			break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

extern "C" void __cdecl quitModule(HINSTANCE dllInst)
{
	RemoveBangCommand("!tVolEzyUp");
	RemoveBangCommand("!tVolEzyDown");
	RemoveBangCommand("!tVolEzyToggleMute");

	delete vol;

	UINT msgs[] = {LM_GETREVID, LM_REFRESH, 0};
	SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)hWnd, (LPARAM)msgs);

	if (hWnd != NULL)
	{
		DestroyWindow(hWnd);
	}

	UnregisterClass(className, dllInst);
}