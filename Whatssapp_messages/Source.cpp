#include <iostream>
#include <string>
#include <cstring>
#include <conio.h>
#include <windows.h>
#include <tchar.h>

#define MYWM_NOTIFYICON (WM_APP+100)

using namespace std;

void hotKey(int key);
string correctNumber(string num);
LRESULT _stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam);

bool RHKret = RegisterHotKey(NULL, 0x00F, MOD_CONTROL, 0x57);

HHOOK hook;
KBDLLHOOKSTRUCT kbStruct;

//������� �������� �������� � ������� ��������
void hotKey() {
    string fromClipboard;//� ��� ���������� �������� ����� �� ������ ������
    if (OpenClipboard(0)) {//��������� ����� ������
        HANDLE hData = GetClipboardData(CF_TEXT);//��������� ����� �� ������ ������
        char* chBuffer = (char*)GlobalLock(hData);//��������� ������
        if (chBuffer) {
            fromClipboard = chBuffer;
            GlobalUnlock(hData);//������������ ������
            CloseClipboard();//��������� ����� ������
            string number = correctNumber(fromClipboard);
            if (number == "") {
                MessageBox(NULL, L"In you're clipboard dosen't have phone number", L"Error", MB_ICONINFORMATION); //�������������� ���� ��� �������� �� ����� ��������
            }
            else {
                string url = "https://api.whatsapp.com/send?phone="; // ������ �� whatsapp
                url = url + number; // ���������� ������ �������� � ������
                string op = string("start ").append(url);
                system(op.c_str()); //������ �������� � whatsapp
            }
        }
        else {
            MessageBox(NULL, L"Clipboard is empty, please copy phonenumber", L"Error", MB_ICONINFORMATION); //�������������� ���� ��� ����� ������ ����
            CloseClipboard();//��������� ����� ������
        }
    }
    else {
        MessageBox(NULL, L"Ooops, programm can't open clipboard", L"Error", MB_ICONINFORMATION); //�������������� ���� ��� ��������� �� ������ ������� ������ ������
    }
}

LRESULT _stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_KEYDOWN) {
            kbStruct = *((KBDLLHOOKSTRUCT*)lParam);
        }
    }
    return CallNextHookEx(hook, nCode, wParam, lParam);
}

// ������� ���������� ������ �������� � ����� "������ �����"
string correctNumber(string num) {
    string correctNum;
    if (num[0] == '0') {
        correctNum = "972";
        for (int i = 1; i < num.length(); i++) {
            if (num[i] == '0' || num[i] == '1' || num[i] == '2' || num[i] == '3' || num[i] == '4' || num[i] == '5' || num[i] == '6' || num[i] == '7' || num[i] == '8' || num[i] == '9') {
                correctNum = correctNum + num[i];
            }
        }
        return correctNum;
    }
    else {
        correctNum = "";
        for (int i = 0; i < num.length(); i++) {
            if (num[i] == '0' || num[i] == '1' || num[i] == '2' || num[i] == '3' || num[i] == '4' || num[i] == '5' || num[i] == '6' || num[i] == '7' || num[i] == '8' || num[i] == '9') {
                correctNum = correctNum + num[i];
            }
        }
        return correctNum;
    }
}


// ��������� ���������
LRESULT CALLBACK WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_USER:
        if (lParam == WM_RBUTTONDOWN)
            if (MessageBox(NULL, L"��������� ������?", L"Whatsapp messages", MB_YESNO) == IDYES)
                DestroyWindow(window);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(window, message, wParam, lParam);
    }
    return 0;
}

// ������� �������
int APIENTRY _tWinMain(HINSTANCE instance, HINSTANCE, LPTSTR, int) {

    // ����������� ������ ����
    WNDCLASSEX main = { 0 };
    main.cbSize = sizeof(WNDCLASSEX);
    main.hInstance = instance;
    main.lpszClassName = TEXT("Main");
    main.lpfnWndProc = WndProc;
    RegisterClassEx(&main);

    // �������� �������� ����
    HWND window = CreateWindowEx(0, TEXT("Main"), NULL, 0, 0, 0, 0, 0, NULL, NULL, instance, NULL);

    // �������� ������
    NOTIFYICONDATA nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = window;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    nid.hIcon = (HICON)LoadImage(NULL, L"favicon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_SHARED);
    nid.uCallbackMessage = WM_USER;
    memcpy(nid.szTip, L"Whatssapp \nmessage", 128);
    Shell_NotifyIcon(NIM_ADD, &nid);

    MSG message;
    while (GetMessage(&message, NULL, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessage(&message);

        //������ � ���������� ������
        if (message.message == WM_HOTKEY) {
            if (message.wParam == 0x00F) {
                hotKey();
            }
        }
    }

    Shell_NotifyIcon(NIM_DELETE, &nid); // �������� ������
    return 0;
}