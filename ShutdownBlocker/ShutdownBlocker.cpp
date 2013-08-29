// ShutdownDelay.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "ShutdownBlocker.h"

#define MAX_LOADSTRING 100

// �O���[�o���ϐ�:
HINSTANCE g_hInst;

// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̐錾��]�����܂�:
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

	// �O���[�o������������������Ă��܂��B
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
   
	// ���C�� ���b�Z�[�W ���[�v:
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
//  �֐�: OnApply
//
//  �ړI: �V���b�g�_�E�������ƃV���b�g�_�E���u���b�N���R��K�p����.
//
void OnApply(HWND hWnd)
{
	WCHAR szBuf[MAX_LOADSTRING];

	LPWSTR pLast = NULL;

	GetDlgItemText(hWnd, IDC_SHUTDOWN_LEVEL, szBuf, MAX_LOADSTRING);
	int shutdownLevel = wcstol(szBuf, &pLast, 16);

    // �V���b�g�_�E���̐���
    // http://dobon.net/vb/bbs/log3-36/22254.html

	// �V�X�e�����̑��̃v���Z�X���猩�����ΓI�ȃV���b�g�_�E���̗D�揇����ݒ肷��.
	SetProcessShutdownParameters(shutdownLevel, 0);

	// �V���b�g�_�E�����u���b�N����.
	GetDlgItemText(hWnd, IDC_SHUTDOWN_REASON, szBuf, MAX_LOADSTRING);
	ShutdownBlockReasonCreate(hWnd, szBuf);
}

//
//  �֐�: OnInit
//
//  �ړI: �_�C�A���O������������.
//
void OnInit(HWND hWnd)
{
	WCHAR szBuf[MAX_LOADSTRING];

	// �V���b�g�_�E������
	LoadString(g_hInst, IDS_SHUTDOWN_LEVEL, szBuf, MAX_LOADSTRING);
	SetDlgItemText(hWnd, IDC_SHUTDOWN_LEVEL, szBuf);

	// �V���b�g�_�E���̃u���b�N�̗��R
	LoadString(g_hInst, IDS_SHUTDOWN_REASON, szBuf, MAX_LOADSTRING);
	SetDlgItemText(hWnd, IDC_SHUTDOWN_REASON, szBuf);

	OnApply(hWnd);
}

//
//  �֐�: MyRegisterClass()
//
//  �ړI: �E�B���h�E �N���X��o�^���܂��B
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	// ���C�� �E�B���h�E �N���X��
	// �g�b�v���x���E�B���h�E�ɂ̓E�B���h�E�N���X���K�v.
	// �_�C�A���O���\�[�X���"CLASS"�̎w��Ɠ���������Ƃ���.
	TCHAR szWindowClass[MAX_LOADSTRING];
	LoadString(hInstance, IDC_SHUTDOWNDELAY, szWindowClass, MAX_LOADSTRING);

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= DLGWINDOWEXTRA; // �� �_�C�A���O�̏ꍇ�A���ꂪ�K�v
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
//  �֐�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  �ړI:  ���C�� �E�B���h�E�̃��b�Z�[�W���������܂��B
//
//  WM_COMMAND	- �A�v���P�[�V���� ���j���[�̏���
//  WM_PAINT	- ���C�� �E�B���h�E�̕`��
//  WM_DESTROY	- ���~���b�Z�[�W��\�����Ė߂�
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
		// �I�����ꂽ���j���[�̉��:
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
		// ���C���E�B���h�E�̔j���ɂ�胁�b�Z�[�W���[�v���I������.
		PostQuitMessage(0);
		break;

	case WM_QUERYENDSESSION:
		// ���O�I�t�A�V���b�g�_�E�����������Ȃ�.
		return 0;

	case WM_ENDSESSION:
		// ���O�I�t�A�V���b�g�_�E�����������Ȃ�.
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
