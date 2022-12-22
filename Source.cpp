#include <Windows.h>
#include <Functions.h>
HMENU SubMenu;
HMENU RootMenu;
HWND list;
HWND edit_path;
HWND cap_name;
HWND cap_size;
HWND cap_path;
HWND edit;
HWND bt_save;
HWND bt_unsave;
HWND bt_search;
HWND bt_paste;
BOOL Edit_GetModify;
BOOL CHECKFLAG = TRUE;
string FileNameText = "Ім'я файлу: ";
string FilePathText = "Шлях до файлу: ";
string FileSizeText = "Poзмір файлу : ";
vector<InfFiles> Files;
int number;
HFONT hFont = CreateFont(15, 5, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, L"Comic Sans");
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
	WNDCLASS SoftwareMainClass = NewWindowClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInst,
		NULL, L"MainWndClass", SoftwareMainProcedure);

	if (!RegisterClassW(&SoftwareMainClass)) { return -1; }
	MSG SoftwareMainMessage = { 0 };

	CreateWindow(L"MainWndClass", L"INF Explore", WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION | WS_SYSMENU, 100, 100, 725, 375, NULL, NULL, NULL, NULL);

	while (GetMessage(&SoftwareMainMessage, NULL, NULL, NULL)) {
		TranslateMessage(&SoftwareMainMessage);
		DispatchMessage(&SoftwareMainMessage);
	}
}

WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Procedure) {
	WNDCLASS NWC = { 0 };

	NWC.hIcon = Icon;
	NWC.hCursor = Cursor;
	NWC.hInstance = hInst;
	NWC.lpszClassName = Name;
	NWC.hbrBackground = BGColor;
	NWC.lpfnWndProc = Procedure;

	return NWC;
}

LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
        
	case WM_COMMAND:
        if (LOWORD(wp) == list_id)
        {
            if (HIWORD(wp) == CBN_SELCHANGE) {
                number = SendMessage(list, CB_GETCURSEL, 0, 0);
                ShowWindow(edit_path, SW_SHOW);
                string buf = FileSizeText + to_string(Files[number].Size);
                SetWindowTextA(cap_name, (FileNameText + Files[number].Name).c_str());
                SetWindowTextA(cap_size, (buf.c_str()));
                SetWindowTextA(cap_path, FilePathText.c_str());
                SetWindowTextA(edit_path, (Files[number].ExecutablePath).c_str());
                }
        }
		switch (wp)	{
        case MenuUTF16:
			CHECKFLAG = TRUE;
			SetCheck(hWnd);
			break;
		case MenuUTF8:
			CHECKFLAG = FALSE;
			SetCheck(hWnd);
			break;
        case ClearMenu:
            SetWindowTextA(edit, "");
            break;
        case BT_SEARCH:
            SearchFiles(Files);
            SortFiles(Files);
            for (int i = 0; i < Files.size(); i++)
            {
                wstring wide_string = wstring(Files[i].Name.begin(), Files[i].Name.end());
                const wchar_t* result = wide_string.c_str();
                SendMessage(list, CB_ADDSTRING, 0, LPARAM(result));
            }
            EnableWindow(GetDlgItem(hWnd, BT_SAVE), TRUE);
            EnableWindow(GetDlgItem(hWnd, BT_UNSAVE), TRUE);
            EnableWindow(GetDlgItem(hWnd, BT_PASTE), TRUE);
            EnableWindow(GetDlgItem(hWnd, BT_SEARCH), FALSE);
            break;
        case BT_PASTE:
            if (CHECKFLAG == TRUE)
            {
                LoadData(Files[number].ExecutablePath);
            }
            else
            {
                LoadDataUTF8(Files[number].ExecutablePath);
            }
            break;
        case BT_SAVE:
            if (CHECKFLAG == TRUE)
            {
             SaveData(Files[number].ExecutablePath);
            }
            else
            {
             SaveDataUTF8(Files[number].ExecutablePath);
            }
            Edit_GetModify = SendMessage(edit, EM_SETMODIFY, 0, FALSE);
            break;
        case BT_UNSAVE:
            
            break;
      
		default:break;
		}
		break;
	case WM_CREATE:
        SetCheck(hWnd);
        MainWndAddWidgets(hWnd);
        EnableWindow(GetDlgItem(hWnd, BT_SAVE), FALSE);
        EnableWindow(GetDlgItem(hWnd, BT_UNSAVE), FALSE);
        EnableWindow(GetDlgItem(hWnd, BT_PASTE), FALSE);
		break;
	case WM_DESTROY:

        Edit_GetModify = SendMessage(edit, EM_GETMODIFY, 0, 0);
        if (Edit_GetModify == TRUE) {
            int msgboxID = MessageBox(
                NULL,
                L"Ви не зберегли файл!.\nБажаєте його зберегти?",
                L"Ви не зберегли файл!",
                MB_ICONEXCLAMATION | MB_YESNO
            );
            if (msgboxID == IDYES)
            {
                if (CHECKFLAG == TRUE)
                {
                    SaveData(Files[number].ExecutablePath);
                }
                else
                {
                    SaveDataUTF8(Files[number].ExecutablePath);
                }
                PostQuitMessage(0);
            }
            else
            {
                PostQuitMessage(0);
            }
        }
        else
        {
            PostQuitMessage(0);
        }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        SelectObject(hdc, hFont);

        //(bt_unsave, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
        // All painting occurs here, between BeginPaint and EndPaint.

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_CTLCOLORSTATIC:
        if (((HWND)lp) == GetDlgItem(hWnd, 3))
        {
            SetTextColor((HDC)wp, RGB(0, 0, 255));
        }
        return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
        break;
  	default: return DefWindowProc(hWnd, msg, wp, lp);
	}
}

