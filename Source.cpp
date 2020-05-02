#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "Offsets.h"

using namespace std;

uintptr_t moduleBase;
DWORD procId;
HWND hwnd;
HANDLE hProcess;
HDC hdc;

struct Vector2
{
	float x, y, z;
};

struct Vector3 
{
	float x, y, z;
};

struct Vector4
{
	float x, y, z, w;
};

//------------------------Get dll------------------------//
uintptr_t GetModuleBaseAddress(const char* modName) {
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry)) {
			do {
				if (!strcmp(modEntry.szModule, modName)) {
					CloseHandle(hSnap);
					return (uintptr_t)modEntry.modBaseAddr;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
}

//------------------------Read memory------------------------//
template<typename T> T RPM(SIZE_T address) {
	T buffer;
	ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, sizeof(T), NULL);
	return buffer;
}

//------------------------Write memory------------------------//
template<typename T> void WPM(SIZE_T address, T buffer) {
	WriteProcessMemory(hProcess, (LPVOID)address, &buffer, sizeof(buffer), NULL);
}

//------------------------Gets localPlayer------------------------//
uintptr_t localPlayer() {
	return RPM<uintptr_t>(moduleBase + dwLocalPlayer);
}

//------------------------Gets team------------------------//
int getTeam(uintptr_t player) {
	return RPM<int>(player + m_iTeamNum);
}

//------------------------Gets Health------------------------//
int getHealth(uintptr_t player) {
	return RPM<int>(player + m_iHealth);
}

//------------------------Main function------------------------//
int main() {
	hwnd = FindWindowA(NULL, "Counter-Strike: Global Offensive");				//Finds the window with the name: Counter-Strike: Global Offensive
	GetWindowThreadProcessId(hwnd, &procId);						                //Looks for the window + process id 
	moduleBase = GetModuleBaseAddress("client_panorama.dll");				    //Returns the address of: client_panorama.dll
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);				    //Opens the process with access to read and write

	while (!GetAsyncKeyState(VK_F1)) {							                    //Loop ends when F1 is pressed
		int localPlayerTeam = getTeam(localPlayer());					            //Gets the team of the localPlayer	
		int localPlayerHeatlh = getHealth(localPlayer());				          //Gets the health of the localPlayer

		if (localPlayerTeam < 1) {
			localPlayerTeam = 0;
		}

		if (localPlayerHeatlh < 0) {
			localPlayerHeatlh = 0;
		}

		cout << "health: " << localPlayerHeatlh << endl;			
		cout << "team number: " << localPlayerTeam << endl;
		system("cls");
	}

	return 0;
}
