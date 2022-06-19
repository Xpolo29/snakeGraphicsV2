#include "framework.h"
#include "snakeGraphicsV2.h"
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <string>
#include "HMAP.h"

#define MAX_LOADSTRING 100

//var
int apple;
int score;
int HIndex;
bool lost = false;
int apHIndex;
int X = 65; //anchor map
int Y = 65; //anchor map
const int FRAME_TIME = 1; //ms
bool init = true;
int adjMap[N][4];
int Map[N];
int body[N];
int HMap[N];


RECT info = { X ,Y + 30 + 10 * MAP_HEIGHT,X + 200 + 10 * MAP_LENGTH,Y + 200 + 10 * MAP_HEIGHT };


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);



struct vect2 {
    int x, y;
};

vect2 idToCoord(int p) {
    vect2 v;
    v.x = 10 * (p % MAP_LENGTH);
    v.y = 10 * (p / MAP_LENGTH);
    return v;
}

//return new point
int getNP(int p, int d) {
    int np = -1;
    if (d == 0) { np = p + 1; }
    else if (d == 1) { np = p + MAP_LENGTH; }
    else if (d == 2) { np = p - 1; }
    else if (d == 3) { np = p - MAP_LENGTH; }
    return np;
}

bool inBorder(int np, int p = -1, int d = -1) {
    if ((np < 0) || (np >= N)) { return false; }
    if (d == 0) { if (p % MAP_LENGTH == MAP_LENGTH - 1) { return false; } }
    if (d == 1) { if (p >= N - MAP_LENGTH) { return false; } }
    if (d == 2) { if (p % MAP_LENGTH == 0) { return false; } }
    if (d == 3) { if (p < MAP_LENGTH) { return false; } }
    return true;
}

int getDir(int p, int np) {
    int delta = np - p;
    if (delta == 1) { return 0; }
    if (delta == -1) { return 2; }
    if (delta == MAP_LENGTH) { return 1; }
    if (delta == -MAP_LENGTH) { return 3; }
    return -1;
}

bool linked(int A, int B) {
    for (int i = 0; i < 4; i++) {
        if (adjMap[A][i] == B) { return true; }
    }
    return false;
}


int getHIndex(int p) {
    int x = sizeof(HMap) / sizeof(*HMap);
    return std::distance(HMap, std::find(HMap, HMap + x, p));
}

int dist(int A, int B) {
    int v1[2] = { A % 100,A / 100 };
    int v2[2] = { B % 100,B / 100 };
    int d = (v1[0] - v2[0]) ^ 2 + (v1[1] - v2[1]) ^ 2;
    return d;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SNAKEGRAPHICSV2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SNAKEGRAPHICSV2));

    MSG msg;

    
    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int) msg.wParam;
}

int rdp(int ap = -1) {
    int p = rand() % N;

    if ((contains(body, p,N) || p == ap)) { return rdp(ap); }
    return p;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SNAKEGRAPHICSV2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(7);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SNAKEGRAPHICSV2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

wchar_t* convertCharArrayToLPCWSTR(const char* charArray)
{
    wchar_t* wString = new wchar_t[4096]; MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096); return wString;
}

