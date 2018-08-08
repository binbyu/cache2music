
// Cache.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Cache.h"
#include "CacheDlg.h"
#include <Dbghelp.h> 


#pragma comment(lib, "Dbghelp.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


LONG WINAPI crush_callback(_EXCEPTION_POINTERS* ep);
// CCacheApp

BEGIN_MESSAGE_MAP(CCacheApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CCacheApp construction

CCacheApp::CCacheApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CCacheApp object

CCacheApp theApp;


// CCacheApp initialization

BOOL CCacheApp::InitInstance()
{
	SetUnhandledExceptionFilter(crush_callback);

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CCacheDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}


LONG WINAPI crush_callback(_EXCEPTION_POINTERS* ep)
{
	time_t t;
	struct tm *p;
	TCHAR fname[MAX_PATH] = {0};

	t = time(NULL) + 8 * 3600;
	p = gmtime(&t);

	_stprintf(fname, _T("dump_%d-%d-%d_%d_%d_%d.DMP"), 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, (p->tm_hour)%24, p->tm_min, p->tm_sec);

	HANDLE hFile = CreateFile(fname,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}
	MINIDUMP_EXCEPTION_INFORMATION    exceptioninfo;
	exceptioninfo.ExceptionPointers = ep;
	exceptioninfo.ThreadId          = GetCurrentThreadId();
	exceptioninfo.ClientPointers    = FALSE;

	if (!MiniDumpWriteDump(GetCurrentProcess(),
		GetCurrentProcessId(),
		hFile,
		MiniDumpWithFullMemory,
		&exceptioninfo,
		NULL,
		NULL))
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}
	CloseHandle(hFile);
	return EXCEPTION_EXECUTE_HANDLER;
}

