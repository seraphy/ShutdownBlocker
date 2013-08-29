// ShutdownDelay.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "ShutdownBlocker.h"

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE g_hInst;

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_     HINSTANCE hInstance,
                       _In_opt_ HINSTANCE hPrevInstance,
                       _In_     LPTSTR    lpCmdLine,
                       _In_     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	g_hInst = hInstance;

	WCHAR langName[10];
	LCID id = GetUserDefaultLCID();
	GetLocaleInfo(id, LOCALE_SISO639LANGNAME, langName, 10);
	MessageBox(NULL, langName, L"LANG", MB_ICONINFORMATION | MB_OK);

	// グローバル文字列を初期化しています。
	MyRegisterClass(hInstance);

    HWND hWnd = CreateDialog(hInstance,
		                     MAKEINTRESOURCE(IDD_SHUTDOWNDELAY),
							 0,
							 reinterpret_cast<DLGPROC>(WndProc)
							 );
    if (!hWnd) {
       return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
   
	// メイン メッセージ ループ:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!IsDialogMessage(hWnd, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

//
//  関数: OnApply
//
//  目的: シャットダウン順序とシャットダウンブロック理由を適用する.
//
void OnApply(HWND hWnd)
{
	WCHAR szBuf[MAX_LOADSTRING];

	LPWSTR pLast = NULL;

	GetDlgItemText(hWnd, IDC_SHUTDOWN_LEVEL, szBuf, MAX_LOADSTRING);
	int shutdownLevel = wcstol(szBuf, &pLast, 16);

    // シャットダウンの制限
    // http://dobon.net/vb/bbs/log3-36/22254.html

	// システム内の他のプロセスから見た相対的なシャットダウンの優先順序を設定する.
	SetProcessShutdownParameters(shutdownLevel, 0);

	// シャットダウンをブロックする.
	GetDlgItemText(hWnd, IDC_SHUTDOWN_REASON, szBuf, MAX_LOADSTRING);
	ShutdownBlockReasonCreate(hWnd, szBuf);
}

//
//  関数: OnInit
//
//  目的: ダイアログを初期化する.
//
void OnInit(HWND hWnd)
{
	WCHAR szBuf[MAX_LOADSTRING];

	// シャットダウン順序
	LoadString(g_hInst, IDS_SHUTDOWN_LEVEL, szBuf, MAX_LOADSTRING);
	SetDlgItemText(hWnd, IDC_SHUTDOWN_LEVEL, szBuf);

	// シャットダウンのブロックの理由
	LoadString(g_hInst, IDS_SHUTDOWN_REASON, szBuf, MAX_LOADSTRING);
	SetDlgItemText(hWnd, IDC_SHUTDOWN_REASON, szBuf);

	OnApply(hWnd);
}

//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	// メイン ウィンドウ クラス名
	// トップレベルウィンドウにはウィンドウクラスが必要.
	// ダイアログリソース上の"CLASS"の指定と同じ文字列とする.
	TCHAR szWindowClass[MAX_LOADSTRING];
	LoadString(hInstance, IDC_SHUTDOWNDELAY, szWindowClass, MAX_LOADSTRING);

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= DLGWINDOWEXTRA; // ← ダイアログの場合、これが必要
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SHUTDOWNDELAY));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SHUTDOWNDELAY);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:  メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND	- アプリケーション メニューの処理
//  WM_PAINT	- メイン ウィンドウの描画
//  WM_DESTROY	- 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_INITDIALOG:
		OnInit(hWnd);
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case IDOK:
			OnApply(hWnd);
			break;

		case IDCANCEL:
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_DESTROY:
		// メインウィンドウの破棄によりメッセージループを終了する.
		PostQuitMessage(0);
		break;

	case WM_QUERYENDSESSION:
		// ログオフ、シャットダウン等を許可しない.
		return 0;

	case WM_ENDSESSION:
		// ログオフ、シャットダウン等を許可しない.
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
