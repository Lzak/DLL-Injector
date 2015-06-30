#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>

using namespace std;

bool InjectDLL(DWORD ProcessId);
char FileToInject[] = "ACubeDll.dll";
char ProcessName[] = "ac_client.exe";
typedef HINSTANCE(*fpLoadLibrary)(char*);

int main() {

	DWORD processId = NULL;
	PROCESSENTRY32 pe32 = { sizeof(PROCESSENTRY32) };
	HANDLE hProcSnap;

	while (!processId) {
		cout << "Searching for " << ProcessName << "...\n";
		cout << "Make sure your game is running!\n\n";

		hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if (Process32First(hProcSnap, &pe32)) {
			do {

				if (!strcmp(pe32.szExeFile, ProcessName)) {
					processId = pe32.th32ProcessID;
					break;
				}
			} while (Process32Next(hProcSnap, &pe32));
		}
		Sleep(1000);
	}
	while (!InjectDLL(processId)) {
		cout << "DLL Failed to inject!\n";
		Sleep(1000);
	}
	cout << "DLL Injected successfuly!\n";
	cout << "Closing injector in FIVE seconds!";
	CloseHandle(hProcSnap);
	Sleep(5000);
	return 0;
}

bool InjectDLL(DWORD ProcessID) {
	HANDLE hProc;
	LPVOID paramAddr;
	HINSTANCE hDll = LoadLibrary("KERNAL32");
	fpLoadLibrary LoadLibraryAddr = (fpLoadLibrary)GetProcAddress(hDll, "LoadLibraryA");

	hProc = OpenProcess(PROCESS_ALL_ACCESS, false, ProcessID);

	char dllPath[250] = "Е:\\HACKS\\";
	strcat_s(dllPath, FileToInject);

	paramAddr = VirtualAllocEx(hProc, 0, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
	bool memoryWritten = WriteProcessMemory(hProc, paramAddr, dllPath, strlen(dllPath) + 1, NULL);
	CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryAddr, paramAddr, 0, 0);

	CloseHandle(hProc);
	return memoryWritten;
}