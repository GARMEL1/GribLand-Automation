#include <iostream>
#include <windows.h>
#include <tlhelp32.h>

using namespace std;

// Функция для проверки, является ли окно окном с нужным процессом
bool isTargetWindow(HWND window, DWORD targetProcessId) {
    DWORD processId;
    GetWindowThreadProcessId(window, &processId);

    return processId == targetProcessId;
}

// Функция для получения ID процесса по имени
DWORD GetProcessIdByName(const char* processName) {
    DWORD processId = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(processEntry);

        if (Process32First(snapshot, &processEntry)) {
            do {
                if (_stricmp(processEntry.szExeFile, processName) == 0) {
                    processId = processEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(snapshot, &processEntry));
        }

        CloseHandle(snapshot);
    }

    return processId;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

    const char* targetProcessName = "java.exe";
    DWORD targetProcessId = GetProcessIdByName(targetProcessName);

    if (targetProcessId == 0) {
        cerr << "Ошибка: Процесс " << targetProcessName << " не найден." << endl;
        return 1;
    }

    bool scriptRunning = false;  
    bool hKeyState = false;      
    HWND currentWindow = GetForegroundWindow();  

    cout << "Нажмите клавишу 'F6' для старта и паузы скрипта." << endl;

   
    setvbuf(stdout, NULL, _IONBF, 0);

    while (true) {
        HWND activeWindow = GetForegroundWindow();

        if (activeWindow != currentWindow) {

            currentWindow = activeWindow;
            scriptRunning = false;  
        }

        if ((GetAsyncKeyState(VK_F6) & 0x8001) != 0) {  
            if (!hKeyState) {
                hKeyState = true;

                // Проверка, является ли текущее активное окно целевым окном
                if (isTargetWindow(currentWindow, targetProcessId)) {
                    scriptRunning = !scriptRunning; 

                    if (scriptRunning) {
                        // Удерживание Левой кнопки мыши, клавиши 'W', клавиши 'Z'
                        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                        keybd_event('W', 0, 0, 0);
                        keybd_event('Z', 0, 0, 0);
                    } else {
                        // Отпускание Левой кнопки мыши, клавиши 'W', клавиши 'Z'
                        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                        keybd_event('W', 0, KEYEVENTF_KEYUP, 0);
                        keybd_event('Z', 0, KEYEVENTF_KEYUP, 0);
                    }
                }
            }
        } else {
            hKeyState = false;
        }
    }

    return 0;
}
