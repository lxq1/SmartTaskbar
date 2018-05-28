#include "stdafx.h"

APPBARDATA msgData;

WINDOWPLACEMENT placement;

HWND maxWindow;

POINT cursor;

BOOL CloakedVal;

bool tryShowBar;

bool IsCursorOverTaskbar();

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    if (IsWindowVisible(hwnd) == FALSE)
        return TRUE;
    GetWindowPlacement(hwnd, &placement);
    if (placement.showCmd != SW_MAXIMIZE)
        return TRUE;
    DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &CloakedVal, sizeof(CloakedVal));
    if (CloakedVal)
        return TRUE;
    maxWindow = hwnd;
    return FALSE;
}


int main()
{
    tryShowBar = true;
    CloakedVal = TRUE;
    msgData.cbSize = sizeof(APPBARDATA);
    placement.length = sizeof(WINDOWPLACEMENT);
    SHAppBarMessage(ABM_GETTASKBARPOS, &msgData);
    while (true) {
        while (IsCursorOverTaskbar())
            Sleep(250);
        EnumWindows(EnumWindowsProc, NULL);
        if (maxWindow == NULL) {
            if (tryShowBar == false) {
                Sleep(375);
                continue;
            }
            tryShowBar = false;
            msgData.lParam = ABS_ALWAYSONTOP;
            SHAppBarMessage(ABM_SETSTATE, &msgData);
            Sleep(500);
            continue;
        }
        SHAppBarMessage(ABM_GETTASKBARPOS, &msgData);
        msgData.lParam = ABS_AUTOHIDE;
        SHAppBarMessage(ABM_SETSTATE, &msgData);
        do {
            Sleep(500);
            CloakedVal = TRUE;
            DwmGetWindowAttribute(maxWindow, DWMWA_CLOAKED, &CloakedVal, sizeof(CloakedVal));
            if (CloakedVal)
                break;
            GetWindowPlacement(maxWindow, &placement);
        } while (placement.showCmd == SW_MAXIMIZE);
        tryShowBar = true;
        maxWindow = NULL;
    }
    return 0;
}

inline bool IsCursorOverTaskbar()
{
    GetCursorPos(&cursor);
    switch (msgData.uEdge)
    {
    case ABE_BOTTOM:
        if (cursor.y >= msgData.rc.top)
            return true;
        return false;
    case ABE_LEFT:
        if (cursor.x <= msgData.rc.right)
            return true;
        return false;
    case ABE_TOP:
        if (cursor.y <= msgData.rc.bottom)
            return true;
        return false;
    default:
        if (cursor.x >= msgData.rc.left)
            return true;
        return false;
    }
}
