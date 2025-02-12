#include <windows.h>
#include <stdio.h>

BOOL CALLBACK checkWindow(HWND hWnd, LPARAM lParam) {
    /* 
        get the window title, check if includes the words 'lego' and 'batman
        and ensure the size matches (in order to prevent false positives)

        it's like this to avoid dealing with microsoft utf nonsense 
    */
    char title[256];
    GetWindowTextA(hWnd, title, sizeof(title));

    if (strstr(title, "LEGO") && strstr(title, "Batman") && strlen(title) == 13) {
        *(HWND*)lParam = hWnd;
        return FALSE;
    }
    
    return TRUE;
}

DWORD hook(LPVOID lpThreadParameter) {
    /* find the Lego Batman window (by searching through other windows) */
    HWND hWnd = NULL;
    while (hWnd == NULL) {
        EnumWindows(checkWindow, (LPARAM)&hWnd);
        if (hWnd == NULL) /* If no window is found, Lego Batman probably isn't open, lets wait before we check again */
            Sleep(100);
    }

    /* get the monitor that the window is on (or the primary monitor) and figure out its size */
    HMONITOR src = MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
	
    MONITORINFOEX  monitorInfo;

	monitorInfo.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfoA(src, (LPMONITORINFO)&monitorInfo);

	UINT width  = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
	UINT height = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

    /* Make Lego Batman borderless fullscreen */
    SetWindowLong(hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
    SetWindowPos(hWnd, HWND_TOP, 0, 0, width, height, SWP_FRAMECHANGED);
}

HMODULE hRealBink = NULL;
void LoadRealBink();

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)hook, NULL, 0, NULL);
            LoadRealBink();
            break;
        case DLL_PROCESS_DETACH:
            if (hRealBink) FreeLibrary(hRealBink);
            break;
    }
    return TRUE;
}


/* 
    All the function passthrough stuff you probably don't care about
*/

typedef void* HBINK;
typedef HBINK (__stdcall *BinkOpen_t)(const char*, UINT32);
typedef void (__stdcall *BinkSetVolume_t)(HBINK, int, int);
typedef void (__stdcall *BinkCopyToBuffer_t)(HBINK, void*, int, int, int, int, int);
typedef void (__stdcall *BinkCopyToBufferRect_t)(HBINK, void*, int, int, int, int, int, int, int, int, int);
typedef void (__stdcall *BinkGetRects_t)(HBINK, void*);
typedef void (__stdcall *BinkClose_t)(HBINK);
typedef void (__stdcall *BinkWait_t)(HBINK);
typedef void (__stdcall *BinkNextFrame_t)(HBINK);
typedef int  (__stdcall *BinkShouldSkip_t)(HBINK);
typedef void (__stdcall *BinkDoFrame_t)(HBINK);
typedef void (__stdcall *BinkGoto_t)(HBINK, int, int);
typedef void (__stdcall *BinkRegisterFrameBuffers_t)(HBINK, void*);
typedef void (__stdcall *BinkPause_t)(HBINK, int);
typedef void (__stdcall *BinkGetFrameBuffersInfo_t)(HBINK, void*);
typedef void (__stdcall *BinkSetSoundTrack_t)(int, void*);
typedef void (__stdcall *BinkSetSoundSystem_t)(void*, void*);
typedef void (__stdcall *BinkOpenDirectSound_t)(void*);

BinkOpen_t real_BinkOpen = NULL;
BinkSetVolume_t real_BinkSetVolume = NULL;
BinkCopyToBuffer_t real_BinkCopyToBuffer = NULL;
BinkCopyToBufferRect_t real_BinkCopyToBufferRect = NULL;
BinkGetRects_t real_BinkGetRects = NULL;
BinkClose_t real_BinkClose = NULL;
BinkWait_t real_BinkWait = NULL;
BinkNextFrame_t real_BinkNextFrame = NULL;
BinkShouldSkip_t real_BinkShouldSkip = NULL;
BinkDoFrame_t real_BinkDoFrame = NULL;
BinkGoto_t real_BinkGoto = NULL;
BinkRegisterFrameBuffers_t real_BinkRegisterFrameBuffers = NULL;
BinkPause_t real_BinkPause = NULL;
BinkGetFrameBuffersInfo_t real_BinkGetFrameBuffersInfo = NULL;
BinkSetSoundTrack_t real_BinkSetSoundTrack = NULL;
BinkSetSoundSystem_t real_BinkSetSoundSystem = NULL;
BinkOpenDirectSound_t real_BinkOpenDirectSound = NULL;

__declspec(dllexport) HBINK __stdcall BinkOpen(const char* filename, UINT32 flags) {
    return real_BinkOpen(filename, flags);
}