void update(HWND hwnd,        // handle to window for timer messages
    UINT message,     // WM_TIMER message
    UINT idTimer,     // timer identifier
    DWORD dwTime)     // current system time  
{
    
    //Update
    if (lost) { KillTimer(hwnd, 1); return; }

    if (init) {
        RECT map = { X - 6, Y - 6, X + 6 + 10 * MAP_LENGTH, Y + 6 + 10 * MAP_HEIGHT };
        InvalidateRect(hwnd, &map, true);
        InvalidateRect(hwnd, &info, true);
        init = !init;
    }
    vect2 vtail = idToCoord(body[0]);
    if ((body[N - 1] == -1)) {
        int np = -1;
        if (np == -1) { HIndex = (HIndex + 1) % N; np = HMap[HIndex]; }

        int d = getDir(body[score - 1], np);
        if (!inBorder(np, body[score - 1], d) || (contains(body, np, 1))) {
            std::cout << body[score - 1] << " " << d << " " << np << std::endl;
            std::cout << "Lost ! Score is " << score << std::endl;
            lost = true;
            return;
        }


        if (np == apple) {
            vect2 vap = idToCoord(apple);
            RECT old = { X+vap.x, Y+vap.y, X+vap.x+9, Y+vap.y+9};
            
            
            //std::cout << "Score is " << score << std::endl;
            
            body[score] = np;
            apple = rdp();
            score++;

            vap = idToCoord(apple);
            RECT newer = { X + vap.x, Y + vap.y, X + vap.x + 9, Y + vap.y + 9 };

            InvalidateRect(hwnd, &newer, true);       
            InvalidateRect(hwnd, &old, true);
            InvalidateRect(hwnd, &info, true);

        }
        else {
            
            for (int i = 0; i < score - 1; i++) {
                body[i] = body[i + 1];
            }
            body[score - 1] = np;
            vect2 vhead = idToCoord(body[score - 1]);

            RECT head = { X + vhead.x, Y + vhead.y, X + vhead.x + 9, Y + vhead.y + 9 };
            RECT tail = { X + vtail.x, Y + vtail.y, X + vtail.x + 9, Y + vtail.y + 9 };
            InvalidateRect(hwnd, &tail, true);
            InvalidateRect(hwnd, &head, true);
        }
        //snake end
    }
    else { 
        lost = true;
        score++;
        vect2 vhead = idToCoord(body[score - 1]);
        RECT head = { X + vhead.x, Y + vhead.y, X + vhead.x + 9, Y + vhead.y + 9 };
        InvalidateRect(hwnd, &head, true);
        InvalidateRect(hwnd, &info, true);
    }
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    
    
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
      0, 0, 2*X + 10 * MAP_LENGTH+12, Y + 170 + 10 * MAP_HEIGHT, nullptr, nullptr, hInstance, nullptr);


   if (!hWnd)
   {
      return FALSE;
   }


   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   //snake3 init
   //init adjMap
   for (int i = 0; i < N; i++) {
       body[i] = -1;
       for (int j = 0; j < 4; j++) {
           adjMap[i][j] = -1;
       }
   }

   for (int k = 0; k < N; k++) {
       for (int d = 0; d < 4; d++) {
           int np = getNP(k, d);
           if (inBorder(np, k, d)) {
               adjMap[k][d] = np;
           }
       }
   }
   HMapstruct temp = getHMap(true);
   for (int i = 0; i < N; i++) {
       HMap[i] = temp.map[i];
   }

   //Init pos
   apple = rdp();
   score = 1;
   body[0] = rdp(apple); //0 is tail and score-1 is head
   HIndex = getHIndex(body[0]);
   apHIndex = getHIndex(apple);
   //snkae init end

   SetTimer(hWnd, 1, FRAME_TIME, (TIMERPROC)update);
   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    int x = X;
    int y = Y;
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        //map
        HPEN pen = CreatePen(PS_SOLID, 10, RGB(0, 0, 0));
        SelectObject(hdc, pen);
        Rectangle(hdc, X-6, Y-6, X+6+10*MAP_LENGTH, Y + 6 + 10 * MAP_HEIGHT);
        DeleteObject(pen);

        //body
        HBRUSH pen2 = CreateSolidBrush(RGB(0, 0, 255));
        SelectObject(hdc, pen2);
        if (!lost) {
            for (int i = 0; i < score; i++) {
                if (body[i] == -1) { continue; }
                vect2 v = idToCoord(body[i]);
                RECT carre = { X + v.x,Y + v.y,X + v.x + 9,Y + v.y + 9 };
                FillRect(hdc, &carre, pen2);
            }
        }
        else {
            for (int i = 0; i < N; i++) {
                vect2 v = idToCoord(i);
                RECT carre = { X + v.x,Y + v.y,X + v.x + 9,Y + v.y + 9 };
                FillRect(hdc, &carre, pen2);
            }
        }
        DeleteObject(pen2);
        //apple
        
        if (!lost) {
            HBRUSH pen3 = CreateSolidBrush(RGB(255, 0, 0));
            SelectObject(hdc, pen3);
            vect2 v = idToCoord(apple);
            RECT carre = { X + v.x,Y + v.y,X + v.x + 9,Y + v.y + 9 };
            FillRect(hdc, &carre, pen3);
            DeleteObject(pen3);
        }

        //debug
        std::string tmp = std::to_string(score);
        char const* num_char = tmp.c_str();
        LPCWSTR text2 = L"Score = ";
        LPCWSTR text = convertCharArrayToLPCWSTR(num_char);
        std::wstring df = std::wstring(text2)+ std::wstring(text);
        LPCWSTR fused = df.c_str(); 

        DrawText(hdc, fused, sizeof(text2)+sizeof(score), &info, DT_LEFT);
        EndPaint(hWnd, &ps);

        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
