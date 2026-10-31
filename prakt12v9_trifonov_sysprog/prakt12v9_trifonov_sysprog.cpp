// prakt12v9_trifonov_sysprog.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <windows.h>
using namespace std;

struct MyProcess {
    DWORD pid;
    HANDLE hProcess;
    wstring name;
    string state;
};

MyProcess list_proc[100];
int proc_count = 0;

void checkStatus() {
    for (int i = 0; i < proc_count; i++) {
        if (list_proc[i].state == "konec") continue;
        
        DWORD code;
        if (GetExitCodeProcess(list_proc[i].hProcess, &code)) {
            if (code != STILL_ACTIVE) {
                list_proc[i].state = "konec";
                CloseHandle(list_proc[i].hProcess);
            }
            else {
                list_proc[i].state = "rabotaet";
            }
        }
    }
}

void CreateProc(const wchar_t* lpApplicationName, wchar_t* lpCommandLine) {
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(STARTUPINFOW));
    si.cb = sizeof(STARTUPINFOW);
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

    if (!CreateProcessW(lpApplicationName, lpCommandLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        cout << "Дочерний процесс не создан" <<GetLastError()<< endl;
        return;
    }
    list_proc[proc_count].pid = pi.dwProcessId;
    list_proc[proc_count].hProcess = pi.hProcess;

    if (lpApplicationName != NULL) {
        list_proc[proc_count].name = lpApplicationName;
    }
    else {
        list_proc[proc_count].name = L"prakt12_trifonov_sysprog.exe";
    }

    list_proc[proc_count].state = "rabotaet";
    proc_count++;
    cout << "Дочерний процесс создан" << endl;
    CloseHandle(pi.hThread);
}

int main()
{
    setlocale(NULL, "Russian");
    int menu = 0;

    while (true) {
        cout << "Меню" << endl;
        cout << "1. Запуск taskmgr.exe" << endl;
        cout << "2. Запуск ping.exe"<<endl;
        cout << "3. Запуск prakt12_trifonov_sysprog.exe" << endl;
        cout << "4. Показать список процессов" << endl;
        cout << "5. Закончить процесс по id"<<endl;
        cout << "6. Выход" << endl;
        cout << "Выбор: " << endl;
        cin >> menu;

        if (menu == 6) {
            for (int i = 0; i < proc_count; i++) {
                if (list_proc[i].state == "rabotaet") {
                    TerminateProcess(list_proc[i].hProcess, 0);
                    CloseHandle(list_proc[i].hProcess);
                }
            }
            break;
        }

        if (menu == 1) {
            const wchar_t* lpAppName = L"C:\\Windows\\System32\\taskmgr.exe";
            CreateProc(lpAppName, NULL);
        }
        else if (menu == 2) {
            wchar_t cmd[] = L"ping.exe 127.0.0.1";
            CreateProc(NULL, cmd);
        }
        else if (menu == 3) {
            string s, e;
            cout << "Начало диапозона: "; cin >> s;
            cout << "Конец диапозона: "; cin >> e;

            string cmdStr = "prakt12_trifonov_sysprog.exe " + s + " " + e;
            wstring wcmd(cmdStr.begin(), cmdStr.end());
            wchar_t* cmd = new wchar_t[wcmd.length() + 1];
            wcscpy_s(cmd, wcmd.length() + 1, wcmd.c_str());

            CreateProc(NULL, cmd);
            delete[] cmd;
        }
        else if (menu == 4) {
            checkStatus();
            cout << endl << "ID PROCESS\tСостояние" << endl;
            cout << endl;
            int active = 0;
            for (int i = 0; i < proc_count; i ++) {
                cout << list_proc[i].pid << "\t\t" << list_proc[i].state << endl;
                if (list_proc[i].state == "rabotaet") active++;
            }
            cout << "Активных: " << active << endl;
        }
        else if (menu == 5) {
            DWORD id;
            cout << "Введите id процесса для завершения: "; cin >> id;
            bool found = false;
            for (int i = 0; i < proc_count; i++) {
                if (list_proc[i].pid = id && list_proc[i].state == "rabotaet") {
                    if (TerminateProcess(list_proc[i].hProcess, 0)) {
                        list_proc[i].state = "konec";
                        CloseHandle(list_proc[i].hProcess);
                        cout << "Завершен" << endl;
                    }
                    found = true;
                    break;
                }
            }
            if (!found) cout << "Такой процесс не найден или уже завершен" << endl;
        }
    }
}