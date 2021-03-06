#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

using namespace std;

typedef unsigned char uint8_t;

template<typename T, size_t N>

size_t countof(T(&array)[N])
{
	return N;
}

/* Offsets */

DWORD dwLocalPlayer;
DWORD dwEntityList;
DWORD dwGlow;

DWORD dwTeam = 0xF0;
DWORD dwDormant = 0xE9;

struct SModule
{
	DWORD dwBase;
	DWORD dwSize;
};
/*writing a simple "debugger"/processAPI imolementation class to our hack...*/

class debugger
{
private:
	DWORD pId;
	HANDLE Process;
public:
	//check if csgo is running, and return it's pid.
	bool Attach(char* process, DWORD rights = PROCESS_ALL_ACCESS)
	{
		HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(entry);
		do {
			if (!strcmp((const char*)entry.szExeFile, process)) {
				pId = entry.th32ProcessID;
				CloseHandle(handle);
				Process = OpenProcess(rights, false, pId);
				return true;
			}
		} while (Process32Next(handle, &entry));
		return false;
	}

	//find module by it's name in progress csgo and return it's addres.
	SModule GetModule(char* modulename) {
		HANDLE module = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, NULL);
		MODULEENTRY32 mEntry;
		mEntry.dwSize = sizeof(mEntry);
		do {
			if (!strcmp((const char*)mEntry.szModule, modulename)) {
				SModule mod = { (DWORD)mEntry.hModule, mEntry.modBaseSize };
				return mod;
			}
		} while (Module32Next(module, &mEntry));
		return { (DWORD)false, (DWORD)false };
	}
	template<typename T>
	//read from memory addres
	T Read(DWORD addres) {
		T _read;
		ReadProcessMemory(Process, (LPVOID)addres, &_read, sizeof(T), NULL);
		return _read;
	}
	//write to memory addres
	template<typename T>
	void Write(DWORD addres, T value) {
		WriteProcessMemory(Process, (LPVOID)addres, &value, sizeof(T), NULL);
	}

	/*compare data*/
	bool DataCompare(const BYTE* data, const BYTE* mask, LPCSTR szMask) {
		for (; *szMask; ++szMask, ++data, ++mask) {
			if (*szMask == 'x' && *data != *mask) {
				return false;
			}
		}
		return (*szMask == NULL);
	}
	
	/*find pattern in addres space of csgo*/
	DWORD FindPattern(DWORD start, DWORD size, LPCSTR sig, LPSTR mask) {
		BYTE* data = new BYTE[size];
		DWORD bytesread;
		if (!ReadProcessMemory(Process, (LPVOID)start, data, size, &bytesread)) {
			return NULL;
		}
		for (DWORD i = 0; i < size; i++) {
			if (DataCompare((CONST BYTE*)(data + i), (CONST BYTE*)sig, mask)) {
				return start + i;
			}
		}
		return NULL;
	}
	
	/*find pattern array*/
	DWORD FindPatternArray(DWORD start, DWORD size, LPSTR mask, int count, ...) {
		char* sig = new char[count + 1];
		va_list ap;
		va_start(ap, count);
		for (int i = 0; i < count; i++) {
			char read = va_arg(ap, char);
			sig[i] = read;
		}
		va_end(ap);
		sig[count] = '\0';
		return FindPattern(start, size, sig, mask);
	}
};

/*finished...*/

struct glow_t
{
	DWORD dwBase;
	float r, g, b, a;
	uint8_t unk[16];
	bool m_bRenderEhenOccluded;
	bool m_bRenderWhenUnoccluded;
	bool m_bFullBloom;
	uint8_t unk2[10];
}; //our glow object structure in csgo

struct entity
{
	DWORD dwBase;
	int team;
	bool is_dowmant;
};

struct player
{
	DWORD dwBase;
	bool isDormant;
};

debugger memory;
SModule module;
int iFriendlies;
int iEnemies;
entity entEnemnies[32];
entity entFriendlies[32];
entity me;
debugger* mem;
SModule moduleClient;

/* retrieve offset ...*/

void update_local_player()
{
	DWORD dwStart = memory.FindPatternArray(module.dwBase, module.dwSize, (LPSTR)"xxx????xx????xxxxx?", 19, 0x8D, 0x34, 0x85, 0x0, 0x0, 0x0, 0x0, 0x89, 0x15, 0x0, 0x0, 0x0, 0x0,0x88, 0x41, 0x8, 0x88,0x48, 0x0);
	DWORD dwP1 = memory.Read<DWORD>(dwStart + 3);
	DWORD dwP2 = memory.Read<DWORD>(dwStart + 18);
	dwLocalPlayer = (dwP1 + dwP2) - module.dwBase;
}

void update_entity_list()
{
	DWORD dwStart = memory.FindPatternArray(module.dwBase, module.dwSize, (LPSTR)"x????xx?xxx", 11, 0x5, 0x0, 0x0, 0x0,0xC1,0xE9,0x0,0x39, 0x48,0x4);
	DWORD dwP1 = memory.Read<DWORD>(dwStart + 1);
	DWORD dwP2 = memory.Read<DWORD>(dwStart + 7);
	dwEntityList = (dwP1 + dwP2) - module.dwBase;
}