__declspec(dllexport) void __stdcall BinkSetVolume(HBINK bink, int trackID, int volume) { if (real_BinkSetVolume) real_BinkSetVolume(bink, trackID, volume); }
__declspec(dllexport) void __stdcall BinkCopyToBuffer(HBINK bink, void* dest, int destPitch, int destHeight, int destX, int destY, int flags) {
    if (real_BinkCopyToBuffer) real_BinkCopyToBuffer(bink, dest, destPitch, destHeight, destX, destY, flags);
}
__declspec(dllexport) void __stdcall BinkCopyToBufferRect(HBINK bink, void* dest, int destPitch, int destHeight, int destX, int destY, int srcX, int srcY, int width, int height, int flags) {
    if (real_BinkCopyToBufferRect) real_BinkCopyToBufferRect(bink, dest, destPitch, destHeight, destX, destY, srcX, srcY, width, height, flags);
}

__declspec(dllexport) void __stdcall BinkGetRects(HBINK bink, void* rects) {
    real_BinkGetRects(bink, rects);
}

__declspec(dllexport) void __stdcall BinkClose(HBINK bink) {
    real_BinkClose(bink);
}

__declspec(dllexport) void __stdcall BinkWait(HBINK bink) {
    real_BinkWait(bink);
}

__declspec(dllexport) void __stdcall  BinkNextFrame(HBINK bink) {
    real_BinkNextFrame(bink);
}
__declspec(dllexport) int __stdcall  BinkShouldSkip(HBINK bink) {
    return real_BinkShouldSkip(bink);
}
__declspec(dllexport) void __stdcall  BinkDoFrame(HBINK bink) {
    real_BinkDoFrame(bink);
}
__declspec(dllexport) void __stdcall  BinkGoto(HBINK bink, int frame, int flags) {
    real_BinkGoto(bink, frame, flags);
}
__declspec(dllexport) void __stdcall  BinkRegisterFrameBuffers(HBINK bink, void* buffers) {
    real_BinkRegisterFrameBuffers(bink, buffers);
}
__declspec(dllexport) void __stdcall  BinkPause(HBINK bink, int pause) {
    real_BinkPause(bink, pause);
}
__declspec(dllexport) void __stdcall  BinkGetFrameBuffersInfo(HBINK bink, void* info) {
    real_BinkGetFrameBuffersInfo(bink, info);
}
__declspec(dllexport) void __stdcall  BinkSetSoundTrack(int trackCount, void* tracks) {
    real_BinkSetSoundTrack(trackCount, tracks);
}
__declspec(dllexport) void __stdcall  BinkSetSoundSystem(void* system, void* param) {
    real_BinkSetSoundSystem(system, param);
}
__declspec(dllexport) void __stdcall  BinkOpenDirectSound(void* dsound) {
    real_BinkOpenDirectSound(dsound);
}

void LoadRealBink() {
    hRealBink = LoadLibraryA("binkw32_real.dll");
    if (!hRealBink) {
        DWORD error = GetLastError();
        char errorMsg[256];
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, 0, errorMsg, sizeof(errorMsg), NULL);
        
        MessageBoxA(NULL, errorMsg, "Failed to Load binkw32_real.dll", MB_ICONERROR);
        exit(1);
    }

    /* Load Function Pointers */
    #define LOAD_FUNC(name, dll) real_##name = (name##_t)GetProcAddress(hRealBink, #dll); \
                            if ( real_##name == NULL ) { \
                                MessageBoxA(NULL, NULL, "Failed to Load binkw32 functions\n", MB_ICONERROR); \
                                exit(1); \
                            }

    LOAD_FUNC(BinkOpen, _BinkOpen@8)
    LOAD_FUNC(BinkSetVolume, _BinkSetVolume@12)
    LOAD_FUNC(BinkCopyToBuffer, _BinkCopyToBuffer@28)
    LOAD_FUNC(BinkCopyToBufferRect, _BinkCopyToBufferRect@44)
    LOAD_FUNC(BinkGetRects, _BinkGetRects@8)
    LOAD_FUNC(BinkClose, _BinkClose@4)
    LOAD_FUNC(BinkWait, _BinkWait@4)
    LOAD_FUNC(BinkNextFrame, _BinkNextFrame@4)
    LOAD_FUNC(BinkShouldSkip, _BinkShouldSkip@4)
    LOAD_FUNC(BinkDoFrame, _BinkDoFrame@4)
    LOAD_FUNC(BinkGoto, _BinkGoto@12)
    LOAD_FUNC(BinkRegisterFrameBuffers, _BinkRegisterFrameBuffers@8)
    LOAD_FUNC(BinkPause, _BinkPause@8)
    LOAD_FUNC(BinkGetFrameBuffersInfo, _BinkGetFrameBuffersInfo@8)
    LOAD_FUNC(BinkSetSoundTrack, _BinkSetSoundTrack@8)
    LOAD_FUNC(BinkSetSoundSystem, _BinkSetSoundSystem@8)
    LOAD_FUNC(BinkOpenDirectSound, _BinkOpenDirectSound@4)                        
}