void SetCheck(HWND hWnd) {
    HMENU Clear = CreateMenu();
    
	RootMenu = CreateMenu();
	SubMenu = CreateMenu();
	AppendMenu(SubMenu, MF_STRING, MenuUTF16, L"UTF-16");
	AppendMenu(SubMenu, MF_STRING, MenuUTF8, L"UTF-8");
	AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)SubMenu, L"Кодування");
    AppendMenu(RootMenu, MF_STRING, ClearMenu, L"Очистити");
	if (CHECKFLAG == TRUE)
	{
		CheckMenuItem(RootMenu, MenuUTF16, MF_CHECKED);
		CheckMenuItem(RootMenu, MenuUTF8, MF_UNCHECKED);
        if (Files.size()>1)
        {
          LoadData(Files[number].ExecutablePath);
        }
	}
	else
	{
		CheckMenuItem(RootMenu, MenuUTF8, MF_CHECKED);
		CheckMenuItem(RootMenu, MenuUTF16, MF_UNCHECKED);
        if (Files.size() > 1)
        {
            LoadDataUTF8(Files[number].ExecutablePath);
        }
	}
	SetMenu(hWnd, RootMenu);

}
void MainWndAddWidgets(HWND hWnd) {

	bt_search = CreateWindow(
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"Пошук",      // Button text 
        WS_VISIBLE | WS_CHILD,  // Styles 
        10,         // x position 
        10,         // y position 
        70,        // Button width
        30,        // Button height
        hWnd,     // Parent window
        HMENU(BT_SEARCH),       // No menu.
        NULL,
        NULL);      // Pointer not needed.
    bt_paste = CreateWindow(
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"Зчитати",      // Button text 
        WS_VISIBLE | WS_CHILD,  // Styles 
        10,         // x position 
        50,         // y position 
        70,        // Button width
        30,        // Button height
        hWnd,     // Parent window
        HMENU(BT_PASTE),       // No menu.
        NULL,
        NULL);      // Pointer not needed.
  bt_save =  CreateWindow(
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"Зберегти",      // Button text 
        WS_VISIBLE | WS_CHILD,  // Styles 
        100,         // x position 
        10,         // y position 
        89,        // Button width
        30,       // Button height
        hWnd,     // Parent window
        HMENU(BT_SAVE),       // No menu.
        NULL,
        NULL);      // Pointer not needed.
   bt_unsave = CreateWindow(
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"Не Зберігати",      // Button text 
        WS_VISIBLE | WS_CHILD,  // Styles 
        100,         // x position 
        50,         // y position 
        88,        // Button width
        30,        // Button height
        hWnd,     // Parent window
        HMENU(BT_UNSAVE),       // No menu.
        NULL,
        NULL);      // Pointer not needed.
    list = CreateWindow(
        L"COMBOBOX",  // Predefined class; Unicode assumed 
        L" ",      // Button text 
        WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL,  // Styles 
        10,         // x position 
        100,         // y position 
        180,        // Button width
        205,        // Button height
        hWnd,     // Parent window
        HMENU(list_id),       // No menu.
        NULL,
        NULL);      // Pointer not needed.
     // Run the message loop.
   edit = CreateWindow(
        L"EDIT",  // Predefined class; Unicode assumed 
        NULL,      // Button text 
        WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL| WS_HSCROLL | ES_MULTILINE,  // Styles 
        200,         // x position 
        100,         // y position 
        500,        // Button width
        205,        // Button height
        hWnd,     // Parent window
        HMENU(EDIT_FILE),       // No menu.
        NULL,
        NULL);      // Pointer not needed.
   cap_name = CreateWindow(
        L"Static",  // Predefined class; Unicode assumed 
        L"",      // Button text 
        WS_CHILD| WS_VISIBLE,   // Styles 
        200,         // x position 
        10,         // y position 
        400,        // Button width
        15,        // Button height
        hWnd,     // Parent window
        HMENU(CAP_NAME),       // No menu.
        NULL,
        NULL);      // Pointer not needed.
   cap_size = CreateWindow(
        L"Static",  // Predefined class; Unicode assumed 
        L"",      // Button text 
        WS_CHILD| WS_VISIBLE,  // Styles 
        200,         // x position 
        40,         // y position 
        400,        // Button width
        15,        // Button height
        hWnd,     // Parent window
        HMENU(CAP_SIZE),       // No menu.
        NULL,
        NULL);      // Pointer not needed.
   cap_path = CreateWindow(
       L"Static",  // Predefined class; Unicode assumed 
       L"",      // Button text 
       WS_CHILD | WS_VISIBLE,  // Styles 
       200,         // x position 
       70,         // y position 
       300,        // Button width
       15,        // Button height
       hWnd,     // Parent window
       NULL,       // No menu.
       NULL,
       NULL);      // Pointer not needed.
   edit_path = CreateWindow(
        L"Edit",  // Predefined class; Unicode assumed 
        L"",      // Button text 
        WS_CHILD | WS_EX_RIGHTSCROLLBAR,  // Styles 
        290,         // x position 
        70,         // y position 
        300,        // Button width
        15,        // Button height
        hWnd,     // Parent window
        HMENU(EDIT_PATH),       // No menu.
        NULL,
        NULL);      // Pointer not needed.
   SendMessage(list, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
   SendMessage(edit, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
   SendMessage(edit_path, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
   SendMessage(cap_name, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
   SendMessage(cap_size, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
   SendMessage(cap_path, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
   SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
   SendMessage(bt_save, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
   SendMessage(bt_paste, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
   SendMessage(bt_unsave, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
   SendMessage(bt_search, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
}
void LoadData(string path) {
    wstring wide_string = wstring(path.begin(), path.end());
    const wchar_t* result = wide_string.c_str();
    HANDLE FileToLoad = CreateFileW(
        result,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    unsigned int SizeBuffer = Files[number].Size;
    unsigned int Length = SizeBuffer;

    DWORD bytesIterated;

    wchar_t* Buffer = new wchar_t[SizeBuffer];
    BOOL rFile = ReadFile(FileToLoad, Buffer, Length-1, &bytesIterated, NULL);
    if (rFile)
    {
        if (rFile)
        {
            bytesIterated = 0;
            //SetWindowTextW(edit, L"");
            SetWindowTextW(edit, Buffer);
        }
        else
        {
            SetWindowTextW(edit, L"Не вдалося відкрити файл UTF-16");
        }
        // SetWindowText(cap_name, LPWSTR(bytesIterated));   
    }
    CloseHandle(FileToLoad);
    delete[] Buffer;
}
void LoadDataUTF8(string path) {
    HANDLE FileToLoad = CreateFileA(
        path.c_str(),
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    unsigned int SizeBuffer = (Files[number].Size);
    unsigned int Length = SizeBuffer;

    char* Buffer = new char[Length + 1];
    DWORD bytesIterated;
    BOOL rFile = ReadFile(FileToLoad, Buffer, Length, &bytesIterated, NULL);
    if (rFile)
    {
        Buffer[bytesIterated] = 0;
        //SetWindowTextW(edit, L"");
        SetWindowTextA(edit, Buffer);
    }
    else
    {
        SetWindowTextA(edit, "Не вдалося відкрити файл UTF-8");
    }
    CloseHandle(FileToLoad);
    delete[] Buffer;
}
void SaveDataUTF8(string path) {
    HANDLE FileToSave = CreateFileA(
        path.c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    int saveLenth = GetWindowTextLength(edit) + 1;
    char* data = new char[saveLenth];
    Files[number].Size = saveLenth;
    saveLenth = GetWindowTextA(edit, data, saveLenth);

    DWORD bytesIterated;
    WriteFile(FileToSave, data, saveLenth, &bytesIterated, NULL);
    string buf = FileSizeText + to_string(Files[number].Size);
    SetWindowTextA(cap_size, (buf.c_str()));
    CloseHandle(FileToSave);
    delete[] data;
}
void SaveData(string path) {
    wstring wide_string = wstring(path.begin(), path.end());
    const wchar_t* result = wide_string.c_str();
    HANDLE FileToSave = CreateFileW(
        result,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    int saveLenth = 2*GetWindowTextLengthW(edit); 
    wchar_t* data = new wchar_t[saveLenth];
    Files[number].Size = saveLenth;
    GetWindowTextW(edit, data, saveLenth);
    DWORD bytesIterated;
    WriteFile(FileToSave, data, saveLenth, &bytesIterated, NULL);
    string buf = FileSizeText + to_string(Files[number].Size);
    SetWindowTextA(cap_size, (buf.c_str()));
    CloseHandle(FileToSave);
    delete[] data;
}