void update_glow()
{
	DWORD dwStart = memory.FindPatternArray(module.dwBase, module.dwSize, (LPSTR)"xx????x????xxx????xx????xx", 27, 0x8D, 0x8F, 0, 0, 0, 0, 0xA1, 0, 0, 0,0,0xC7, 0x4, 0x2, 0, 0, 0, 0,0x89, 0x35, 0x0, 0x0, 0x0, 0x0, 0x88, 0x51);
	dwGlow = memory.Read<DWORD>(dwStart + 7) - module.dwBase;
}

//well that sucked...

void GetOffsets(debugger* d) {
	mem = d;
	moduleClient = mem->GetModule((char*)"client.dll");
	update_local_player();
	update_entity_list();
	update_glow();
}

void update_entity_data(entity* e, DWORD dwBase) {
	int Dormant = memory.Read<int>(dwBase + dwDormant);
	e->dwBase = dwBase;
	e->team = memory.Read<int>(dwBase + dwTeam);
	e->is_dowmant = Dormant == 1;
}

SModule* GetClientModule() {
	if (module.dwBase == 0 && module.dwSize == 0) {
		module = memory.GetModule((char*)"client.dll");
	}
	return &module;
}

entity* GetEntityByBase(DWORD dwBase) {
	for (int i = 0; i < iFriendlies; i++) {
		if (dwBase == entEnemnies[i].dwBase) {
			return &entEnemnies[i];
		}
	}
	for (int i = 0; i < iEnemies; i++) {
		if (dwBase == entEnemnies[i].dwBase) {
			return &entEnemnies[i];
		}
	}
}

DWORD __stdcall UpdateOffsets(LPVOID) {
	entity players[64];
	while (true) {
		DWORD PlayerBase = memory.Read<DWORD>(GetClientModule()->dwBase + dwLocalPlayer);
		int cp = 0;

		update_entity_data(&me, PlayerBase);
		for (int i = 0; i < cp; i++) {
			DWORD entBase = memory.Read<DWORD>((GetClientModule()->dwBase + dwEntityList) + i * 0x10);
			if (entBase == NULL)
				continue;
			update_entity_data(&players[cp], entBase);
			cp++;
		}

		int cf = 0, ce = 0;
		for (int i = 0; i < cp; i++) {
			if (players[i].team == me.team) {
				cf++;
			} else {
				entEnemnies[ce] = players[i];
				ce++;
			}			
		}
		iEnemies = ce;
		iFriendlies = cf;
	}
}

void start() {
	while (!memory.Attach((char*)"csgo.exe")) {
		Sleep(100);
	} do {
		Sleep(1000);
		GetOffsets(&memory);
	} while (dwLocalPlayer < 65535);
	CreateThread(0, 0, &UpdateOffsets, 0, 0, 0);
}

void glow_player(DWORD mObj, float r, float g, float b)
{
	memory.Write<float>(mObj + 0x4, r);
	memory.Write<float>(mObj + 0x8, g);
	memory.Write<float>(mObj + 0xC, b);
	memory.Write<bool>(mObj + 0x24, true);
	memory.Write<bool>(mObj + 0x25, false);
}

float sanitize_color(int val) {
	if (val > 255) val = 255;
	if (val < 0) val = 0;
	return (float)val / 255;
}

DWORD __stdcall ESP_THREAD(LPVOID)
{
	int objectCount;
	DWORD pointerToGlow;

	float Friend = sanitize_color(100);
	float Enemy = sanitize_color(140);

	while (true)
	{
		pointerToGlow = memory.Read<DWORD>(GetClientModule()->dwBase + dwGlow);
		objectCount = memory.Read<DWORD>(GetClientModule()->dwBase + dwGlow + 0x4);
		if (pointerToGlow != NULL && objectCount > 0) {
			for (int i = 0; i < objectCount; i++)
			{
				DWORD mObj = pointerToGlow + i * sizeof(glow_t);
				glow_t glowObject = memory.Read<glow_t>(mObj);
				entity* Player = GetEntityByBase(glowObject.dwBase);
				if (glowObject.dwBase == NULL || Player == nullptr || Player->is_dowmant) {
					continue;
				}
				if (me.team == Player->team) {
					glow_player(mObj, 0, 0, Friend);
				}
				else {
					glow_player(mObj, Enemy, 0, 0);
				}
			}
		}
	}
	return 0;
}

int main()
{
	bool enabled = false;
	HANDLE ESP = NULL;
	start();
	std::cout << "F1 to toggle ESP!" << std::endl;
	while (true)
	{
		if (GetAsyncKeyState(VK_F1) & 1)
		{
			enabled = !enabled;
			if (enabled) {
				std::cout << "ESP on" << std::endl;
				ESP = CreateThread(0, 0, &ESP_THREAD, 0, 0, 0);
			}
			else {
				std::cout << "ESP off" << std::endl;
				TerminateThread(ESP, 0);
				CloseHandle(ESP);
			}
		}
	}
}