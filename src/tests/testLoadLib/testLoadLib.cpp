// testLoadLib.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "testLoadLib.h"
#include <dshow.h>
#include <windows.h>
#include <commctrl.h>


static const GUID CLSID_XX_OggDemux =  
{ 0xc9361f5a, 0x3282, 0x4944, { 0x98, 0x99, 0x6d, 0x99, 0xcd, 0xc5, 0x37, 0xb } };

static const GUID  CLSID_X_Theora_Decoder =
{ 0x5187161, 0x5c36, 0x4324, { 0xa7, 0x34, 0x22, 0xbf, 0x37, 0x50, 0x9f, 0x2d } };


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE			g_hInst;			// current instance
HWND				g_hWndMenuBar;		// menu bar handle

// Forward declarations of functions included in this code module:
ATOM			MyRegisterClass(HINSTANCE, LPTSTR);
BOOL			InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
	MSG msg;

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	HACCEL hAccelTable;
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TESTLOADLIB));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
ATOM MyRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TESTLOADLIB));
	wc.hCursor       = 0;
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = szWindowClass;

	return RegisterClass(&wc);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    TCHAR szTitle[MAX_LOADSTRING];		// title bar text
    TCHAR szWindowClass[MAX_LOADSTRING];	// main window class name

    g_hInst = hInstance; // Store instance handle in our global variable

    // SHInitExtraControls should be called once during your application's initialization to initialize any
    // of the device specific controls such as CAPEDIT and SIPPREF.
    SHInitExtraControls();

    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING); 
    LoadString(hInstance, IDC_TESTLOADLIB, szWindowClass, MAX_LOADSTRING);

    //If it is already running, then focus on the window, and exit
    hWnd = FindWindow(szWindowClass, szTitle);	
    if (hWnd) 
    {
        // set focus to foremost child window
        // The "| 0x00000001" is used to bring any owned windows to the foreground and
        // activate them.
        SetForegroundWindow((HWND)((ULONG) hWnd | 0x00000001));
        return 0;
    } 

    if (!MyRegisterClass(hInstance, szWindowClass))
    {
    	return FALSE;
    }

    hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    // When the main window is created using CW_USEDEFAULT the height of the menubar (if one
    // is created is not taken into account). So we resize the window after creating it
    // if a menubar is present
    if (g_hWndMenuBar)
    {
        RECT rc;
        RECT rcMenuBar;

        GetWindowRect(hWnd, &rc);
        GetWindowRect(g_hWndMenuBar, &rcMenuBar);
        rc.bottom -= (rcMenuBar.bottom - rcMenuBar.top);
		
        MoveWindow(hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, FALSE);
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);


    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    static SHACTIVATEINFO s_sai;
	
    switch (message) 
    {
        case WM_COMMAND:
            wmId    = LOWORD(wParam); 
            wmEvent = HIWORD(wParam); 
            // Parse the menu selections:
            switch (wmId)
            {
                case IDM_HELP_ABOUT:
                    DialogBox(g_hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, About);
                    break;
                case IDM_OK:
                    SendMessage (hWnd, WM_CLOSE, 0, 0);				
                    break;
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        case WM_CREATE:
            SHMENUBARINFO mbi;

            memset(&mbi, 0, sizeof(SHMENUBARINFO));
            mbi.cbSize     = sizeof(SHMENUBARINFO);
            mbi.hwndParent = hWnd;
            mbi.nToolBarId = IDR_MENU;
            mbi.hInstRes   = g_hInst;

            if (!SHCreateMenuBar(&mbi)) 
            {
                g_hWndMenuBar = NULL;
            }
            else
            {
                g_hWndMenuBar = mbi.hwndMB;
            }

            // Initialize the shell activate info structure
            memset(&s_sai, 0, sizeof (s_sai));
            s_sai.cbSize = sizeof (s_sai);
            break;
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            
            // TODO: Add any drawing code here...
            
            EndPaint(hWnd, &ps);
            break;
        case WM_DESTROY:
            CommandBar_Destroy(g_hWndMenuBar);
            PostQuitMessage(0);
            break;

        case WM_ACTIVATE:
            // Notify shell of our activate message
            SHHandleWMActivate(hWnd, wParam, lParam, &s_sai, FALSE);
            break;
        case WM_SETTINGCHANGE:
            SHHandleWMSettingChange(hWnd, wParam, lParam, &s_sai);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
            {
                // Create a Done button and size it.  
                SHINITDLGINFO shidi;
                shidi.dwMask = SHIDIM_FLAGS;
                shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN | SHIDIF_EMPTYMENU;
                shidi.hDlg = hDlg;
                SHInitDialog(&shidi);
            }
            return (INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {

				//IBaseFilter* locVidRender = NULL;
				//CoCreateInstance(CLSID_VideoRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)locVidRender);

				//IEnumPins* locPinEnum = NULL;
				//locVidRender->EnumPins(&locPinEnum);

				//IPin* locPin = NULL;
				//ULONG locHowMany = 0;
				//while(locPinEnum->Next(1, &locPin, &locHowMany)) {

				//	IEnumMediaTypes* locMTEnum = NULL;
				//	
				//	locPin->EnumMediaTypes(&locMTEnum);

				//	ULONG locHowManyMT = 0;
				//	AM_MEDIA_TYPE* locMediaType = NULL;
				//	while (locMTEnum->Next(1, &locMediaType, &locHowManyMT)) {
				//		if (locMediaType->majortype == MEDIATYPE_Video) {
				//			if (locMediaType->subtype == MEDIASUBTYPE_YUY2) {
				//				locHowManyMT = locHowManyMT;
				//			} else if (locMediaType->subtype == MEDIASUBTYPE_RGB565) {
				//				locHowManyMT = locHowManyMT;
				//			} else if (locMediaType->subtype == MEDIASUBTYPE_RGB555) {
				//				locHowManyMT = locHowManyMT;
				//			} else if (locMediaType->subtype == MEDIASUBTYPE_RGB24) {
				//				locHowManyMT = locHowManyMT;
				//			} else {
				//				locHowManyMT = locHowManyMT;
				//			}
				//		}

				//	}
				//}

				//IDirectDrawVideo* locDDV = NULL;
				//locVidRender->QueryInterface(IID_IDirectDrawVideo, (void**)&locDDV);

				//DWORD locNumCodes = 0;
				//locDDV->GetFourCCCodes(&locNumCodes, NULL);

				//DWORD* locCodes = new DWORD[locNumCodes];
				//locDDV->GetFourCCCodes(&locNumCodes, locCodes);




				//HINSTANCE locLib;
				//DWORD locErr;
				////FARPROC locProc;
				//HRESULT (__stdcall*locProc)(); 
				//HRESULT locRes;

				//locErr = GetLastError();
				//locLib = LoadLibrary(L"dsfVorbisDecoder.dll");
				//locErr = GetLastError();
				////locProc = (HRESULT (__stdcall*)())GetProcAddress(locLib, L"DllRegisterServer");
				////locRes = locProc();
				//FreeLibrary(locLib);

				//locErr = GetLastError();
				//locLib = LoadLibrary(L"dsfSpeexDecoder.dll");
				//locErr = GetLastError();
				////locProc = (HRESULT (__stdcall*)())GetProcAddress(locLib, L"DllRegisterServer");
				////locRes = locProc();
				//FreeLibrary(locLib);

				//locErr = GetLastError();
				//locLib = LoadLibrary(L"dsfTheoraDecoder.dll");
				//locErr = GetLastError();
				////locProc = (HRESULT (__stdcall*)())GetProcAddress(locLib, L"DllRegisterServer");
				////locRes = locProc();
				//FreeLibrary(locLib);

				//locErr = GetLastError();
				//locLib = LoadLibrary(L"dsfOggDemux2.dll");
				//locErr = GetLastError();
				//locProc = (HRESULT (__stdcall*)())GetProcAddress(locLib, L"DllRegisterServer");
				//locRes = locProc();
				//FreeLibrary(locLib);




				c_initialise();
				////wstring locStr = L"\\Storage Card\\do_you_see.ogg";
				c_loadFileWithParams( L"http://www.illiminable.com/temp/do_you_see.ogg", hDlg, 0,0,64,48);
				c_play();

















				//IGraphBuilder* locGraphBuilder = NULL;
				//IMediaControl* locMediaControl = NULL;
				//HRESULT locHR = S_FALSE;;
				//CoInitialize(NULL);
				//IBaseFilter* locTheoDecode = NULL;
				//locHR = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&locGraphBuilder);

				////locHR = CoCreateInstance(CLSID_X_Theora_Decoder, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&locTheoDecode);
				//

				//
				////IBaseFilter* locDemux = NULL;
				////locHR = CoCreateInstance(CLSID_XX_OggDemux, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&locDemux);
				//locHR = locGraphBuilder->RenderFile(L"http://www.illiminable.com/temp/summer_city.ogg", NULL);


















/*






				IGraphBuilder* locGraphBuilder = NULL;
				IMediaControl* locMediaControl = NULL;
				IBaseFilter* locDemuxer = NULL;
				IBaseFilter* locTheoDecoder = NULL;
				//ICustomSource* locCustomSourceSetter = NULL;
				IFileSourceFilter* locFS = NULL;
				HRESULT locHR = S_FALSE;;
				CoInitialize(NULL);
				locHR = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&locGraphBuilder);

				locHR = CoCreateInstance(CLSID_XX_OggDemux, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&locDemuxer);

				locHR = CoCreateInstance(CLSID_X_Theora_Decoder, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&locTheoDecoder);

				locHR = locGraphBuilder->AddFilter(locDemuxer, L"Custom Ogg Source");
				locHR = locGraphBuilder->AddFilter(locTheoDecoder, L"Theo Filter");

				

				locHR = locDemuxer->QueryInterface(IID_IFileSourceFilter, (void**)&locFS);

				locHR = locFS->Load(L"\\Storage Card\\carbon.ogg", NULL);


				//CustomSourceClass* locCustomFileSourceInterface = new CustomSourceClass;
				//locCustomFileSourceInterface->open("D:\\testfile.ogg");
				//
				//locCustomSourceSetter->setCustomSourceAndLoad(locCustomFileSourceInterface);

				//Do not release, it's not really a COM interface
				//locCustomSourceSetter->Release();

				IEnumPins* locPinEnum = NULL;

				locTheoDecoder->EnumPins(&locPinEnum);
				vector<IPin*> locTheoPins;

				ULONG locHowMany = 0;
				
				IPin* locPin = NULL;

				

				while (locPinEnum->Next(1, &locPin, &locHowMany) == S_OK) {
					locTheoPins.push_back(locPin);
				}

				

				locDemuxer->EnumPins(&locPinEnum);
				locPin = NULL;
				
				
				
				locHowMany = 0;
				while (locPinEnum->Next(1, &locPin, &locHowMany) == S_OK) {
					//locHR = locGraphBuilder->Render(locPin);

					//for (size_t i = 0; i < locTheoPins.size(); i++) {
						//locPin->Connect(locTheoPins[i], 
						locHR = locGraphBuilder->ConnectDirect(locPin, locTheoPins[0], NULL);
						locHR = locGraphBuilder->Render(locTheoPins[1]);
					//}
					locPin->Release();
					locPin = NULL;
				}


*/



				//locHR = locGraphBuilder->QueryInterface(IID_IMediaControl, (void**)&locMediaControl);


				//locHR = locMediaControl->Run();

				//IMediaEvent* locMediaEvent = NULL;
				//locHR = locGraphBuilder->QueryInterface(IID_IMediaEvent, (void**)&locMediaEvent);
				//
				//HANDLE  hEvent; 
				//long    evCode, param1, param2;
				//BOOLEAN bDone = FALSE;
				//HRESULT hr = S_OK;
				//hr = locMediaEvent->GetEventHandle((OAEVENT*)&hEvent);
				//if (FAILED(hr))
				//{
				//	/* Insert failure-handling code here. */
				//}
				//while(!bDone) 
				//{
				//	if (WAIT_OBJECT_0 == WaitForSingleObject(hEvent, 100))
				//	{ 
				//		while (hr = locMediaEvent->GetEvent(&evCode, &param1, &param2, 0), SUCCEEDED(hr)) 
				//		{
				//			//printf("Event code: %#04x\n Params: %d, %d\n", evCode, param1, param2);
				//			//cout<<"Event : "<<evCode<<" Params : "<<param1<<", "<<param2<<endl;
				//			locMediaEvent->FreeEventParams(evCode, param1, param2);
				//			bDone = (EC_COMPLETE == evCode);
				//		}
				//	}
				//} 

				////cout<<"Finished..."<<endl;
				//int x;
				////cin>>x;
				//locMediaControl->Release();
				//locGraphBuilder->Release();
				//CoUninitialize();






				
	
                EndDialog(hDlg, LOWORD(wParam));
                return TRUE;
            }
            break;

        case WM_CLOSE:
            EndDialog(hDlg, message);
            return TRUE;

#ifdef _DEVICE_RESOLUTION_AWARE
        case WM_SIZE:
            {
		DRA::RelayoutDialog(
			g_hInst, 
			hDlg, 
			DRA::GetDisplayMode() != DRA::Portrait ? MAKEINTRESOURCE(IDD_ABOUTBOX_WIDE) : MAKEINTRESOURCE(IDD_ABOUTBOX));
            }
            break;
#endif
    }
    return (INT_PTR)FALSE;
}
